#include "pch.h"
#include "DetectionAlgorithm.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CDetectionAlgorithm::DoRemoveNoise()
{
	DoRemoveNoiseProcImage();
	DoRemoveNoiseThresholdImage();

	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto removeNoiseParams = ::g_RecipeManager.m_sRemoveNoiseParams;

	long slotCount = roiParams.nSlotCount;
	long layerCount = roiParams.nLayerCount;
	long layerPair = layerCount / 2;

	BYTE* pbyUnrolledImage = ::g_MemoryManager.GetByteImage((long)IMAGE::UNROLLED);
	BYTE* pbyThreshold = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_THRESHOLD); // Threshold Image
	BYTE* pbyResult = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_RESULT); // Result Image
	long* pnLabelImage = ::g_MemoryManager.GetLongImage(); // Label Image
	
	memset(pnLabelImage, 0, m_nUnrolledImageSize * sizeof(long));
	memset(pbyResult, 0, m_nUnrolledImageSize);

	auto &blobList = m_vecRemoveNoiseBlobInfoList;

	// BlobInfo 초기화
	HUBSDK::BlobInfo value;
	value.Init();
	m_pAlgorithm->ResizeVector(blobList, removeNoiseParams.nMaxBlobCount, value);

	// Temp BlobInfo 초기화
	std::vector<std::vector<std::vector<HUBSDK::BlobInfo>>> vecBlobInfoTemp;
	m_pAlgorithm->ResizeVector(vecBlobInfoTemp, roiParams.nSlotCount, layerPair, 100, value);

	// Blob
	m_nRemoveNoiseBlobTotalCount = 0;
	long blob = 0;
	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerPair; layer++)
		{
			long nBlobCount(0);
			auto &tempBlobInfo = vecBlobInfoTemp[slot][layer];

			if(!m_pAlgorithm->Blob(pbyThreshold, m_nUnrolledImageWidth, m_nUnrolledImageHeight, m_vecrtHairpin[slot][layer], FALSE, removeNoiseParams.nMinBlobSize, removeNoiseParams.nMaxBlobCount, &tempBlobInfo[0], nBlobCount, pnLabelImage))
			{
				return FALSE;
			}

			// 헤어핀 ROI 영역에 없는 Blob 들은 지워 버린다.(Noise로 간주)
			auto rtRoiHairpin = m_vecrtHairpin[slot][layer];
			rtRoiHairpin.left += roiParams.nHairpinAreaROICenterToLeftOffset;
			rtRoiHairpin.top += roiParams.nHairpinAreaROICenterToTopOffset;
			rtRoiHairpin.right -= roiParams.nHairpinAreaROICenterToRightOffset;
			rtRoiHairpin.bottom -= roiParams.nHairpinAreaROICenterToBottomOffset;

			for(long idx = 0; idx < (long)tempBlobInfo.size(); idx++)
			{
				auto roi = tempBlobInfo[idx].m_roi & rtRoiHairpin;

				if(roi.Width() == 0 || roi.Height() == 0)
				{
					HUBIP::ImageFill(tempBlobInfo[idx].m_roi, m_nUnrolledImageWidth, m_nUnrolledImageHeight, 0, pbyThreshold);
				}
				else
				{
					blobList[blob] = tempBlobInfo[idx];
					blob++;
				}
			}
		}
	}

	m_nRemoveNoiseBlobTotalCount = blob;
	blobList.resize(m_nRemoveNoiseBlobTotalCount);

	// Blob Info 계산
	double bgAverageValue(0.f); // Contrast 계산시 필요
	if(HUBIP::CalcBlobInfo(pbyUnrolledImage, (BYTE)bgAverageValue, pnLabelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, m_nRemoveNoiseBlobTotalCount, &blobList[0]) != retStatus::success)
	{
		return FALSE;
	}

	BYTE* pbyTemp = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_HYSTERESIS_TEMP); // Temp Image
	memcpy(pbyTemp, pbyThreshold, m_nUnrolledImageSize);
	memset(pbyThreshold, 0, m_nUnrolledImageSize);

	for(long blob = 0; blob < m_nRemoveNoiseBlobTotalCount; blob++)
	{
		auto blobInfo = blobList[blob];
		auto aspectRatio = blobInfo.m_aspectRatio;

		// Use Remove Blob
		if(removeNoiseParams.bUseRemoveBlob == TRUE)
		{
			if(aspectRatio < removeNoiseParams.fAspectRatioMin || aspectRatio > removeNoiseParams.fAspectRatioMax)
			{
				continue;
			}
		}

		HUBIP::ImageCopy(pbyTemp, m_nUnrolledImageWidth, m_nUnrolledImageHeight, blobInfo.m_roi, pbyThreshold);
	}

	// Use Mask Convolution
	if(removeNoiseParams.bUseMaskConvolution == TRUE)
	{
		CalcMaskConvolution();
	}
	else
	{
		HUBIP::BitwiseAnd(pbyUnrolledImage, pbyThreshold, m_nUnrolledImageWidth, m_nUnrolledImageHeight, pbyResult);
	}

	return TRUE;
}

void CDetectionAlgorithm::CalcMaskConvolution()
{
	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto removeNoiseParams = ::g_RecipeManager.m_sRemoveNoiseParams;

	BYTE* pbyThreshold = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_THRESHOLD); // Threshold Image
	BYTE* pbyMask = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_MASK); // 마스크 영상
	BYTE* pbySubResult = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_SUB); // 빼기 결과 영상

	memset(pbyMask, 0, m_nUnrolledImageSize);
	memset(pbySubResult, 0, m_nUnrolledImageSize);

	long slotCount = roiParams.nSlotCount;
	long layerCount = roiParams.nLayerCount;
	long layerPair = layerCount / 2;

	// ROI 영역만 255 로 채운다.
	HUBIP::ImageFill(m_rtHairpinWhole, m_nUnrolledImageWidth, m_nUnrolledImageHeight, (BYTE)255, pbyMask);

	// ROI별로 Processing을 했는지 확인을 위한 변수
	m_pAlgorithm->ResizeVector(m_vecbProcessing, slotCount, layerPair);

	// Processing을 총 2번 한다.
	for(long idx = 0; idx < 2; idx++)
	{
		// idx 별로 mask 크기가 다르다.
		m_nMaskWidth = 0;
		m_nMaskHeight = 0;
		m_nMaskSize = 0;
		if(idx == 0)
		{
			m_nMaskWidth = removeNoiseParams.nMask1Width;
			m_nMaskHeight = removeNoiseParams.nMask1Height;
		}
		else
		{
			m_nMaskWidth = removeNoiseParams.nMask2Width;
			m_nMaskHeight = removeNoiseParams.nMask2Height;
		}
		m_nMaskSize = removeNoiseParams.nMask1Width * removeNoiseParams.nMask1Height;

		// idx 별로 판단 기준값이 다른다.
		float diffMax = 255.f * m_nMaskSize;
		m_fRefValue = 0.f;
		if(idx == 0)
		{
			m_fRefValue = diffMax * (1.f - removeNoiseParams.fRef1);
		}
		else
		{
			m_fRefValue = diffMax * (1.f - removeNoiseParams.fRef2);
		}

		// 계산을 진행한다.
		BeginThread(THREAD_METHOD::REMOVE_NOISE);
	}
}

void CDetectionAlgorithm::CalcRemoveNoise(const long startIndex, const long endIndex)
{
	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto removeNoiseParams = ::g_RecipeManager.m_sRemoveNoiseParams;

	BYTE* pbyProcImage = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_PROC); // Proc. Image
	BYTE* pbyThreshold = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_THRESHOLD); // Threshold Image
	BYTE* pbyMask = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_MASK); // 마스크 영상
	BYTE* pbySubResult = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_SUB); // 빼기 결과 영상(buffer)
	BYTE* pbyResult = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_RESULT); // Result Image

	long layerPair = roiParams.nLayerCount / 2;

	// ROI 별로 계산을 진행한다.
	for(long slot = startIndex; slot < endIndex; slot++)
	{
		for(long layer = 0; layer < layerPair; layer++)
		{
			if(m_vecbProcessing[slot][layer] == TRUE) // Processing 이 이미 되었으면 Skip 한다.
			{
				continue;
			}

			auto roi = m_vecrtHairpin[slot][layer];

			HUBIP::BinaryMaskConvolution(pbyProcImage, pbyThreshold, pbyMask, pbySubResult, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roi, m_nMaskWidth, m_nMaskHeight, m_fRefValue, 2, pbyResult, m_vecbProcessing[slot][layer]);
		}
	}
}

BOOL CDetectionAlgorithm::DoRemoveNoiseThresholdImage()
{
	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto removeNoiseParams = ::g_RecipeManager.m_sRemoveNoiseParams;

	long slotCount = roiParams.nSlotCount;
	long layerCount = roiParams.nLayerCount;
	long layerPair = layerCount / 2;

	BYTE* pbyProcImage = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_PROC); // Proc. Image
	BYTE* pbyThreshold = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_THRESHOLD); // Threshold Image
	BYTE* pbyHysteresisTemp = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_HYSTERESIS_TEMP); // Hysteresis Temp Image

	memcpy(pbyThreshold, pbyProcImage, m_nUnrolledImageSize);
	memset(pbyHysteresisTemp, 0, m_nUnrolledImageSize);

	// Threshold
	if(removeNoiseParams.nSelectThreshold == (long)THRESHOLD::NORMAL)
	{
		HUBIP::Binarization(pbyProcImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, m_rtHairpinWhole, (BYTE)removeNoiseParams.nThresholdValue, (bool)removeNoiseParams.bThresholdReverse, pbyThreshold);
	}
	else if(removeNoiseParams.nSelectThreshold == (long)THRESHOLD::DOUBLE)
	{
		m_pAlgorithm->DoubleThreshold(pbyProcImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, m_rtHairpinWhole, removeNoiseParams.nThresholdLow, removeNoiseParams.nThresholdHigh, removeNoiseParams.bThresholdReverse, pbyThreshold);
	}
	else if(removeNoiseParams.nSelectThreshold == (long)THRESHOLD::ADAPTIVE)
	{
#pragma omp parallel for
		for(long slot = 0; slot < slotCount; slot++)
		{
			for(long layer = 0; layer < layerPair; layer++)
			{
				HUBIP::BinarizeAdaptive(pbyProcImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, m_vecrtHairpin[slot][layer], CSize(100, 100), removeNoiseParams.nAdaptiveOffset, pbyThreshold);

				if(removeNoiseParams.bThresholdReverse == TRUE)
				{
					HUBIP::BitwiseNot(m_nUnrolledImageWidth, m_nUnrolledImageHeight, m_vecrtHairpin[slot][layer], pbyThreshold);
				}
			}
		}
	}
	else if(removeNoiseParams.nSelectThreshold == (long)THRESHOLD::HYSTERISIS)
	{
		HUBIP::BinarizeHysteresis(pbyProcImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, CRect(0, 0, m_nUnrolledImageWidth, m_nUnrolledImageHeight), (BYTE)removeNoiseParams.nHysteresisLow, (BYTE)removeNoiseParams.nHysteresisHigh, false, (bool)removeNoiseParams.bThresholdReverse, removeNoiseParams.nHighThresholdRegionNoiseFilteringSize, m_nUnrolledImageWidth, m_nUnrolledImageHeight, pbyHysteresisTemp, pbyThreshold);
	}

	return TRUE;
}

BOOL CDetectionAlgorithm::DoRemoveNoiseProcImage()
{
	auto &params = ::g_RecipeManager.m_sRemoveNoiseParams;
	auto &procParams = params.sProcParam;

	BYTE* pbyUnrolledImage = ::g_MemoryManager.GetByteImage((long)IMAGE::UNROLLED); // Unrolled Image
	BYTE* pbyProcImage = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_PROC); // Processing Image

	if((long)procParams.m_vecnProc.size() > 0 && (long)procParams.m_vecnProcParams.size() > 0)
	{
		HUBIP::ImageProcessingManage(pbyUnrolledImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, CRect(0, 0, m_nUnrolledImageWidth, m_nUnrolledImageHeight), &procParams.m_vecnProc[0], &procParams.m_vecnProcParams[0], (long)procParams.m_vecnProc.size(), pbyProcImage);
	}
	else
	{
		memcpy(pbyProcImage, pbyUnrolledImage, m_nUnrolledImageSize);
	}

	return TRUE;
}
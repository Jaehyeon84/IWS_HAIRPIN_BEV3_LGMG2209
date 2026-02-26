#include "pch.h"
#include "DetectionAlgorithm.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CDetectionAlgorithm::DoInterpolation()
{
	DoInterProcImage();
	DoInterThresholdImage();

	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto interHairpinParams = ::g_RecipeManager.m_sInterHairpinParams;

	long slotCount = roiParams.nSlotCount;
	long layerCount = roiParams.nLayerCount;
	long layerPair = layerCount / 2;

	BYTE* pbyProcImage = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_PROC); // Proc. Image
	BYTE* pbyThreshold = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_THRESHOLD); // Threshold Image
	long* pnLabelImage = ::g_MemoryManager.GetLongImage(); // Label Image
	memset(pnLabelImage, 0, m_nUnrolledImageSize * sizeof(long));

	auto &interBlobList = m_vecInterpolationBlobInfoList;

	// BlobInfo 초기화
	HUBSDK::BlobInfo value;
	value.Init();
	m_pAlgorithm->ResizeVector(interBlobList, interHairpinParams.nMaxBlobCount, value);

	// Temp BlobInfo 초기화
	std::vector<std::vector<std::vector<HUBSDK::BlobInfo>>> vecBlobInfoTemp;
	m_pAlgorithm->ResizeVector(vecBlobInfoTemp, roiParams.nSlotCount, layerPair, 100, value);

	// Blob
	m_nInterpolationBlobTotalCount = 0;
	long blob(0);
#pragma omp parallel for
	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerPair; layer++)
		{
			long nBlobCount(0);
			auto &tempBlobInfo = vecBlobInfoTemp[slot][layer];

			if(!m_pAlgorithm->Blob(pbyThreshold, m_nUnrolledImageWidth, m_nUnrolledImageHeight, m_vecrtHairpin[slot][layer], FALSE, interHairpinParams.nMinBlobSize, interHairpinParams.nMaxBlobCount, &tempBlobInfo[0], nBlobCount, pnLabelImage))
			{
				return FALSE;
			}

			// Blob 개수 별로 resize
			tempBlobInfo.resize(nBlobCount);

			// Blob Merge
			if(interHairpinParams.bBlobMerge == TRUE && nBlobCount > 1)
			{
				long nBlobMergeCount(0);
				if(!m_pAlgorithm->BlobMerge(m_nUnrolledImageWidth, m_nUnrolledImageHeight, pnLabelImage, nBlobCount, (float)interHairpinParams.nBlobMergeDist, &tempBlobInfo[0], nBlobMergeCount))
				{
					return FALSE;
				}

				nBlobCount = nBlobMergeCount;
			}

			for(long idx = 0; idx < nBlobCount; idx++)
			{
				interBlobList[blob] = tempBlobInfo[idx];
				blob++;
			}

			m_nInterpolationBlobTotalCount += nBlobCount;
		}
	}

	interBlobList.resize(m_nInterpolationBlobTotalCount);

	// Blob Info 계산
	double bgAverageValue(0.f); // Contrast 계산시 필요
	if(HUBIP::CalcBlobInfo(pbyProcImage, (BYTE)bgAverageValue, pnLabelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, m_nInterpolationBlobTotalCount, &m_vecInterpolationBlobInfoList[0]) != retStatus::success)
	{
		return FALSE;
	}

	// Threshold Image Interpolation
	if(interHairpinParams.bInterpolationY == TRUE)
	{
#pragma omp parallel for
		for(long blob = 0; blob < (long)interBlobList.size(); blob++)
		{
			HUBIP::FillBlobEmptyAreaY(pbyThreshold, m_nUnrolledImageWidth, m_nUnrolledImageHeight, pbyThreshold, pnLabelImage, m_vecInterpolationBlobInfoList[blob]);
		}
	}

	if(interHairpinParams.bInterpolationX == TRUE)
	{
#pragma omp parallel for
		for(long blob = 0; blob < (long)interBlobList.size(); blob++)
		{
			HUBIP::FillBlobEmptyAreaX(pbyThreshold, m_nUnrolledImageWidth, m_nUnrolledImageHeight, pbyThreshold, pnLabelImage, m_vecInterpolationBlobInfoList[blob]);
		}
	}

	return TRUE;
}

BOOL CDetectionAlgorithm::DoInterThresholdImage()
{
	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto interHairpinParams = ::g_RecipeManager.m_sInterHairpinParams;

	long slotCount = roiParams.nSlotCount;
	long layerCount = roiParams.nLayerCount;
	long layerPair = layerCount / 2;

	BYTE* pbyProcImage = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_PROC); // Proc. Image
	BYTE* pbyThreshold = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_THRESHOLD); // Threshold Image
	BYTE* pbyHysteresisTemp = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_HYSTERESIS_TEMP); // Hysteresis Temp Image

	memcpy(pbyThreshold, pbyProcImage, m_nUnrolledImageSize);
	memset(pbyHysteresisTemp, 0, m_nUnrolledImageSize);

	// Threshold
	if(interHairpinParams.nSelectThreshold == (long)THRESHOLD::NORMAL)
	{
		HUBIP::Binarization(pbyProcImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, m_rtHairpinWhole, (BYTE)interHairpinParams.nThresholdValue, (bool)interHairpinParams.bThresholdReverse, pbyThreshold);
	}
	else if(interHairpinParams.nSelectThreshold == (long)THRESHOLD::ADAPTIVE)
	{
#pragma omp parallel for
		for(long slot = 0; slot < slotCount; slot++)
		{
			for(long layer = 0; layer < layerPair; layer++)
			{
				HUBIP::BinarizeAdaptive(pbyProcImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, m_vecrtHairpin[slot][layer], CSize(100, 100), interHairpinParams.nAdaptiveOffset, pbyThreshold);

				if(interHairpinParams.bThresholdReverse == TRUE)
				{
					HUBIP::BitwiseNot(m_nUnrolledImageWidth, m_nUnrolledImageHeight, m_vecrtHairpin[slot][layer], pbyThreshold);
				}
			}
		}
	}
	else if(interHairpinParams.nSelectThreshold == (long)THRESHOLD::HYSTERISIS)
	{
		HUBIP::BinarizeHysteresis(pbyProcImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, CRect(0, 0, m_nUnrolledImageWidth, m_nUnrolledImageHeight), (BYTE)interHairpinParams.nHysteresisLow, (BYTE)interHairpinParams.nHysteresisHigh, false, (bool)interHairpinParams.bThresholdReverse, interHairpinParams.nHighThresholdRegionNoiseFilteringSize, m_nUnrolledImageWidth, m_nUnrolledImageHeight, pbyHysteresisTemp, pbyThreshold);
	}

	return TRUE;
}

BOOL CDetectionAlgorithm::DoInterProcImage()
{
	auto &params = ::g_RecipeManager.m_sInterHairpinParams;
	auto &procParams = params.sProcParam;

	BYTE* pbyRemoveNoiseResult = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_RESULT); // Remove Noise Result Image
	BYTE* pbyProcImage = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_PROC); // Processing Image

	if((long)procParams.m_vecnProc.size() > 0 && (long)procParams.m_vecnProcParams.size() > 0)
	{
		HUBIP::ImageProcessingManage(pbyRemoveNoiseResult, m_nUnrolledImageWidth, m_nUnrolledImageHeight, CRect(0, 0, m_nUnrolledImageWidth, m_nUnrolledImageHeight), &procParams.m_vecnProc[0], &procParams.m_vecnProcParams[0], (long)procParams.m_vecnProc.size(), pbyProcImage);
	}
	else
	{
		memcpy(pbyProcImage, pbyRemoveNoiseResult, m_nUnrolledImageSize);
	}

	return TRUE;
}


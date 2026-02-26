#include "pch.h"
#include "DetectionAlgorithm.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CDetectionAlgorithm::DoDetectBlob()
{
	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto detectBlobParams = ::g_RecipeManager.m_sDetectBlobParams;

	long slotCount = roiParams.nSlotCount;
	long layerCount = roiParams.nLayerCount;
	long layerPair = layerCount / 2;

	BYTE* pbyProcImage = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_PROC); // Proc. Image
	BYTE* pbyThreshold = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_THRESHOLD); // Interpolation Result Image
	long* pnLabelImage = ::g_MemoryManager.GetLongImage(); // Label Image
	memset(pnLabelImage, 0, m_nUnrolledImageSize * sizeof(long));

	// BlobInfo 초기화
	HUBSDK::BlobInfo value;
	value.Init();
	m_pAlgorithm->ResizeVector(m_vecDetectBlobBlobInfoList, roiParams.nSlotCount, layerPair, 100, value);

	// Blob
	m_nBlobTotalCount = 0;
	long nBlobCount(0);
	long nBlobMergeCount(0);
#pragma omp parallel for
	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerPair; layer++)
		{
			auto &blobInfo = m_vecDetectBlobBlobInfoList[slot][layer];

			if(!m_pAlgorithm->Blob(pbyThreshold, m_nUnrolledImageWidth, m_nUnrolledImageHeight, m_vecrtHairpin[slot][layer], FALSE, detectBlobParams.nMinBlobSize, detectBlobParams.nMaxBlobCount, &blobInfo[0], nBlobCount, pnLabelImage))
			{
				return FALSE;
			}

			// Blob Merge
			if(detectBlobParams.bBlobMerge && nBlobCount > 2)
			{
				if(!m_pAlgorithm->BlobMerge(m_nUnrolledImageWidth, m_nUnrolledImageHeight, pnLabelImage, nBlobCount, (float)detectBlobParams.nBlobMergeDist, &blobInfo[0], nBlobMergeCount))
				{
					return FALSE;
				}

				nBlobCount = nBlobMergeCount;
			}

			// Blob 개수 별로 resize
			blobInfo.resize(nBlobCount);

			m_nBlobTotalCount += nBlobCount;
		}
	}

	// Blob Info 계산
#pragma omp parallel for
	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerPair; layer++)
		{
			if((long)m_vecDetectBlobBlobInfoList[slot][layer].size() == 0)
			{
				continue;
			}

			auto &blobInfo = m_vecDetectBlobBlobInfoList[slot][layer];
			auto blobNum = (long)m_vecDetectBlobBlobInfoList[slot][layer].size();

			double bgAverageValue(0.f); // Contrast 계산시 필요
			if(HUBIP::CalcBlobInfo(pbyProcImage, (BYTE)bgAverageValue, pnLabelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, blobNum, &blobInfo[0]) != retStatus::success)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

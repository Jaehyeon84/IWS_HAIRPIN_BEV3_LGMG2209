#include "pch.h"
#include "DetectionAlgorithm.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CDetectionAlgorithm::DoDataExchange()
{
	CString strTemp;

	auto params = ::g_RecipeManager.m_sROIParams;
	auto unrolledParams = ::g_RecipeManager.m_sUnrolledParams;

	auto slotCount = params.nSlotCount;
	auto layerCount = params.nLayerCount;
	auto layerPair = layerCount / 2;

	BYTE* pbyImage = ::g_MemoryManager.GetByteImage((long)IMAGE::SUB_ORIG);
	CPoint ptCenter = CPoint(m_nSubOriginalImageWidth / 2, m_nSubOriginalImageHeight / 2);

	m_pAlgorithm->ResizeVector(m_vecptResult, slotCount, layerCount);
	m_pAlgorithm->ResizeVector(m_vecptCriteriaGravityCenter_forTheta, slotCount, layerCount);

#pragma omp parallel for
	for (long slot = 0; slot < slotCount; slot++)
	{
		for (long layer1 = 0, layer2 = layerCount - 1; layer1 < layerCount; layer1++, layer2--)
		{
			m_vecptCriteriaGravityCenter_forTheta[slot][layer1] = m_vecptCriteriaGravityCenter[slot][layer2];
		}
	}
	

	// Unrolled Image 의 헤어핀 중심 좌표를 rolled Image 의 좌표로 변환
#pragma omp parallel for
	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerCount; layer++)
		{
			auto pt = m_vecptCriteriaGravityCenter[slot][layer];

			if(pt == CPoint(0, 0))
			{
				continue;
			}

			long sum = pt.y * unrolledParams.nUnrolledLength;

			long xIndex(0), yIndex(0);
			BOOL bFind(FALSE);
			for(long idx = 0; idx < unrolledParams.nUnrolledLength; idx++)
			{
				if(m_pptUnrolledPos[sum + idx].x == pt.x)
				{
					xIndex = idx;
					yIndex = pt.y;

					bFind = TRUE;
					break;
				}
			}

			// 만약에 일치하는 포인트를 못찾으면 가장 가까운 포인트로...
			if(bFind == FALSE)
			{
				long minDiff(LONG_MAX);
				for(long idx = 0; idx < unrolledParams.nUnrolledLength; idx++)
				{
					auto unrollX = m_pptUnrolledPos[sum + idx].x;
					long diff = abs(unrollX - pt.x);

					if(minDiff > diff)
					{
						minDiff = diff;
						xIndex = idx;
						yIndex = pt.y;
					}
				}
			}

			m_vecptResult[slot][layer] = m_pptOriginalPos[yIndex * unrolledParams.nUnrolledLength + xIndex];

			if(unrolledParams.fUnrolledStartAngle != 0)
			{
				double angle = (double)(unrolledParams.fUnrolledStartAngle) * ITP_DEG_TO_RAD;
				HUBSDK::Geometry::RotatePoint(m_vecptResult[slot][layer], (float)angle, ptCenter);
			}
		}
	}

	// Layer 가 1~8이 아닌 8~1 순서로 되어 있기 때문에 Layer Index를 거꾸로 바꾼다.
	// IWS 에서 사용할때만 1~8 로 사용하기 때문에 IWS에서 사용할 변수에 거꾸로 저장한다.
	std::vector<std::vector<CPoint>> vecptResult_real; // IWS 에서 사용할 좌표
	m_pAlgorithm->ResizeVector(vecptResult_real, slotCount, params.nLayerCount);

#pragma omp parallel for
	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer1 = 0, layer2 = layerCount - 1; layer1 < layerCount; layer1++, layer2--)
		{
			vecptResult_real[slot][layer1] = m_vecptResult[slot][layer2];
		}
	}

	// 영상좌표를 캐드좌표로 변환 한다.
	float resolX_mm = m_fResolX / 1000.f;
	float resolY_mm = m_fResolY / 1000.f;
	float subOriginalImageHalfWidth_mm = ((float)m_nSubOriginalImageWidth * resolX_mm) / 2.f;
	float subOriginalImageHalfHeight_mm = ((float)m_nSubOriginalImageHeight * resolY_mm) / 2.f;

	m_pAlgorithm->ResizeVector(m_vecfResultX_cad, slotCount, layerCount);
	m_pAlgorithm->ResizeVector(m_vecfResultY_cad, slotCount, layerCount);

#pragma omp parallel for
	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerCount; layer++)
		{
			auto point = vecptResult_real[slot][layer];
			auto &newPointX = m_vecfResultX_cad[slot][layer];
			auto &newPointY = m_vecfResultY_cad[slot][layer];

			if(point == CPoint(0, 0))
			{
				newPointX = 0.f;
				newPointY = 0.f;
			}
			else
			{
				newPointX = (((float)point.x + (float)unrolledParams.ptCenterPosCorrection.x) * resolX_mm) - subOriginalImageHalfWidth_mm + m_fimageCenterXAfterCalibration;
				newPointY = -((((float)point.y + (float)unrolledParams.ptCenterPosCorrection.y) * resolY_mm) - subOriginalImageHalfHeight_mm) + m_fimageCenterYAfterCalibration;
			}
		}
	}


	// dhpark 2023.10.31 - HairpinCenter 창에서 Inspection 클릭시 2D좌표로 파란점 찍는 버그 개선을 위함
	m_pAlgorithm->ResizeVector(m_vecbReplace3DPosition, slotCount, layerCount, FALSE);



	// 좌표계산만 하려다가 실패한 코드
	//for(long blob = 0; blob < m_nBlobFinalCount; blob++)
	//{
	//	if(blob >= 1)
	//		continue;

	//	auto info = m_vecCriteriaBlobInfoList[blob];

	//	CPoint pt;
	//	pt.x = info.m_roi.CenterPoint().x - unrolledImageWidth / 2;
	//	pt.y = info.m_roi.CenterPoint().y;
	//	
	//	float angle = -((float)pt.y * UNROLLED_ANGLE + 90.f) * ITP_DEG_TO_RAD;

	//	float r0 = ((float)originalImageHeight / 2.f) - 1400.f;
	//	CPoint ref = CPoint(originalImageWidth / 2, 400 /*+ r0*/); // 기준점

	//	long x = (long)(((float)pt.x + r0) * cos(angle) + (float)ptCenter.x);
	//	long y = (long)(((float)pt.x + r0) * sin(angle) + (float)ptCenter.y);

	//	m_vecptResult.push_back(CPoint(x,y));
	//}

	return TRUE;
}

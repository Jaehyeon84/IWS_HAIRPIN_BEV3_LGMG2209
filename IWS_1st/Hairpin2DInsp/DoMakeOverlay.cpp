#include "pch.h"
#include "DetectionAlgorithm.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CDetectionAlgorithm::DoMakeOverlay()
{
	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto params = ::g_RecipeManager.m_sUnrolledParams;

	auto unrolledImageHeight = m_nUnrolledImageHeight;
	auto slotCount = roiParams.nSlotCount;
	auto layerCount = roiParams.nLayerCount;
	auto layerPair = layerCount / 2;
	auto slot2slotDist = UNROLLED_IMAGE_HEIGHT / slotCount;
	auto slot2slotHalfDist = slot2slotDist / 2;

	// 영상에서 슬롯별 줄긋기, 슬롯번호 출력
	CPoint ptStart = CPoint(m_nSubOriginalImageWidth / 2, params.nUnrolledStartOffsetY);
	CPoint ptEnd = CPoint(m_nSubOriginalImageWidth / 2, params.nUnrolledStartOffsetY + params.nUnrolledLength);
	CPoint ptSlotNo1 = CPoint(m_nSubOriginalImageWidth / 2, params.nUnrolledStartOffsetY);
	CPoint ptSlotNo2 = CPoint(m_nSubOriginalImageWidth / 2, params.nUnrolledStartOffsetY + params.nUnrolledLength);
	CPoint ptCenter = CPoint(m_nSubOriginalImageWidth / 2, m_nSubOriginalImageHeight / 2);

	double angle = (double)(params.fUnrolledStartAngle) * ITP_DEG_TO_RAD;
	HUBSDK::Geometry::RotatePoint(ptStart, (float)angle, ptCenter);
	HUBSDK::Geometry::RotatePoint(ptEnd, (float)angle, ptCenter);
	HUBSDK::Geometry::RotatePoint(ptSlotNo1, (float)angle, ptCenter);
	HUBSDK::Geometry::RotatePoint(ptSlotNo2, (float)angle, ptCenter);

	m_vecptSlotLineOverlayStartPoint.clear();
	m_vecptSlotLineOverlayEndPoint.clear();
	m_vecptSlotNo1OverlayPoint.clear();
	m_vecptSlotNo2OverlayPoint.clear();
	m_vecptLayerPairOverlayPoint.clear();

	long idx2(slot2slotHalfDist);
	for(long idx1 = 0; idx1 < unrolledImageHeight; idx1 += slot2slotDist)
	{
		double angle1 = -(double)idx1 * UNROLLED_ANGLE * ITP_DEG_TO_RAD;
		double angle2 = -(double)idx2 * UNROLLED_ANGLE * ITP_DEG_TO_RAD;

		CPoint ptStartDst(ptStart);
		HUBSDK::Geometry::RotatePoint(ptStartDst, (float)angle1, ptCenter);

		CPoint ptEndDst(ptEnd);
		HUBSDK::Geometry::RotatePoint(ptEndDst, (float)angle1, ptCenter);

		CPoint ptSlotNo1Dst(ptSlotNo1);
		HUBSDK::Geometry::RotatePoint(ptSlotNo1Dst, (float)angle2, ptCenter);

		CPoint ptSlotNo2Dst(ptSlotNo2);
		HUBSDK::Geometry::RotatePoint(ptSlotNo2Dst, (float)angle2, ptCenter);

		m_vecptSlotLineOverlayStartPoint.push_back(ptStartDst);
		m_vecptSlotLineOverlayEndPoint.push_back(ptEndDst);
		m_vecptSlotNo1OverlayPoint.push_back(ptSlotNo1Dst);
		m_vecptSlotNo2OverlayPoint.push_back(ptSlotNo2Dst);

		idx2 += slot2slotDist;
	}

	// 각 layerPair 별 위치 표시
	long areaWidth = 210;
	CPoint ptLayerPair0 = CPoint(ptEnd.x, ptEnd.y - 80);
	CPoint ptLayerPair1 = CPoint(ptEnd.x, ptEnd.y - 80 - areaWidth);
	CPoint ptLayerPair2 = CPoint(ptEnd.x, ptEnd.y - 80 - areaWidth * 2);
	CPoint ptLayerPair3 = CPoint(ptEnd.x, ptEnd.y - 80 - areaWidth * 3);
	CPoint ptLayerPair4 = CPoint(ptEnd.x, ptEnd.y - 80 - areaWidth * 4);

	m_pAlgorithm->ResizeVector(m_vecptLayerPairOverlayPoint, slotCount, layerCount);

	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerPair; layer++)
		{
			auto pt = m_vecrtHairpin[slot][layer].CenterPoint();

			if(pt == CPoint(0, 0))
			{
				continue;
			}

			long sum = pt.y * params.nUnrolledLength;

			long xIndex(0), yIndex(0);
			BOOL bFind(FALSE);
			for(long idx = 0; idx < params.nUnrolledLength; idx++)
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
				for(long idx = 0; idx < params.nUnrolledLength; idx++)
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

			m_vecptLayerPairOverlayPoint[slot][layer] = m_pptOriginalPos[yIndex * params.nUnrolledLength + xIndex];

			if(params.fUnrolledStartAngle != 0)
			{
				double angle = (double)(params.fUnrolledStartAngle) * ITP_DEG_TO_RAD;
				HUBSDK::Geometry::RotatePoint(m_vecptLayerPairOverlayPoint[slot][layer], (float)angle, ptCenter);
			}
		}
	}

	return TRUE;
}

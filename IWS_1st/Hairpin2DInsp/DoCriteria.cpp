#include "pch.h"
#include "DetectionAlgorithm.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CDetectionAlgorithm::DoCriteria()
{
	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto unrolledParams = ::g_RecipeManager.m_sUnrolledParams;
	auto hairpinParams = ::g_RecipeManager.m_sHairpinCenterParams;
	auto criteriaParams = ::g_RecipeManager.m_sCriteriaParams;
	auto publicParams = ::g_RecipeManager.m_sPublicParams;

	long slotCount = roiParams.nSlotCount;
	long layerCount = roiParams.nLayerCount;
	long layerPair = layerCount / 2;
	auto slot2slotDist = UNROLLED_IMAGE_HEIGHT / slotCount;
	auto slot2slotHalfDist = slot2slotDist / 2;

	m_nOKCount = 0;
	m_nNGCount = 0;

	SResult value;
	value.Init();
	m_pAlgorithm->ResizeVector(m_vecsCriteriaResult, slotCount, layerPair, value);
	m_pAlgorithm->ResizeVector(m_vecbReplace3DPosition, slotCount, layerCount, FALSE);

	// 못찾은 핀에 대해 IWI 결과로 덮어쓰기 한다.(단, 해당 좌표의 IWI 결과가 OK 일경우)
	if(publicParams.bReplace3DResult == TRUE)
	{
		if(VECTOR_SIZE(m_pnIWIResult) > 0 && VECTOR_SIZE(m_vecfIWIX_afterMatrix) > 0 && VECTOR_SIZE(m_vecfIWIY_afterMatrix) > 0)
		{
			// 캐드좌표를 영상좌표로 변환하기 위한 변수
			float resolX_mm = m_fResolX / 1000.f;
			float resolY_mm = m_fResolY / 1000.f;
			float subOriginalImageHalfWidth_mm = ((float)m_nSubOriginalImageWidth * resolX_mm) / 2.f;
			float subOriginalImageHalfHeight_mm = ((float)m_nSubOriginalImageHeight * resolY_mm) / 2.f;

			float calX = m_fimageCenterXAfterCalibration;
			float calY = m_fimageCenterYAfterCalibration;
			float corPosX = unrolledParams.ptCenterPosCorrection.x;
			float corPosY = unrolledParams.ptCenterPosCorrection.y;

			float fPositionLimitX = hairpinParams.f2D3DPositionLimitX;
			float fPositionLimitY = hairpinParams.f2D3DPositionLimitY;

#pragma omp parallel for
			for(long slot = 0; slot < slotCount; slot++)
			{
				long index1(0), index2(1);
				long index_rev1 = layerCount - 1, index_rev2 = layerCount - 2;
				for(long layer = 0; layer < layerPair; layer++)
				{
					auto &center1X = m_vecfResultX_cad[slot][index1];
					auto &center1Y = m_vecfResultY_cad[slot][index1];
					auto &center2X = m_vecfResultX_cad[slot][index2];
					auto &center2Y = m_vecfResultY_cad[slot][index2];

					auto iwiResult = m_pnIWIResult[slot][layer];
					auto center1X_IWI = m_vecfIWIX_afterMatrix[slot][index1];
					auto center1Y_IWI = m_vecfIWIY_afterMatrix[slot][index1];
					auto center2X_IWI = m_vecfIWIX_afterMatrix[slot][index2];
					auto center2Y_IWI = m_vecfIWIY_afterMatrix[slot][index2];

					
					// 2D에서 찾은 좌표와 3D 좌표를 비교하여 Position Limit 값 이상으로 차이나면 3D 좌표를 사용한다.
					if (iwiResult == 1)
					{
						if (center1X != 0.f || center1Y != 0.f)
						{
							float fsub1X = fabs(center1X - center1X_IWI);
							float fsub1Y = fabs(center1Y - center1Y_IWI);

							if ((fPositionLimitX < fsub1X) || (fPositionLimitY < fsub1Y))
							{
								center1X = center1X_IWI;
								center1Y = center1Y_IWI;

								m_vecbReplace3DPosition[slot][index_rev1] = TRUE;

								float pointX = (center1X + subOriginalImageHalfWidth_mm - calX) / resolX_mm - corPosX;
								float pointY = (-(center1Y - calY) + subOriginalImageHalfHeight_mm) / resolY_mm - corPosY;


								auto pt = CPoint((long)pointX, (long)pointY);

								m_vecptResult[slot][index_rev1] = pt;

								long xIndex(0), yIndex(0);
								GetClosedPosition(pt, slot * slot2slotDist, (slot + 1) * slot2slotDist, m_pptOriginalPos, xIndex, yIndex);

								m_vecptCriteriaGravityCenter_forTheta[slot][index1] = m_pptUnrolledPos[yIndex * unrolledParams.nUnrolledLength + xIndex];
							}
						}
						
						if (center2X != 0.f || center2Y != 0.f)
						{
							float fsub2X = fabs(center2X - center2X_IWI);
							float fsub2Y = fabs(center2Y - center2Y_IWI);

							if ((fPositionLimitX < fsub2X) || (fPositionLimitY < fsub2Y))
							{
								center2X = center2X_IWI;
								center2Y = center2Y_IWI;

								m_vecbReplace3DPosition[slot][index_rev2] = TRUE;

								float pointX = (center2X + subOriginalImageHalfWidth_mm - calX) / resolX_mm - corPosX;
								float pointY = (-(center2Y - calY) + subOriginalImageHalfHeight_mm) / resolY_mm - corPosY;


								auto pt = CPoint((long)pointX, (long)pointY);

								m_vecptResult[slot][index_rev2] = pt;

								long xIndex(0), yIndex(0);
								GetClosedPosition(pt, slot * slot2slotDist, (slot + 1) * slot2slotDist, m_pptOriginalPos, xIndex, yIndex);

								m_vecptCriteriaGravityCenter_forTheta[slot][index2] = m_pptUnrolledPos[yIndex * unrolledParams.nUnrolledLength + xIndex];
							}
						}	
					}
						

					// 헤어핀이 없을 때(2개 다 없거나 1개만 없거나) IWI 결과로 덮어쓰기 한다.
					if(center1X == 0.f || center1Y == 0.f)
					{
						if(iwiResult == 1)
						{
							center1X = center1X_IWI;
							center1Y = center1Y_IWI;

							m_vecbReplace3DPosition[slot][index_rev1] = TRUE;


							//float pointX = (center1X + subOriginalImageHalfWidth_mm - calX) / resolX_mm + corPosX;
							//float pointY = (-center1Y + subOriginalImageHalfHeight_mm - calY) / resolY_mm + corPosY;

							float pointX = (center1X + subOriginalImageHalfWidth_mm - calX) / resolX_mm - corPosX;
							float pointY = (-(center1Y - calY)+ subOriginalImageHalfHeight_mm) / resolY_mm - corPosY;


							auto pt = CPoint((long)pointX, (long)pointY);

							m_vecptResult[slot][index_rev1] = pt;

							long xIndex(0), yIndex(0);
							GetClosedPosition(pt, slot * slot2slotDist, (slot + 1) * slot2slotDist, m_pptOriginalPos, xIndex, yIndex);

							m_vecptCriteriaGravityCenter_forTheta[slot][index1] = m_pptUnrolledPos[yIndex * unrolledParams.nUnrolledLength + xIndex];
						}
					}

					if(center2X == 0.f || center2Y == 0.f)
					{
						if(iwiResult == 1)
						{
							center2X = center2X_IWI;
							center2Y = center2Y_IWI;

							m_vecbReplace3DPosition[slot][index_rev2] = TRUE;

							//float pointX = (center2X + subOriginalImageHalfWidth_mm - calX) / resolX_mm + corPosX;
							//float pointY = (-center2Y + subOriginalImageHalfHeight_mm - calY) / resolY_mm + corPosY;

							float pointX = (center2X + subOriginalImageHalfWidth_mm - calX) / resolX_mm - corPosX;
							float pointY = (-(center2Y - calY) + subOriginalImageHalfHeight_mm) / resolY_mm - corPosY;


							auto pt = CPoint((long)pointX, (long)pointY);

							m_vecptResult[slot][index_rev2] = pt;

							long xIndex(0), yIndex(0);
							GetClosedPosition(pt, slot * slot2slotDist, (slot + 1) * slot2slotDist, m_pptOriginalPos, xIndex, yIndex);

							m_vecptCriteriaGravityCenter_forTheta[slot][index2] = m_pptUnrolledPos[yIndex * unrolledParams.nUnrolledLength + xIndex];
						}
					}

					index1 += 2;
					index2 += 2;

					index_rev1 -= 2;
					index_rev2 -= 2;
				}
			}
		}
	}

#pragma omp parallel for
	for(long slot = 0; slot < slotCount; slot++)
	{
		long index1(0), index2(1);
		for(long layer = 0; layer < layerPair; layer++)
		{
			auto &result = m_vecsCriteriaResult[slot][layer];
			auto center1X = m_vecfResultX_cad[slot][index1];
			auto center1Y = m_vecfResultY_cad[slot][index1];
			auto center2X = m_vecfResultX_cad[slot][index2];
			auto center2Y = m_vecfResultY_cad[slot][index2];

			auto center1_t = m_vecptCriteriaGravityCenter_forTheta[slot][index1];
			auto center2_t = m_vecptCriteriaGravityCenter_forTheta[slot][index2];

			// 헤어핀이 없을 때(2개 다 없거나 1개만 없거나)
			if((center1X == 0.f || center1Y == 0.f) && (center2X == 0.f || center2Y == 0.f))
			{
				result.nErrorCode = INSP_ERR_F12;
			}
			else if(center1X == 0.f || center1Y == 0.f)
			{
				result.nErrorCode = INSP_ERR_F1;
			}
			else if(center2X == 0.f || center2Y == 0.f)
			{
				result.nErrorCode = INSP_ERR_F2;
			}
			else // 헤어핀이 2개 다 있을 경우
			{
				// C 계산
				float dist(0.f);
				HUBSDK::Geometry::GetDistance_PointToPoint(center1X, center1Y, center2X, center2Y, dist);
				result.fC = dist;

				// C 판단
				if(result.fC < criteriaParams.fMin_C || result.fC > criteriaParams.fMax_C)
				{
					result.nErrorCode = INSP_ERR_C;
				}

				// T 계산
				float theta = (float)fabs(center1_t.y - center2_t.y) * UNROLLED_ANGLE;
				result.fT = theta;

				// T 판단
				if(result.fT < criteriaParams.fMin_T || result.fT > criteriaParams.fMax_T)
				{
					result.nErrorCode = INSP_ERR_T;
				}
			}

			index1 += 2;
			index2 += 2;
		}
	}

	// Calc Jump
	float J = 0.0f;
#pragma omp parallel for
	for(long slot = 0; slot < slotCount; slot++)
	{
		long index1(1), index2(2);
		for(long layer = 0; layer < layerPair - 1; layer++)
		{
			auto &result1 = m_vecsCriteriaResult[slot][layer];
			auto &result2 = m_vecsCriteriaResult[slot][layer + 1];

			//if(result1.nErrorCode == INSP_ERR_F2 || result1.nErrorCode == INSP_ERR_F1)
			//{
			//	if(result1.nErrorCode != INSP_ERR_F1 && result1.nErrorCode != INSP_ERR_F2 && result1.nErrorCode != INSP_ERR_F12)
			//	{
			//		result1.nErrorCode = INSP_ERR_J;
			//	}

			//	continue;
			//}

			auto center1X = m_vecfResultX_cad[slot][index1];
			auto center1Y = m_vecfResultY_cad[slot][index1];
			auto center2X = m_vecfResultX_cad[slot][index2];
			auto center2Y = m_vecfResultY_cad[slot][index2];

			if(center1X == 0.f || center1Y == 0.f || center2X == 0.f || center2Y == 0.f)
			{
				continue;
			}

			// J 계산
			float dist(0.f);
			HUBSDK::Geometry::GetDistance_PointToPoint(center1X, center1Y, center2X, center2Y, dist);
			result1.fJ = dist;

			// J 판단
			if(result1.fJ < criteriaParams.fMin_J || result1.fJ > criteriaParams.fMax_J)
			{
				result1.nErrorCode = INSP_ERR_J;
			}

			index1 += 2;
			index2 += 2;
		}
	}

	// 3D 결과가 높이 불량일 경우 2D 결과를 무시한다.
	if(VECTOR_SIZE(m_vecfIWIH1) > 0 && VECTOR_SIZE(m_vecfIWIH2) > 0 && VECTOR_SIZE(m_vecfIWIH3) > 0)
	{
#pragma omp parallel for
		for(long slot = 0; slot < slotCount; slot++)
		{
			for(long layer = 0; layer < layerPair; layer++)
			{
				auto &result = m_vecsCriteriaResult[slot][layer];
				auto H1 = m_vecfIWIH1[slot][layer];
				auto H2 = m_vecfIWIH2[slot][layer];
				auto H3 = m_vecfIWIH3[slot][layer];

				result.fH1 = H1;
				result.fH2 = H2;
				result.fH3 = H3;

				if(criteriaParams.fMin_H12 > H1 || criteriaParams.fMax_H12 < H1)
				{
					result.nErrorCode = INSP_ERR_H1;
				}

				if(criteriaParams.fMin_H12 > H2 || criteriaParams.fMax_H12 < H2)
				{
					result.nErrorCode = INSP_ERR_H2;
				}

				if(criteriaParams.fMax_H3 < H3)
				{
					result.nErrorCode = INSP_ERR_H3;
				}
			}
		}
	}

	// Total Result 및 NG Count 를 계산한다.
	BOOL bNG(FALSE);
#pragma omp parallel for
	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerPair; layer++)
		{
			auto result = m_vecsCriteriaResult[slot][layer].nErrorCode;

			if(result == INSP_ERR_OK)
			{
				m_nOKCount++;
			}
			else
			{
				bNG = TRUE;
				m_nNGCount++;
			}
		}
	}

	if(bNG == TRUE)
	{
		m_strTotalResult = _T("NG");
	}
	else
	{
		m_strTotalResult = _T("OK");
	}

	return TRUE;
}

void CDetectionAlgorithm::GetClosedPosition(const CPoint src, const long yStart, const long yEnd, const CPoint* pPoints, long &xIndex, long &yIndex)
{
	auto params = ::g_RecipeManager.m_sUnrolledParams;

	CRect roi(src.x - 2, src.y - 2, src.x + 2, src.y + 2);
	
	long ptInX(0), ptInY(0);
	BOOL bFind(FALSE);
	for(long y = yStart; y < yEnd; y++)
	{
		long sum = y * params.nUnrolledLength;
		for(long x = 0; x < params.nUnrolledLength; x++)
		{
			if(roi.PtInRect(pPoints[sum + x]) == TRUE)
			{
				ptInX = x;
				ptInY = y;
				bFind = TRUE;
				break;
			}
		}

		if(bFind == TRUE)
		{
			break;
		}
	}

	if(bFind == FALSE)
	{
		return;
	}

	long minDiffX(LONG_MAX), minDiffY(LONG_MAX);
	for(long y = ptInY - 5; y < ptInY + 5; y++)
	{
		long sum = y * params.nUnrolledLength;
		for(long x = ptInX - 5; x < ptInX + 5; x++)
		{
			auto origX = pPoints[sum + x].x;
			auto origY = pPoints[sum + x].y;
			long diffX = abs(origX - src.x);
			long diffY = abs(origY - src.y);

			if(minDiffX > diffX)
			{
				minDiffX = diffX;
				xIndex = x;
			}

			if(minDiffY > diffY)
			{
				minDiffY = diffY;
				yIndex = y;
			}
		}
	}
}
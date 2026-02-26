#include "pch.h"
#include "DetectionAlgorithm.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CDetectionAlgorithm::DoCalcHairpinCenter()
{
	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto hairpinParams = ::g_RecipeManager.m_sHairpinCenterParams;

	long slotCount = roiParams.nSlotCount;
	long layerCount = roiParams.nLayerCount;
	long layerPair = layerCount / 2;

	BYTE* pbyThreshold = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_THRESHOLD); // Interpolation Threshold Image
	long* pnLabelImage = ::g_MemoryManager.GetLongImage(); // Label Image

	auto &detectBlobList = m_vecDetectBlobBlobInfoList;
	auto &gravityCenter = m_vecptCriteriaGravityCenter;
	auto &centerAlgorithm = m_vecstrCenterAlgorithm;
	auto &hairpinWidth = m_vecnHairpinWidth;
	auto &hairpinHeight = m_vecnHairpinHeight;

	m_pAlgorithm->ResizeVector(gravityCenter, slotCount, layerCount);
	m_pAlgorithm->ResizeVector(centerAlgorithm, slotCount, layerCount);
	m_pAlgorithm->ResizeVector(hairpinWidth, slotCount, layerCount);
	m_pAlgorithm->ResizeVector(hairpinHeight, slotCount, layerCount);

	for(long slot = 0; slot < slotCount; slot++)
	{
		long index1(0), index2(1);
		for(long layer = 0; layer < layerPair; layer++)
		{
			auto &blobInfo = detectBlobList[slot][layer];
			auto blobNum = blobInfo.size();

			if(blobNum > 2) // 2개 이상일 경우
			{
				centerAlgorithm[slot][index1] += _T("Blob>2 / ");
				centerAlgorithm[slot][index2] += _T("Blob>2 / ");

				// area 내림차순으로 정렬
				for(long i = 0; i < blobNum - 1; i++)
				{
					for(long j = 0; j < blobNum - 1 - i; j++)
					{
						if(blobInfo[j].m_area < blobInfo[j + 1].m_area)
						{
							HUBSDK::BlobInfo temp = blobInfo[j];
							blobInfo[j] = blobInfo[j + 1];
							blobInfo[j + 1] = temp;
						}
					}
				}

				auto info1 = blobInfo[0];
				auto info2 = blobInfo[1];

				hairpinWidth[slot][index1] = info1.m_roi.Width();
				hairpinHeight[slot][index1] = info1.m_roi.Height();
				hairpinWidth[slot][index2] = info2.m_roi.Width();
				hairpinHeight[slot][index2] = info2.m_roi.Height();

				// 1번 blob이 blobCenter 기준 왼쪽에 있을때 TRUE, 아니면 FALSE
				BOOL bLeft = (info1.m_roi.right < info2.m_roi.left) ? TRUE : FALSE;

				BOOL bOneROI(FALSE); // 이게 TRUE 이면 무조건 ROI 를 1개로 합쳐서 계산한다.
				if(bLeft == TRUE)
				{
					bOneROI = (info1.m_roi.right < info2.m_roi.left) ? FALSE : TRUE;
				}
				else
				{
					bOneROI = (info2.m_roi.right < info1.m_roi.left) ? FALSE : TRUE;
				}

				CRect rtTemp = info1.m_roi & info2.m_roi;
				auto mergedRoi = info1.m_roi | info2.m_roi;

				if(rtTemp.Width() == 0 && rtTemp.Height() == 0 && bOneROI == FALSE) // ROI 2개가 서로 겹치지 않을 경우
				{
					centerAlgorithm[slot][index1] += _T("NoOverlapROI / ");
					centerAlgorithm[slot][index2] += _T("NoOverlapROI / ");

					float fCenX(0.f), fCenY(0.f);
					CRect roi_temp1, roi_temp2;
					if(bLeft == TRUE)
					{
						roi_temp1 = info1.m_roi;
						roi_temp2 = info2.m_roi;
					}
					else
					{
						roi_temp1 = info2.m_roi;
						roi_temp2 = info1.m_roi;
					}

					HUBIP::GetProfileCenter(pbyThreshold, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roi_temp1, fCenX, fCenY);
					gravityCenter[slot][index1] = CPoint((long)fCenX, (long)fCenY);

					HUBIP::GetProfileCenter(pbyThreshold, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roi_temp2, fCenX, fCenY);
					gravityCenter[slot][index2] = CPoint((long)fCenX, (long)fCenY);

					// 커팅날로 인해 윗부분이 검게 보이는 부분 보상하는 부분
					InterpolationHairpinPoints5(pbyThreshold, pnLabelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, mergedRoi, slot, index1, index2, roi_temp1, roi_temp2, gravityCenter[slot][index1], gravityCenter[slot][index2]);
				}
				else // ROI 2개가 조금이라도 겹칠 경우 ROI를 1개로 합친 후 계산한다.
				{
					centerAlgorithm[slot][index1] += _T("OverlapROI / ");
					centerAlgorithm[slot][index2] += _T("OverlapROI / ");

					CalcHairpinCenter(pbyThreshold, pnLabelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, mergedRoi, slot, index1, index2, hairpinParams.nWidthSize_2MoreBlobMinSize, info1.m_label, info2.m_label, bLeft, FALSE, gravityCenter[slot][index1], gravityCenter[slot][index2]);
				}
			}
			else if(blobNum == 2) // 정상적으로 2개로 찾았을 경우
			{
				centerAlgorithm[slot][index1] += _T("Blob2 / ");
				centerAlgorithm[slot][index2] += _T("Blob2 / ");

				auto info1 = blobInfo[0];
				auto info2 = blobInfo[1];

				hairpinWidth[slot][index1] = info1.m_roi.Width();
				hairpinHeight[slot][index1] = info1.m_roi.Height();
				hairpinWidth[slot][index2] = info2.m_roi.Width();
				hairpinHeight[slot][index2] = info2.m_roi.Height();

				// 1번 blob right 가 2번 blob left 보다 작으면 확실히 1번 blob이 왼쪽에 있다
				BOOL bLeft = (info1.m_roi.right < info2.m_roi.left) ? TRUE : FALSE;

				BOOL bOneROI(FALSE); // 이게 TRUE 이면 무조건 ROI 를 1개로 합쳐서 계산한다.
				if(bLeft == TRUE)
				{
					bOneROI = (info1.m_roi.right < info2.m_roi.left) ? FALSE : TRUE;
				}
				else
				{
					bOneROI = (info2.m_roi.right < info1.m_roi.left) ? FALSE : TRUE;
				}

				CRect rtTemp = info1.m_roi & info2.m_roi;
				auto mergedRoi = info1.m_roi | info2.m_roi;

				if(rtTemp.Width() == 0 && rtTemp.Height() == 0 && bOneROI == FALSE) // ROI 2개가 서로 겹치지 않을 경우
				{
					centerAlgorithm[slot][index1] += _T("NoOverlapROI / ");
					centerAlgorithm[slot][index2] += _T("NoOverlapROI / ");

					float fCenX(0.f), fCenY(0.f);
					CRect roi_temp1, roi_temp2;
					if(bLeft == TRUE)
					{
						roi_temp1 = info1.m_roi;
						roi_temp2 = info2.m_roi;
					}
					else
					{
						roi_temp1 = info2.m_roi;
						roi_temp2 = info1.m_roi;
					}

					HUBIP::GetProfileCenter(pbyThreshold, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roi_temp1, fCenX, fCenY);
					gravityCenter[slot][index1] = CPoint((long)fCenX, (long)fCenY);

					HUBIP::GetProfileCenter(pbyThreshold, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roi_temp2, fCenX, fCenY);
					gravityCenter[slot][index2] = CPoint((long)fCenX, (long)fCenY);

					// 커팅날로 인해 윗부분이 검게 보이는 부분 보상하는 부분
					InterpolationHairpinPoints5(pbyThreshold, pnLabelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, mergedRoi, slot, index1, index2, roi_temp1, roi_temp2, gravityCenter[slot][index1], gravityCenter[slot][index2]);
				}
				else // ROI 2개가 조금이라도 겹칠 경우 ROI를 1개로 합친 후 계산한다.
				{
					centerAlgorithm[slot][index1] += _T("OverlapROI / ");
					centerAlgorithm[slot][index2] += _T("OverlapROI / ");

					CalcHairpinCenter(pbyThreshold, pnLabelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, mergedRoi, slot, index1, index2, hairpinParams.nWidthSize_2BlobMinSize, info1.m_label, info2.m_label, bLeft, FALSE, gravityCenter[slot][index1], gravityCenter[slot][index2]);
				}
			}
			else if(blobNum == 1) // 하나만 있을 경우
			{
				centerAlgorithm[slot][index1] += _T("Blob1 / ");
				centerAlgorithm[slot][index2] += _T("Blob1 / ");

				// 정상적인 헤어핀 2개가 붙어있을 경우
				// 정상적인 헤어핀과 미커팅 헤어핀이 붙어있을 경우
				// 정상적인 헤어핀 하나만 있을 경우

				auto info1 = blobInfo[0];

				long roiCenterX = m_vecrtHairpin[slot][layer].CenterPoint().x;

				// 1번 blob이 roi center 기준 왼쪽에 있을때 TRUE, 아니면 FALSE
				BOOL bLeft = roiCenterX >= info1.m_roi.CenterPoint().x ? TRUE : FALSE;

				CalcHairpinCenter(pbyThreshold, pnLabelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, info1.m_roi, slot, index1, index2, hairpinParams.nWidthSize_1BlobMinSize, info1.m_label, info1.m_label, bLeft, TRUE, gravityCenter[slot][index1], gravityCenter[slot][index2]);


				// 놓친 핀을 찾는다.
				//CRect rtSearch(m_vecrtHairpin[slot][layer]);
				//auto rtRoiHairpin = m_vecrtHairpin[slot][layer];
				//rtRoiHairpin.left += roiParams.nHairpinAreaROICenterToLeftOffset;
				//rtRoiHairpin.top += roiParams.nHairpinAreaROICenterToTopOffset;
				//rtRoiHairpin.right -= roiParams.nHairpinAreaROICenterToRightOffset;
				//rtRoiHairpin.bottom -= roiParams.nHairpinAreaROICenterToBottomOffset;

				//if(bLeft == FALSE)
				//{
				//	if (gravityCenter[slot][index1] == CPoint(0, 0))
				//	{
				//		rtSearch.right = info1.m_roi.left;
				//		HUBIP::ImageFill(rtSearch, m_nUnrolledImageWidth, m_nUnrolledImageHeight, 0, pnLabelImage);

				//		SearchLostHairpin(pbyThreshold, pnLabelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, rtSearch, rtRoiHairpin, gravityCenter[slot][index1]);
				//	}
				//}
				//else
				//{
				//	if (gravityCenter[slot][index2] == CPoint(0, 0))
				//	{
				//		rtSearch.left = info1.m_roi.right;
				//		HUBIP::ImageFill(rtSearch, m_nUnrolledImageWidth, m_nUnrolledImageHeight, 0, pnLabelImage);

				//		SearchLostHairpin(pbyThreshold, pnLabelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, rtSearch, rtRoiHairpin, gravityCenter[slot][index2]);
				//	}
				//}
			}
			else if(blobNum == 0) // 하나도 없을 경우
			{
				centerAlgorithm[slot][index1] += _T("Blob0 / ");
				centerAlgorithm[slot][index2] += _T("Blob0 / ");

				//// 놓친 핀을 찾는다.
				//auto rtRoiHairpin = m_vecrtHairpin[slot][layer];
				//rtRoiHairpin.left += roiParams.nHairpinAreaROICenterToLeftOffset;
				//rtRoiHairpin.top += roiParams.nHairpinAreaROICenterToTopOffset;
				//rtRoiHairpin.right -= roiParams.nHairpinAreaROICenterToRightOffset;
				//rtRoiHairpin.bottom -= roiParams.nHairpinAreaROICenterToBottomOffset;

				//CRect rtSearch(m_vecrtHairpin[slot][layer]);
				//rtSearch.right = m_vecrtHairpin[slot][layer].CenterPoint().x;

				//HUBIP::ImageFill(rtSearch, m_nUnrolledImageWidth, m_nUnrolledImageHeight, 0, pnLabelImage);

				//SearchLostHairpin(pbyThreshold, pnLabelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, rtSearch, rtRoiHairpin, gravityCenter[slot][index1]);

				//rtSearch = m_vecrtHairpin[slot][layer];
				//rtSearch.left = m_vecrtHairpin[slot][layer].CenterPoint().x;

				//HUBIP::ImageFill(rtSearch, m_nUnrolledImageWidth, m_nUnrolledImageHeight, 0, pnLabelImage);

				//SearchLostHairpin(pbyThreshold, pnLabelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, rtSearch, rtRoiHairpin, gravityCenter[slot][index2]);
			}

			index1 += 2;
			index2 += 2;
		}
	}

	// LayerPair 중 한쪽 Layer의 핀만 찾았을 경우 다른 한쪽 Layer의 핀도 0, 0 처리 하여 3D 좌표를 Layerpair 모두 가져오도록 한다.
	if(hairpinParams.bUseLayerPairReplace == TRUE)
	{
		for(long slot = 0; slot < slotCount; slot++)
		{
			long index1(0), index2(1);
			for(long layer = 0; layer < layerPair; layer++)
			{
				if(gravityCenter[slot][index1] == CPoint(0, 0) || gravityCenter[slot][index2] == CPoint(0, 0))
				{
					gravityCenter[slot][index1] = CPoint(0, 0);
					gravityCenter[slot][index2] = CPoint(0, 0);
				}

				index1 += 2;
				index2 += 2;
			}
		}
	}

	return TRUE;
}

void CDetectionAlgorithm::CalcHairpinCenter(const BYTE* image, long* labelImage, const long imageWidth, const long imageHeight, const CRect hairpinRoi, const long slot, const long layer1, const long layer2, const long hairpinSize, const long label1, const long label2, const BOOL bLeft, const BOOL bCalcOneHairpin, CPoint &point1, CPoint &point2)
{
	CString strTemp;

	auto hairpinParams = ::g_RecipeManager.m_sHairpinCenterParams;

	float fCenX(0.f), fCenY(0.f);
	if(hairpinRoi.Width() > hairpinSize) // 두 헤어핀이 붙어있을 경우 135
	{
		strTemp.Format(_T("%d>%d / "), hairpinRoi.Width(), hairpinSize);
		m_vecstrCenterAlgorithm[slot][layer1] += strTemp;
		m_vecstrCenterAlgorithm[slot][layer2] += strTemp;

		// Blob을 Blob center 기준으로 반씩 나누고 각각 무게중심을 구한다.
		CRect roi1 = hairpinRoi;
		roi1.right = hairpinRoi.CenterPoint().x;
		HUBIP::GetProfileCenter(image, imageWidth, imageHeight, roi1, fCenX, fCenY);
		point1 = CPoint((long)fCenX, (long)fCenY);

		CRect roi2 = hairpinRoi;
		roi2.left = hairpinRoi.CenterPoint().x;
		HUBIP::GetProfileCenter(image, imageWidth, imageHeight, roi2, fCenX, fCenY);
		point2 = CPoint((long)fCenX, (long)fCenY);

		// 커팅날로 인해 윗부분이 검게 보이는 부분 보상하는 부분
		InterpolationHairpinPoints3(image, labelImage, imageWidth, imageHeight, hairpinRoi, slot, layer1, layer2, roi1, roi2, point1, point2);


		//if(hairpinRoi.Width() > hairpinSize2)
		//{
		//	strTemp.Format(_T("%d>%d / "), hairpinRoi.Width(), hairpinSize2);
		//	m_vecstrCenterAlgorithm[slot][layer1] += strTemp;
		//	m_vecstrCenterAlgorithm[slot][layer2] += strTemp;

		//	if(hairpinParams.nMinHairpinWidthfor1Blob > hairpinRoi.Width())
		//	{
		//		strTemp.Format(_T("%d>%d / "), hairpinParams.nMinHairpinWidthfor1Blob, hairpinRoi.Width());
		//		m_vecstrCenterAlgorithm[slot][layer1] += strTemp;
		//		m_vecstrCenterAlgorithm[slot][layer2] += strTemp;

		//		if(bLeft == TRUE)
		//		{
		//			CRect roi = CRect(hairpinRoi.left, hairpinRoi.top, hairpinRoi.left + hairpinParams.nMaxHairpinWidth, hairpinRoi.bottom);
		//			HUBIP::GetProfileCenter(image, imageWidth, imageHeight, roi, fCenX, fCenY);
		//			point1 = CPoint((long)fCenX, (long)fCenY);
		//			InterpolationHairpinPoints4(image, labelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roi, slot, layer1, point1);
		//		}
		//		else
		//		{
		//			CRect roi = CRect(hairpinRoi.right - hairpinParams.nMaxHairpinWidth, hairpinRoi.top, hairpinRoi.right, hairpinRoi.bottom);
		//			HUBIP::GetProfileCenter(image, imageWidth, imageHeight, roi, fCenX, fCenY);
		//			point2 = CPoint((long)fCenX, (long)fCenY);
		//			InterpolationHairpinPoints4(image, labelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roi, slot, layer2, point2);
		//		}
		//	}
		//	else
		//	{
		//		// Blob을 Blob center 기준으로 반씩 나누고 각각 무게중심을 구한다.
		//		CRect roi1 = hairpinRoi;
		//		roi1.right = hairpinRoi.CenterPoint().x;
		//		HUBIP::GetProfileCenter(image, imageWidth, imageHeight, roi1, fCenX, fCenY);
		//		point1 = CPoint((long)fCenX, (long)fCenY);

		//		CRect roi2 = hairpinRoi;
		//		roi2.left = hairpinRoi.CenterPoint().x;
		//		HUBIP::GetProfileCenter(image, imageWidth, imageHeight, roi2, fCenX, fCenY);
		//		point2 = CPoint((long)fCenX, (long)fCenY);

		//		// 커팅날로 인해 윗부분이 검게 보이는 부분 보상하는 부분
		//		InterpolationHairpinPoints3(image, labelImage, imageWidth, imageHeight, hairpinRoi, slot, layer1, layer2, roi1, roi2, point1, point2);
		//	}
		//}
		//else
		//{
		//	CalcDivisionHairpinPair(image, labelImage, imageWidth, imageHeight, hairpinRoi, label1, label2, point1, point2);
		//}
	}
	else // 헤어핀이 하나만 있을 경우
	{
		if(bCalcOneHairpin == TRUE)
		{
			m_vecstrCenterAlgorithm[slot][layer1] += _T("OneHairpin / ");
			m_vecstrCenterAlgorithm[slot][layer2] += _T("OneHairpin / ");

			if(hairpinRoi.Width() > hairpinParams.nMinHairpinWidthfor1Blob) // 한쪽이 미커팅되어 하나로 보일 경우
			{
				strTemp.Format(_T("%d>%d / "), hairpinRoi.Width(), hairpinParams.nMinHairpinWidthfor1Blob);
				m_vecstrCenterAlgorithm[slot][layer1] += strTemp;
				m_vecstrCenterAlgorithm[slot][layer2] += strTemp;

				CRect roiLeft(hairpinRoi);
				roiLeft.right = hairpinRoi.CenterPoint().x;
				CRect roiRight(hairpinRoi);
				roiRight.left = hairpinRoi.CenterPoint().x;

				double meanLeft(0.f), meanRight(0.f);
				HUBIP::GetMean(image, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roiLeft, meanLeft);
				HUBIP::GetMean(image, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roiRight, meanRight);

				BOOL bLeftPos = meanLeft > meanRight ? TRUE : FALSE;

				roiLeft = CRect(hairpinRoi.left, hairpinRoi.top, hairpinRoi.left + hairpinParams.nMaxHairpinWidth, hairpinRoi.bottom);
				roiRight = CRect(hairpinRoi.right - hairpinParams.nMaxHairpinWidth, hairpinRoi.top, hairpinRoi.right, hairpinRoi.bottom);

				if(bLeftPos == TRUE)
				{
					HUBIP::GetMean(image, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roiLeft, meanLeft);
					if(meanLeft > 180.f)
					{
						HUBIP::GetProfileCenter(image, imageWidth, imageHeight, roiLeft, fCenX, fCenY);
						point1 = CPoint((long)fCenX, (long)fCenY);
						InterpolationHairpinPoints4(image, labelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roiLeft, slot, layer1, point1);
					
						m_vecstrCenterAlgorithm[slot][layer2] += _T("No / ");
					}
				}
				else
				{
					HUBIP::GetMean(image, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roiRight, meanRight);
					if(meanRight > 180.f)
					{
						HUBIP::GetProfileCenter(image, imageWidth, imageHeight, roiRight, fCenX, fCenY);
						point2 = CPoint((long)fCenX, (long)fCenY);
						InterpolationHairpinPoints4(image, labelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roiRight, slot, layer2, point2);
					
						m_vecstrCenterAlgorithm[slot][layer1] += _T("No / ");
					}
				}

				//if(bLeft == TRUE)
				//{
				//	CRect roi = CRect(hairpinRoi.left, hairpinRoi.top, hairpinRoi.left + hairpinParams.nMaxHairpinWidth, hairpinRoi.bottom);
				//	HUBIP::GetProfileCenter(image, imageWidth, imageHeight, roi, fCenX, fCenY);
				//	point1 = CPoint((long)fCenX, (long)fCenY);
				//	InterpolationHairpinPoints4(image, labelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roi, slot, layer1, point1);
				//}
				//else
				//{
				//	CRect roi = CRect(hairpinRoi.right - hairpinParams.nMaxHairpinWidth, hairpinRoi.top, hairpinRoi.right, hairpinRoi.bottom);
				//	HUBIP::GetProfileCenter(image, imageWidth, imageHeight, roi, fCenX, fCenY);
				//	point2 = CPoint((long)fCenX, (long)fCenY);
				//	InterpolationHairpinPoints4(image, labelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, roi, slot, layer2, point2);
				//}
			}
			else // 헤어핀이 하나만 있을 경우
			{
				HUBIP::GetProfileCenter(image, imageWidth, imageHeight, hairpinRoi, fCenX, fCenY);

				if(bLeft == TRUE)
				{
					point1 = CPoint((long)fCenX, (long)fCenY);

					m_vecstrCenterAlgorithm[slot][layer2] += _T("No / ");
				}
				else
				{
					point2 = CPoint((long)fCenX, (long)fCenY);

					m_vecstrCenterAlgorithm[slot][layer1] += _T("No / ");
				}

				if(bLeft == TRUE)
				{
					InterpolationHairpinPoints4(image, labelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, hairpinRoi, slot, layer1, point1);
				}
				else
				{
					InterpolationHairpinPoints4(image, labelImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, hairpinRoi, slot, layer2, point2);
				}
			}
		}
	}
}

void CDetectionAlgorithm::CalcDivisionHairpinPair(const BYTE* image, long* labelImage, const long imageWidth, const long imageHeight, const CRect blobRect, const long label1, const long label2, CPoint &point1, CPoint &point2)
{
	CRect blobRoi(blobRect);
	blobRoi.DeflateRect(20, 0);

	long xPos(0);
	long minSum(LONG_MAX);
	for(long x = blobRoi.left; x < blobRoi.right; x++)
	{
		long sum(0);
		for(long y = blobRoi.top; y < blobRoi.bottom; y++)
		{
			long pos = y * imageWidth + x;

			if(image[pos] == 255 && (label1 == labelImage[pos] || label2 == labelImage[pos]))
			{
				sum += image[pos];
			}
		}

		if(minSum > sum)
		{
			minSum = sum;
			xPos = x;
		}
	}

	float fCenX(0.f), fCenY(0.f);

	CRect roi1 = blobRect;
	roi1.right = xPos;
	HUBIP::GetProfileCenter(image, imageWidth, imageHeight, roi1, fCenX, fCenY);
	point1 = CPoint((long)fCenX, (long)fCenY);

	CRect roi2 = blobRect;
	roi2.left = xPos;
	HUBIP::GetProfileCenter(image, imageWidth, imageHeight, roi2, fCenX, fCenY);
	point2 = CPoint((long)fCenX, (long)fCenY);

	//InterpolationHairpinPoints3(image, labelImage, imageWidth, imageHeight, blobRect, roi1, roi2, point1, point2);
}

//void CDetectionAlgorithm::InterpolationHairpinPoints(const BYTE* image, const long imageWidth, const long imageHeight, const CRect roi, const CRect roi1, const CRect roi2, CPoint &point1, CPoint &point2)
//{
//	auto hairpinParams = ::g_RecipeManager.m_sHairpinCenterParams;
//
//	if(hairpinParams.bUseComplementHairpinCenter == FALSE)
//	{
//		return;
//	}
//
//	CRect profileRoi = roi;
//	profileRoi.top += roi.Height() / 4;
//	profileRoi.bottom -= roi.Height() / 4;
//
//	float fCenX(0.f), fCenY(0.f);
//	HUBIP::GetProfileCenter(image, imageWidth, imageHeight, profileRoi, fCenX, fCenY);
//
//	CRect rtCenterRange = roi;
//	rtCenterRange.left = roi.CenterPoint().x - roi.Width() / 20;
//	rtCenterRange.right = roi.CenterPoint().x + roi.Width() / 20;
//
//	CRect roiTemp;
//	if(rtCenterRange.PtInRect(CPoint((long)fCenX, (long)fCenY)) == FALSE)
//	{
//		if(fCenX > rtCenterRange.right) // 오른쪽에 있을 경우
//		{
//			roiTemp = roi1;
//			roiTemp.top = roi1.bottom - hairpinParams.nComplementHeightSize;
//			if(roiTemp.CenterPoint().y < point1.y)
//			{
//				point1.y = roiTemp.CenterPoint().y;
//			}
//		}
//		else
//		{
//			roiTemp = roi2;
//			roiTemp.top = roi2.bottom - hairpinParams.nComplementHeightSize;
//			if(roiTemp.CenterPoint().y < point2.y)
//			{
//				point2.y = roiTemp.CenterPoint().y;
//			}
//		}
//	}
//}
//
//void CDetectionAlgorithm::InterpolationHairpinPoints2(const BYTE* image, const long imageWidth, const long imageHeight, const CRect roi, const CRect roi1, const CRect roi2, CPoint &point1, CPoint &point2)
//{
//	auto hairpinParams = ::g_RecipeManager.m_sHairpinCenterParams;
//
//	if(hairpinParams.bUseComplementHairpinCenter == FALSE)
//	{
//		return;
//	}
//
//	CRect rtRoi1;
//	rtRoi1.left = point1.x - 35;
//	rtRoi1.top = point1.y - hairpinParams.nComplementHeightSize / 2;
//	rtRoi1.right = point1.x + 35;
//	rtRoi1.bottom = point1.y;
//
//	CRect rtRoi2;
//	rtRoi2.left = point2.x - 35;
//	rtRoi2.top = point2.y - hairpinParams.nComplementHeightSize / 2;
//	rtRoi2.right = point2.x + 35;
//	rtRoi2.bottom = point2.y;
//
//	double roi1Mean(0.f);
//	HUBIP::GetMean(image, imageWidth, imageHeight, rtRoi1, roi1Mean);
//
//	double roi2Mean(0.f);
//	HUBIP::GetMean(image, imageWidth, imageHeight, rtRoi2, roi2Mean);
//
//	CRect roiTemp;
//	if(roi1Mean < 230)
//	{
//		roiTemp = roi1;
//		roiTemp.top = roi1.bottom - hairpinParams.nComplementHeightSize;
//		if(roiTemp.CenterPoint().y < point1.y)
//		{
//			point1.y = roiTemp.CenterPoint().y;
//		}
//	}
//
//	if(roi2Mean < 230)
//	{
//		roiTemp = roi2;
//		roiTemp.top = roi2.bottom - hairpinParams.nComplementHeightSize;
//		if(roiTemp.CenterPoint().y < point2.y)
//		{
//			point2.y = roiTemp.CenterPoint().y;
//		}
//	}
//}

void CDetectionAlgorithm::InterpolationHairpinPoints3(const BYTE* image, long* labelImage, const long imageWidth, const long imageHeight, const CRect roi, const long slot, const long layer1, const long layer2, const CRect roiLeft, const CRect roiRight, CPoint &point1, CPoint &point2)
{
	m_vecstrCenterAlgorithm[slot][layer1] += _T("Interpolation / ");
	m_vecstrCenterAlgorithm[slot][layer2] += _T("Interpolation / ");

	auto hairpinParams = ::g_RecipeManager.m_sHairpinCenterParams;

	CRect rtRoi[2];
	rtRoi[0] = roiLeft;
	rtRoi[0].right = (long)((float)roiLeft.right - (float)roiLeft.Width() * 0.1f);

	rtRoi[1] = roiRight;
	rtRoi[1].left = (long)((float)roiRight.left + (float)roiRight.Width() * 0.1f);

	HUBIP::ImageFill(rtRoi[0], imageWidth, imageHeight, 0, labelImage);
	HUBIP::ImageFill(rtRoi[1], imageWidth, imageHeight, 0, labelImage);

	long blobCount(0);
	HUBSDK::BlobInfo resultBlobInfo[2];
	std::vector<std::vector<HUBSDK::BlobInfo>> vecsBlobInfo(2);
	
	HUBSDK::BlobInfo value;
	value.Init();
	m_pAlgorithm->ResizeVector(vecsBlobInfo, 2, 100, value);

	for(long idx = 0; idx < 2; idx++)
	{
		blobCount = 0;
		auto &blobInfo = vecsBlobInfo[idx];

		if(!m_pAlgorithm->Blob(const_cast<BYTE *>(image), imageWidth, imageHeight, rtRoi[idx], FALSE, 100, 100, &blobInfo[0], blobCount, labelImage))
		{
			return;
		}

		if(blobCount > 1)
		{
			long maxArea(-LONG_MAX);
			long index(0);
			for(long blob = 0; blob < blobCount; blob++)
			{
				if(maxArea < blobInfo[blob].m_area)
				{
					maxArea = blobInfo[blob].m_area;
					index = blob;
				}
			}

			resultBlobInfo[idx] = blobInfo[index];
		}
		else if(blobCount == 1)
		{
			resultBlobInfo[idx] = blobInfo[0];
		}
		else // 한개도 없는데 여기 들어올 경우가 있을까??
		{
			continue;
		}
	}

	m_vecnHairpinWidth[slot][layer1] = resultBlobInfo[0].m_roi.Width();
	m_vecnHairpinHeight[slot][layer1] = resultBlobInfo[0].m_roi.Height();
	m_vecnHairpinWidth[slot][layer2] = resultBlobInfo[1].m_roi.Width();
	m_vecnHairpinHeight[slot][layer2] = resultBlobInfo[1].m_roi.Height();

	// 무게 중심을 찾는다.
	float fCenX(0.f), fCenY(0.f);
	HUBIP::GetProfileCenter(image, imageWidth, imageHeight, resultBlobInfo[0].m_roi, fCenX, fCenY);
	point1 = CPoint((long)fCenX, (long)fCenY);

	fCenX = fCenY = 0.f;
	HUBIP::GetProfileCenter(image, imageWidth, imageHeight, resultBlobInfo[1].m_roi, fCenX, fCenY);
	point2 = CPoint((long)fCenX, (long)fCenY);

	// 설정한 Height Size 보다 작을 경우만 수행한다.
	if(hairpinParams.bUseComplementHairpinCenter == TRUE)
	{
		if (hairpinParams.nComplementHeightLimit > resultBlobInfo[0].m_roi.Height())
		{
			rtRoi[0].bottom = resultBlobInfo[0].m_roi.bottom;
			rtRoi[0].top = rtRoi[0].bottom - hairpinParams.nComplementHeightSize;
			point1.y = rtRoi[0].CenterPoint().y;
		}

		if (hairpinParams.nComplementHeightLimit > resultBlobInfo[1].m_roi.Height())
		{
			rtRoi[1].bottom = resultBlobInfo[1].m_roi.bottom;
			rtRoi[1].top = rtRoi[1].bottom - hairpinParams.nComplementHeightSize;
			point2.y = rtRoi[1].CenterPoint().y;
		}
	}

	// Blob Width, Height 사이즈 비교해서 작은 놈은 Center를 0, 0 으로 만든다.
	if(hairpinParams.nMinHairpinWidth > resultBlobInfo[0].m_roi.Width() && hairpinParams.nMinHairpinHeight > resultBlobInfo[0].m_roi.Height())
	{
		m_vecstrCenterAlgorithm[slot][layer1] += _T("SmallSize / ");

		point1 = CPoint(0, 0);
	}

	if(hairpinParams.nMinHairpinWidth > resultBlobInfo[1].m_roi.Width() && hairpinParams.nMinHairpinHeight > resultBlobInfo[1].m_roi.Height())
	{
		m_vecstrCenterAlgorithm[slot][layer2] += _T("SmallSize / ");

		point2 = CPoint(0, 0);
	}

	// Blob Width, Height 사이즈 비교해서 큰 놈은 Center를 0, 0 으로 만든다.
	if(hairpinParams.nMaxHairpinWidth < resultBlobInfo[0].m_roi.Width() || hairpinParams.nMaxHairpinHeight < resultBlobInfo[0].m_roi.Height())
	{
		m_vecstrCenterAlgorithm[slot][layer1] += _T("BigSize / ");

		point1 = CPoint(0, 0);
	}

	if(hairpinParams.nMaxHairpinWidth < resultBlobInfo[1].m_roi.Width() || hairpinParams.nMaxHairpinHeight < resultBlobInfo[1].m_roi.Height())
	{
		m_vecstrCenterAlgorithm[slot][layer2] += _T("BigSize / ");

		point2 = CPoint(0, 0);
	}
}

void CDetectionAlgorithm::InterpolationHairpinPoints4(const BYTE* image, long* labelImage, const long imageWidth, const long imageHeight, const CRect roi, const long slot, const long layer, CPoint &point)
{
	m_vecstrCenterAlgorithm[slot][layer] += _T("Interpolation / ");

	auto hairpinParams = ::g_RecipeManager.m_sHairpinCenterParams;

	CRect rtRoi(roi);
	HUBIP::ImageFill(rtRoi, imageWidth, imageHeight, 0, labelImage);

	long blobCount(0);
	HUBSDK::BlobInfo resultBlobInfo;
	std::vector<HUBSDK::BlobInfo> vecsBlobInfo;

	HUBSDK::BlobInfo value;
	value.Init();
	m_pAlgorithm->ResizeVector(vecsBlobInfo, 100, value);

	auto &blobInfo = vecsBlobInfo;

	if (!m_pAlgorithm->Blob(const_cast<BYTE *>(image), imageWidth, imageHeight, rtRoi, FALSE, 100, 100, &blobInfo[0], blobCount, labelImage))
	{
		return;
	}

	if (blobCount > 1)
	{
		long maxArea(-LONG_MAX);
		long index(0);
		for (long blob = 0; blob < blobCount; blob++)
		{
			if (maxArea < blobInfo[blob].m_area)
			{
				maxArea = blobInfo[blob].m_area;
				index = blob;
			}
		}

		resultBlobInfo = blobInfo[index];
	}
	else if (blobCount == 1)
	{
		resultBlobInfo = blobInfo[0];
	}
	else // 한개도 없는데 여기 들어올 경우가 있을까??
	{
		return;
	}

	m_vecnHairpinWidth[slot][layer] = resultBlobInfo.m_roi.Width();
	m_vecnHairpinHeight[slot][layer] = resultBlobInfo.m_roi.Height();

	// 무게 중심을 찾는다.
	float fCenX(0.f), fCenY(0.f);
	HUBIP::GetProfileCenter(image, imageWidth, imageHeight, resultBlobInfo.m_roi, fCenX, fCenY);
	point = CPoint((long)fCenX, (long)fCenY);

	// 설정한 Height Size 보다 작을 경우만 수행한다.
	if(hairpinParams.bUseComplementHairpinCenter == TRUE)
	{
		if (hairpinParams.nComplementHeightLimit > resultBlobInfo.m_roi.Height())
		{
			rtRoi.bottom = resultBlobInfo.m_roi.bottom;
			rtRoi.top = rtRoi.bottom - hairpinParams.nComplementHeightSize;
			point.y = rtRoi.CenterPoint().y;
		}
	}

	// Blob Width, Height 사이즈 비교해서 작은 놈은 Center를 0, 0 으로 만든다.
	if (hairpinParams.nMinHairpinWidth > resultBlobInfo.m_roi.Width() && hairpinParams.nMinHairpinHeight > resultBlobInfo.m_roi.Height())
	{
		m_vecstrCenterAlgorithm[slot][layer] += _T("SmallSize / ");

		point = CPoint(0, 0);
	}

	// Blob Width, Height 사이즈 비교해서 큰 놈은 Center를 0, 0 으로 만든다.
	if(hairpinParams.nMaxHairpinWidth < resultBlobInfo.m_roi.Width() || hairpinParams.nMaxHairpinHeight < resultBlobInfo.m_roi.Height())
	{
		m_vecstrCenterAlgorithm[slot][layer] += _T("BigSize / ");

		point = CPoint(0, 0);
	}
}

// 완벽히 두개의 Blob 을 잡았을 경우 사용
void CDetectionAlgorithm::InterpolationHairpinPoints5(const BYTE* image, long* labelImage, const long imageWidth, const long imageHeight, const CRect roi, const long slot, const long layer1, const long layer2, const CRect roiLeft, const CRect roiRight, CPoint &point1, CPoint &point2)
{
	m_vecstrCenterAlgorithm[slot][layer1] += _T("Interpolation / ");
	m_vecstrCenterAlgorithm[slot][layer2] += _T("Interpolation / ");

	auto hairpinParams = ::g_RecipeManager.m_sHairpinCenterParams;

	CRect rtRoi[2];
	rtRoi[0] = roiLeft;
	rtRoi[1] = roiRight;

	HUBIP::ImageFill(rtRoi[0], imageWidth, imageHeight, 0, labelImage);
	HUBIP::ImageFill(rtRoi[1], imageWidth, imageHeight, 0, labelImage);

	long blobCount(0);
	HUBSDK::BlobInfo resultBlobInfo[2];
	std::vector<std::vector<HUBSDK::BlobInfo>> vecsBlobInfo(2);

	HUBSDK::BlobInfo value;
	value.Init();
	m_pAlgorithm->ResizeVector(vecsBlobInfo, 2, 100, value);

	for(long idx = 0; idx < 2; idx++)
	{
		blobCount = 0;
		auto &blobInfo = vecsBlobInfo[idx];

		if(!m_pAlgorithm->Blob(const_cast<BYTE *>(image), imageWidth, imageHeight, rtRoi[idx], FALSE, 100, 100, &blobInfo[0], blobCount, labelImage))
		{
			return;
		}

		if(blobCount > 1)
		{
			long maxArea(-LONG_MAX);
			long index(0);
			for(long blob = 0; blob < blobCount; blob++)
			{
				if(maxArea < blobInfo[blob].m_area)
				{
					maxArea = blobInfo[blob].m_area;
					index = blob;
				}
			}

			resultBlobInfo[idx] = blobInfo[index];
		}
		else if(blobCount == 1)
		{
			resultBlobInfo[idx] = blobInfo[0];
		}
		else // 한개도 없는데 여기 들어올 경우가 있을까??
		{
			continue;
		}
	}

	m_vecnHairpinWidth[slot][layer1] = resultBlobInfo[0].m_roi.Width();
	m_vecnHairpinHeight[slot][layer1] = resultBlobInfo[0].m_roi.Height();
	m_vecnHairpinWidth[slot][layer2] = resultBlobInfo[1].m_roi.Width();
	m_vecnHairpinHeight[slot][layer2] = resultBlobInfo[1].m_roi.Height();

	// 무게 중심을 찾는다.
	float fCenX(0.f), fCenY(0.f);
	HUBIP::GetProfileCenter(image, imageWidth, imageHeight, resultBlobInfo[0].m_roi, fCenX, fCenY);
	point1 = CPoint((long)fCenX, (long)fCenY);

	fCenX = fCenY = 0.f;
	HUBIP::GetProfileCenter(image, imageWidth, imageHeight, resultBlobInfo[1].m_roi, fCenX, fCenY);
	point2 = CPoint((long)fCenX, (long)fCenY);

	// 설정한 Height Size 보다 작을 경우만 수행한다.
	if(hairpinParams.bUseComplementHairpinCenter == TRUE)
	{
		if(hairpinParams.nComplementHeightLimit > resultBlobInfo[0].m_roi.Height())
		{
			rtRoi[0].bottom = resultBlobInfo[0].m_roi.bottom;
			rtRoi[0].top = rtRoi[0].bottom - hairpinParams.nComplementHeightSize;
			point1.y = rtRoi[0].CenterPoint().y;
		}

		if(hairpinParams.nComplementHeightLimit > resultBlobInfo[1].m_roi.Height())
		{
			rtRoi[1].bottom = resultBlobInfo[1].m_roi.bottom;
			rtRoi[1].top = rtRoi[1].bottom - hairpinParams.nComplementHeightSize;
			point2.y = rtRoi[1].CenterPoint().y;
		}
	}

	// Blob Width, Height 사이즈 비교해서 작은 놈은 Center를 0, 0 으로 만든다.
	if(hairpinParams.nMinHairpinWidth > resultBlobInfo[0].m_roi.Width() && hairpinParams.nMinHairpinHeight > resultBlobInfo[0].m_roi.Height())
	{
		m_vecstrCenterAlgorithm[slot][layer1] += _T("SmallSize / ");

		point1 = CPoint(0, 0);
	}

	if(hairpinParams.nMinHairpinWidth > resultBlobInfo[1].m_roi.Width() && hairpinParams.nMinHairpinHeight > resultBlobInfo[1].m_roi.Height())
	{
		m_vecstrCenterAlgorithm[slot][layer2] += _T("SmallSize / ");

		point2 = CPoint(0, 0);
	}

	// Blob Width, Height 사이즈 비교해서 큰 놈은 Center를 0, 0 으로 만든다.
	if(hairpinParams.nMaxHairpinWidth < resultBlobInfo[0].m_roi.Width() || hairpinParams.nMaxHairpinHeight < resultBlobInfo[0].m_roi.Height())
	{
		m_vecstrCenterAlgorithm[slot][layer1] += _T("BigSize / ");

		point1 = CPoint(0, 0);
	}

	if(hairpinParams.nMaxHairpinWidth < resultBlobInfo[1].m_roi.Width() || hairpinParams.nMaxHairpinHeight < resultBlobInfo[1].m_roi.Height())
	{
		m_vecstrCenterAlgorithm[slot][layer2] += _T("BigSize / ");

		point2 = CPoint(0, 0);
	}
}

void CDetectionAlgorithm::SearchLostHairpin(const BYTE* image, long* labelImage, const long imageWidth, const long imageHeight, const CRect searchRoi, const CRect hairpinArea, CPoint &point)
{
	std::vector<HUBSDK::BlobInfo> vecsBlobInfoList_temp;
	std::vector<HUBSDK::BlobInfo> vecsBlobInfoList;

	// BlobInfo 초기화
	HUBSDK::BlobInfo value;
	value.Init();
	m_pAlgorithm->ResizeVector(vecsBlobInfoList_temp, 100, value);

	long blobCount(0);
	if(!m_pAlgorithm->Blob(const_cast<BYTE*>(image), imageWidth, imageHeight, searchRoi, FALSE, 10, 100, &vecsBlobInfoList_temp[0], blobCount, labelImage))
	{
		return;
	}

	// Blob Count 가 0 이면 진짜 핀이 없는거다
	if(blobCount > 0)
	{
		vecsBlobInfoList_temp.resize(blobCount);

		// 헤어핀 ROI 영역에 없는 Blob 들은 지워 버린다.(Noise로 간주)
		for(long idx = 0; idx < blobCount; idx++)
		{
			auto roi = vecsBlobInfoList_temp[idx].m_roi & hairpinArea;

			if(roi.Width() > 0 && roi.Height() > 0)
			{
				vecsBlobInfoList.push_back(vecsBlobInfoList_temp[idx]);
			}
		}

		blobCount = (long)vecsBlobInfoList.size();

		HUBSDK::BlobInfo resultBlobInfo;
		resultBlobInfo.Init();

		if(blobCount > 1) // Blob이 여러개일 경우
		{
			// 가장 큰놈을 고른다.
			long maxArea(-LONG_MAX);
			long index(0);
			for(long blob = 0; blob < blobCount; blob++)
			{
				if(maxArea < vecsBlobInfoList[blob].m_area)
				{
					maxArea = vecsBlobInfoList[blob].m_area;
					index = blob;
				}
			}

			resultBlobInfo = vecsBlobInfoList[index];
		}
		else if(blobCount == 1) // Blob이 하나 일 경우
		{
			resultBlobInfo = vecsBlobInfoList[0];
		}
		else // Blob이 없을 경우
		{
			;; //아무짓도 안한다.
		}

		if(resultBlobInfo.m_area > 0)
		{
			// 무게 중심을 찾는다.
			float fCenX(0.f), fCenY(0.f);
			HUBIP::GetProfileCenter(image, imageWidth, imageHeight, resultBlobInfo.m_roi, fCenX, fCenY);
			point = CPoint((long)fCenX, (long)fCenY);
		}
	}
}
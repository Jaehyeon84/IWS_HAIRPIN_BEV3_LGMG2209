#include "pch.h"
#include "DetectionAlgorithm.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CDetectionAlgorithm::ResultDataSave(const CString strTime)
{
	CString strTemp;

	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto criteriaParams = ::g_RecipeManager.m_sCriteriaParams;

	auto slotCount = roiParams.nSlotCount;
	auto layerCount = roiParams.nLayerCount;
	auto layerPair = layerCount / 2;

	auto dataPath = ::g_RecipeManager.m_sPublicParams.strResultDataSavePath;

	if(VECTOR_SIZE(m_vecfIWIX_afterMatrix) <= 0 || VECTOR_SIZE(m_vecfIWIY_afterMatrix) <= 0)
	{
		return;
	}

	if(dataPath != _T(""))
	{
		CString strSavePath = _T("");
		strSavePath = CreateSaveFolder(dataPath);

		if (m_strBarcodeID == _T(""))
		{
			m_strBarcodeID = _T("Barcode_Empty");
		}

		CString strPath;
		strPath.Format(_T("%s\\%s_%s_result.csv"), strSavePath, strTime, m_strBarcodeID);

		CFile pFile;
		if(pFile.Open(strPath, CFile::modeCreate | CFile::modeWrite, NULL) == FALSE)
		{
			return;
		}

		int nLen = 0;
		char pTmpData[200] = {0,};

		nLen = sprintf_s(pTmpData, 200, "Slot:%d\n", slotCount);
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "Layer:%d\n", layerCount);
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "H12 - Spec Min Value : %f\n", criteriaParams.fMin_H12);
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "H12 - Spec Max Value : %f\n", criteriaParams.fMax_H12);
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "H3 - Spec Max Value : %f\n", criteriaParams.fMax_H3);
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "C - Spec Min Value : %f\n", criteriaParams.fMin_C);
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "C - Spec Max Value : %f\n", criteriaParams.fMax_C);
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "T - Spec Min Value : %f\n", criteriaParams.fMin_T);
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "T - Spec Max Value : %f\n", criteriaParams.fMax_T);
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "J - Spec Min Value : %f\n", criteriaParams.fMin_J);
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "J - Spec Max Value : %f\n\n", criteriaParams.fMax_J);
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "[Status 0] : NG (H1 or H2 pin not found) \n");
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "[Status 1] : OK \n");
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "[Status 2] : NG or Spec Error \n\n");
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		nLen = sprintf_s(pTmpData, 200, "Slot,Layer,Status,OK/NG/SpecError,ax,ay,bx,by,3D_ax,3D_ay,3D_bx,3D_by,H1,H2,H3,c,jump,theta\n");
		pFile.Write(pTmpData, nLen);
		memset(pTmpData, 0, 200);

		CStringA strLayer, strResult;
		long result(0);

		for(long slot = 0; slot < slotCount; slot++)
		{
			long index1(0), index2(1);
			for(int layer = 0; layer < layerPair; layer++)
			{
				switch(layer)
				{
				case 0: strLayer = "1~2"; break;
				case 1: strLayer = "3~4"; break;
				case 2: strLayer = "5~6"; break;
				case 3: strLayer = "7~8"; break;
				case 4: strLayer = "9~10"; break;
				}

				switch(m_vecsCriteriaResult[slot][layer].nErrorCode)
				{
				case INSP_ERR_OK://1:
					result = 1;
					strResult = "OK";
					break;
				case INSP_ERR_F1://- 1:
				case INSP_ERR_F2://- 2:
				case INSP_ERR_F12:// -3:
					result = 0;
					strResult = "NG";
					break;
				case INSP_ERR_H1://- 4:
				case INSP_ERR_H2://- 5:
				case INSP_ERR_H3://- 6:
				case INSP_ERR_C://- 7:
				case INSP_ERR_T://- 8:
				case INSP_ERR_J://- 9:
					result = 2;
					strResult = "SpecError";
					break;
				default:
					result = 0;
					break;
				}

				nLen = sprintf_s(pTmpData, 200, "%d,%s,%d,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
					slot + 1,
					strLayer.GetBuffer(),
					result,
					strResult,
					(m_vecfResultX_cad[slot][index1]),
					(m_vecfResultY_cad[slot][index1]),
					(m_vecfResultX_cad[slot][index2]),
					(m_vecfResultY_cad[slot][index2]),
					(m_vecfIWIX_afterMatrix[slot][index1]),
					(m_vecfIWIY_afterMatrix[slot][index1]),
					(m_vecfIWIX_afterMatrix[slot][index2]),
					(m_vecfIWIY_afterMatrix[slot][index2]),
					(m_vecsCriteriaResult[slot][layer].fH1),
					(m_vecsCriteriaResult[slot][layer].fH2),
					(m_vecsCriteriaResult[slot][layer].fH3),
					(m_vecsCriteriaResult[slot][layer].fC),
					(m_vecsCriteriaResult[slot][layer].fJ),
					(m_vecsCriteriaResult[slot][layer].fT));

				pFile.Write(pTmpData, nLen);
				memset(pTmpData, 0, 200);

				index1 += 2;
				index2 += 2;
			}
		}

		pFile.Close();
	}
}

void CDetectionAlgorithm::ResultImageSave(const CString strTime)
{
	CString strTemp;

	auto params = ::g_RecipeManager.m_sROIParams;
	BYTE* pbyImage = ::g_MemoryManager.GetByteImage((long)IMAGE::SUB_ORIG);

	if(::g_RecipeManager.m_sPublicParams.strResultImageSavePath != _T(""))
	{
		CImage* image = new CImage;
		image->Destroy();
		image->Create(m_nSubOriginalImageWidth, m_nSubOriginalImageHeight, 24);

		m_pAlgorithm->GetGrayToColorImage(pbyImage, m_nSubOriginalImageWidth, m_nSubOriginalImageHeight, 8, CRect(0, 0, m_nSubOriginalImageWidth, m_nSubOriginalImageHeight), image);

		if(image != nullptr)
		{
			CDC* pDC = CDC::FromHandle(image->GetDC());

			pDC->SetBkMode(TRANSPARENT);
			CBrush redBrush(RGB(255, 0, 0));
			CPen redPen(PS_SOLID, 3, RGB(255, 0, 0));
			CPen redPen2(PS_SOLID, 10, RGB(255, 0, 0));

			CBrush blueBrush(RGB(0, 0, 255));
			CPen bluePen(PS_SOLID, 3, RGB(0, 0, 255));

			CBrush yellowBrush(RGB(255, 255, 0));
			CPen yellowPen(PS_SOLID, 3, RGB(255, 255, 0));

			CFont *pOldFont;
			pOldFont = pDC->GetCurrentFont();

			CFont font;
			font.CreatePointFont(750, _T("Tahoma"));

			pDC->SelectObject(&font);

			// 헤어핀 포인트 표시
			for(long slot = 0; slot < params.nSlotCount; slot++)
			{
				for(long layer = 0; layer < params.nLayerCount; layer++)
				{
					BOOL replace = m_vecbReplace3DPosition[slot][layer];

					if (replace == TRUE) // 3D 대체 좌표는 파란색
					{
						pDC->SelectObject(&blueBrush);
						pDC->SelectObject(&bluePen);
					}
					else
					{
						pDC->SelectObject(&redBrush);
						pDC->SelectObject(&redPen);
					}

					pDC->Ellipse(m_vecptResult[slot][layer].x - 5, m_vecptResult[slot][layer].y - 5, m_vecptResult[slot][layer].x + 5, m_vecptResult[slot][layer].y + 5);
				}
			}

			// 슬롯 구분 라인 및 슬롯 번호 표시
			auto startPoint = m_vecptSlotLineOverlayStartPoint;
			auto endPoint = m_vecptSlotLineOverlayEndPoint;
			auto slotNo1Point = m_vecptSlotNo1OverlayPoint;
			auto slotNo2Point = m_vecptSlotNo2OverlayPoint;

			pDC->SelectObject(&yellowBrush);
			pDC->SelectObject(&yellowPen);
			pDC->SetTextColor(RGB(255, 255, 0));

			for(long idx = 0; idx < (long)startPoint.size(); idx++)
			{
				long index = params.nSlotCount - params.nStartSlotIndex + 1 + idx + 1; // ehjang. start slot index
				if(index > params.nSlotCount)
				{
					index -= params.nSlotCount;
				}

				strTemp.Format(_T("%d"), index);

				pDC->MoveTo(startPoint[idx]);
				pDC->LineTo(endPoint[idx]);
				pDC->DrawText(strTemp, CRect(slotNo1Point[idx].x - 5, slotNo1Point[idx].y - 5, slotNo1Point[idx].x + 5, slotNo1Point[idx].y + 5), DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
				pDC->DrawText(strTemp, CRect(slotNo2Point[idx].x - 5, slotNo2Point[idx].y - 5, slotNo2Point[idx].x + 5, slotNo2Point[idx].y + 5), DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
			}

			// NG 포인트 표시
			auto layerPairPoint = m_vecptLayerPairOverlayPoint;
			auto inspResult = m_vecsCriteriaResult;

			if(VECTOR_SIZE(layerPairPoint) <= 0)
			{
				return;
			}

			// NG 부분 표시
			pDC->SetBkMode(TRANSPARENT);
			CBrush clsBrush, *pclsBrush;
			clsBrush.CreateStockObject(NULL_BRUSH);
			pclsBrush = pDC->SelectObject(&clsBrush);
			pDC->SelectObject(&redPen2);

			for(long slot = 0; slot < params.nSlotCount; slot++)
			{
				for(long layer = 0; layer < params.nLayerCount / 2; layer++)
				{
					if(inspResult[slot][layer].nErrorCode == INSP_ERR_OK)
					{
						continue;
					}

					auto points = layerPairPoint[slot][(params.nLayerCount / 2) - 1 - layer];

					CRect ellipse1(points.x - 99, points.y - 99, points.x + 99, points.y + 99);
					CRect ellipse2(points.x - 100, points.y - 100, points.x + 100, points.y + 100);
					CRect ellipse3(points.x - 101, points.y - 101, points.x + 101, points.y + 101);
					pDC->Ellipse(ellipse1);
					pDC->Ellipse(ellipse2);
					pDC->Ellipse(ellipse3);
				}
			}

			pDC->SelectObject(pclsBrush);
			pDC->SelectObject(&pOldFont);

			image->ReleaseDC();


			CString strSavePath = _T("");
			strSavePath = CreateSaveFolder(::g_RecipeManager.m_sPublicParams.strResultImageSavePath);

			if (m_strBarcodeID == _T(""))
			{
				m_strBarcodeID = _T("Barcode_Empty");
			}

			CString strPath;
			if (m_strTotalResult == _T("NG"))
			{
				strPath.Format(_T("%s\\%s_%s_result_NG.jpg"), strSavePath, strTime, m_strBarcodeID);
			}
			else
			{
				strPath.Format(_T("%s\\%s_%s_result.jpg"), strSavePath, strTime, m_strBarcodeID);
			}

			image->Save(strPath);

			if(::g_RecipeManager.m_sPublicParams.bSaveResultImageForJigA == TRUE && m_strJigName == _T("JigA"))
			{
				CString strJigAPath;
				strJigAPath.Format(_T("%s\\Result_%d.jpg"), ::g_RecipeManager.m_sPublicParams.strResultImageSavePathForJigA, m_nNGCount);

				image->Save(strJigAPath);
			}

			if(::g_RecipeManager.m_sPublicParams.bSaveResultImageForJigB == TRUE && m_strJigName == _T("JigB"))
			{
				CString strJigBPath;
				strJigBPath.Format(_T("%s\\Result_%d.jpg"), ::g_RecipeManager.m_sPublicParams.strResultImageSavePathForJigB, m_nNGCount);

				image->Save(strJigBPath);
			}
		}
		delete[] image;
	}
}

void CDetectionAlgorithm::RawImageSave(const CString strTime)
{
	if(::g_RecipeManager.m_sPublicParams.strRawImageSavePath != _T(""))
	{
		CString strSavePath = _T("");
		strSavePath = CreateSaveFolder(::g_RecipeManager.m_sPublicParams.strRawImageSavePath);

		if (m_strBarcodeID == _T(""))
		{
			m_strBarcodeID = _T("Barcode_Empty");
		}

		CString strPath;
		strPath.Format(_T("%s\\%s_%s_raw.bmp"), strSavePath, strTime, m_strBarcodeID);

		BYTE* pbyImage = ::g_MemoryManager.GetByteImage((long)IMAGE::ORIG);

		HUBSDK::ImageInterface::ImageSaveBMP(strPath, pbyImage, m_nOriginalImageWidth, m_nOriginalImageHeight, 8, CRect(0, 0, m_nOriginalImageWidth, m_nOriginalImageHeight), TRUE);
	}
}

CString CDetectionAlgorithm::CreateSaveFolder(CString strPath)
{
	if (m_strJigName == _T(""))
	{
		m_strJigName = _T("No_Jig");
	}

	CString strJigName;
	strJigName.Format(_T("\\%s"), m_strJigName);

	CString strModelNo = _T("");
	switch (m_nModelNo)
	{
		case 0: strModelNo = _T("\\No_ModelName"); break;
		case 1:
		case 2: strModelNo = _T("\\77R"); break;
		case 3:
		case 4: strModelNo = _T("\\76F"); break;
		default: strModelNo = _T("\\No_ModelName"); break;
	}

	CString Path = _T("");

	Path = strPath + strJigName;
	CreateDirectory(Path, NULL);
	
	Path = Path + strModelNo;
	CreateDirectory(Path, NULL);


	CTime pCurTime = CTime::GetCurrentTime();

	CString sYear, sMonth, sDay;
	sYear.Format(_T("\\%04d"), pCurTime.GetYear());
	Path = Path + sYear;
	CreateDirectory(Path, NULL);

	sMonth.Format(_T("\\%02d"), pCurTime.GetMonth());
	Path = Path + sMonth;
	CreateDirectory(Path, NULL);

	sDay.Format(_T("\\%02d"), pCurTime.GetDay());
	Path = Path + sDay;
	CreateDirectory(Path, NULL);

	return Path;
}


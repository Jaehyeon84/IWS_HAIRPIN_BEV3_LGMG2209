// DlgResultViewer.cpp: 구현 파일
//

#include "pch.h"
#include "Hairpin2DInsp.h"
#include "DlgResultViewer.h"
#include "afxdialogex.h"

#include "DetectionAlgorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgResultViewer 대화 상자

IMPLEMENT_DYNAMIC(CDlgResultViewer, CDialogEx)

CDlgResultViewer::CDlgResultViewer(CDetectionAlgorithm* pDetectionAlgorithm, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_RESULT_VIEWER, pParent)
	, m_pImageDisplay(nullptr)
	, m_pDetectionAlgorithm(pDetectionAlgorithm)
	, m_bResult(FALSE)
{

}

CDlgResultViewer::~CDlgResultViewer()
{
	delete m_pImageDisplay;
}

void CDlgResultViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESULT, m_listResult);
	DDX_Control(pDX, IDC_STATIC_JIG, m_lblJig);
	DDX_Control(pDX, IDC_STATIC_RESULT, m_lblResult);
	DDX_Control(pDX, IDC_STATIC_H12, m_lblH12);
	DDX_Control(pDX, IDC_STATIC_H3, m_lblH3);
	DDX_Control(pDX, IDC_STATIC_C, m_lblC);
	DDX_Control(pDX, IDC_STATIC_T, m_lblT);
	DDX_Control(pDX, IDC_STATIC_J, m_lblJ);
}


BEGIN_MESSAGE_MAP(CDlgResultViewer, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RESULT, &CDlgResultViewer::OnLvnItemchangedListResult)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RESULT, &CDlgResultViewer::OnNMDblclkListResult)
	ON_BN_CLICKED(IDC_BTN_WHOLE_IMAGE, &CDlgResultViewer::OnBnClickedBtnWholeImage)
END_MESSAGE_MAP()


// CDlgResultViewer 메시지 처리기

BOOL CDlgResultViewer::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 2D Viewer
	m_pImageDisplay = new CImageDisplay;
	m_pImageDisplay->Initialize(IDC_STATIC_2D_VIEWER, this, RGB(50, 50, 50));
	m_pImageDisplay->SetCrossLineColor(RGB(255, 0, 0));
	m_pImageDisplay->SetCustomLineColor(RGB(255, 0, 0));

	m_listResult.SetExtendedStyle(m_listResult.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listResult.InsertColumn(0, _T("NG"), LVCFMT_CENTER, 30);
	m_listResult.InsertColumn(1, _T("Slot"), LVCFMT_CENTER, 45);
	m_listResult.InsertColumn(2, _T("Layer"), LVCFMT_CENTER, 45);
	m_listResult.InsertColumn(3, _T("C"), LVCFMT_CENTER, 75);
	m_listResult.InsertColumn(4, _T("Theta"), LVCFMT_CENTER, 75);
	m_listResult.InsertColumn(5, _T("Jump"), LVCFMT_CENTER, 75);
	m_listResult.InsertColumn(6, _T("H1"), LVCFMT_CENTER, 75);
	m_listResult.InsertColumn(7, _T("H2"), LVCFMT_CENTER, 75);
	m_listResult.InsertColumn(8, _T("H3"), LVCFMT_CENTER, 75);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CDlgResultViewer::ShowResult()
{
	CString strTemp;

	auto criteriaParams = ::g_RecipeManager.m_sCriteriaParams;

	SetCriteriaSpec(m_lblH12, _T("H12"), _T("mm"), criteriaParams.fMin_H12, criteriaParams.fMax_H12);
	SetCriteriaSpec(m_lblH3, _T("H3"), _T("mm"), criteriaParams.fMax_H3);
	SetCriteriaSpec(m_lblC, _T("C"), _T("mm"), criteriaParams.fMin_C, criteriaParams.fMax_C);
	SetCriteriaSpec(m_lblT, _T("T"), _T("deg"), criteriaParams.fMin_T, criteriaParams.fMax_T);
	SetCriteriaSpec(m_lblJ, _T("J"), _T("mm"), criteriaParams.fMin_J, criteriaParams.fMax_J);

	ShowReviewOverlay(TRUE, TRUE);

	UpdateResult();
}

void CDlgResultViewer::UpdateResult()
{
	CString strNG, strSlot, strLayer, strTemp;

	auto roiParams = ::g_RecipeManager.m_sROIParams;

	auto resultList = m_pDetectionAlgorithm->m_vecsCriteriaResult;

	if(VECTOR_SIZE(resultList) <= 0)
	{
		return;
	}

	// List 에 있는 내용을 모두 지운다.
	m_listResult.DeleteAllItems();

	// 결과 정보 보여주기
	long index(0);
	for(long slot = 0; slot < roiParams.nSlotCount; slot++)
	{
		for(long layer = 0; layer < roiParams.nLayerCount / 2; layer++)
		{
			auto result = resultList[slot][layer];

			m_listResult.InsertItem(index, _T(""));

			strNG = _T("");
			if(result.nErrorCode == INSP_ERR_F1)
			{
				strNG.Format(_T("F1"));
			}
			else if(result.nErrorCode == INSP_ERR_F2)
			{
				strNG.Format(_T("F2"));
			}
			else if(result.nErrorCode == INSP_ERR_F12)
			{
				strNG.Format(_T("F12"));
			}
			else if(result.nErrorCode == INSP_ERR_H1)
			{
				strNG.Format(_T("H1"));
			}
			else if(result.nErrorCode == INSP_ERR_H2)
			{
				strNG.Format(_T("H2"));
			}
			else if(result.nErrorCode == INSP_ERR_H3)
			{
				strNG.Format(_T("H3"));
			}
			else if(result.nErrorCode == INSP_ERR_C)
			{
				strNG.Format(_T("C"));
			}
			else if(result.nErrorCode == INSP_ERR_T)
			{
				strNG.Format(_T("T"));
			}
			else if(result.nErrorCode == INSP_ERR_J)
			{
				strNG.Format(_T("J"));
			}
			m_listResult.SetItemText(index, 0, strNG);

			strSlot.Format(_T("%d"), slot + 1);
			m_listResult.SetItemText(index, 1, strSlot);
			strLayer.Format(_T("%d-%d"), layer * 2 + 1, layer * 2 + 2);
			m_listResult.SetItemText(index, 2, strLayer);

			strTemp.Format(_T("%.3f"), result.fC);
			m_listResult.SetItemText(index, 3, strTemp);
			strTemp.Format(_T("%.3f"), result.fT);
			m_listResult.SetItemText(index, 4, strTemp);
			strTemp.Format(_T("%.3f"), result.fJ);
			m_listResult.SetItemText(index, 5, strTemp);
			strTemp.Format(_T("%.3f"), result.fH1);
			m_listResult.SetItemText(index, 6, strTemp);
			strTemp.Format(_T("%.3f"), result.fH2);
			m_listResult.SetItemText(index, 7, strTemp);
			strTemp.Format(_T("%.3f"), result.fH3);
			m_listResult.SetItemText(index, 8, strTemp);

			index++;
		}
	}

	SetJigName(m_pDetectionAlgorithm->m_strJigName);
	SetResult(m_pDetectionAlgorithm->m_strTotalResult);
}


void CDlgResultViewer::OnLvnItemchangedListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	CString strTemp;

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if((pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)
	{
		int nItem = pNMLV->iItem;

		auto roiParams = ::g_RecipeManager.m_sROIParams;
		long slot = nItem / (roiParams.nLayerCount / 2);
		long layer = nItem - slot * (roiParams.nLayerCount / 2);

		SetOverlay(slot, layer, FALSE);
	}

	*pResult = 0;
}

void CDlgResultViewer::OnNMDblclkListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int nItem = pNMItemActivate->iItem;

	auto roiParams = ::g_RecipeManager.m_sROIParams;
	long slot = nItem / (roiParams.nLayerCount / 2);
	long layer = nItem - slot * (roiParams.nLayerCount / 2);

	SetOverlay(slot, layer, TRUE);

	*pResult = 0;
}


void CDlgResultViewer::SetOverlay(const long slot, const long layer, const BOOL bDBClick)
{
	CString strTemp;

	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto layerPairPoint = m_pDetectionAlgorithm->m_vecptLayerPairOverlayPoint;

	if(VECTOR_SIZE(layerPairPoint) <= 0)
	{
		return;
	}

	if(bDBClick == TRUE)
	{
		ShowReviewOverlay(TRUE, TRUE, slot, layer, TRUE);
	}
	else
	{
		ShowReviewOverlay(TRUE, TRUE);
	}

	auto points = layerPairPoint[slot][(roiParams.nLayerCount / 2) - 1 - layer];

	CRect ellipse1(points.x - 99, points.y - 99, points.x + 99, points.y + 99);
	CRect ellipse2(points.x - 100, points.y - 100, points.x + 100, points.y + 100);
	CRect ellipse3(points.x - 101, points.y - 101, points.x + 101, points.y + 101);
	m_pImageDisplay->Overlay_AddEllipse(ellipse1, RGB(0, 255, 0));
	m_pImageDisplay->Overlay_AddEllipse(ellipse2, RGB(0, 255, 0));
	m_pImageDisplay->Overlay_AddEllipse(ellipse3, RGB(0, 255, 0));

	auto result = m_pDetectionAlgorithm->m_vecsCriteriaResult[slot][layer];
	auto imageCenter = CPoint(m_pDetectionAlgorithm->m_nSubOriginalImageWidth / 2, m_pDetectionAlgorithm->m_nSubOriginalImageHeight / 2);

	auto position = CPoint(imageCenter.x - 1000, imageCenter.y - 1700);
	auto positionH1 = CPoint(imageCenter.x - 1000, imageCenter.y - 1300);
	auto positionH2 = CPoint(imageCenter.x - 1000, imageCenter.y - 900);
	auto positionH3 = CPoint(imageCenter.x - 1000, imageCenter.y - 500);
	auto positionC = CPoint(imageCenter.x - 1000, imageCenter.y - 100);
	auto positionT = CPoint(imageCenter.x - 1000, imageCenter.y + 300);
	auto positionJ = CPoint(imageCenter.x - 1000, imageCenter.y + 700);
	auto positionOKNG = CPoint(imageCenter.x - 1000, imageCenter.y + 1100);

	COLORREF color(RGB(0, 255, 0));

	strTemp.Format(_T("Slot : %d, Layer : %d-%d"), slot + 1, layer * 2 + 1, layer * 2 + 2);
	m_pImageDisplay->Overlay_AddText(position, strTemp, color, 250, _T("Tahoma"));

	color = result.nErrorCode == INSP_ERR_H1 ? RGB(255, 0, 0) : RGB(0, 255, 0);
	strTemp.Format(_T("H1 : %.3f mm"), result.fH1);
	m_pImageDisplay->Overlay_AddText(positionH1, strTemp, color, 300, _T("Tahoma"));

	color = result.nErrorCode == INSP_ERR_H2 ? RGB(255, 0, 0) : RGB(0, 255, 0);
	strTemp.Format(_T("H2 : %.3f mm"), result.fH2);
	m_pImageDisplay->Overlay_AddText(positionH2, strTemp, color, 300, _T("Tahoma"));

	color = result.nErrorCode == INSP_ERR_H3 ? RGB(255, 0, 0) : RGB(0, 255, 0);
	strTemp.Format(_T("H3 : %.3f mm"), result.fH3);
	m_pImageDisplay->Overlay_AddText(positionH3, strTemp, color, 300, _T("Tahoma"));

	color = result.nErrorCode == INSP_ERR_C ? RGB(255, 0, 0) : RGB(0, 255, 0);
	strTemp.Format(_T("C : %.3f mm"), result.fC);
	m_pImageDisplay->Overlay_AddText(positionC, strTemp, color, 300, _T("Tahoma"));

	color = result.nErrorCode == INSP_ERR_T ? RGB(255, 0, 0) : RGB(0, 255, 0);
	strTemp.Format(_T("T : %.3f deg"), result.fT);
	m_pImageDisplay->Overlay_AddText(positionT, strTemp, color, 300, _T("Tahoma"));

	color = result.nErrorCode == INSP_ERR_J ? RGB(255, 0, 0) : RGB(0, 255, 0);
	strTemp.Format(_T("J : %.3f mm"), result.fJ);
	m_pImageDisplay->Overlay_AddText(positionJ, strTemp, color, 300, _T("Tahoma"));

	color = result.nErrorCode == INSP_ERR_OK ? RGB(0, 255, 0) : RGB(255, 0, 0);
	CString strNG;
	strNG = _T("OK");
	if(result.nErrorCode == INSP_ERR_F1)
	{
		strNG.Format(_T("NG : F1"));
	}
	else if(result.nErrorCode == INSP_ERR_F2)
	{
		strNG.Format(_T("NG : F2"));
	}
	else if(result.nErrorCode == INSP_ERR_F12)
	{
		strNG.Format(_T("NG : F12"));
	}
	else if(result.nErrorCode == INSP_ERR_H1)
	{
		strNG.Format(_T("NG : H1"));
	}
	else if(result.nErrorCode == INSP_ERR_H2)
	{
		strNG.Format(_T("NG : H2"));
	}
	else if(result.nErrorCode == INSP_ERR_H3)
	{
		strNG.Format(_T("NG : H3"));
	}
	else if(result.nErrorCode == INSP_ERR_C)
	{
		strNG.Format(_T("NG : C"));
	}
	else if(result.nErrorCode == INSP_ERR_T)
	{
		strNG.Format(_T("NG : T"));
	}
	else if(result.nErrorCode == INSP_ERR_J)
	{
		strNG.Format(_T("NG : J"));
	}
	m_pImageDisplay->Overlay_AddText(positionOKNG, strNG, color, 300, _T("Tahoma"));

	m_pImageDisplay->Overlay_Show(TRUE);
}

void CDlgResultViewer::ShowReviewOverlay(const BOOL bRemoveOverlay, const BOOL bShowOverlay, const long slot, const long layer, const BOOL bSelect)
{
	if(m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		return;
	}

	CString strTemp;

	auto params = ::g_RecipeManager.m_sROIParams;

	auto imageWidth = m_pDetectionAlgorithm->m_nSubOriginalImageWidth;
	auto imageHeight = m_pDetectionAlgorithm->m_nSubOriginalImageHeight;

	BYTE* pbyImage = ::g_MemoryManager.GetByteImage((long)IMAGE::SUB_ORIG);
	auto layerPairPoint = m_pDetectionAlgorithm->m_vecptLayerPairOverlayPoint;

	if(bRemoveOverlay == TRUE)
	{
		m_pImageDisplay->Overlay_RemoveAll();
	}

	m_pImageDisplay->SetWholeImage();
	m_pImageDisplay->SetImage(pbyImage, imageWidth, imageHeight, imageWidth);

	if(bSelect == TRUE)
	{
		auto pt = layerPairPoint[slot][(params.nLayerCount / 2) - 1 - layer];
		CRect fov(pt.x - 300, pt.y - 300, pt.x + 300, pt.y + 300);

		m_pImageDisplay->SetFieldOfView(fov);
	}

	auto ptResult = m_pDetectionAlgorithm->m_vecptResult;
	auto inspResult = m_pDetectionAlgorithm->m_vecsCriteriaResult;

	if(VECTOR_SIZE(ptResult) <= 0 || VECTOR_SIZE(inspResult) <= 0)
	{
		return;
	}

	if(bShowOverlay == TRUE)
	{
		CString strNo;
		for(long slot = 0; slot < params.nSlotCount; slot++)
		{
			for(long layer = 0; layer < params.nLayerCount; layer++)
			{
				auto centerPoint = ptResult[slot][layer];

				CRect rtCenter(0, 0, 0, 0);
				rtCenter.left = centerPoint.x - 5;
				rtCenter.top = centerPoint.y - 5;
				rtCenter.right = centerPoint.x + 5;
				rtCenter.bottom = centerPoint.y + 5;

				auto replace = m_pDetectionAlgorithm->m_vecbReplace3DPosition[slot][layer];
				if (replace == TRUE)
				{
					m_pImageDisplay->Overlay_AddEllipse2(rtCenter, RGB(0, 0, 255));
				}
				else
				{
					m_pImageDisplay->Overlay_AddEllipse2(rtCenter, RGB(255, 0, 0));
				}
			}
		}

		if(VECTOR_SIZE(layerPairPoint) <= 0)
		{
			return;
		}

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
				m_pImageDisplay->Overlay_AddEllipse(ellipse1, RGB(255, 0, 0));
				m_pImageDisplay->Overlay_AddEllipse(ellipse2, RGB(255, 0, 0));
				m_pImageDisplay->Overlay_AddEllipse(ellipse3, RGB(255, 0, 0));
			}
		}

		auto startPoint = m_pDetectionAlgorithm->m_vecptSlotLineOverlayStartPoint;
		auto endPoint = m_pDetectionAlgorithm->m_vecptSlotLineOverlayEndPoint;
		auto slotNo1Point = m_pDetectionAlgorithm->m_vecptSlotNo1OverlayPoint;
		auto slotNo2Point = m_pDetectionAlgorithm->m_vecptSlotNo2OverlayPoint;

		if(VECTOR_SIZE(startPoint) <= 0 || VECTOR_SIZE(endPoint) <= 0 || VECTOR_SIZE(slotNo1Point) <= 0 || VECTOR_SIZE(slotNo2Point) <= 0)
		{
			return;
		}

		for(long idx = 0; idx < (long)startPoint.size(); idx++)
		{
			long index = params.nSlotCount - params.nStartSlotIndex + 1 + idx + 1; // ehjang. start slot index
			if(index > params.nSlotCount)
			{
				index -= params.nSlotCount;
			}

			strTemp.Format(_T("%d"), index);

			m_pImageDisplay->Overlay_AddLine(startPoint[idx], endPoint[idx], RGB(255, 255, 0));
			m_pImageDisplay->Overlay_AddText(slotNo1Point[idx], strTemp, RGB(255, 255, 0), 150, _T("Tahoma"));
			m_pImageDisplay->Overlay_AddText(slotNo2Point[idx], strTemp, RGB(255, 255, 0), 150, _T("Tahoma"));
		}
	}

	m_pImageDisplay->Overlay_Show(TRUE);
}

void CDlgResultViewer::SetJigName(const CString strJig)
{
	m_lblJig.SetText(strJig);
	m_lblJig.SetFontSize(40);
	m_lblJig.SetFontBold(TRUE);
	m_lblJig.SetTextColor(RGB(0, 0, 0));
	m_lblJig.SetBorder(TRUE);
}

void CDlgResultViewer::SetResult(const CString strResult)
{
	CString strInspResult;

	if(strResult != _T(""))
	{
		if(m_bResult)
		{
			strInspResult = _T("/") + strResult;
		}
		else
		{
			strInspResult = strResult + _T("/");
		}
	}

	COLORREF color;
	if(m_pDetectionAlgorithm->m_nNGCount == 0)
	{
		color = RGB(0, 255, 0);
	}
	else
	{
		strInspResult.Format(_T("%s (%d)"), strInspResult, m_pDetectionAlgorithm->m_nNGCount);
		color = RGB(255, 0, 0);
	}

	m_lblResult.SetText(strInspResult);
	m_lblResult.SetFontSize(40);
	m_lblResult.SetFontBold(TRUE);
	m_lblResult.SetTextColor(color);
	m_lblResult.SetBorder(TRUE);

	m_bResult = !m_bResult;
}

void CDlgResultViewer::SetCriteriaSpec(CLabel &lbl, const CString strName, const CString strmm, const float max)
{
	CString strTemp;
	strTemp.Format(_T("  %s: ~ %.3f %s"), strName, max, strmm);

	lbl.SetText(strTemp);
	lbl.SetFontSize(30);
	lbl.SetFontBold(TRUE);
	lbl.SetTextColor(RGB(0, 0, 0));
}

void CDlgResultViewer::SetCriteriaSpec(CLabel &lbl, const CString strName, const CString strmm, const float min, const float max)
{
	CString strTemp;
	strTemp.Format(_T("  %s: %.3f ~ %.3f %s"), strName, min, max, strmm);

	lbl.SetText(strTemp);
	lbl.SetFontSize(30);
	lbl.SetFontBold(TRUE);
	lbl.SetTextColor(RGB(0, 0, 0));
}


void CDlgResultViewer::OnBnClickedBtnWholeImage()
{
	m_pImageDisplay->SetWholeImage();
}

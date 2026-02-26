// DlgAlign2.cpp: 구현 파일
//

#include "pch.h"
#include "Hairpin2DInsp.h"
#include "Hairpin2DInspDlg.h"
#include "DlgCriteria.h"
#include "afxdialogex.h"

#include "DetectionAlgorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgCriteria 대화 상자

IMPLEMENT_DYNAMIC(CDlgCriteria, CDialog)

CDlgCriteria::CDlgCriteria(CHairpin2DInspDlg* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_CRITERIA, pParent)
	, m_pParent(pParent)
	, m_pImageDisplay(m_pParent->m_pImageDisplay)
	, m_pDetectionAlgorithm(m_pParent->m_pDetectionAlgorithm)
	, m_fMin_C(1.f)
	, m_fMax_C(2.5f)
	, m_fMin_T(0.f)
	, m_fMax_T(360.f)
	, m_fMin_J(0.f)
	, m_fMax_J(20.f)
	, m_fMin_H12(1.f)
	, m_fMax_H12(10.f)
	, m_fMax_H3(10.f)
	, m_bOriginalImageNow(FALSE)
	, m_bUnrolledImageNow(FALSE)
{

}

CDlgCriteria::~CDlgCriteria()
{
}

void CDlgCriteria::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_C_MIN, m_fMin_C);
	DDX_Text(pDX, IDC_EDIT_C_MAX, m_fMax_C);
	DDX_Text(pDX, IDC_EDIT_T_MIN, m_fMin_T);
	DDX_Text(pDX, IDC_EDIT_T_MAX, m_fMax_T);
	DDX_Text(pDX, IDC_EDIT_J_MIN, m_fMin_J);
	DDX_Text(pDX, IDC_EDIT_J_MAX, m_fMax_J);
	DDX_Text(pDX, IDC_EDIT_H12_MIN, m_fMin_H12);
	DDX_Text(pDX, IDC_EDIT_H12_MAX, m_fMax_H12);
	DDX_Text(pDX, IDC_EDIT_H3_MAX, m_fMax_H3);
	DDX_Control(pDX, IDC_LIST_RESULT, m_listResult);
	DDX_Control(pDX, IDC_CHECK_REPLACE_3D_RESULT, m_checkReplace3DResult);
}


BEGIN_MESSAGE_MAP(CDlgCriteria, CDialog)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CDlgCriteria::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_INSPECTION, &CDlgCriteria::OnBnClickedBtnInspection)
	ON_BN_CLICKED(IDC_BTN_ORIGINAL_IMAGE2, &CDlgCriteria::OnBnClickedBtnOriginalImage2)
	ON_BN_CLICKED(IDC_BTN_UNROLLED_IMAGE2, &CDlgCriteria::OnBnClickedBtnUnrolledImage2)
	ON_BN_CLICKED(IDC_BTN_UPDATE_RESULT, &CDlgCriteria::OnBnClickedBtnUpdateResult)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RESULT, &CDlgCriteria::OnLvnItemchangedListResult)
	ON_BN_CLICKED(IDC_CHECK_REPLACE_3D_RESULT, &CDlgCriteria::OnBnClickedCheckReplace3dResult)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RESULT, &CDlgCriteria::OnNMDblclkListResult)
	ON_BN_CLICKED(IDC_BTN_WHOLE_IMAGE, &CDlgCriteria::OnBnClickedBtnWholeImage)
END_MESSAGE_MAP()


// CDlgCriteria 메시지 처리기


BOOL CDlgCriteria::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	return TRUE;
}

void CDlgCriteria::UpdateUI()
{
	auto criteriaParams = ::g_RecipeManager.m_sCriteriaParams;
	auto publicParams = ::g_RecipeManager.m_sPublicParams;

	UpdateData(TRUE);

	m_fMin_C = criteriaParams.fMin_C;
	m_fMax_C = criteriaParams.fMax_C;
	m_fMin_T = criteriaParams.fMin_T;
	m_fMax_T = criteriaParams.fMax_T;
	m_fMin_J = criteriaParams.fMin_J;
	m_fMax_J = criteriaParams.fMax_J;
	m_fMin_H12 = criteriaParams.fMin_H12;
	m_fMax_H12 = criteriaParams.fMax_H12;
	m_fMax_H3 = criteriaParams.fMax_H3;

	m_checkReplace3DResult.SetCheck(publicParams.bReplace3DResult);

	UpdateData(FALSE);
}

void CDlgCriteria::UpdateParameters()
{
	UpdateData(TRUE);

	auto &criteriaParams = ::g_RecipeManager.m_sCriteriaParams;
	auto &publicParams = ::g_RecipeManager.m_sPublicParams;

	criteriaParams.fMin_C = m_fMin_C;
	criteriaParams.fMax_C = m_fMax_C;
	criteriaParams.fMin_T = m_fMin_T;
	criteriaParams.fMax_T = m_fMax_T;
	criteriaParams.fMin_J = m_fMin_J;
	criteriaParams.fMax_J = m_fMax_J;
	criteriaParams.fMin_H12 = m_fMin_H12;
	criteriaParams.fMax_H12 = m_fMax_H12;
	criteriaParams.fMax_H3 = m_fMax_H3;

	publicParams.bReplace3DResult = m_checkReplace3DResult.GetCheck();
}

void CDlgCriteria::Inspection()
{
	HUBSDK::ThreadedProgress progressX(_T("Processing now..."));
	
	UpdateParameters();

	m_pDetectionAlgorithm->DoInspection();

	m_pParent->ShowCriteriaOverlay_Original(TRUE, TRUE);
}

void CDlgCriteria::OnBnClickedBtnUpdateResult()
{
	HUBSDK::ThreadedProgress progressX(_T("Processing now..."));

	CString strNG, strSlot, strLayer, strTemp;

	auto roiParams = ::g_RecipeManager.m_sROIParams;

	Inspection();

	auto resultList = m_pDetectionAlgorithm->m_vecsCriteriaResult;

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

	m_pParent->SetResult(m_pDetectionAlgorithm->m_strTotalResult);

	m_bOriginalImageNow = TRUE;
	m_bUnrolledImageNow = FALSE;
}

void CDlgCriteria::OnBnClickedBtnApply()
{
	UpdateParameters();
}

void CDlgCriteria::OnBnClickedBtnInspection()
{
	OnBnClickedBtnUpdateResult();
}

void CDlgCriteria::OnBnClickedBtnOriginalImage2()
{
	m_pParent->ShowCriteriaOverlay_Original(TRUE, TRUE);

	m_bOriginalImageNow = TRUE;
	m_bUnrolledImageNow = FALSE;
}

void CDlgCriteria::OnBnClickedBtnUnrolledImage2()
{
	m_pParent->ShowROI(TRUE, TRUE);
	m_pParent->ShowCriteriaOverlay_Unrolled(FALSE, TRUE);

	m_bOriginalImageNow = FALSE;
	m_bUnrolledImageNow = TRUE;
}

void CDlgCriteria::OnLvnItemchangedListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if((pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)
	{
		int nItem = pNMLV->iItem;

		auto roiParams = ::g_RecipeManager.m_sROIParams;

		long slot = nItem / (roiParams.nLayerCount / 2);
		long layer = nItem - slot * (roiParams.nLayerCount / 2);

		if(m_bOriginalImageNow == TRUE && m_bUnrolledImageNow == FALSE)
		{
			SetOverlay(slot, layer, (roiParams.nLayerCount / 2), FALSE);
		}
	}

	*pResult = 0;
}

void CDlgCriteria::OnNMDblclkListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int nItem = pNMItemActivate->iItem;

	auto roiParams = ::g_RecipeManager.m_sROIParams;

	long slot = nItem / (roiParams.nLayerCount / 2);
	long layer = nItem - slot * (roiParams.nLayerCount / 2);

	SetOverlay(slot, layer, (roiParams.nLayerCount / 2), TRUE);

	*pResult = 0;
}

void CDlgCriteria::SetOverlay(const long slot, const long layer, const long layerCount, const BOOL bDBClick)
{
	auto layerPairPoint = m_pDetectionAlgorithm->m_vecptLayerPairOverlayPoint;

	if(VECTOR_SIZE(layerPairPoint) <= 0)
	{
		return;
	}

	if(m_bOriginalImageNow == TRUE && m_bUnrolledImageNow == FALSE)
	{
		if(bDBClick == TRUE)
		{
			m_pParent->ShowCriteriaOverlay_Original(TRUE, TRUE, slot, layer, TRUE);
		}
		else
		{
			m_pParent->ShowCriteriaOverlay_Original(TRUE, TRUE);
		}

		auto points = layerPairPoint[slot][layerCount - 1 - layer];

		CRect ellipse1(points.x - 99, points.y - 99, points.x + 99, points.y + 99);
		CRect ellipse2(points.x - 100, points.y - 100, points.x + 100, points.y + 100);
		CRect ellipse3(points.x - 101, points.y - 101, points.x + 101, points.y + 101);
		m_pImageDisplay->Overlay_AddEllipse(ellipse1, RGB(0, 255, 0));
		m_pImageDisplay->Overlay_AddEllipse(ellipse2, RGB(0, 255, 0));
		m_pImageDisplay->Overlay_AddEllipse(ellipse3, RGB(0, 255, 0));
	}
	else if(m_bOriginalImageNow == FALSE && m_bUnrolledImageNow == TRUE)
	{
		if(bDBClick == TRUE)
		{
			m_pParent->ShowCriteriaOverlay_Unrolled(FALSE, TRUE, slot, layerCount - 1 - layer, TRUE);
		}
		else
		{
			m_pParent->ShowCriteriaOverlay_Unrolled(FALSE, TRUE);
		}
	}

	m_pImageDisplay->Overlay_Show(TRUE);
}

void CDlgCriteria::OnBnClickedCheckReplace3dResult()
{
	auto &publicParams = ::g_RecipeManager.m_sPublicParams;

	publicParams.bReplace3DResult = m_checkReplace3DResult.GetCheck();
}


void CDlgCriteria::OnBnClickedBtnWholeImage()
{
	m_pImageDisplay->SetWholeImage();
}

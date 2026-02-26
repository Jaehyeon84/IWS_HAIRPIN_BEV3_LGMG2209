// DlgInterpolationHairpin.cpp: 구현 파일
//

#include "pch.h"
#include "Hairpin2DInsp.h"
#include "Hairpin2DInspDlg.h"
#include "DlgInterpolation.h"
#include "afxdialogex.h"

#include "DetectionAlgorithm.h"
#include "DlgImageProcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgInterpolation 대화 상자

IMPLEMENT_DYNAMIC(CDlgInterpolation, CDialog)

CDlgInterpolation::CDlgInterpolation(CHairpin2DInspDlg* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_INTERPOLATION, pParent)
	, m_pParent(pParent)
	, m_pImageDisplay(m_pParent->m_pImageDisplay)
	, m_pDetectionAlgorithm(m_pParent->m_pDetectionAlgorithm)
	, m_nMinBlobSize(0)
	, m_nMaxBlobCount(0)
	, m_nBlobMergeDist(0)
{

}

CDlgInterpolation::~CDlgInterpolation()
{
}

void CDlgInterpolation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCROLLBAR_THRESHOLD, m_scrollbarThreshold);
	DDX_Control(pDX, IDC_SCROLLBAR_THRESHOLD_HIGH, m_scrollbarThreshold_High);
	DDX_Control(pDX, IDC_COMBO_THRESHOLD, m_cmbThreshold);
	DDX_Control(pDX, IDC_CHECK_THRRESHOLD_REVERSE, m_checkThresholdReverse);
	DDX_Text(pDX, IDC_EDIT_NOISE_FILTERING_SIZE, m_nHighThresholdRegionNoiseFilteringSize);
	DDX_Text(pDX, IDC_EDIT_MIN_SIZE, m_nMinBlobSize);
	DDX_Text(pDX, IDC_EDIT_MAX_BLOB_COUNT, m_nMaxBlobCount);
	DDX_Text(pDX, IDC_EDIT_MERGEDISTANCE, m_nBlobMergeDist);
	DDX_Control(pDX, IDC_LIST_BLOBLIST, m_listBlobInfo);
	DDX_Control(pDX, IDC_CHECK_USEMERGEBLOB, m_checkBlobMerge);
	DDX_Control(pDX, IDC_CHECK_INTERPOLATION_X, m_checkInterpolationX);
	DDX_Control(pDX, IDC_CHECK_INTERPOLATION_Y, m_checkInterpolationY);
}


BEGIN_MESSAGE_MAP(CDlgInterpolation, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_IMAGE_PROC_MANAGER, &CDlgInterpolation::OnBnClickedBtnImageProcManager)
	ON_CBN_SELCHANGE(IDC_COMBO_THRESHOLD, &CDlgInterpolation::OnCbnSelchangeComboThreshold)
	ON_BN_CLICKED(IDC_CHECK_THRRESHOLD_REVERSE, &CDlgInterpolation::OnBnClickedCheckThrresholdReverse)
	ON_BN_CLICKED(IDC_BTN_UPDATEBLOBLIST, &CDlgInterpolation::OnBnClickedBtnUpdatebloblist)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CDlgInterpolation::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_INSPECTION, &CDlgInterpolation::OnBnClickedBtnInspection)
	ON_BN_CLICKED(IDC_CHECK_USEMERGEBLOB, &CDlgInterpolation::OnBnClickedCheckUsemergeblob)
	ON_BN_CLICKED(IDC_BTN_ORIGINAL_IMAGE, &CDlgInterpolation::OnBnClickedBtnOriginalImage)
	ON_BN_CLICKED(IDC_BTN_PROCEEDED_IMAGE, &CDlgInterpolation::OnBnClickedBtnProceededImage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BLOBLIST, &CDlgInterpolation::OnLvnItemchangedListBloblist)
	ON_BN_CLICKED(IDC_BTN_THRESHOLD_IMAGE, &CDlgInterpolation::OnBnClickedBtnThresholdImage)
	ON_BN_CLICKED(IDC_BTN_UNROLLED_IMAGE, &CDlgInterpolation::OnBnClickedBtnUnrolledImage)
	ON_BN_CLICKED(IDC_CHECK_INTERPOLATION_X, &CDlgInterpolation::OnBnClickedCheckInterpolationX)
	ON_BN_CLICKED(IDC_CHECK_INTERPOLATION_Y, &CDlgInterpolation::OnBnClickedCheckInterpolationY)
END_MESSAGE_MAP()


// CDlgInterpolation 메시지 처리기

BOOL CDlgInterpolation::OnInitDialog()
{
	CDialog::OnInitDialog();

	auto params = ::g_RecipeManager.m_sInterHairpinParams;

	m_cmbThreshold.AddString(_T("Normal"));
	m_cmbThreshold.AddString(_T("Adaptive"));
	m_cmbThreshold.AddString(_T("Hysteresis"));

	m_checkThresholdReverse.SetCheck(params.bThresholdReverse);

	m_listBlobInfo.SetExtendedStyle(m_listBlobInfo.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listBlobInfo.InsertColumn(0, _T("No."), LVCFMT_CENTER, 40);
	m_listBlobInfo.InsertColumn(1, _T("Area(px)"), LVCFMT_CENTER, 80);
	m_listBlobInfo.InsertColumn(2, _T("Width(px)"), LVCFMT_CENTER, 80);
	m_listBlobInfo.InsertColumn(3, _T("Height(px)"), LVCFMT_CENTER, 80);
	m_listBlobInfo.InsertColumn(4, _T("AspectRatio"), LVCFMT_CENTER, 80);

	return TRUE;
}

void CDlgInterpolation::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nID = pScrollBar->GetDlgCtrlID();

	auto &params = ::g_RecipeManager.m_sInterHairpinParams;

	UpdateData(TRUE);

	int step = 1;
	int Mstep = 10;
	int nNext, nmin, nmax;

	pScrollBar->GetScrollRange(&nmin, &nmax);

	switch(nSBCode)
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		nNext = nPos;
		if(nNext > nmax) nNext = nmax;
		else if(nNext < nmin) nNext = nmin;
		pScrollBar->SetScrollPos(nNext);
		break;
	case SB_LINEDOWN:
		nNext = pScrollBar->GetScrollPos();
		if(nNext + step <= nmax) {
			pScrollBar->SetScrollPos(nNext + step);
			nNext += step;
		}
		break;
	case SB_LINEUP:
		nNext = pScrollBar->GetScrollPos();
		if(nNext - step >= nmin) {
			pScrollBar->SetScrollPos(nNext - step);
			nNext -= step;
		}
		break;
	case SB_PAGEDOWN:
		nNext = pScrollBar->GetScrollPos();
		if(nNext + Mstep <= nmax) {
			pScrollBar->SetScrollPos(nNext + Mstep);
			nNext += Mstep;
		}
		else {
			nNext = nmax;
			pScrollBar->SetScrollPos(nNext);
		}
		break;
	case SB_PAGEUP:
		nNext = pScrollBar->GetScrollPos();
		if(nNext - Mstep >= nmin) {
			pScrollBar->SetScrollPos(nNext - Mstep);
			nNext -= Mstep;
		}
		else {
			nNext = nmin;
			pScrollBar->SetScrollPos(nNext);
		}
		break;
	default:
		return;
		break;
	}

	int pos = pScrollBar->GetScrollPos();

	switch(nID)
	{
	case IDC_SCROLLBAR_THRESHOLD:
		if(params.nSelectThreshold == (long)THRESHOLD::NORMAL)
		{
			params.nThresholdValue = pos;
			m_scrollbarThreshold.SetScrollPos(params.nThresholdValue, TRUE);
			SetDlgItemInt(IDC_EDIT_THRESHOLD, params.nThresholdValue);
		}
		else if(params.nSelectThreshold == (long)THRESHOLD::ADAPTIVE)
		{
			params.nAdaptiveOffset = pos;
			m_scrollbarThreshold.SetScrollPos(params.nAdaptiveOffset, TRUE);
			SetDlgItemInt(IDC_EDIT_THRESHOLD, params.nAdaptiveOffset);
		}
		else if(params.nSelectThreshold == (long)THRESHOLD::HYSTERISIS)
		{
			params.nHysteresisLow = pos;
			m_scrollbarThreshold.SetScrollPos(params.nHysteresisLow, TRUE);
			SetDlgItemInt(IDC_EDIT_THRESHOLD, params.nHysteresisLow);
		}
		break;
	case IDC_SCROLLBAR_THRESHOLD_HIGH:
		if(params.nSelectThreshold == (long)THRESHOLD::HYSTERISIS)
		{
			params.nHysteresisHigh = pos;
			m_scrollbarThreshold_High.SetScrollPos(params.nHysteresisHigh, TRUE);
			SetDlgItemInt(IDC_EDIT_THRESHOLD_HIGH, params.nHysteresisHigh);
		}
		break;
	}

	m_pParent->ShowROI(TRUE, TRUE);
	m_pParent->UpdateInterThresholdImage(TRUE);

	UpdateData(FALSE);

	Invalidate(FALSE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgInterpolation::UpdateUI()
{
	CString strTemp;
	auto params = ::g_RecipeManager.m_sInterHairpinParams;

	UpdateData(TRUE);

	m_cmbThreshold.SetCurSel(params.nSelectThreshold);
	if(params.nSelectThreshold == (long)THRESHOLD::NORMAL)
	{
		m_scrollbarThreshold.SetScrollRange(0, 255);
		m_scrollbarThreshold.SetScrollPos(params.nThresholdValue, TRUE);
		SetDlgItemInt(IDC_EDIT_THRESHOLD, params.nThresholdValue);

		GetDlgItem(IDC_SCROLLBAR_THRESHOLD_HIGH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_THRESHOLD_HIGH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_NOISE_FILTERING_SIZE)->EnableWindow(FALSE);
	}
	else if(params.nSelectThreshold == (long)THRESHOLD::ADAPTIVE)
	{
		m_scrollbarThreshold.SetScrollRange(-255, 255);
		m_scrollbarThreshold.SetScrollPos(params.nAdaptiveOffset, TRUE);
		SetDlgItemInt(IDC_EDIT_THRESHOLD, params.nAdaptiveOffset);
	
		GetDlgItem(IDC_SCROLLBAR_THRESHOLD_HIGH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_THRESHOLD_HIGH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_NOISE_FILTERING_SIZE)->EnableWindow(FALSE);
	}
	else if(params.nSelectThreshold == (long)THRESHOLD::HYSTERISIS)
	{
		m_scrollbarThreshold.SetScrollRange(0, 255);
		m_scrollbarThreshold.SetScrollPos(params.nHysteresisLow, TRUE);
		SetDlgItemInt(IDC_EDIT_THRESHOLD, params.nHysteresisLow);

		m_scrollbarThreshold_High.SetScrollRange(0, 255);
		m_scrollbarThreshold_High.SetScrollPos(params.nHysteresisHigh, TRUE);
		SetDlgItemInt(IDC_EDIT_THRESHOLD_HIGH, params.nHysteresisHigh);

		GetDlgItem(IDC_SCROLLBAR_THRESHOLD_HIGH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_THRESHOLD_HIGH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_NOISE_FILTERING_SIZE)->EnableWindow(TRUE);
	}

	m_checkThresholdReverse.SetCheck(params.bThresholdReverse);
	m_nHighThresholdRegionNoiseFilteringSize = params.nHighThresholdRegionNoiseFilteringSize;
	m_nMinBlobSize = params.nMinBlobSize;
	m_nMaxBlobCount = params.nMaxBlobCount;
	m_checkBlobMerge.SetCheck(params.bBlobMerge);
	m_nBlobMergeDist = params.nBlobMergeDist;
	m_checkInterpolationX.SetCheck(params.bInterpolationX);
	m_checkInterpolationY.SetCheck(params.bInterpolationY);

	UpdateData(FALSE);
}

void CDlgInterpolation::UpdateParameters()
{
	UpdateData(TRUE);

	auto &params = ::g_RecipeManager.m_sInterHairpinParams;

	if(params.nSelectThreshold == (long)THRESHOLD::NORMAL)
	{
		params.nThresholdValue = m_scrollbarThreshold.GetScrollPos();
	}
	else if(params.nSelectThreshold == (long)THRESHOLD::ADAPTIVE)
	{
		params.nAdaptiveOffset = m_scrollbarThreshold.GetScrollPos();
	}
	else if(params.nSelectThreshold == (long)THRESHOLD::HYSTERISIS)
	{
		params.nHysteresisLow = m_scrollbarThreshold.GetScrollPos();
		params.nHysteresisHigh = m_scrollbarThreshold_High.GetScrollPos();
	}

	params.bThresholdReverse = m_checkThresholdReverse.GetCheck();
	params.nHighThresholdRegionNoiseFilteringSize = m_nHighThresholdRegionNoiseFilteringSize;
	params.nMinBlobSize = m_nMinBlobSize;
	params.nMaxBlobCount = m_nMaxBlobCount;
	params.bBlobMerge = m_checkBlobMerge.GetCheck();
	params.nBlobMergeDist = m_nBlobMergeDist;
	params.bInterpolationX = m_checkInterpolationX.GetCheck();
	params.bInterpolationY = m_checkInterpolationY.GetCheck();
}

void CDlgInterpolation::OnBnClickedCheckThrresholdReverse()
{
	auto &params = ::g_RecipeManager.m_sInterHairpinParams;

	params.bThresholdReverse = m_checkThresholdReverse.GetCheck();

	m_pParent->UpdateInterThresholdImage(TRUE);
}

void CDlgInterpolation::OnBnClickedCheckUsemergeblob()
{
	auto &params = ::g_RecipeManager.m_sInterHairpinParams;

	params.bBlobMerge = m_checkBlobMerge.GetCheck();
}

void CDlgInterpolation::OnBnClickedBtnOriginalImage()
{
	m_pParent->ShowOriginal(TRUE, TRUE);
}

void CDlgInterpolation::OnBnClickedBtnUnrolledImage()
{
	m_pParent->ShowMakeUnrolled(FALSE);
}

void CDlgInterpolation::OnBnClickedBtnProceededImage()
{
	m_pParent->ShowInterpolation(IMAGE::INTER_PROC, TRUE, TRUE);
}

void CDlgInterpolation::OnBnClickedBtnThresholdImage()
{
	m_pParent->ShowInterpolation(IMAGE::INTER_THRESHOLD, TRUE, TRUE);
}

void CDlgInterpolation::OnBnClickedBtnUpdatebloblist()
{
	HUBSDK::ThreadedProgress progressX(_T("Processing now..."));

	CString strNo, strTemp;

	m_pImageDisplay->Overlay_RemoveAll();

	UpdateParameters();

	m_pDetectionAlgorithm->DoMakeSubOriginalImage();
	m_pDetectionAlgorithm->DoMakeUnrolledImage();
	m_pDetectionAlgorithm->DoSetDetectionROI();
	m_pDetectionAlgorithm->DoMakeOverlay();
	m_pDetectionAlgorithm->DoRemoveNoise();
	m_pDetectionAlgorithm->DoInterpolation();

	// List 에 있는 내용을 모두 지운다.
	m_listBlobInfo.DeleteAllItems();

	// Blob List에 Blob 정보 보여주기
	for(long blob = 0; blob < m_pDetectionAlgorithm->m_nInterpolationBlobTotalCount; blob++)
	{
		auto info = m_pDetectionAlgorithm->m_vecInterpolationBlobInfoList[blob];

		m_listBlobInfo.InsertItem(blob, _T(""));

		strNo.Format(_T("%d"), blob + 1);
		m_listBlobInfo.SetItemText(blob, 0, strNo);

		strTemp.Format(_T("%d"), info.m_area);
		m_listBlobInfo.SetItemText(blob, 1, strTemp);
		strTemp.Format(_T("%d"), info.m_roi.Width());
		m_listBlobInfo.SetItemText(blob, 2, strTemp);
		strTemp.Format(_T("%d"), info.m_roi.Height());
		m_listBlobInfo.SetItemText(blob, 3, strTemp);
		strTemp.Format(_T("%.3f"), (float)info.m_roi.Width() / (float)info.m_roi.Height());
		m_listBlobInfo.SetItemText(blob, 4, strTemp);
	}

	m_pParent->ShowROI(TRUE, TRUE);
	m_pParent->ShowInterpolation(IMAGE::INTER_THRESHOLD, FALSE, TRUE);
}

void CDlgInterpolation::OnLvnItemchangedListBloblist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if((pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)
	{
		int nItem = pNMLV->iItem;

		m_pParent->ShowROI(TRUE, TRUE);

		m_pImageDisplay->Overlay_AddRectangle(m_pDetectionAlgorithm->m_vecInterpolationBlobInfoList[nItem].m_roi, RGB(255, 0, 0));

		m_pImageDisplay->Overlay_Show(TRUE);
	}

	*pResult = 0;
}


void CDlgInterpolation::OnBnClickedBtnImageProcManager()
{
	if(m_pParent->m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		return;
	}

	auto &params = ::g_RecipeManager.m_sInterHairpinParams;
	auto &procParams = params.sProcParam;
	const long paramSize = (long)procParams.m_vecnProc.size();
	auto imageWidth = m_pParent->m_nUnrolledImageWidth;
	auto imageHeight = m_pParent->m_nUnrolledImageHeight;

	BYTE* pbyUnrolledImage = ::g_MemoryManager.GetByteImage((long)IMAGE::UNROLLED); // Unrolled Image

	CDlgImageProcManager DlgImageProcManager(pbyUnrolledImage, imageWidth, imageHeight, paramSize > 0 ? &procParams.m_vecnProc[0] : nullptr, paramSize > 0 ? &procParams.m_vecnProcParams[0] : nullptr, paramSize);
	if(DlgImageProcManager.DoModal() == IDOK)
	{
		GetProcData(DlgImageProcManager.GetProc(), DlgImageProcManager.GetProcSize(), DlgImageProcManager.GetProcParams(), DlgImageProcManager.GetProcParamsSize());

		m_pParent->UpdateInterProcImage(TRUE);
	}
}

void CDlgInterpolation::GetProcData(short* pProc, long procSize, short* pProcParams, long procParamsSize)
{
	auto &params = ::g_RecipeManager.m_sInterHairpinParams;
	auto &procParams = params.sProcParam;

	procParams.nImageProcNum = procSize;

	procParams.m_vecnProc.clear();
	procParams.m_vecnProc.resize(procSize);
	for(long idx = 0; idx < procSize; idx++)
	{
		procParams.m_vecnProc[idx] = pProc[idx];
	}

	procParams.m_vecnProcParams.clear();
	procParams.m_vecnProcParams.resize(procParamsSize);
	for(long idx = 0; idx < procParamsSize; idx++)
	{
		procParams.m_vecnProcParams[idx] = pProcParams[idx];
	}
}

void CDlgInterpolation::OnCbnSelchangeComboThreshold()
{
	auto &params = ::g_RecipeManager.m_sInterHairpinParams;

	params.nSelectThreshold = m_cmbThreshold.GetCurSel();

	if(params.nSelectThreshold == (long)THRESHOLD::NORMAL)
	{
		m_scrollbarThreshold.SetScrollRange(0, 255);
		m_scrollbarThreshold.SetScrollPos(params.nThresholdValue, TRUE);
		SetDlgItemInt(IDC_EDIT_THRESHOLD, params.nThresholdValue);

		GetDlgItem(IDC_SCROLLBAR_THRESHOLD_HIGH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_THRESHOLD_HIGH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_NOISE_FILTERING_SIZE)->EnableWindow(FALSE);
	}
	else if(params.nSelectThreshold == (long)THRESHOLD::ADAPTIVE)
	{
		m_scrollbarThreshold.SetScrollRange(-255, 255);
		m_scrollbarThreshold.SetScrollPos(params.nAdaptiveOffset, TRUE);
		SetDlgItemInt(IDC_EDIT_THRESHOLD, params.nAdaptiveOffset);

		GetDlgItem(IDC_SCROLLBAR_THRESHOLD_HIGH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_THRESHOLD_HIGH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_NOISE_FILTERING_SIZE)->EnableWindow(FALSE);
	}
	else if(params.nSelectThreshold == (long)THRESHOLD::HYSTERISIS)
	{
		m_scrollbarThreshold.SetScrollRange(0, 255);
		m_scrollbarThreshold.SetScrollPos(params.nHysteresisLow, TRUE);
		SetDlgItemInt(IDC_EDIT_THRESHOLD, params.nHysteresisLow);

		m_scrollbarThreshold_High.SetScrollRange(0, 255);
		m_scrollbarThreshold_High.SetScrollPos(params.nHysteresisHigh, TRUE);
		SetDlgItemInt(IDC_EDIT_THRESHOLD_HIGH, params.nHysteresisHigh);

		GetDlgItem(IDC_SCROLLBAR_THRESHOLD_HIGH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_THRESHOLD_HIGH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_NOISE_FILTERING_SIZE)->EnableWindow(TRUE);
	}

	m_pParent->UpdateInterThresholdImage(TRUE);
}


void CDlgInterpolation::OnBnClickedCheckInterpolationX()
{
	auto &params = ::g_RecipeManager.m_sInterHairpinParams;

	params.bInterpolationX = m_checkInterpolationX.GetCheck();
}


void CDlgInterpolation::OnBnClickedCheckInterpolationY()
{
	auto &params = ::g_RecipeManager.m_sInterHairpinParams;

	params.bInterpolationY = m_checkInterpolationY.GetCheck();
}

void CDlgInterpolation::OnBnClickedBtnApply()
{
	UpdateParameters();
}

void CDlgInterpolation::OnBnClickedBtnInspection()
{
	OnBnClickedBtnUpdatebloblist();
}



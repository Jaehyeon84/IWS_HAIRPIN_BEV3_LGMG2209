// DlgRemoveNoise.cpp: 구현 파일
//

#include "pch.h"
#include "Hairpin2DInsp.h"
#include "Hairpin2DInspDlg.h"
#include "DlgRemoveNoise.h"
#include "afxdialogex.h"

#include "DetectionAlgorithm.h"
#include "DlgImageProcManager.h"
#include "DlgMaskConvolution.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgRemoveNoise 대화 상자

IMPLEMENT_DYNAMIC(CDlgRemoveNoise, CDialog)

CDlgRemoveNoise::CDlgRemoveNoise(CHairpin2DInspDlg* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_REMOVE_NOISE, pParent)
	, m_pParent(pParent)
	, m_pImageDisplay(m_pParent->m_pImageDisplay)
	, m_pDetectionAlgorithm(m_pParent->m_pDetectionAlgorithm)
	, m_nHighThresholdRegionNoiseFilteringSize(0)
	, m_fAspectRatioMin(0)
	, m_fAspectRatioMax(100.f)
{

}

CDlgRemoveNoise::~CDlgRemoveNoise()
{
}

void CDlgRemoveNoise::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCROLLBAR_THRESHOLD, m_scrollbarThreshold);
	DDX_Control(pDX, IDC_SCROLLBAR_THRESHOLD_HIGH, m_scrollbarThreshold_High);
	DDX_Control(pDX, IDC_COMBO_THRESHOLD, m_cmbThreshold);
	DDX_Control(pDX, IDC_CHECK_THRRESHOLD_REVERSE, m_checkThresholdReverse);
	DDX_Text(pDX, IDC_EDIT_NOISE_FILTERING_SIZE, m_nHighThresholdRegionNoiseFilteringSize);
	DDX_Control(pDX, IDC_CHECK_USE_REMOVE_BLOB2, m_checkUseRemoveBlob);
	DDX_Text(pDX, IDC_EDIT_REMOVE_BLOB_AR_MIN2, m_fAspectRatioMin);
	DDX_Text(pDX, IDC_EDIT_REMOVE_BLOB_AR_MAX3, m_fAspectRatioMax);
	DDX_Control(pDX, IDC_CHECK_USEMERGEBLOB, m_checkBlobMerge);
	DDX_Text(pDX, IDC_EDIT_MIN_SIZE, m_nMinBlobSize);
	DDX_Text(pDX, IDC_EDIT_MAX_BLOB_COUNT, m_nMaxBlobCount);
	DDX_Text(pDX, IDC_EDIT_MERGEDISTANCE, m_nBlobMergeDist);
	DDX_Control(pDX, IDC_LIST_BLOBLIST, m_listBlobInfo);
}


BEGIN_MESSAGE_MAP(CDlgRemoveNoise, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_IMAGE_PROC_MANAGER, &CDlgRemoveNoise::OnBnClickedBtnImageProcManager)
	ON_CBN_SELCHANGE(IDC_COMBO_THRESHOLD, &CDlgRemoveNoise::OnCbnSelchangeComboThreshold)
	ON_BN_CLICKED(IDC_CHECK_THRRESHOLD_REVERSE, &CDlgRemoveNoise::OnBnClickedCheckThrresholdReverse)
	ON_BN_CLICKED(IDC_BTN_ORIGINAL_IMAGE, &CDlgRemoveNoise::OnBnClickedBtnOriginalImage)
	ON_BN_CLICKED(IDC_BTN_UNROLLED_IMAGE, &CDlgRemoveNoise::OnBnClickedBtnUnrolledImage)
	ON_BN_CLICKED(IDC_BTN_PROCEEDED_IMAGE, &CDlgRemoveNoise::OnBnClickedBtnProceededImage)
	ON_BN_CLICKED(IDC_BTN_THRESHOLD_IMAGE, &CDlgRemoveNoise::OnBnClickedBtnThresholdImage)
	ON_BN_CLICKED(IDC_BTN_RESULT_IMAGE, &CDlgRemoveNoise::OnBnClickedBtnResultImage)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CDlgRemoveNoise::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_INSPECTION, &CDlgRemoveNoise::OnBnClickedBtnInspection)
	ON_BN_CLICKED(IDC_BTN_MASK_CONVOLUTION, &CDlgRemoveNoise::OnBnClickedBtnMaskConvolution)
	ON_BN_CLICKED(IDC_CHECK_USEMERGEBLOB, &CDlgRemoveNoise::OnBnClickedCheckUsemergeblob)
	ON_BN_CLICKED(IDC_BTN_UPDATEBLOBLIST2, &CDlgRemoveNoise::OnBnClickedBtnUpdatebloblist2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BLOBLIST, &CDlgRemoveNoise::OnLvnItemchangedListBloblist)
END_MESSAGE_MAP()


// CDlgRemoveNoise 메시지 처리기

BOOL CDlgRemoveNoise::OnInitDialog()
{
	CDialog::OnInitDialog();

	auto params = ::g_RecipeManager.m_sRemoveNoiseParams;

	m_cmbThreshold.AddString(_T("Normal"));
	m_cmbThreshold.AddString(_T("Double"));
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

void CDlgRemoveNoise::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nID = pScrollBar->GetDlgCtrlID();

	auto &params = ::g_RecipeManager.m_sRemoveNoiseParams;

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
		else if(params.nSelectThreshold == (long)THRESHOLD::DOUBLE)
		{
			params.nThresholdLow = pos;
			m_scrollbarThreshold.SetScrollPos(params.nThresholdLow, TRUE);
			SetDlgItemInt(IDC_EDIT_THRESHOLD, params.nThresholdLow);
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
		if(params.nSelectThreshold == (long)THRESHOLD::DOUBLE)
		{
			params.nThresholdHigh = pos;
			m_scrollbarThreshold_High.SetScrollPos(params.nThresholdHigh, TRUE);
			SetDlgItemInt(IDC_EDIT_THRESHOLD_HIGH, params.nThresholdHigh);
		}
		else if(params.nSelectThreshold == (long)THRESHOLD::HYSTERISIS)
		{
			params.nHysteresisHigh = pos;
			m_scrollbarThreshold_High.SetScrollPos(params.nHysteresisHigh, TRUE);
			SetDlgItemInt(IDC_EDIT_THRESHOLD_HIGH, params.nHysteresisHigh);
		}
		break;
	}

	m_pParent->ShowROI(TRUE, TRUE);
	m_pParent->UpdateRemoveNoiseThresholdImage(TRUE);

	UpdateData(FALSE);

	Invalidate(FALSE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgRemoveNoise::UpdateUI()
{
	CString strTemp;
	auto params = ::g_RecipeManager.m_sRemoveNoiseParams;

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
	else if(params.nSelectThreshold == (long)THRESHOLD::DOUBLE)
	{
		m_scrollbarThreshold.SetScrollRange(0, 255);
		m_scrollbarThreshold.SetScrollPos(params.nThresholdLow, TRUE);
		SetDlgItemInt(IDC_EDIT_THRESHOLD, params.nThresholdLow);

		m_scrollbarThreshold_High.SetScrollRange(0, 255);
		m_scrollbarThreshold_High.SetScrollPos(params.nThresholdHigh, TRUE);
		SetDlgItemInt(IDC_EDIT_THRESHOLD_HIGH, params.nThresholdHigh);

		GetDlgItem(IDC_SCROLLBAR_THRESHOLD_HIGH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_THRESHOLD_HIGH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_NOISE_FILTERING_SIZE)->EnableWindow(TRUE);
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
	m_checkUseRemoveBlob.SetCheck(params.bUseRemoveBlob);
	m_fAspectRatioMin = params.fAspectRatioMin;
	m_fAspectRatioMax = params.fAspectRatioMax;

	UpdateData(FALSE);
}

void CDlgRemoveNoise::UpdateParameters()
{
	UpdateData(TRUE);

	auto &params = ::g_RecipeManager.m_sRemoveNoiseParams;

	if(params.nSelectThreshold == (long)THRESHOLD::NORMAL)
	{
		params.nThresholdValue = m_scrollbarThreshold.GetScrollPos();
	}
	else if(params.nSelectThreshold == (long)THRESHOLD::DOUBLE)
	{
		params.nThresholdLow = m_scrollbarThreshold.GetScrollPos();
		params.nThresholdHigh = m_scrollbarThreshold_High.GetScrollPos();
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
	params.bUseRemoveBlob = m_checkUseRemoveBlob.GetCheck();
	params.fAspectRatioMin = m_fAspectRatioMin;
	params.fAspectRatioMax = m_fAspectRatioMax;
}


void CDlgRemoveNoise::OnBnClickedBtnImageProcManager()
{
	if(m_pParent->m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		return;
	}

	auto &params = ::g_RecipeManager.m_sRemoveNoiseParams;
	auto &procParams = params.sProcParam;
	const long paramSize = (long)procParams.m_vecnProc.size();
	auto imageWidth = m_pParent->m_nUnrolledImageWidth;
	auto imageHeight = m_pParent->m_nUnrolledImageHeight;

	BYTE* pbyUnrolledImage = ::g_MemoryManager.GetByteImage((long)IMAGE::UNROLLED); // Unrolled Image

	CDlgImageProcManager DlgImageProcManager(pbyUnrolledImage, imageWidth, imageHeight, paramSize > 0 ? &procParams.m_vecnProc[0] : nullptr, paramSize > 0 ? &procParams.m_vecnProcParams[0] : nullptr, paramSize);
	if(DlgImageProcManager.DoModal() == IDOK)
	{
		GetProcData(DlgImageProcManager.GetProc(), DlgImageProcManager.GetProcSize(), DlgImageProcManager.GetProcParams(), DlgImageProcManager.GetProcParamsSize());

		m_pParent->UpdateRemoveNoiseProcImage(TRUE);
	}
}

void CDlgRemoveNoise::GetProcData(short* pProc, long procSize, short* pProcParams, long procParamsSize)
{
	auto &params = ::g_RecipeManager.m_sRemoveNoiseParams;
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

void CDlgRemoveNoise::OnCbnSelchangeComboThreshold()
{
	auto &params = ::g_RecipeManager.m_sRemoveNoiseParams;

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
	else if(params.nSelectThreshold == (long)THRESHOLD::DOUBLE)
	{
		m_scrollbarThreshold.SetScrollRange(0, 255);
		m_scrollbarThreshold.SetScrollPos(params.nThresholdLow, TRUE);
		SetDlgItemInt(IDC_EDIT_THRESHOLD, params.nThresholdLow);

		m_scrollbarThreshold_High.SetScrollRange(0, 255);
		m_scrollbarThreshold_High.SetScrollPos(params.nThresholdHigh, TRUE);
		SetDlgItemInt(IDC_EDIT_THRESHOLD_HIGH, params.nThresholdHigh);

		GetDlgItem(IDC_SCROLLBAR_THRESHOLD_HIGH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_THRESHOLD_HIGH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_NOISE_FILTERING_SIZE)->EnableWindow(TRUE);
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

	m_pParent->UpdateRemoveNoiseThresholdImage(TRUE);
}

void CDlgRemoveNoise::OnBnClickedCheckUseRemoveBlob()
{
	auto &params = ::g_RecipeManager.m_sRemoveNoiseParams;

	params.bUseRemoveBlob = m_checkUseRemoveBlob.GetCheck();
}


void CDlgRemoveNoise::OnBnClickedCheckThrresholdReverse()
{
	auto &params = ::g_RecipeManager.m_sRemoveNoiseParams;

	params.bThresholdReverse = m_checkThresholdReverse.GetCheck();

	m_pParent->UpdateRemoveNoiseThresholdImage(TRUE);
}

void CDlgRemoveNoise::OnBnClickedBtnOriginalImage()
{
	m_pParent->ShowOriginal(TRUE, TRUE);
}

void CDlgRemoveNoise::OnBnClickedBtnUnrolledImage()
{
	m_pParent->ShowMakeUnrolled(FALSE);
}

void CDlgRemoveNoise::OnBnClickedBtnProceededImage()
{
	m_pParent->ShowRemoveNoise(IMAGE::REMOVE_NOISE_PROC, TRUE);
}

void CDlgRemoveNoise::OnBnClickedBtnThresholdImage()
{
	m_pParent->ShowRemoveNoise(IMAGE::REMOVE_NOISE_THRESHOLD, TRUE);
}

void CDlgRemoveNoise::OnBnClickedBtnResultImage()
{
	m_pParent->ShowRemoveNoise(IMAGE::REMOVE_NOISE_RESULT, TRUE, TRUE);
}


void CDlgRemoveNoise::OnBnClickedBtnApply()
{
	UpdateParameters();
}


void CDlgRemoveNoise::OnBnClickedBtnInspection()
{
	OnBnClickedBtnUpdatebloblist2();
}



void CDlgRemoveNoise::OnBnClickedBtnMaskConvolution()
{
	auto &params = ::g_RecipeManager.m_sRemoveNoiseParams;

	CDlgMaskConvolution DlgMaskConvolution(
		params.bUseMaskConvolution,
		params.nMask1Width,
		params.nMask1Height,
		params.nMask2Width,
		params.nMask2Height,
		params.fRef1,
		params.fRef2);

	if(DlgMaskConvolution.DoModal() == IDOK)
	{
		params.bUseMaskConvolution = DlgMaskConvolution.m_bUseMaskConvolution;
		params.nMask1Width = DlgMaskConvolution.m_nMaskWidth1;
		params.nMask1Height = DlgMaskConvolution.m_nMaskHeight1;
		params.nMask2Width = DlgMaskConvolution.m_nMaskWidth2;
		params.nMask2Height = DlgMaskConvolution.m_nMaskHeight2;
		params.fRef1 = DlgMaskConvolution.m_fRef1;
		params.fRef2 = DlgMaskConvolution.m_fRef2;
	}
}


void CDlgRemoveNoise::OnBnClickedCheckUsemergeblob()
{
	auto &params = ::g_RecipeManager.m_sRemoveNoiseParams;

	params.bBlobMerge = m_checkBlobMerge.GetCheck();
}


void CDlgRemoveNoise::OnBnClickedBtnUpdatebloblist2()
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

	// List 에 있는 내용을 모두 지운다.
	m_listBlobInfo.DeleteAllItems();

	// Blob List에 Blob 정보 보여주기
	for(long blob = 0; blob < m_pDetectionAlgorithm->m_nRemoveNoiseBlobTotalCount; blob++)
	{
		auto info = m_pDetectionAlgorithm->m_vecRemoveNoiseBlobInfoList[blob];

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
	m_pParent->ShowRemoveNoise(IMAGE::REMOVE_NOISE_RESULT, FALSE, TRUE);
}


void CDlgRemoveNoise::OnLvnItemchangedListBloblist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if((pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)
	{
		int nItem = pNMLV->iItem;

		m_pParent->ShowROI(TRUE, TRUE);

		m_pImageDisplay->Overlay_AddRectangle(m_pDetectionAlgorithm->m_vecRemoveNoiseBlobInfoList[nItem].m_roi, RGB(255, 0, 0));

		m_pImageDisplay->Overlay_Show(TRUE);
	}

	*pResult = 0;
}

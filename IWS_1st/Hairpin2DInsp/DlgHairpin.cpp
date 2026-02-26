// DlgHairpin.cpp: 구현 파일
//

#include "pch.h"
#include "Hairpin2DInsp.h"
#include "Hairpin2DInspDlg.h"
#include "DlgHairpin.h"
#include "afxdialogex.h"

#include "DetectionAlgorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgHairpin 대화 상자

IMPLEMENT_DYNAMIC(CDlgHairpin, CDialogEx)

CDlgHairpin::CDlgHairpin(CHairpin2DInspDlg* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_HAIRPIN_CENTER, pParent)
	, m_pParent(pParent)
	, m_pImageDisplay(m_pParent->m_pImageDisplay)
	, m_pDetectionAlgorithm(m_pParent->m_pDetectionAlgorithm)
	, m_nWidthSize_1BlobMinSize(145)
	, m_nWidthSize_2BlobMinSize(145)
	, m_nWidthSize_2MoreBlobMinSize(145)
	, m_nComplementHeightSize(42)
	, m_nMinHairpinWidth(50)
	, m_nMinHairpinHeight(35)
	, m_nComplementHeightLimit(30)
	, m_nMaxHairpinWidth(80)
	, m_nMaxHairpinHeight(50)
	, m_nMinHairpinWidthfor1Blob(120)
	, m_f2D3D_Potion_Limit_X(0)
	, m_f2D3D_Potion_Limit_Y(0)
{

}

CDlgHairpin::~CDlgHairpin()
{
}

void CDlgHairpin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_1BLOB_WIDTH_MIN_SIZE, m_nWidthSize_1BlobMinSize);
	DDX_Text(pDX, IDC_EDIT_2BLOB_WIDTH_MIN_SIZE, m_nWidthSize_2BlobMinSize);
	DDX_Text(pDX, IDC_EDIT_2MOREBLOB_WIDTH_MIN_SIZE, m_nWidthSize_2MoreBlobMinSize);
	DDX_Text(pDX, IDC_EDIT_HAIRPIN_HEIGHT_SIZE, m_nComplementHeightSize);
	DDX_Control(pDX, IDC_CHECK_USE_COMPLEMENT, m_checkUseComplementCenter);
	DDX_Text(pDX, IDC_EDIT_MIN_HAIRPIN_SIZE_WIDTH, m_nMinHairpinWidth);
	DDX_Text(pDX, IDC_EDIT_MIN_HAIRPIN_SIZE_HEIGHT, m_nMinHairpinHeight);
	DDX_Text(pDX, IDC_EDIT_HAIRPIN_HEIGHT_LIMIT, m_nComplementHeightLimit);
	DDX_Text(pDX, IDC_EDIT_MAX_HAIRPIN_SIZE_WIDTH, m_nMaxHairpinWidth);
	DDX_Text(pDX, IDC_EDIT_MAX_HAIRPIN_SIZE_HEIGHT, m_nMaxHairpinHeight);
	DDX_Text(pDX, IDC_EDIT_MIN_HAIRPIN_SIZE_WIDTH_FOR_1BLOB, m_nMinHairpinWidthfor1Blob);
	DDX_Control(pDX, IDC_LIST_CENTER_ALGORITHM_LOG, m_listAlgorithmLog);
	DDX_Control(pDX, IDC_CHECK_USE_COMPLEMENT2, m_checkUseCalCenterOffset_replace3D);
	DDX_Control(pDX, IDC_CHECK_USE_COMPLEMENT3, m_checkUseCorrectionOffset_replace3D);
	DDX_Text(pDX, IDC_EDIT_2D3D_POSITION_LIMIT_X, m_f2D3D_Potion_Limit_X);
	DDX_Text(pDX, IDC_EDIT_2D3D_POSITION_LIMIT_Y, m_f2D3D_Potion_Limit_Y);
	DDX_Control(pDX, IDC_CHECK_USE_LAYERPAIR_REPLACE, m_checkUseLayerPairReplace);
}


BEGIN_MESSAGE_MAP(CDlgHairpin, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ORIGINAL_IMAGE, &CDlgHairpin::OnBnClickedBtnOriginalImage)
	ON_BN_CLICKED(IDC_BTN_UNROLLED_IMAGE, &CDlgHairpin::OnBnClickedBtnUnrolledImage)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CDlgHairpin::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_INSPECTION2, &CDlgHairpin::OnBnClickedBtnInspection2)
END_MESSAGE_MAP()


// CDlgHairpin 메시지 처리기

BOOL CDlgHairpin::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_listAlgorithmLog.SetExtendedStyle(m_listAlgorithmLog.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listAlgorithmLog.InsertColumn(0, _T("Slot"), LVCFMT_CENTER, 45);
	m_listAlgorithmLog.InsertColumn(1, _T("Layer"), LVCFMT_CENTER, 45);
	m_listAlgorithmLog.InsertColumn(2, _T("Width(px)"), LVCFMT_CENTER, 80);
	m_listAlgorithmLog.InsertColumn(3, _T("Height(px)"), LVCFMT_CENTER, 80);
	m_listAlgorithmLog.InsertColumn(4, _T("Log"), LVCFMT_LEFT, 350);

	return TRUE;
}

void CDlgHairpin::UpdateLog()
{
	CString strTemp;

	auto roiParams = ::g_RecipeManager.m_sROIParams;

	// List 에 있는 내용을 모두 지운다.
	m_listAlgorithmLog.DeleteAllItems();

	long index(0);
	for(long slot = 0; slot < roiParams.nSlotCount; slot++)
	{
		for(long layer = roiParams.nLayerCount - 1 ; layer >= 0; layer--)
		{
			m_listAlgorithmLog.InsertItem(index, _T(""));

			strTemp.Format(_T("%d"), slot + 1);
			m_listAlgorithmLog.SetItemText(index, 0, strTemp);
			strTemp.Format(_T("%d"), roiParams.nLayerCount - 1 - layer + 1);
			m_listAlgorithmLog.SetItemText(index, 1, strTemp);
			strTemp.Format(_T("%d"), m_pDetectionAlgorithm->m_vecnHairpinWidth[slot][layer]);
			m_listAlgorithmLog.SetItemText(index, 2, strTemp);
			strTemp.Format(_T("%d"), m_pDetectionAlgorithm->m_vecnHairpinHeight[slot][layer]);
			m_listAlgorithmLog.SetItemText(index, 3, strTemp);
			m_listAlgorithmLog.SetItemText(index, 4, m_pDetectionAlgorithm->m_vecstrCenterAlgorithm[slot][layer]);

			index++;
		}
	}
}

void CDlgHairpin::UpdateUI()
{
	CString strTemp;
	auto params = ::g_RecipeManager.m_sHairpinCenterParams;

	UpdateData(TRUE);

	m_nWidthSize_1BlobMinSize = params.nWidthSize_1BlobMinSize;
	m_nWidthSize_2BlobMinSize = params.nWidthSize_2BlobMinSize;
	m_nWidthSize_2MoreBlobMinSize = params.nWidthSize_2MoreBlobMinSize;
	m_nComplementHeightSize = params.nComplementHeightSize;
	m_nComplementHeightLimit = params.nComplementHeightLimit;
	m_checkUseComplementCenter.SetCheck(params.bUseComplementHairpinCenter);
	m_nMinHairpinWidth = params.nMinHairpinWidth;
	m_nMinHairpinHeight = params.nMinHairpinHeight;
	m_nMaxHairpinWidth = params.nMaxHairpinWidth;
	m_nMaxHairpinHeight = params.nMaxHairpinHeight;
	m_nMinHairpinWidthfor1Blob = params.nMinHairpinWidthfor1Blob;
	m_checkUseCalCenterOffset_replace3D.SetCheck(params.bUseCalCenterOffset_replace3D);
	m_checkUseCorrectionOffset_replace3D.SetCheck(params.bUseCorrectionOffset_replace3D);
	m_f2D3D_Potion_Limit_X = params.f2D3DPositionLimitX;
	m_f2D3D_Potion_Limit_Y = params.f2D3DPositionLimitY;
	m_checkUseLayerPairReplace.SetCheck(params.bUseLayerPairReplace);

	UpdateData(FALSE);
}

void CDlgHairpin::UpdateParameters()
{
	UpdateData(TRUE);

	auto &params = ::g_RecipeManager.m_sHairpinCenterParams;

	params.nWidthSize_1BlobMinSize = m_nWidthSize_1BlobMinSize;
	params.nWidthSize_2BlobMinSize = m_nWidthSize_2BlobMinSize;
	params.nWidthSize_2MoreBlobMinSize = m_nWidthSize_2MoreBlobMinSize;
	params.nComplementHeightSize = m_nComplementHeightSize;
	params.nComplementHeightLimit = m_nComplementHeightLimit;
	params.bUseComplementHairpinCenter = m_checkUseComplementCenter.GetCheck();
	params.nMinHairpinWidth = m_nMinHairpinWidth;
	params.nMinHairpinHeight = m_nMinHairpinHeight;
	params.nMaxHairpinWidth = m_nMaxHairpinWidth;
	params.nMaxHairpinHeight = m_nMaxHairpinHeight;
	params.nMinHairpinWidthfor1Blob = m_nMinHairpinWidthfor1Blob;
	params.bUseCalCenterOffset_replace3D = m_checkUseCalCenterOffset_replace3D.GetCheck();
	params.bUseCorrectionOffset_replace3D = m_checkUseCorrectionOffset_replace3D.GetCheck();
	params.f2D3DPositionLimitX = m_f2D3D_Potion_Limit_X;
	params.f2D3DPositionLimitY = m_f2D3D_Potion_Limit_Y;
	params.bUseLayerPairReplace = m_checkUseLayerPairReplace.GetCheck();
}

void CDlgHairpin::OnBnClickedBtnOriginalImage()
{
	m_pParent->ShowHairpinOverlay_Original(TRUE, TRUE);
}


void CDlgHairpin::OnBnClickedBtnUnrolledImage()
{
	m_pParent->ShowROI(TRUE, TRUE);
	m_pParent->ShowHairpinOverlay_Unrolled(FALSE, TRUE);
}


void CDlgHairpin::OnBnClickedBtnApply()
{
	UpdateParameters();
}


void CDlgHairpin::OnBnClickedBtnInspection2()
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
	m_pDetectionAlgorithm->DoDetectBlob();
	m_pDetectionAlgorithm->DoCalcHairpinCenter();
	m_pDetectionAlgorithm->DoDataExchange();

	m_pParent->ShowHairpinOverlay_Original(TRUE, TRUE);

	UpdateLog();
}

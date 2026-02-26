// DlgMakeUnrolledImage.cpp: 구현 파일
//

#include "pch.h"
#include "Hairpin2DInsp.h"
#include "Hairpin2DInspDlg.h"
#include "DlgMakeUnrolled.h"
#include "afxdialogex.h"

#include "DetectionAlgorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgMakeUnrolled 대화 상자

IMPLEMENT_DYNAMIC(CDlgMakeUnrolled, CDialog)

CDlgMakeUnrolled::CDlgMakeUnrolled(CHairpin2DInspDlg* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_MAKE_UNROLLED, pParent)
	, m_pParent(pParent)
	, m_pDetectionAlgorithm(m_pParent->m_pDetectionAlgorithm)
	, m_nCenterPosCorrectionX(0)
	, m_nCenterPosCorrectionY(0)
	, m_fUnrolledStartAngle(0.f)
	, m_nUnrolledStartOffsetY(200)
	, m_nUnrolledLength(1000)
	, m_fSubOriginalWidth_mm(210)
	, m_fSubOriginalHeight_mm(210)
{

}

CDlgMakeUnrolled::~CDlgMakeUnrolled()
{
}

void CDlgMakeUnrolled::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_CORRECTION_X, m_nCenterPosCorrectionX);
	DDX_Text(pDX, IDC_EDIT_CENTER_POS_CORRECTION_Y, m_nCenterPosCorrectionY);
	DDX_Text(pDX, IDC_EDIT_UNROLLED_START_ANGLE, m_fUnrolledStartAngle);
	DDX_Text(pDX, IDC_EDIT_UNROLLED_START_OFFSET_Y, m_nUnrolledStartOffsetY);
	DDX_Text(pDX, IDC_EDIT_UNROLLED_LENGTH, m_nUnrolledLength);
	DDX_Text(pDX, IDC_EDIT_SUB_ORIGINAL_WIDTH_MM, m_fSubOriginalWidth_mm);
	DDX_Text(pDX, IDC_EDIT_SUB_ORIGINAL_HEIGHT_MM, m_fSubOriginalHeight_mm);
}


BEGIN_MESSAGE_MAP(CDlgMakeUnrolled, CDialog)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CDlgMakeUnrolled::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_INSPECTION, &CDlgMakeUnrolled::OnBnClickedBtnInspection)
	ON_BN_CLICKED(IDC_BTN_ORIGINAL_IMAGE, &CDlgMakeUnrolled::OnBnClickedBtnOriginalImage)
	ON_BN_CLICKED(IDC_BTN_UNROLLED_IMAGE, &CDlgMakeUnrolled::OnBnClickedBtnUnrolledImage)
END_MESSAGE_MAP()


// CDlgMakeUnrolled 메시지 처리기

void CDlgMakeUnrolled::UpdateUI()
{
	CString strTemp;
	auto params = ::g_RecipeManager.m_sUnrolledParams;

	UpdateData(TRUE);

	m_fSubOriginalWidth_mm = params.fSubOriginalWidth_mm;
	m_fSubOriginalHeight_mm = params.fSubOriginalHeight_mm;
	m_nCenterPosCorrectionX = params.ptCenterPosCorrection.x;
	m_nCenterPosCorrectionY = params.ptCenterPosCorrection.y;
	m_fUnrolledStartAngle = params.fUnrolledStartAngle;
	m_nUnrolledStartOffsetY = params.nUnrolledStartOffsetY;
	m_nUnrolledLength = params.nUnrolledLength;

	UpdateData(FALSE);
}

void CDlgMakeUnrolled::UpdateParameters()
{
	UpdateData(TRUE);

	auto &params = ::g_RecipeManager.m_sUnrolledParams;

	params.fSubOriginalWidth_mm = m_fSubOriginalWidth_mm;
	params.fSubOriginalHeight_mm = m_fSubOriginalHeight_mm;
	params.ptCenterPosCorrection = CPoint(m_nCenterPosCorrectionX, m_nCenterPosCorrectionY);
	params.fUnrolledStartAngle = m_fUnrolledStartAngle;
	params.nUnrolledStartOffsetY = m_nUnrolledStartOffsetY;
	params.nUnrolledLength = m_nUnrolledLength;
}


void CDlgMakeUnrolled::OnBnClickedBtnApply()
{
	UpdateParameters();
}


void CDlgMakeUnrolled::OnBnClickedBtnInspection()
{
	UpdateParameters();

	m_pDetectionAlgorithm->DoMakeSubOriginalImage();
	m_pDetectionAlgorithm->DoMakeUnrolledImage();

	m_pParent->ShowMakeUnrolled(TRUE);
}


void CDlgMakeUnrolled::OnBnClickedBtnOriginalImage()
{
	m_pParent->ShowOriginal(TRUE, TRUE);
}


void CDlgMakeUnrolled::OnBnClickedBtnUnrolledImage()
{
	m_pParent->ShowMakeUnrolled(TRUE);
}

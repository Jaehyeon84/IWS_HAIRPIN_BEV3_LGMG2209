// DlgMaskConvolution.cpp: 구현 파일
//

#include "pch.h"
#include "Hairpin2DInsp.h"
#include "DlgMaskConvolution.h"
#include "afxdialogex.h"


// CDlgMaskConvolution 대화 상자

IMPLEMENT_DYNAMIC(CDlgMaskConvolution, CDialogEx)

CDlgMaskConvolution::CDlgMaskConvolution(BOOL bUseMaskConvolution, long nMaskWidth1, long nMaskHeight1, long nMaskWidth2, long nMaskHeight2, float fRef1, float fRef2, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_MASK_CONVOLUTION, pParent)
	, m_bUseMaskConvolution(bUseMaskConvolution)
	, m_nMaskWidth1(nMaskWidth1)
	, m_nMaskHeight1(nMaskHeight1)
	, m_nMaskWidth2(nMaskWidth2)
	, m_nMaskHeight2(nMaskHeight2)
	, m_fRef1(fRef1)
	, m_fRef2(fRef2)
{

}

CDlgMaskConvolution::~CDlgMaskConvolution()
{
}

void CDlgMaskConvolution::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_USE_MASK_CONVOLUTION, m_chkUseMaskConvolution);
	DDX_Text(pDX, IDC_EDIT_MASK1_SIZE_X, m_nMaskWidth1);
	DDX_Text(pDX, IDC_EDIT_MASK2_SIZE_X, m_nMaskWidth2);
	DDX_Text(pDX, IDC_EDIT_MASK1_SIZE_Y, m_nMaskHeight1);
	DDX_Text(pDX, IDC_EDIT_MASK2_SIZE_Y, m_nMaskHeight2);
	DDX_Text(pDX, IDC_EDIT_REF1, m_fRef1);
	DDX_Text(pDX, IDC_EDIT_REF2, m_fRef2);
}


BEGIN_MESSAGE_MAP(CDlgMaskConvolution, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgMaskConvolution::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgMaskConvolution 메시지 처리기

BOOL CDlgMaskConvolution::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_chkUseMaskConvolution.SetCheck(m_bUseMaskConvolution);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgMaskConvolution::OnBnClickedOk()
{
	UpdateData(TRUE);

	m_bUseMaskConvolution = m_chkUseMaskConvolution.GetCheck();

	CDialogEx::OnOK();
}

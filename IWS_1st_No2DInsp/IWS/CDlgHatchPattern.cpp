// CDlgHatchPattern.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgHatchPattern.h"
#include "afxdialogex.h"


// CDlgHatchPattern 대화 상자

IMPLEMENT_DYNAMIC(CDlgHatchPattern, CDialogEx)

CDlgHatchPattern::CDlgHatchPattern(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HATCH_PATTERN, pParent)
{
	m_dDistance = 0.2;
	m_dAngle = 45.0;
}

CDlgHatchPattern::~CDlgHatchPattern()
{

}

void CDlgHatchPattern::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DISTANCE, m_editDistance);
	DDX_Control(pDX, IDC_EDIT_ANGLE, m_editAngle);
}


BEGIN_MESSAGE_MAP(CDlgHatchPattern, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgHatchPattern::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgHatchPattern 메시지 처리기


void CDlgHatchPattern::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_dDistance = m_editDistance.GetValue();
	m_dAngle = m_editAngle.GetValue();
	CDialogEx::OnOK();
}


BOOL CDlgHatchPattern::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_editDistance.SetValue(m_dDistance);
	m_editAngle.SetValue(m_dAngle);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

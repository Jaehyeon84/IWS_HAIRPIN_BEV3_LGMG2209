// DlgManualWork.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "DlgManualWork.h"
#include "afxdialogex.h"


// CDlgManualWork 대화 상자

IMPLEMENT_DYNAMIC(CDlgManualWork, CDialogEx)

CDlgManualWork::CDlgManualWork(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MANUAL_WORK, pParent)
	, m_iSelection(0)
{

}

CDlgManualWork::~CDlgManualWork()
{
}

void CDlgManualWork::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editPowerRatio);
	DDX_Control(pDX, IDC_EDIT2, m_editSpeedRatio);
	DDX_Radio(pDX, IDC_RADIO_WORKALL, m_iSelection);
}


BEGIN_MESSAGE_MAP(CDlgManualWork, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgManualWork::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgManualWork 메시지 처리기


void CDlgManualWork::OnBnClickedOk()
{
	UpdateData();
	m_dPowerRatio = m_editPowerRatio.GetValue();
	m_dSpeedRatio = m_editSpeedRatio.GetValue();
	CDialogEx::OnOK();
}


BOOL CDlgManualWork::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_editPowerRatio.SetValue(m_dPowerRatio , L"%.1f");
	m_editSpeedRatio.SetValue(m_dSpeedRatio, L"%.1f");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

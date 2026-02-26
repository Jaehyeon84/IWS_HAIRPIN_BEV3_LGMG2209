// CDlgWaitWork.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgWaitWork.h"
#include "afxdialogex.h"


// CDlgWaitWork 대화 상자

IMPLEMENT_DYNAMIC(CDlgWaitWork, CDialogEx)

CDlgWaitWork::CDlgWaitWork(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_WAIT_WORK, pParent)
{

}

CDlgWaitWork::~CDlgWaitWork()
{
}

void CDlgWaitWork::SetMessage(CString strMsg)
{
	m_strMessage = strMsg;
	m_stcMessage.SetText(m_strMessage);	
}

void CDlgWaitWork::AssignStopFlag(BOOL * pFlag, BOOL *pAbort)
{
	if (m_btnStop.GetSafeHwnd()) {
		if (pFlag)m_btnStop.EnableWindow(TRUE);
		else
			m_btnStop.EnableWindow(FALSE);
	}
	m_pStop = pFlag;
	m_pAbort = pAbort;
}

void CDlgWaitWork::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_stcMessage);
	DDX_Control(pDX, IDC_BUTTON_WORK_STOP, m_btnStop);
}


BEGIN_MESSAGE_MAP(CDlgWaitWork, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_WORK_STOP, &CDlgWaitWork::OnBnClickedButtonWorkStop)
END_MESSAGE_MAP()


// CDlgWaitWork 메시지 처리기


void CDlgWaitWork::OnBnClickedButtonWorkStop()
{
	*m_pStop = TRUE;
	if (m_pAbort) *m_pAbort = TRUE;
	SetMessage(L"Requesting Stop");
}


BOOL CDlgWaitWork::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_stcMessage.SetBkColor(COLOR_BLACK);
	m_stcMessage.SetTextColor(COLOR_OLIVEDRAB1);
	m_stcMessage.SetText(m_strMessage);
	return TRUE;  // return TRUE unless you set the focus to a control			  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgWaitWork::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete  this;
	CDialogEx::PostNcDestroy();
}

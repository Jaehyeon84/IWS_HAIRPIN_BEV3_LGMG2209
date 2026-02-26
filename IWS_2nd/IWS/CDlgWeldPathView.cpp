// CDlgWeldPathView.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgWeldPathView.h"
#include "afxdialogex.h"


// CDlgWeldPathView 대화 상자

IMPLEMENT_DYNAMIC(CDlgWeldPathView, CDialogEx)

CDlgWeldPathView::CDlgWeldPathView(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_WELDPATH, pParent)
{

}

CDlgWeldPathView::~CDlgWeldPathView()
{
}

void CDlgWeldPathView::AddEntity(HANDLE hEnt)
{
	m_cadPathView.AddEntity(hEnt);
}

void CDlgWeldPathView::SetJobs(IWS_VECTOR_JOB* pJobs)
{
	m_cadPathView.m_pJobs = pJobs;
}

void CDlgWeldPathView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_WELDPATH, m_cadPathView);
}


BEGIN_MESSAGE_MAP(CDlgWeldPathView, CDialogEx)
END_MESSAGE_MAP()


// CDlgWeldPathView 메시지 처리기


BOOL CDlgWeldPathView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//m_cadPathView.CreatePointBuff(m_hBlock);
	m_cadPathView.SetWorkSize(0, 0, m_wndSizeW, m_wndSizeH);
	if (m_cadPathView.m_pJobs)
		m_cadPathView.CreatePointBuffFromJobs();
	else
		m_cadPathView.CreatePointBuffV();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

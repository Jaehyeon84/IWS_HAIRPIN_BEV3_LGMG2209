// CDlgAnlogPowerCtrl.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgAnlogPowerCtrl.h"
#include "afxdialogex.h"


// CDlgAnlogPowerCtrl 대화 상자

IMPLEMENT_DYNAMIC(CDlgAnlogPowerCtrl, CDialogEx)

CDlgAnlogPowerCtrl::CDlgAnlogPowerCtrl(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_POWER_PROFILE, pParent)
{

}

CDlgAnlogPowerCtrl::~CDlgAnlogPowerCtrl()
{
}

void CDlgAnlogPowerCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAnlogPowerCtrl, CDialogEx)
END_MESSAGE_MAP()


// CDlgAnlogPowerCtrl 메시지 처리기


BOOL CDlgAnlogPowerCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

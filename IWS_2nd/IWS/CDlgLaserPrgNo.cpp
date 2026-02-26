// CDlgLaserPrgNo.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgLaserPrgNo.h"
#include "afxdialogex.h"


// CDlgLaserPrgNo 대화 상자

IMPLEMENT_DYNAMIC(CDlgLaserPrgNo, CDialogEx)

CDlgLaserPrgNo::CDlgLaserPrgNo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LASER_PRGNO, pParent)
{

}

CDlgLaserPrgNo::~CDlgLaserPrgNo()
{
}

void CDlgLaserPrgNo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PRGNO, m_editPrgNo);
}


BEGIN_MESSAGE_MAP(CDlgLaserPrgNo, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &CDlgLaserPrgNo::OnBnClickedButtonChange)
END_MESSAGE_MAP()


// CDlgLaserPrgNo 메시지 처리기


void CDlgLaserPrgNo::OnBnClickedButtonChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_nValue = m_editPrgNo.GetValue();
	 if (m_nValue < 0 || m_nValue > 3)
	 {
		 AfxMessageBox(L"The Program number is from 0 to 3");
		 return;
	 }

	 CDlgLaserPrgNo::OnOK();
}

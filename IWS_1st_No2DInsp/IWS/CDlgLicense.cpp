// CDlgLicense.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgLicense.h"
#include "afxdialogex.h"


// CDlgLicense 대화 상자

IMPLEMENT_DYNAMIC(CDlgLicense, CDialogEx)

CDlgLicense::CDlgLicense(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LICENSE, pParent)
{

}

CDlgLicense::~CDlgLicense()
{
}

void CDlgLicense::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SERIAL, m_editLicense);
}


BEGIN_MESSAGE_MAP(CDlgLicense, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgLicense::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgLicense 메시지 처리기


void CDlgLicense::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str;
	m_editLicense.GetWindowTextW(str);
	m_strLicense = CStringA(str);
	CDialogEx::OnOK();
}

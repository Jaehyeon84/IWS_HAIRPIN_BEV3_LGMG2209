// DlgPassword.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "DlgPassword.h"
#include "MainFrm.h"

// CDlgPassword 대화 상자

IMPLEMENT_DYNAMIC(CDlgPassword, CDialog)

CDlgPassword::CDlgPassword(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPassword::IDD, pParent)
	, m_strPassword(_T(""))
{
	m_nUser = 0;
	m_bChgPassword = FALSE;
}

CDlgPassword::~CDlgPassword()
{
}

void CDlgPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_NEW_PASSWORD, m_strNewPassword);
	DDX_Text(pDX, IDC_CHK_PASSWORD, m_strChkPassword);
}


BEGIN_MESSAGE_MAP(CDlgPassword, CDialog)
	ON_BN_CLICKED(IDC_APPLY, &CDlgPassword::OnBnClickedApply)
	ON_BN_CLICKED(IDOK, &CDlgPassword::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgPassword 메시지 처리기

// 2024.02.09 jh.kim 추가
BOOL CDlgPassword::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	if (pDoc->m_User.GetUser() != USER_OPERATOR)
	{
		GetDlgItem(IDC_NEW_PASSWORD)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_PASSWORD)->EnableWindow(TRUE);
		GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
	}

	return TRUE;
}

void CDlgPassword::OnBnClickedApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	m_strOriPassword = pDoc->m_config.m_strPassword;

	m_strOriPassword.MakeUpper();
	m_strPassword.MakeUpper();
	m_strNewPassword.MakeUpper();
	m_strChkPassword.MakeUpper();

	//if (m_nUser == 0)
	//{
	//	AfxMessageBox(L"Operator cannot change password.");
	//	return;
	//}
	
	if (m_strOriPassword != m_strPassword)
	{
		AfxMessageBox(L"Passwords do not match.");
		return;
	}

	if (m_strNewPassword == m_strChkPassword)
	{		
		/*char in[255];
		char out[255];

		USES_CONVERSION;
		memcpy(in, W2A(m_strNewPassword), 255);

		pDoc->m_config.Encrypt(in, out, 255);
		m_strPassword = A2W(out);*/
		m_strPassword = m_strNewPassword;

		CIni ini;
		ini.SetPathName(pDoc->m_config.m_strConfigPath);
		ini.WriteString(L"IPE", L"EDW", m_strPassword);

		AfxMessageBox(L"Changed Password.");
		m_bChgPassword = TRUE;
	}
	else
	{
		AfxMessageBox(L"New password and check password do not match.");
		return;
	}
}


void CDlgPassword::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	UpdateData();

	if (!pDoc->m_User.LogOn(m_strPassword))
	{
		AfxMessageBox(L"Passwords do not match.");
		return;
	}

	CDialog::OnOK();	
}

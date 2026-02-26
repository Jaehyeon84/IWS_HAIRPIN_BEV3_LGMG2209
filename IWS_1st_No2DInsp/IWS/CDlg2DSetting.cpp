// CDlg2DSetting.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlg2DSetting.h"
#include "afxdialogex.h"


// CDlg2DSetting 대화 상자

IMPLEMENT_DYNAMIC(CDlg2DSetting, CDialogEx)

CDlg2DSetting::CDlg2DSetting(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_2D_INSPECTION_SET, pParent)
	, m_strEditPath(_T(""))
{
	m_str2DPathBefore = _T("");
	m_b2DUseBefore = FALSE;

	m_str2DPath = _T("");
	m_b2DUse = FALSE;	

}

CDlg2DSetting::~CDlg2DSetting()
{
}

void CDlg2DSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strEditPath);
}


BEGIN_MESSAGE_MAP(CDlg2DSetting, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_2D_PATH_SET, &CDlg2DSetting::OnBnClickedButton2dPathSet)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CDlg2DSetting::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDlg2DSetting::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_RADIO_2D_OFF, &CDlg2DSetting::OnBnClickedRadio2dOff)
	ON_BN_CLICKED(IDC_RADIO_2D_ON, &CDlg2DSetting::OnBnClickedRadio2dOn)
END_MESSAGE_MAP()


// CDlg2DSetting 메시지 처리기

BOOL CDlg2DSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_strEditPath = m_str2DPathBefore;
	UpdateData(FALSE);

	if (m_b2DUseBefore)
	{
		((CButton*)GetDlgItem(IDC_RADIO_2D_OFF))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_2D_ON))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_2D_OFF))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO_2D_ON))->SetCheck(0);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlg2DSetting::OnBnClickedRadio2dOff()
{
	((CButton*)GetDlgItem(IDC_RADIO_2D_OFF))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO_2D_ON))->SetCheck(0);

	m_b2DUseBefore = FALSE;
}


void CDlg2DSetting::OnBnClickedRadio2dOn()
{
	((CButton*)GetDlgItem(IDC_RADIO_2D_OFF))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_2D_ON))->SetCheck(1);

	m_b2DUseBefore = TRUE;
}


void CDlg2DSetting::OnBnClickedButton2dPathSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString str = _T("Recipe Setting(*.recipe)|*.recipe||");
	CString file = (CString)m_str2DPath.GetBuffer();
	
	CFileDialog dlgFile(
		TRUE,														//BOOL bOpenFileDialog,
		_T("*.wrk"),												//LPCTSTR lpszDefExt = NULL,
		file, 									//LPCTSTR lpszFileName = NULL,												
		OFN_HIDEREADONLY | OFN_LONGNAMES,		//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		str													//LPCTSTR lpszFilter = NULL,
																	//CWnd* pParentWnd = NULL,
																	//DWORD dwSize = 0,
																	//BOOL bVistaStyle = TRUE
	);

	if (dlgFile.DoModal() != IDOK) return;

	m_str2DPathBefore = dlgFile.GetPathName();
	
	m_strEditPath = m_str2DPathBefore;
	UpdateData(FALSE);

}

void CDlg2DSetting::OnBnClickedButtonCancel()
{
	CDialogEx::OnCancel();
}


void CDlg2DSetting::OnBnClickedButtonOk()
{
	m_str2DPath = m_str2DPathBefore;// +_T("\r\n");
	m_b2DUse = m_b2DUseBefore;

	CDialogEx::OnOK();
}


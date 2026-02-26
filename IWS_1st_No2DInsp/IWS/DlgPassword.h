#pragma once


// CDlgPassword 대화 상자

class CDlgPassword : public CDialog
{
	DECLARE_DYNAMIC(CDlgPassword)

public:
	CDlgPassword(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgPassword();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PASSWORD };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	WORD m_nUser;
	CString m_strOriPassword;

	CString m_strPassword;
	CString m_strNewPassword;
	CString m_strChkPassword;
	BOOL m_bChgPassword;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedOk();
};

#pragma once


// CDlgLicense 대화 상자

class CDlgLicense : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLicense)

public:
	CDlgLicense(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgLicense();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LICENSE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editLicense;
	CStringA m_strLicense;
	afx_msg void OnBnClickedOk();
};

#pragma once


// CDlg2DSetting 대화 상자

class CDlg2DSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlg2DSetting)

public:
	CDlg2DSetting(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlg2DSetting();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_2D_INSPECTION_SET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_str2DPathBefore;
	CString m_str2DPath;

	BOOL m_b2DUseBefore;
	BOOL m_b2DUse;

	CString m_strEditPath;

	afx_msg void OnBnClickedButton2dPathSet();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedRadio2dOff();
	afx_msg void OnBnClickedRadio2dOn();

	virtual BOOL OnInitDialog();
};

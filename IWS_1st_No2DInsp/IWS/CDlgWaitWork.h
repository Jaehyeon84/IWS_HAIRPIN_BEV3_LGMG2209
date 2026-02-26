#pragma once
#include "./util/Label.h"
// CDlgWaitWork 대화 상자

class CDlgWaitWork : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWaitWork)

public:
	CDlgWaitWork(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgWaitWork();

	void SetMessage(CString strMsg);
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_WAIT_WORK };
#endif
	void AssignStopFlag(BOOL *pFlag, BOOL *pAbort= NULL);
	virtual void OnOk() {};
	virtual void OnCancel() {};
	BOOL *m_pStop;
	BOOL *m_pAbort;
protected:
	CString m_strMessage;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonWorkStop();
	CLabel m_stcMessage;
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	CButton m_btnStop;
};

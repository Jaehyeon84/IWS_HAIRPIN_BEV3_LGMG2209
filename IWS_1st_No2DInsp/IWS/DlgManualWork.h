#pragma once
#include "./util/AutoEdit.h"

// CDlgManualWork 대화 상자

class CDlgManualWork : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgManualWork)

public:
	CDlgManualWork(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgManualWork();

	double m_dPowerRatio;
	double m_dSpeedRatio;


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MANUAL_WORK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CAutoEditDbl m_editPowerRatio;
	CAutoEditDbl m_editSpeedRatio;
	int m_iSelection;
	virtual BOOL OnInitDialog();
};

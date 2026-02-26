#pragma once
#include "util/AutoEdit.h"


// CDlgHatchPattern 대화 상자

class CDlgHatchPattern : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHatchPattern)

public:
	CDlgHatchPattern(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgHatchPattern();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HATCH_PATTERN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	double m_dAngle;
	double m_dDistance;

	CAutoEditDbl m_editDistance;
	CAutoEditDbl m_editAngle;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};

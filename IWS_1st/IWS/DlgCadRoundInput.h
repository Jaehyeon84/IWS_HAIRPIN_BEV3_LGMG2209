#pragma once


// CDlgCadRoundInput 대화 상자
#include "BaseDialog.h"
#include "./util/AutoEdit.h"
class CDlgCadRoundInput : public CBaseDialog
{
	DECLARE_DYNAMIC(CDlgCadRoundInput)

public:
	CDlgCadRoundInput(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgCadRoundInput();
	static double m_dRound;
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CAD_ROUND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CAutoEditDbl m_editRound;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};

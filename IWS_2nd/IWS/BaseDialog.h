#pragma once

#define COLOR_FORVIEW_BACK COLOR_GREY32

// CVisDialogII 대화 상자입니다.

class CBaseDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CBaseDialog)

public:
//	CBaseDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CBaseDialog(UINT nIDTemplate, CWnd* pParent);
	virtual ~CBaseDialog();
	void SetBkColor(COLORREF refColor);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_BASE };
#endif
protected:
	CBrush m_backBrush;
	CBrush m_ctrlBrush;
	COLORREF m_backColor;
	COLORREF m_ctrlColor;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};

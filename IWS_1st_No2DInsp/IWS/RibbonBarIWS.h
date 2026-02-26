#pragma once
#include <afxribbonbar.h>
#define UWM_ACTIVE_RIBBON_TAB_CHANGED_MSG _T("CRibbonBarIWSTabChanged")

class CRibbonBarIWS :
	public CMFCRibbonBar
{
	DECLARE_DYNAMIC(CRibbonBarIWS)
public:
	CRibbonBarIWS();
	virtual ~CRibbonBarIWS();
	int m_LastActiveCategory;
	void DelQAT();
	void DelAppButton();
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnPaneContextMenu(CWnd* pParentFrame, CPoint point);
	virtual BOOL OnShowRibbonContextMenu(CWnd* pWnd, int x, int y, CMFCRibbonBaseElement* pHit);

	afx_msg void OnSize(UINT nType, int cx, int cy);
};


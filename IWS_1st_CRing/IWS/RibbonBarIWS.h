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
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


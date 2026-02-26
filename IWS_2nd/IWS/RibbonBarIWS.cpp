#include "stdafx.h"
#include "RibbonBarIWS.h"

DECLARE_USER_MESSAGE(UWM_ACTIVE_RIBBON_TAB_CHANGED)

IMPLEMENT_DYNAMIC(CRibbonBarIWS, CMFCRibbonBar)

CRibbonBarIWS::CRibbonBarIWS()
{
	m_LastActiveCategory = 1;
}


CRibbonBarIWS::~CRibbonBarIWS()
{
}

BEGIN_MESSAGE_MAP(CRibbonBarIWS, CMFCRibbonBar)
END_MESSAGE_MAP()



BOOL CRibbonBarIWS::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONUP && pMsg->wParam == 0)
	{

		//Allow ribbon to handle itself first
		BOOL result = CMFCRibbonBar::PreTranslateMessage(pMsg);

		//Get new active tab
		int activeTab = GetCategoryIndex(GetActiveCategory());
		//If tab has changed, forward message
		if (activeTab != m_LastActiveCategory)
		{
			//forward message to parent
			::SendMessage(GetParentFrame()->GetSafeHwnd(), UWM_ACTIVE_RIBBON_TAB_CHANGED, activeTab, 0);
			m_LastActiveCategory = activeTab;
		}
		   
		return result;
	}
	 return CMFCRibbonBar::PreTranslateMessage(pMsg);
}

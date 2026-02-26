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
void CRibbonBarIWS::DelQAT()
{
	//빠른 실행 도구 모음 사용자 지정 (Customize Quick Access tool : QATool) 버튼 제거
	//  -> QATool의  drop down menu 제거 
	m_QAToolbar.RemoveAll();
}

void CRibbonBarIWS::DelAppButton()
{
	SetApplicationButton(NULL, CSize());
	ForceRecalcLayout();
}

void CRibbonBarIWS::OnPaneContextMenu(CWnd* pParentFrame, CPoint point)
{
}
BOOL CRibbonBarIWS::OnShowRibbonContextMenu(CWnd* pWnd, int x, int y, CMFCRibbonBaseElement* pHit)
{
	return TRUE;
}

void CRibbonBarIWS::OnSize(UINT nType, int cx, int cy)
{
	CMFCRibbonBar::OnSize(nType, cx, cy);

	if ((GetHideFlags() & AFX_RIBBONBAR_HIDE_ELEMENTS))
		ToggleMimimizeState();
}
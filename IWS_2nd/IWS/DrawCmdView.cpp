#include "stdafx.h"
#include "IWS.h"
#include "DrawCmdView.h"


CDrawCmdView::CDrawCmdView()
{
	g_hLcCmd = NULL;
}


CDrawCmdView::~CDrawCmdView()
{
}

void CDrawCmdView::InitPane()
{
	lcWndSetCmdwin(g_hLcWnd, g_hLcCmd);
}
BEGIN_MESSAGE_MAP(CDrawCmdView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


int CDrawCmdView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	DWORD dwStyle = GetControlBarStyle();
	dwStyle &= ~(AFX_CBRS_CLOSE | AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE);
	SetControlBarStyle(dwStyle);

	g_hLcCmd = lcCreateCmdwin(this->GetSafeHwnd(), 0, 0, 100, 100);

	return 0;
}


void CDrawCmdView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	if (g_hLcCmd)
	{
		lcCmdwinResize(g_hLcCmd, 0, 0, cx, cy);
	}
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

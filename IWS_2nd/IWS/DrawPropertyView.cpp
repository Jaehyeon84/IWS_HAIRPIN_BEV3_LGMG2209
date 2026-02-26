#include "stdafx.h"
#include "IWS.h"
#include "DrawPropertyView.h"


CDrawPropertyView::CDrawPropertyView()
{
	g_hLcProp = NULL;
}


CDrawPropertyView::~CDrawPropertyView()
{
}

void CDrawPropertyView::InitPane()
{
	lcWndSetProps(g_hLcWnd, g_hLcProp);
}

BEGIN_MESSAGE_MAP(CDrawPropertyView, CDockablePane)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


void CDrawPropertyView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	if(g_hLcProp)
		lcPropsResize(g_hLcProp, 0,0,cx,cy);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


int CDrawPropertyView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	DWORD dwStyle = GetControlBarStyle();
	dwStyle &= ~(AFX_CBRS_CLOSE | AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE);
	SetControlBarStyle(dwStyle);
	g_hLcProp = lcCreateProps(this->GetSafeHwnd());
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	return 0;
}

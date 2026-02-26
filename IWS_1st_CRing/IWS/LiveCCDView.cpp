#include "stdafx.h"
#include "IWS.h"
#include "LiveCCDView.h"
//#include "VRSaux/HubiVRSDlg.h"
#include "VRSaux/DispImgDlg.h"

CLiveCCDView::CLiveCCDView()
{
	m_pDisplayImgDlg = NULL;
}


CLiveCCDView::~CLiveCCDView()
{
	if (m_pDisplayImgDlg != NULL)
	{
		delete m_pDisplayImgDlg;
		m_pDisplayImgDlg = NULL;
	}
}

void CLiveCCDView::InitLiveCCD()
{
	m_pDisplayImgDlg = new CDispImgDlg();
	m_pDisplayImgDlg->Create(IDD_CAMERA_VIEW_DLG, this);
}

BEGIN_MESSAGE_MAP(CLiveCCDView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


int CLiveCCDView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	DWORD dwStyle = GetControlBarStyle();
	dwStyle &= ~(/*AFX_CBRS_CLOSE | AFX_CBRS_FLOAT | */AFX_CBRS_AUTOHIDE);
	SetControlBarStyle(dwStyle);

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void CLiveCCDView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_pDisplayImgDlg)
		m_pDisplayImgDlg->MoveWindow(0, 0, cx, cy);
}


void CLiveCCDView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
				   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(0,0,0));
}

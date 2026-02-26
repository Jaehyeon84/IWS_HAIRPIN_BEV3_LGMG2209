// AW_CMultiViewSplitter.cpp: implementation of the AW_CMultiViewSplitter class.
// mspark 2006.05.17
//  - no size
//	- multi-view    
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "NSizeMultiViewSplitter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CNSizeMultiViewSplitter, CSplitterWndEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
using namespace std;

CNSizeMultiViewSplitter::CNSizeMultiViewSplitter()
{
	m_nIDCounter = 1;
	m_bFixSplitBar = FALSE;
}

CNSizeMultiViewSplitter::~CNSizeMultiViewSplitter()
{

}

// override: don't allow resize
void CNSizeMultiViewSplitter::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_bFixSplitBar)	CWnd::OnLButtonDown(nFlags, point);
	else				CSplitterWndEx::OnLButtonDown(nFlags, point);
}

// override: don't allow setting cursor
void CNSizeMultiViewSplitter::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bFixSplitBar)	CWnd::OnMouseMove(nFlags, point);
	else				CSplitterWndEx::OnMouseMove(nFlags, point);
}

int CNSizeMultiViewSplitter::AddView(int nRow, int nCol, CRuntimeClass * pViewClass, 
								   CCreateContext* pContext)
{
	// hide the current view of the pane
	int PreviousID = HideCurrentView(nRow, nCol);

	// create the new view, if fail, set the previous view current 
	if (CreateView(nRow, nCol, pViewClass, CSize(10,10), pContext) == 0)
	{
		if (PreviousID != -1)
			SetCurrentView(nRow, nCol, PreviousID);
		return -1;
	}

	// get and store the niew view
	int NewViewID = m_nIDCounter;
	CWnd* pNewWnd = GetPane(nRow, nCol);
	CPoint pane(nRow, nCol);
	long paneID = MAKELONG(pane.x,pane.y);
	m_mapViewPane.insert(map<int, long>::value_type(NewViewID,paneID));
	m_mapIDViews.insert(map<int, CWnd*>::value_type(NewViewID, pNewWnd));

	// set the new view current
	SetCurrentView(nRow, nCol, NewViewID);

	RedrawWindow();
	m_nIDCounter ++;
	return NewViewID;
}

void CNSizeMultiViewSplitter::ShowView(int nViewID, BOOL bRepaint)
{
	if (GetView(nViewID) == NULL)
		return;

	// find the pane containing the view 
	CPoint pane;
	GetPaneFromViewID(nViewID, &pane);

	// switch views
	HideCurrentView(pane.x, pane.y);
	SetCurrentView(pane.x, pane.y, nViewID);	

	if(bRepaint) RecalcLayout();
}

CWnd* CNSizeMultiViewSplitter::GetView(int nViewID)
{
	map<int, CWnd*>::iterator itView;

	itView = m_mapIDViews.find(nViewID);
	if(itView==m_mapIDViews.end())
		return NULL;
	else
		return (*itView).second;
}

CWnd* CNSizeMultiViewSplitter::GetCurrentView(int nRow, int nCol, int * nCurID)
{
	long paneID = MAKELONG(nRow,nCol);
	
	map<long, int>::iterator itCur;
	itCur = m_mapCurrentViews.find(paneID);
	if (itCur == m_mapCurrentViews.end())
		return NULL;
	else
	{
		int PreviousID = (*itCur).second;
		*nCurID = PreviousID;
		return GetView(PreviousID);
	}
}

void CNSizeMultiViewSplitter::SetCurrentView(int nRow, int nCol, int nViewID)
{
	long paneID = MAKELONG(nRow,nCol);

	map<long, int>::iterator itCur;
	itCur = m_mapCurrentViews.find(paneID);
	if (itCur != m_mapCurrentViews.end())
		(*itCur).second = nViewID;
	else
		m_mapCurrentViews.insert(map<long,int>::value_type(paneID,nViewID));

	CWnd * pView = GetView(nViewID);
	pView->SetDlgCtrlID(IdFromRowCol(nRow, nCol));
	pView->ShowWindow(SW_SHOW);
}

int CNSizeMultiViewSplitter::HideCurrentView(int nRow, int nCol)
{
	int prevID;
	CWnd * pCurView = GetCurrentView(nRow, nCol, &prevID);
	if (pCurView == NULL)
		return -1;
	else
	{
		pCurView->SetDlgCtrlID(0);
		pCurView->ShowWindow(SW_HIDE);	
		return prevID;
	}
}

void CNSizeMultiViewSplitter::GetPaneFromViewID(int nViewID, CPoint * pane)
{
	map<int, long>::iterator itPane;
	itPane = m_mapViewPane.find(nViewID);
	if (itPane==m_mapViewPane.end())
	{
		pane = NULL;
		return;
	}
	long paneID = (*itPane).second;
	CPoint p(paneID);
	pane->x = p.x;
	pane->y = p.y;
}

void CNSizeMultiViewSplitter::SetSplitBarFix(BOOL bFix) 
{
	m_bFixSplitBar = bFix;
}

void CNSizeMultiViewSplitter::ShowSplitBar(BOOL bShow)
{
	if(bShow)	SetSplitBarSize(7, 2);	// default size
	else		SetSplitBarSize(1, 0);
}

void CNSizeMultiViewSplitter::SetSplitBarSize(int iBar, int iBorder)
{
	if(iBar == 0)
	{
		TRACE("SetSplitterBarSize() Err. -> iBar cannot 0");
		iBar = 1;
	}

	m_cxSplitter	= m_cySplitter		= iBar;
	m_cxSplitterGap	= m_cySplitterGap	= iBar;
	m_cxBorder		= m_cyBorder		= iBorder;

	RecalcLayout();
}

void CNSizeMultiViewSplitter::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg)
{
	CSplitterWndEx::OnDrawSplitter(pDC, nType, rectArg);

	if (pDC == NULL)
	{
		RedrawWindow(rectArg, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);
		return;
	}

	if(m_bShowSplitBar)
		CSplitterWndEx::OnDrawSplitter(pDC, nType, rectArg);
	else
	{
		CRect rect = rectArg;
		int CX_BORDER = 1, CY_BORDER = 1;
		if(nType == splitBar) //nType == splitBorder
		{
			//COLORREF clr = RGB_MAIN;

			pDC->Draw3dRect(rect, m_bColor, m_bColor);
			rect.InflateRect(-CX_BORDER, -CY_BORDER);
			
			pDC->FillSolidRect(rect, m_bColor);
			return;
		}
		else if(nType == splitBorder)
		{
			pDC->Draw3dRect(rect, m_bColor, m_bColor);
			rect.InflateRect(-CX_BORDER, -CY_BORDER);
			pDC->Draw3dRect(rect, m_bColor, m_bColor);
			return;
		}
		else
			CSplitterWndEx::OnDrawSplitter(pDC, nType, rectArg);


		pDC->FillSolidRect(rectArg, m_bColor);
	}
}

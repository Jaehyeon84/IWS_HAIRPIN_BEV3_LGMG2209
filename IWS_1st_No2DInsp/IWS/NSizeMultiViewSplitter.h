// NSizeMultiViewSplitter.h: interface for the NSizeMultiViewSplitter class.
/////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_AW_CMULTIVIEWSPLITTER_H__464C08E9_8989_11D4_B4E3_005004D85AA1__INCLUDED_)
#define AFX_AW_CMULTIVIEWSPLITTER_H__464C08E9_8989_11D4_B4E3_005004D85AA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable:4786)
#pragma warning(push)
#pragma warning(disable:4100)
#include <map>
#pragma warning(pop)
  
class CNSizeMultiViewSplitter : public CSplitterWndEx  
{

public:
	CNSizeMultiViewSplitter();
	virtual ~CNSizeMultiViewSplitter();
	int AddView(int nRow, int nCol, CRuntimeClass * pViewClass, 
				CCreateContext* pContext);
	void ShowView(int nViewID, BOOL bRepaint=FALSE);	// blue 2009.10.28 add bRepaint
	CWnd* GetView(int nViewID);

	BOOL	m_bFixSplitBar;		// default : FALSE
	void	SetSplitBarFix(BOOL bFix);
	void	ShowSplitBar(BOOL bShow);
	void	SetSplitBarSize(int iBar, int iBorder);

	// blue 2009.10.26 
	BOOL		m_bShowSplitBar;
	COLORREF	m_bColor;
	void		SetSplitColor(COLORREF color) { m_bColor = color; }

	//m_bHasHScroll	// blue 2009.10.28 

protected:
	std::map<int, long> m_mapViewPane;
	std::map<long, int> m_mapCurrentViews;
	std::map<int, CWnd*> m_mapIDViews;

	int m_nIDCounter;

	CWnd* GetCurrentView(int nRow, int nCol, int * nCurID);
	void SetCurrentView(int nRow, int nCol, int nViewID);
	int HideCurrentView(int nRow, int nCol);
	void GetPaneFromViewID(int nViewID, CPoint * pane);

	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);	// blue 2009.10.26 
	//virtual void RecalcLayout();    // call after changing sizes				// blue 2009.10.28 

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
}; 

#endif // !defined(AFX_AW_CMULTIVIEWSPLITTER_H__464C08E9_8989_11D4_B4E3_005004D85AA1__INCLUDED_)

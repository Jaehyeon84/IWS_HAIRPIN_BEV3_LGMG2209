#include "stdafx.h"
#include "IWS.h"

#include "IWSDoc.h"
#include "DrawView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDrawView, CView)

BEGIN_MESSAGE_MAP(CDrawView, CView)

END_MESSAGE_MAP()

CDrawView::CDrawView()
{
}


CDrawView::~CDrawView()
{
}

#ifdef _DEBUG
#endif
CIWSDoc* CDrawView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIWSDoc)));
		return (CIWSDoc*)m_pDocument;
}


void CDrawView::OnDraw(CDC* pDC)
{
	CIWSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}

BOOL CDrawView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

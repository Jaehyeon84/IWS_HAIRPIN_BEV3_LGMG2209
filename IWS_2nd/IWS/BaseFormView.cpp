#include "stdafx.h"
#include "BaseFormView.h"


CBaseFormView::CBaseFormView(UINT nID):CFormView(nID)
{
	m_bInitUpdated = FALSE;
}


CBaseFormView::~CBaseFormView()
{
	DeleteObject(m_brushBack);
}
BEGIN_MESSAGE_MAP(CBaseFormView, CFormView)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()


BOOL CBaseFormView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (pDC) {
		CRect r;
		GetClientRect(&r);
		if ((HBRUSH)m_brushBack == NULL)
			m_brushBack.CreateSolidBrush(COLOR_FORVIEW_BACK);
		pDC->FillRect(&r, &m_brushBack);
	}
	
	return TRUE;

}


HBRUSH CBaseFormView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

//	TCHAR classname[MAX_PATH];
	
//	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
//		return hbr;
//	if (_tcsicmp(classname, _T("EDIT")) == 0)
//		return hbr;
//	if (_tcsicmp(classname, _T("COMBOBOX")) == 0)
//		return hbr;
//	if (_tcsicmp(classname, _T("COMBOLBOX")) == 0)
//		     return hbr;
//	if (_tcsicmp(classname, _T("LISTBOX")) == 0)
//		return hbr;
//	if (_tcsicmp(classname, WC_TREEVIEW) == 0)
//		return hbr;
//	if (_tcsicmp(classname, _T("BUTTON")) == 0)
//		return hbr;
//	if (_tcsicmp(classname, _T("STATIC")) == 0)
//		return hbr;
	pDC->SetBkColor(COLOR_FORVIEW_BACK);
	pDC->SetTextColor(COLOR_WHITESMOKE);
	if ((HBRUSH)m_brushBack == NULL)
		m_brushBack.CreateSolidBrush(COLOR_FORVIEW_BACK);
	return (HBRUSH)m_brushBack;
}


void CBaseFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void CBaseFormView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);			//버튼의 dc구하기
	RECT rect = lpDrawItemStruct->rcItem;        //버튼영역 구하기

	UINT state = lpDrawItemStruct->itemState;    //버튼상태구하기
	if ((state & (ODS_SELECTED | ODS_DISABLED)))
	{
		//		dc.DrawEdge(&rect, EDGE_ETCHED, BF_RECT);
		CRect rc = rect;
		rc.DeflateRect(1,1,1,1);
		dc.FillSolidRect(&rect, COLOR_GREY40);
		dc.RoundRect(&rect, CPoint(6, 6));
		dc.Draw3dRect(&rect, COLOR_ORANGERED4, COLOR_ORANGERED4);//버튼색상

		dc.Draw3dRect(rc, COLOR_ORANGERED4, COLOR_ORANGERED4);//버튼색상
		dc.SetTextColor(COLOR_ORANGERED4);
	}
	else if ((state & ODS_FOCUS)) {
		dc.SetTextColor(COLOR_OLIVEDRAB);
		dc.FillSolidRect(&rect, COLOR_FORVIEW_BACK);             //버튼색상
		dc.Draw3dRect(&rect, COLOR_WHITESMOKE, COLOR_WHITESMOKE);		//버튼의 외각선 그리기
	}
	else
	{
		dc.FillSolidRect(&rect, COLOR_FORVIEW_BACK);             //버튼색상
		dc.Draw3dRect(&rect, COLOR_WHITESMOKE, COLOR_WHITESMOKE);		//버튼의 외각선 그리기
		dc.SetTextColor(COLOR_WHITESMOKE);
																		//		dc.SetBkMode(TRANSPARENT);
	}
	dc.SetBkMode(TRANSPARENT);
	                  //texttort
	TCHAR buffer[MAX_PATH];                            //버튼의 text를 얻기위한 임시버퍼
	ZeroMemory(buffer, MAX_PATH);                       //버퍼초기화
	::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH); //버튼의 text얻기
	dc.DrawTextW(buffer, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE); //버튼의 text넣기
	dc.Detach();
	CFormView::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

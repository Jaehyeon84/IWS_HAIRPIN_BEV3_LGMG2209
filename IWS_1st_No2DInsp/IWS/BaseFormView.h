#pragma once
#define COLOR_FORVIEW_BACK COLOR_GREY32

class CBaseFormView :
	public CFormView
{
public:
	CBaseFormView(UINT nID);
	virtual ~CBaseFormView();
	CBrush m_brushBack;
	BOOL m_bInitUpdated;

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnInitialUpdate();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};


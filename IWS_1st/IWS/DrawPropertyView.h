#pragma once
#include <afxdockablepane.h>
class CDrawPropertyView :
	public CDockablePane
{
public:
	CDrawPropertyView();
	~CDrawPropertyView();

	//HANDLE m_hProps;
	void InitPane();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


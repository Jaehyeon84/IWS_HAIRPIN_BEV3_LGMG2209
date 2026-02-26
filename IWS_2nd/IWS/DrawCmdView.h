#pragma once
#include <afxdockablepane.h>
class CDrawCmdView :
	public CDockablePane
{
public:
	CDrawCmdView();
	~CDrawCmdView();
	void InitPane();
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


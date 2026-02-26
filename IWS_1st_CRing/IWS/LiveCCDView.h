#pragma once
#include <afxdockablepane.h>

class CDispImgDlg;
class CLiveCCDView :
	public CDockablePane
{
public:
	CLiveCCDView();
	~CLiveCCDView();

	CDispImgDlg* m_pDisplayImgDlg;

	void InitLiveCCD();
public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
};


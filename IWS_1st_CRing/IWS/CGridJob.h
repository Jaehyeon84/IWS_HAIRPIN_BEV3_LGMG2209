#pragma once

#include "UltimateGridLib/Include/UGCtrl.h"

#define  UWM_GRIDJOB_MSG _T("CGridJob")

class CGridJob :
	public CUGCtrl
{
public:
	CGridJob();
	~CGridJob();

	virtual void OnSetup();
	virtual COLORREF OnGetDefBackColor(int section);
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnKeyDown(UINT *vcKey, int processed);
	virtual void OnSize(UINT nType, int cx, int cy);
	void AddWorklist(CString str);
	void ClearWorklist();
};


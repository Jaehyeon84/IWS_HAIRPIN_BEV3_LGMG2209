#pragma once

#include "UltimateGridLib/Include/UGCtrl.h"

#define  UWM_GRIDWELDCAL_MSG _T("CGridWeldCal")

class CGridWeldCal :
	public CUGCtrl
{
public:
	CGridWeldCal();
	~CGridWeldCal();

	HWND m_hRcvHandle;
	virtual void OnSetup();
	virtual COLORREF OnGetDefBackColor(int section);
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnKeyDown(UINT *vcKey, int processed);

	void SetData(int iRow, double x, double y, double sx, double sy);
	int SetCurrentData(double sx, double sy);

};


#pragma once

#include "UltimateGridLib/Include/UGCtrl.h"

#define UWM_CGRIDPOWERSHAPE_MSG _T("CGridPowerShape_message")

class CGridPowerShape :
	public CUGCtrl
{
public:
	CGridPowerShape();
	~CGridPowerShape();
	void SetEventReciveHandle(HWND hwnd) { m_hEventRcv = hwnd; };

	void SetDouble(int col, long row, double dval);
	double GetDouble(int col, long row);
	virtual void OnSetup();
	virtual COLORREF OnGetDefBackColor(int section);
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnKeyDown(UINT *vcKey, int processed);
	virtual int OnEditStart(int col, long row, CWnd **edit);
	virtual int OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag);
	virtual int OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow);
protected:
	HWND m_hEventRcv;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


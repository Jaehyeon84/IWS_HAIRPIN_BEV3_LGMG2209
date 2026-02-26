#pragma once

#include "UltimateGridLib/Include/UGCtrl.h"

class CGridLayerDia :
	public CUGCtrl
{
public:
	CGridLayerDia();
	~CGridLayerDia();

	void SetLayerDia(int iLayer, double dval);
	void SetNumberOfLayers(int nLayer);
	virtual void OnSetup();
	virtual COLORREF OnGetDefBackColor(int section);
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnKeyDown(UINT *vcKey, int processed);
	virtual int OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag);
};


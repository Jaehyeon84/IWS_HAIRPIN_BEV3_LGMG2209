#pragma once

#include "UltimateGridLib/Include/UGCtrl.h"

#define  UWM_GRIDJOBLIST_MSG _T("CGridJoblist")
#define MSG_GRID_CHANGEROW 1

class CGridJoblist :
	public CUGCtrl
{
public:
	CGridJoblist();
	~CGridJoblist();

	HWND m_hRcvEventHandle;
	void SetReciveHandle(HWND hwnd) {
		m_hRcvEventHandle = hwnd;
		m_bEnableEvent = TRUE;
	};
	virtual void OnSetup();
	virtual COLORREF OnGetDefBackColor(int section);
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnKeyDown(UINT *vcKey, int processed);
	virtual void OnRowChange(long oldrow, long newrow);
	virtual void OnSelectionChanged(int startCol, long startRow, int endCol, long endRow, int blockNum);

	void AddJoblist(CString str, BOOL bDoJob = FALSE);
	void ClearJoblist();
	virtual void OnSize(UINT nType, int cx, int cy);

	void EnableEvent(BOOL benable) { m_bEnableEvent = benable; };
	void SetLayer(int nLayer) {
		m_nLayer = nLayer;
	}
	BOOL m_bEnableEvent;
	int m_nSlot, m_nLayer;
};


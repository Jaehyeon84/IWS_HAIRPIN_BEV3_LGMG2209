#pragma once

#include "UltimateGridLib/Include/UGCtrl.h"

#define MAX_GRID_PLC_DI 12
#define MAX_GRID_PLC_DO 12

class CGridPLC :
	public CUGCtrl
{
public:
	CGridPLC();
	~CGridPLC();

	CStringArray m_strArrInputname;
	CStringArray m_strArrOutputname;
	virtual void OnSetup();
	virtual COLORREF OnGetDefBackColor(int section);
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnKeyDown(UINT *vcKey, int processed);
	void SetInOut(int in, int out, int inCmd, int outCmd);
	void SetNumDIO(int nIn, int nOut) {
		m_numDI = nIn;
		m_numDO = nOut;
	}
	void SetNumCmdIO(int nInOut) { m_numDIOCmd = nInOut; };
	int m_numDI;
	int m_numDO;
	int m_numDIOCmd;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


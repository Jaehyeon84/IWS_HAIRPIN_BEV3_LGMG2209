#pragma once
#include "./UltimateGridLib/include/UGCtrl.h"
#define MAX_GRID_DIO 15
#define MAX_SCANNER_DO 15
#define MAX_SCANNER_DI 15

class CGridDIO :
	public CUGCtrl
{
public:
	CGridDIO();
	~CGridDIO();

	CStringArray m_strArrInputname;
	CStringArray m_strArrOutputname;
	virtual void OnSetup();
	virtual COLORREF OnGetDefBackColor(int section);
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnKeyDown(UINT *vcKey, int processed);
	void SetInOut(int in, int out);
	void SetErrorDIO(int iDI, int iDO) { 
		m_errDI = iDI; 
		m_errDO = iDO;
	}
	void SetNumDIO(int nIn, int nOut) {
		m_numDI = nIn;
		m_numDO = nOut;
	}

	int m_numDI;
	int m_numDO;
	int m_errDI;
	int m_errDO;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


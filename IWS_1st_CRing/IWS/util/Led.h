#if !defined(AFX_LED_H__20C72BAE_748A_4880_8258_CF57D3A8A83E__INCLUDED_)
#define AFX_LED_H__20C72BAE_748A_4880_8258_CF57D3A8A83E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Led.h : header file
#define LIMIT_SENSOR	0
#define UNIVERSAL_IO	1
#define EXTERNAL_IO		2
#define ALARM_IO		3
/////////////////////////////////////////////////////////////////////////////
const COLORREF CLOUDBLUE = RGB(128, 184, 223);
const COLORREF WHITE = RGB(255, 255, 255);
const COLORREF BLACK = RGB(1, 1, 1);
const COLORREF DKGRAY = RGB(128, 128, 128);
const COLORREF LTGRAY = RGB(192, 192, 192);
const COLORREF YELLOW = RGB(255, 255, 0);
const COLORREF DKYELLOW = RGB(128, 128, 0);
const COLORREF LTYELLOW	= RGB(255, 246, 188);
const COLORREF RED = RGB(255, 0, 0);
const COLORREF DKRED = RGB(128, 0, 0);
const COLORREF BLUE = RGB(0, 0, 255);
const COLORREF DKBLUE = RGB(0, 0, 128);
const COLORREF CYAN = RGB(0, 255, 255);
const COLORREF DKCYAN = RGB(0, 128, 128);
const COLORREF GREEN = RGB(0, 255, 0);
const COLORREF DKGREEN = RGB(0, 128, 0);
const COLORREF MAGENTA = RGB(255, 0, 255);
const COLORREF DKMAGENTA = RGB(128, 0, 128);



/////////////////////////////////////////////////////////////////////////////
// CLed window
#include "Label.h"
class CLed : public CLabel
{
// Construction
public:
	CLed();

// Attributes
public:

	COLORREF m_bOffColor[4];
	COLORREF m_bOnColor[4];
	COLORREF m_LedColor;

	int m_bSensor;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLed)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetStatus(BOOL bStatus);
	void SetType(int type);
	BOOL GetStatus();
	BOOL bLedStatus;
	void Off();
	void On();
	virtual ~CLed();

	// Generated message map functions
protected:


	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LED_H__20C72BAE_748A_4880_8258_CF57D3A8A83E__INCLUDED_)

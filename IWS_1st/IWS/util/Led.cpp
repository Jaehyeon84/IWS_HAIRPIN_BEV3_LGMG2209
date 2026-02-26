// Led.cpp : implementation file
//

#include "stdafx.h"
#include "Led.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLed
CLed::CLed()
{
	m_bOffColor[0] = RGB(100, 0, 0);
	m_bOnColor[0]	= RED;
	m_bOffColor[1] = DKYELLOW; //RGB(255, 253, 208);
	m_bOnColor[1]	= YELLOW;
	m_bOffColor[2] = DKGREEN;
	m_bOnColor[2] = GREEN;
	m_bOffColor[3] = RED;
	m_bOnColor[3] = GREEN;

	m_LedColor = m_bOffColor[0];
	bLedStatus = FALSE;
	m_bSensor = EXTERNAL_IO;
}

CLed::~CLed()
{
}


BEGIN_MESSAGE_MAP(CLed, CStatic)
	//{{AFX_MSG_MAP(CLed)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLed message handlers


void CLed::On()
{
	if (bLedStatus) return;
	m_LedColor = m_bOnColor[m_bSensor];
	bLedStatus = TRUE;
	this->SetBkColor(m_LedColor);
}

void CLed::Off()
{
	if (!bLedStatus) return;
	m_LedColor = m_bOffColor[m_bSensor];
	bLedStatus = FALSE;
	this->SetBkColor(m_LedColor);
}

BOOL CLed::GetStatus()
{
	return bLedStatus;
}

void CLed::SetType(int type)
{
	m_bSensor = type;
	if ( bLedStatus )	On();
	else				Off();
}

void CLed::SetStatus(BOOL bStatus)
{
	if ( bStatus ) 	On();
	else			Off();
}

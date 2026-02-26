#include "stdafx.h"
#include "CLaserIOCoherent.h"

CLaserIOCoherent::CLaserIOCoherent()
{
	m_strDINameArr.SetSize(15);
	m_strDINameArr[0] = L"-";
	m_strDINameArr[1] = L"-";
	m_strDINameArr[2] = L"STANDBY READY";
	m_strDINameArr[3] = L"LASER READY";
	m_strDINameArr[4] = L"GENERAL FAULT";
	m_strDINameArr[5] = L"WARNING";
	for (int i = 6; i < 10; i++)
		m_strDINameArr[i] = L"-";

	m_strDONameArr.SetSize(15);
	m_strDONameArr[0] = L"FAULT RESET";
	m_strDONameArr[1] = L"TEST WARNING LAMP";
	m_strDONameArr[2] = L"MAINS ON";
	m_strDONameArr[3] = L"STANDBY ON";
	m_strDONameArr[4] = L"GUIDE LASER";
	for (int i = 5; i < 12; i++)
		m_strDONameArr[i] = L"-";

	m_strDONameArr[13] = L"LIGHT";
}

CLaserIOCoherent::~CLaserIOCoherent()
{
}

void CLaserIOCoherent::Init()
{
	// Reset
	SetReset(TRUE);
	Sleep(100);
	SetReset(FALSE);
	Sleep(50);

	// Mains ON
	LaserPowerOn(TRUE);
	Sleep(100);
	// Laser ON
	LaserReady(TRUE);
}

void CLaserIOCoherent::Terminate()
{
	LaserReady(FALSE);
	LaserPowerOn(FALSE);
}

BOOL CLaserIOCoherent::GetStatus()
{
	return GetDILaserOn() && GetDIEmission() && !(GetDIError());
}

BOOL CLaserIOCoherent::LaserPowerOn(BOOL bFlag)
{
	BOOL bRes;
	m_pScanner->DO(DO_LaserOn, bFlag);
	_tracer(TRACE_ERROR, 1, _T("Standby ON - %s"), bFlag ? _T("ON"): _T("OFF"));
	
	Sleep(100);

	BOOL bOK = LaserPowerOnCheck(bFlag);
	if (bOK) {
		bRes = TRUE;
	}
	else {
		if (bFlag)	{
			_tracer(TRACE_ERROR, 1, _T("Laser On Timeout!"));
		}
		else {
			_tracer(TRACE_ERROR, 1, _T("Laser Off Timeout!"));
		}
		bRes = FALSE;
	}
	return bRes;
}

#define TIME_OUT 20000	// 10sec
BOOL CLaserIOCoherent::LaserPowerOnCheck(BOOL bFlag)
{
	long told = clock();
	BOOL bRes = TRUE;

	if (bFlag) // Laser Enable ON check
	{
		while (!GetDILaserOn()) {
			if (clock() - told > TIME_OUT) {
				bRes = FALSE;
				break;
			}
			//	PeekAndPump();
			Sleep(100);
		}
	}
	else	// Laser Enable OFF check
	{
		while (GetDILaserOn()) {
			if (clock() - told > TIME_OUT) {
				bRes = FALSE;
				break;
			}
			//	PeekAndPump();
			Sleep(100);
		}
	}
	return bRes;
}

void CLaserIOCoherent::LaserOn(BOOL bFlag, double dblPower /*= 0.0*/)
{
	//_tracer(TRACE_NORMAL, 1, L"LaserOn %d, Power=%f", bFlag, dblPower);
	if (bFlag) {
		m_pScanner->LaserOn(bFlag);
		SetPower(dblPower);
	}
	else {
		m_pScanner->LaserOn(FALSE);
		SetPower(0.0);
	}

}

void CLaserIOCoherent::LaserEnable(BOOL bFlag, double dblPower)
{
	//_tracer(TRACE_NORMAL, 1, L"LaserEnable %d, Power=%f", bFlag, dblPower);
	if (bFlag) {
		//DO(IDO_EmissionEnable, bFlag);
		m_pScanner->DO(DO_EmissionEnable, TRUE);
		SetPower(dblPower);
	}
	else {
		m_pScanner->DO(DO_EmissionEnable, FALSE);
		SetPower(0.0);
	}
}

BOOL CLaserIOCoherent::LaserReady(BOOL bFlag)
{
	BOOL bRes;
	m_pScanner->DO(DO_EmissionEnable, bFlag);
	_tracer(TRACE_ERROR, 1, _T("Laser Beam Ready - %s"), bFlag ? _T("ON") : _T("OFF"));
	Sleep(100);

	BOOL bOK = LaserEnableCheck(bFlag);
	if (bOK) {
		bRes = TRUE;
	}
	else {
		if (bFlag)	{
			_tracer(TRACE_ERROR, 1, _T("Laser On Timeout!"));
		}
		else{
			_tracer(TRACE_ERROR, 1, _T("Laser Off Timeout!"));
		}
		bRes = FALSE;
	}
	return bRes;
}

BOOL CLaserIOCoherent::LaserEnableCheck(BOOL bFlag)
{
	long told = clock();
	BOOL bRes = TRUE;

	if (bFlag) // Laser Beam Ready ON check
	{
		while (!GetDIEmission()) {
			if (clock() - told > TIME_OUT) {
				bRes = FALSE;
				break;
			}
			//	PeekAndPump();
			Sleep(100);
		}
	}
	else	// Laser Beam Ready OFF check
	{
		while (GetDIEmission()) {
			if (clock() - told > TIME_OUT) {
				bRes = FALSE;
				break;
			}
			//	PeekAndPump();
			Sleep(100);
		}
	}
	return bRes;
}

void CLaserIOCoherent::ChangePrgNo(int nPrgNo)
{
}

void CLaserIOCoherent::SetProgramNo(short ch)
{
}

BOOL CLaserIOCoherent::GetDIError()
{
	return check_bit(m_StatusIO, DI_Error);
}

void CLaserIOCoherent::SetGuideOn(BOOL bFlag)
{
	if (bFlag) {
		//DO(DO_PilotLaser, bFlag);
		m_pScanner->DO(DO_PilotLaser, TRUE);
		SetPower(0.0);
	}
	else {
		m_pScanner->DO(DO_PilotLaser, FALSE);
		SetPower(0.0);
	}
}


int CLaserIOCoherent::GetDOPilotLaser()
{
	return m_pScanner->GetDO(DO_PilotLaser);
}
#include "stdafx.h"
#include "CLaserIOTrump.h"

CLaserIOTrump::CLaserIOTrump()
{
	m_strDINameArr.SetSize(15);
	/*m_strDINameArr[0] = L"Ready";
	m_strDINameArr[1] = L"Ext.Active";
	m_strDINameArr[2] = L"LaserOn";
	m_strDINameArr[3] = L"LaserAssigned";
	m_strDINameArr[4] = L"Prg.Active";
	m_strDINameArr[5] = L"Monitoring";
	m_strDINameArr[6] = L"Fault Laser";*/
	m_strDINameArr[0] = L"LASER READY";
	m_strDINameArr[1] = L"EXT ACTIVE";
	m_strDINameArr[2] = L"LASER IS ON";
	m_strDINameArr[3] = L"LASER ASSIGNED";
	m_strDINameArr[4] = L"PROG ACTIVE";
	m_strDINameArr[5] = L"MONITORING";
	m_strDINameArr[6] = L"FAULT LASER";
	m_strDINameArr[7] = L"LIGHT PATH BIT0";
	m_strDINameArr[8] = L"LIGHT PATH BIT1";
	for (int i = 9; i < 10; i++)
		m_strDINameArr[i] = L"-";

	m_strDONameArr.SetSize(15);
	/*m_strDONameArr[0] = L"Reset";
	m_strDONameArr[1] = L"Ext.Active";
	m_strDONameArr[2] = L"LaserOn";
	m_strDONameArr[3] = L"ReauestOn";
	m_strDONameArr[4] = L"PilotLaser";
	m_strDONameArr[5] = L"Prg.Start";
	m_strDONameArr[6] = L"Fault.EXT";
	m_strDONameArr[7] = L"Prg.Bit.0";
	m_strDONameArr[8] = L"Prg.Bit.1";*/
	m_strDONameArr[0] = L"RESET";
	m_strDONameArr[1] = L"EXT ACTIVATION";
	m_strDONameArr[2] = L"LASER ON";
	m_strDONameArr[3] = L"REQUEST LASER";
	m_strDONameArr[4] = L"PILOT LASER";
	m_strDONameArr[5] = L"PSTART STATICAL";
	m_strDONameArr[6] = L"FAULT EXT";
	m_strDONameArr[7] = L"PROG. NO:BIT0";
	m_strDONameArr[8] = L"PROG. NO:BIT1";
	for (int i = 9; i < 13; i++)
		m_strDONameArr[i] = L"-";

	m_strDONameArr[13] = L"LIGHT";
}

CLaserIOTrump::~CLaserIOTrump()
{
}

void CLaserIOTrump::Init()
{
	SetReset(TRUE);
	SetFaultExt(FALSE);
	Sleep(100);
	LaserPowerOn(TRUE);
	Sleep(5000);
	SetRemote(TRUE);	
	SetReset(FALSE);
}

void CLaserIOTrump::Terminate()
{
	LaserEnable(FALSE);
	SetRemote(FALSE);
	LaserPowerOn(FALSE);
}

BOOL CLaserIOTrump::GetStatus()
{
	return GetDILaserOn() && GetDILaserAssigned() && !(GetDIError());
}

BOOL CLaserIOTrump::LaserPowerOn(BOOL bFlag)
{
	BOOL bRes;

	m_pScanner->DO(DO_ExtActivation, bFlag);
	Sleep(100);
	m_pScanner->DO(DO_LaserOn, bFlag);

	Sleep(100);
	BOOL bOK = LaserPowerOnCheck(bFlag);
	if (bOK)
	{
		_tracer(TRACE_ERROR, 1, _T("Laser Power ON - OK"));
		bRes = TRUE;
	}
	else
	{
		if (bFlag)
		{
			_tracer(TRACE_ERROR, 1, _T("Laser On Timeout!"));
		}
		else
		{
			_tracer(TRACE_ERROR, 1, _T("Laser Off Timeout!"));
		}
		bRes = FALSE;
	}
	return bRes;
}

BOOL CLaserIOTrump::LaserPowerOnCheck(BOOL bFlag)
{
	long told = clock();
	BOOL bRes = TRUE;
#define TIME_OUT 10000
	if (bFlag) // Laser Power ON check
	{
		while (!GetDILaserOn()) {
			if (clock() - told > TIME_OUT) {
				bRes = FALSE;
				break;
			}
			//	PeekAndPump();
			Sleep(10);
		}
	}
	else	// Laser Power OFF check
	{
		while (GetDILaserOn()) {
			if (clock() - told > TIME_OUT) {
				bRes = FALSE;
				break;
			}
			//	PeekAndPump();
			Sleep(10);
		}
	}
	return bRes;
}

void CLaserIOTrump::LaserEnable(BOOL bFlag, double dblPower)
{
	//_tracer(TRACE_NORMAL, 1, L"LaserEnable %d, Power=%f", bFlag, dblPower);
	if (bFlag) {
		SetProgramNo(1);	// Progrm no 1 : Laser
		Sleep(500);
		//DO(IDO_EmissionEnable, bFlag);
		m_pScanner->DO(DO_EmissionEnable, TRUE);
		SetPower(dblPower);
	}
	else {
		SetProgramNo(0);	// Progrm no 0 : Guide
		Sleep(500);
		m_pScanner->DO(DO_EmissionEnable, FALSE);
		SetPower(0.0);
	}
}

void CLaserIOTrump::ChangePrgNo(int nPrgNo)
{
	SetProgramNo(nPrgNo);
	Sleep(500);
	if(nPrgNo == 0)
		m_pScanner->DO(DO_EmissionEnable, FALSE);
	else
		m_pScanner->DO(DO_EmissionEnable, TRUE);
}

void CLaserIOTrump::SetProgramNo(short ch)
{
	//int nBit = 7;
	//int nLaser = 8;

	//WORD no;
	
	if(ch == 1)					// Laser beam
	{
		m_pScanner->DO(7, TRUE);
		m_pScanner->DO(8, FALSE);
	}
	else if (ch == 2)			// TEST1
	{
		m_pScanner->DO(7, FALSE);
		m_pScanner->DO(8, TRUE);
	}
	else if (ch == 3)			// TEST2
	{
		m_pScanner->DO(7, TRUE);
		m_pScanner->DO(8, TRUE);
	}
	else						// Guide
	{
		m_pScanner->DO(7, FALSE);
		m_pScanner->DO(8, FALSE);
	}

	//UINT pinnum[2] = { 7, 8 };

	/*for (int n = 0; n < 2; n++) 
	{
		BOOL bFlag = (no&(1 << n) ? TRUE : FALSE);
		m_pScanner->DO(pinnum[n], bFlag);
	}*/
	Sleep(20);
}

BOOL CLaserIOTrump::GetDIError()
{
	return check_bit(m_StatusIO, DI_Error);
}

int CLaserIOTrump::GetDOPilotLaser()
{
	return !m_pScanner->GetDO(DO_EmissionEnable);
}

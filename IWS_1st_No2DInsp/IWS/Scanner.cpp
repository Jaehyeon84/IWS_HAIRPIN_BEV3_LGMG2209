#include "stdafx.h"
#include "Scanner.h"
#include "CLaserIOTrump.h"

CScanner::CScanner()
{
	m_nCard = 0;
	//m_StatusIO = 0;
	m_dRatioPwr = 1.0;
	m_dRatioSpd = 1.0;
	m_bAbortExe = FALSE;
	//IDI_Interlock = -1;

	m_pLaserIO = new CLaserIOTrump();
	m_pLaserIO->SetScanner(this);

	for (int i = 0; i < 4; i++)
		bAlarmArr[i] = FALSE;
}

CScanner::~CScanner()
{
	if (m_pLaserIO) delete m_pLaserIO;
}

void CScanner::SetRatio(double dPwr, double dSpd)
{
	m_dRatioPwr = dPwr;
	m_dRatioSpd = dSpd;
}

void CScanner::SetCorrParam(CorrParam param)
{
	m_CorrParam = param;
}

void CScanner::SetLaserPowerSupplyON(BOOL bOn)
{
	/*switch (m_iLaser)
	{
	case TRUMPF_TRUDISK:
	case IPG_CW_YLR2000SM:
		break;
	case IPG_CW_YLR6000QCW:
	case IPG_PULSE_YLR6000QCW:
		DO(4, bOn);
		break;
	}*/
}

void CScanner::SetLaser(int iLaser)
{
	m_iLaser = iLaser;
}


BOOL CScanner::HeadStatusCheck()
{

#ifdef INTELLISCAN
	unsigned short status = GetHeadStatus();
	//unsigned short power = (unsigned short)(1 << 15);
	//unsigned short temp = (unsigned short)(1 << 14);
	unsigned short x = (unsigned short)(1 << 3);
	unsigned short y = (unsigned short)(1 << 4);

	BOOL bResPosAckX = (x & status ? TRUE : FALSE);
	BOOL bResPosAckY = (y & status ? TRUE : FALSE);

	WriteAlarmLog(TRACE_WARNING, 0, _T("Scanner Pos Ack X Warning = [%d]"), bResPosAckX);
	WriteAlarmLog(TRACE_WARNING, 1, _T("Scanner Pos Ack Y Warning = [%d]"), bResPosAckY);

	unsigned short power = (unsigned short)(1 << 7);
	unsigned short temp = (unsigned short)(1 << 6);

	BOOL bResPower = (power & status ? TRUE : FALSE);
	BOOL bResTemp = (temp & status ? TRUE : FALSE);

	WriteAlarmLog(TRACE_ERROR, 2, _T("Scanner power alarm = [%d]."), bResPower);
	WriteAlarmLog(TRACE_ERROR, 3, _T("Scanner temperature alarm = [%d]."), bResTemp);

	return (bResPower == TRUE) && (bResTemp == TRUE);
#else
	return TRUE;
#endif // (LASER_SOURCE==_SPI_LASER)
}

void CScanner::WriteAlarmLog(vTRACE_TYPE type, int idx, CString msg, BOOL result)
{
	if (result)
	{
		bAlarmArr[idx] = FALSE;
	}
	else
	{
		if (!bAlarmArr[idx])
		{
			_tracer(type, 1, msg);
			bAlarmArr[idx] = TRUE;
		}
	}

	
}

BOOL CScanner::LaserPowerON(BOOL bFlag) { return m_pLaserIO->LaserPowerOn(bFlag); }
void CScanner::LaserEnable(BOOL bFlag, double dblPower) { m_pLaserIO->LaserEnable(bFlag, dblPower); }

void CScanner::ChangePrgNo(int nPrgNo) { m_pLaserIO->ChangePrgNo(nPrgNo); }

void CScanner::SetPower(double per, BOOL list) 
{ 
	if (per > 99.9)
		per = 99.9;

	m_pLaserIO->SetPower(per, list); 
}
void CScanner::SetGuideOn(BOOL bFlag) { m_pLaserIO->SetGuideOn(bFlag); }

BOOL CScanner::GetDOGuideOn() { return m_pLaserIO->GetDOPilotLaser(); }

BOOL CScanner::GetDIReady() { return m_pLaserIO->GetDIReady(); }
BOOL CScanner::GetDIError() { return m_pLaserIO->GetDIError(); }
BOOL CScanner::GetDILaserOn() { return m_pLaserIO->GetDILaserOn(); }
BOOL CScanner::GetDIEmission() { return m_pLaserIO->GetDIEmission(); }
BOOL CScanner::GetDILaserAssigned() { return m_pLaserIO->GetDILaserAssigned(); }

void CScanner::DigitalIoRegisterData(BOOL bFlag)
{
	m_pLaserIO->DigitalIoRegisterData(bFlag, m_strConfigPath, m_nCard);
}

void CScanner::LaserIOInit() { m_pLaserIO->Init(); }
void CScanner::LaserIOTerminate() { m_pLaserIO->Terminate(); }
BOOL CScanner::LaserIOStatus() { return m_pLaserIO->GetStatus(); }
int CScanner::GetDOEmission() { return m_pLaserIO->GetDOEmission(); }

void CScanner::LightOn(BOOL bOn)
{
	m_pLaserIO->LightOn(bOn);
}

BOOL CScanner::IsLightOn()
{
	return m_pLaserIO->IsLightOn();
}

BOOL CScanner::PeekAndPump()
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (GetMessage(&msg, NULL, NULL, NULL)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {  // if WM_QUIT
			return FALSE;
		}
	}
	return TRUE;
}


#include "stdafx.h"
#include "CLaserIO.h"

CLaserIO::CLaserIO()
{
	m_StatusIO = 0;
	m_StatusDO = 0;

	AO_SetPower = 0;

	DO_Reset = 0;
	DO_ExtActivation = 0;
	DO_LaserOn = 0;
	DO_Remote = 0;
	DO_PilotLaser = 0;
	DO_EmissionEnable = 0;
	DO_FaultExt = 0;

	//DO_PROG_GUIDE = 0;
	//DO_PROG_LASER = 0;

	DI_Ready = 0;
	DI_ExtActive = 0;
	DI_LaserOn = 0;
	DI_LaserAssigned = 0;
	DI_EmissionOn = 0;
	DI_Monitoring = 0;
	DI_Error = 0;
}

void CLaserIO::SetScanner(CScanner * pScanner)
{
	m_pScanner = pScanner;
}

void CLaserIO::DigitalIoRegisterData(BOOL bFlag, CString strConfigPath, int nCard)
{
	CString pname;
	CIni ini;
	pname.Format(_T("%s%s"), strConfigPath, STR_CONFIG_FILE);
	ini.SetPathName(pname);
	CString section;
	if (bFlag) {
		// Save
		section.Format(_T("RTC_AO%d"), nCard);
		ini.WriteInt(section, _T("AO_SetPower"), AO_SetPower);

		section.Format(_T("PROG_NO%d"), nCard);
		//ini.WriteInt(section, _T("Guide"), DO_PROG_GUIDE);	// Guide Beam
		//ini.WriteInt(section, _T("Laser"), DO_PROG_LASER);	// Laser Beam

		section.Format(_T("RTC_DO%d"), nCard);
		ini.WriteInt(section, _T("DO_Reset"),		   DO_Reset);
		ini.WriteInt(section, _T("DO_ExtActivation"),  DO_ExtActivation);	// EXT ACTIVATION
		ini.WriteInt(section, _T("DO_LaserOn"),		   DO_LaserOn);	// LASER ON
		ini.WriteInt(section, _T("DO_Remote"),		   DO_Remote);	// Request
		ini.WriteInt(section, _T("DO_PilotLaser"),	   DO_PilotLaser);
		ini.WriteInt(section, _T("DO_EmissionEnable"), DO_EmissionEnable);
		ini.WriteInt(section, _T("DO_FalutExt"),	   DO_FaultExt);
		ini.WriteInt(section, _T("DO_Light0"),		   DO_Light0);

		ini.WriteInt(section, _T("DI_Ready"),		   DI_Ready);
		ini.WriteInt(section, _T("DI_ExtActive"),	   DI_ExtActive);
		ini.WriteInt(section, _T("DI_LaserOn"),		   DI_LaserOn);
		ini.WriteInt(section, _T("DI_LaserAssigned"),  DI_LaserAssigned);
		ini.WriteInt(section, _T("DI_EmissionOn"),	   DI_EmissionOn);
		ini.WriteInt(section, _T("DI_Monitoring"),     DI_Monitoring);
		ini.WriteInt(section, _T("DI_Error"),		   DI_Error);
	}
	else {
		// Load
		section.Format(_T("RTC_AO%d"), nCard);
		AO_SetPower = ini.GetInt(section, _T("AO_SetPower"), 1);

		section.Format(_T("PROG_NO%d"), nCard);
		//DO_PROG_GUIDE = ini.GetInt(section, _T("Guide"), 0);	
		//DO_PROG_LASER = ini.GetInt(section, _T("Laser"), 1);	

		section.Format(_T("RTC_DO%d"), nCard);
		DO_Reset		  = ini.GetInt(section, _T("DO_Reset"), 0);
		DO_ExtActivation  = ini.GetInt(section, _T("DO_ExtActivation"), 1);	
		DO_LaserOn		  = ini.GetInt(section, _T("DO_LaserOn"), 2);		
		DO_Remote		  = ini.GetInt(section, _T("DO_Remote"), 3);		
		DO_PilotLaser	  = ini.GetInt(section, _T("DO_PilotLaser"), 4);
		DO_EmissionEnable = ini.GetInt(section, _T("DO_EmissionEnable"), 5);
		DO_FaultExt		  = ini.GetInt(section, _T("DO_FaultExt"), 6);
		DO_Light0 =			ini.GetInt(section, _T("DO_Light0"), 13);
		section.Format(_T("RTC_DO%d"), nCard);

		DI_Ready	     = ini.GetInt(section, _T("DI_Ready"), 0);
		DI_ExtActive	 = ini.GetInt(section, _T("DI_ExtActive"), 1);
		DI_LaserOn		 = ini.GetInt(section, _T("DI_LaserOn"), 2);
		DI_LaserAssigned = ini.GetInt(section, _T("DI_LaserAssigned"), 3);
		DI_EmissionOn	 = ini.GetInt(section, _T("DI_EmissionOn"), 4);
		DI_Monitoring	 = ini.GetInt(section, _T("DI_Monitoring"), 5);
		DI_Error		 = ini.GetInt(section, _T("DI_Error"), 6);
	}
}

void CLaserIO::SetPower(double per, BOOL list)
{
	if (list)	m_pScanner->AO_List(AO_SetPower, per);
	else		m_pScanner->AO(AO_SetPower, per);
}

void CLaserIO::LightOn(BOOL bOn)
{
	m_pScanner->DO(DO_Light0, bOn);
}

BOOL CLaserIO::IsLightOn()
{
	return m_pScanner->GetDO(DO_Light0);
}
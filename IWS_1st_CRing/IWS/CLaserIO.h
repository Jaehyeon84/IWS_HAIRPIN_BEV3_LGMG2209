#pragma once
#include "Scanner.h"

class CLaserIO
{
public:
	CLaserIO();

protected:
	CScanner* m_pScanner;

public:
	void SetScanner(CScanner* pScanner);
	void DigitalIoRegisterData(BOOL bFlag, CString strConfigPath, int nCard);

	virtual void Init() = 0;
	virtual void Terminate() = 0;
	virtual BOOL GetStatus() = 0;
	virtual BOOL LaserPowerOn(BOOL bFlag) = 0;
	virtual BOOL LaserPowerOnCheck(BOOL bFlag) = 0;
	virtual void LaserEnable(BOOL bFlag, double dblPower = 0.0) = 0;
	virtual BOOL GetDIError() = 0;
	virtual void SetGuideOn(BOOL bFlag) = 0; // INT_DO_GuideOn
	virtual int GetDOPilotLaser() = 0; // INT_BE_GuideOn { return !m_pScanner->GetDO(DO_EmissionEnable); }		// 

	virtual void ChangePrgNo(int nPrgNo) = 0;

	void SetPower(double per, BOOL list = FALSE); // INT_AO_SetPower
	void SetReset(BOOL bFlag) { m_pScanner->DO(DO_Reset, bFlag); } 
	void SetRemote(BOOL bFlag) { m_pScanner->DO(DO_Remote, bFlag); } 
	void SetFaultExt(BOOL bFlag) { m_pScanner->DO(DO_FaultExt, bFlag); }
	
	virtual BOOL GetDIReady() { return m_pScanner->DI(DI_Ready); };
	virtual BOOL GetDILaserOn() { return m_pScanner->DI(DI_LaserOn); }; // INT_DI_PowerOn
	virtual BOOL GetDIEmission() { return m_pScanner->DI(DI_EmissionOn); }; // INT_DI_EmissionOn
	virtual BOOL GetDILaserAssigned() { return m_pScanner->DI(DI_LaserAssigned); }; // DI_LASER_ASSIGNED

	int GetDOEmission() { return m_pScanner->GetDO(DO_EmissionEnable); }	// INT_BE_EmissionEnable()

	int GetLaserErrorIndex() { return DI_Error; }

	virtual void LightOn(BOOL bOn);
	virtual BOOL IsLightOn();
public:
	// DO
	// Trump: Reset, ExtActivation, LaserOn,                Request(RemoteOn), PilotLaser,          PStartStatical(EmissionEnable)
	// IPG:   Reset, -,             PowerSupplyOn(LaserOn), RemoteOn,          GuideControl(Pilot), EmissionEnable
	// SPI:   EmissionEnable, PilotLaser

	// DI
	// Trump: Error(0: OK, 1: Error), ExtActive, LaserOn, LaserAssigned, ProgramActive, Monitoring
	// IPG:   Error(0: Error, 1: OK), -, -, PowerOn(LaserAssigned), EmissionOn, 
	// SPI:   LaserIsOn, Monitor, Alarm(0: OK, 1: Error)
	UINT m_StatusIO;
	UINT m_StatusDO;

	int AO_SetPower;

	CStringArray m_strDONameArr;
	CStringArray m_strDINameArr;

protected:
	int DO_Reset;			// o
	int DO_ExtActivation;	// o
	int DO_LaserOn;			// o
	int DO_Remote;			// Request On
	int DO_PilotLaser;		// o
	int DO_EmissionEnable;	// PGM.START
	int DO_FaultExt;		// add

	int DO_Light0;			// add

	//int DO_PROG_GUIDE;
	//int DO_PROG_LASER;

	int DI_Ready;			// add
	int DI_ExtActive;		// add
	int DI_LaserOn;
	int DI_LaserAssigned;
	int DI_EmissionOn;		// Program Active
	int DI_Monitoring;		// add
	int DI_Error;			// Falut Laser
};


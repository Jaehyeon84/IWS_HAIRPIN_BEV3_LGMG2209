#pragma once

#include "CLaserIO.h"
#include "Scanner.h"

class CLaserIOCoherent : public CLaserIO
{
public:
	CLaserIOCoherent();
	~CLaserIOCoherent();

	virtual void Init();
	virtual void Terminate();
	virtual BOOL GetStatus();

	virtual BOOL LaserPowerOn(BOOL bFlag);
	virtual BOOL LaserPowerOnCheck(BOOL bFlag);
	virtual void LaserEnable(BOOL bFlag, double dblPower = 0.0);
	virtual void LaserOn(BOOL bFlag, double dblPower = 0.0);

	virtual BOOL GetDIError();
	virtual int GetDOPilotLaser();

	virtual void ChangePrgNo(int nPrgNo);

private:
	void SetProgramNo(short ch);

public:
	// DO
	// Trump: Reset, ExtActivation, LaserOn, Request(RemoteOn), PilotLaser, PStartStatical(EmissionEnable), Prg.Bit0, Prg.Bit1
	virtual void SetGuideOn(BOOL bFlag);
public:
	// DI
	// Trump: Error(0: OK, 1: Error), -, LaserOn, -, ProgramActive
	BOOL LaserReady(BOOL bFlag);
	BOOL LaserEnableCheck(BOOL bFlag);
};


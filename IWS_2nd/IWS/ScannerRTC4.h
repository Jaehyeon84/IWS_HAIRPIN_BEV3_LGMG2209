#pragma once
#include "Scanner.h"
#ifdef _WIN64
#pragma comment(lib, "API/x64/RTC4DLLx64.lib")
#else
#pragma comment(lib, "API/RTC4DLL.lib")
#endif


class CScannerRTC4 : public CScanner
{
public:
	CScannerRTC4();
	CScannerRTC4(CString strScannerPath, CString strConfigPath);
	~CScannerRTC4();

public:
	virtual BOOL Open(unsigned short nCard);
	virtual void Close();

	// Ctrl command
	virtual void GetEncoder(long &nx, long &ny) {};
	virtual BOOL IsBusy();
	virtual void WaitMoving(BOOL bPeekAndPump = FALSE);
	virtual void Move(double x, double y);
	virtual void MoveRel(double x, double y);
	virtual void Execute(int n = 1);
	virtual void ExecuteStop();
	virtual void ExecuteAtPointer(int position);
	virtual void CircularQueueMode(BOOL mode);
	virtual void SetLaserPenParam();
	virtual BOOL ApplyCorrParam();
	virtual void SetOffset(double x, double y);
	virtual void SetMatrix(double m11, double m12, double m21, double m22);
	virtual UINT ReadInput();
	virtual unsigned short GetHeadStatus();
	virtual void SetWobbel(double dFreq, double dLongi, double dTrans) {};
	// List command
	virtual void SetSmoothContinue(BOOL bFlag);
	virtual void SetSpeed(double jump, double mark);
	virtual void SetScanParamList();
	virtual void SetAddMarkSpeed(double dAddSpeed);
	virtual void SetMatrixList(double m11, double m12, double m21, double m22);
	virtual void SetInputPointer(unsigned int position);
	virtual void StartList(int n = 1);
	virtual void JumpList(double x, double y);
	virtual void MarkList(double x, double y);
	virtual void ArcList(double x, double y, double th);
	virtual void LaserOnList(unsigned int us);
	virtual void LaserOn(BOOL bFlag);
	virtual void DelayList(unsigned short u10);
	virtual void EndList();
	virtual void WriteOutputList(unsigned short value);
	virtual void LaserOnHold(unsigned int ms);

	// Digital I/O
	virtual void AO(unsigned int x, double per);
	virtual double GetAO(unsigned int x){ return 0.0; };
	virtual void AO_List(unsigned int x, double per);
	virtual BOOL DI(int pinnum);
	virtual void DO(int pinnum, BOOL flag);
	virtual BOOL GetDO(int pinnum);

protected:
	virtual void ErrorMessage(short err);
};


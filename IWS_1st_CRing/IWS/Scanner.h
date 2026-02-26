#pragma once
#include "LaserPen.h"
class CLaserIO;

class CScanner
{
public:
	CScanner();
	~CScanner();
	BOOL m_bAbortExe;

	CLaserIO* m_pLaserIO;

protected:
	unsigned short m_nCard;
	/*UINT m_StatusIO;
	UINT m_StatusDO;*/
	UINT m_currPos;
	short m_nError;
	double m_lastPosX, m_lastPosY;
	double m_listPosX, m_listPosY;                
	CorrParam m_CorrParam;
	CLaserPen *m_pLaserPen;
	CString m_strScannerPath;
	CString m_strConfigPath;
	double m_dRatioPwr, m_dRatioSpd;

private:
	BOOL bAlarmArr[4];
	void WriteAlarmLog(vTRACE_TYPE type, int idx, CString msg, BOOL result);

public:

	void GetLastPos(double &xpos, double &ypos) {
		xpos = m_lastPosX / m_CorrParam.dblScaleFactor;
		ypos = m_lastPosY / m_CorrParam.dblScaleFactor;
	}
	virtual BOOL Open(unsigned short nCard) = 0;// { return true; }
	virtual void Close() = 0;//{};

	void SetRatio(double dPwr, double dSpd);
	double GetPowerRatio() { return m_dRatioPwr; };
	double GetSpeedRatio() { return m_dRatioSpd; };

	virtual void SetCorrParam(CorrParam param);
	CorrParam* GetCorrParam() { return &m_CorrParam; };
	void SetLaserPen(CLaserPen *laserPen) {		m_pLaserPen = laserPen;	};
	CLaserPen* GetLaserPen() { return m_pLaserPen; };

	
	void SetLaserPowerSupplyON(BOOL bOn);
	void SetLaser(int iLaser);
	int  GetLaser() {
		return m_iLaser;
	}

	BOOL HeadStatusCheck();
	BOOL LaserPowerON(BOOL bFlag);
	void LaserEnable(BOOL bFlag, double dblPower = 0.0);	
	void ChangePrgNo(int nPrgNo);
	void DigitalIoRegisterData(BOOL bFlag = TRUE);

	// Ctrl command
	//virtual void SetControlMode(UINT n) {};
	virtual void GetEncoder(long &nx, long &ny) =0;
	//virtual void ExecuteListPos(UINT n, UINT pos) {};
	virtual BOOL IsBusy() = 0;// { return FALSE; };
	virtual void WaitMoving(BOOL bPeekAndPump = FALSE) = 0;// {};
	virtual void Move(double x, double y) = 0;
	virtual void MoveRel(double x, double y) = 0;
	virtual void Execute(int n = 1) = 0;
	virtual void ExecuteStop() = 0;
	virtual void ExecuteAtPointer(int position) = 0;
	virtual void SetLaserPenParam() = 0;
	virtual BOOL ApplyCorrParam() = 0; //{ return FALSE; };
	virtual void CircularQueueMode(BOOL mode) = 0; //{};
	virtual void SetOffset(double x, double y) = 0; //{};
	virtual void SetMatrix(double m11, double m12, double m21, double m22) = 0; //{};
	virtual UINT ReadInput() = 0; // { return 0; };
	virtual unsigned short GetHeadStatus() = 0; // { return 0; };

	virtual void SetWobbel(double dFreq, double dLongi, double dTrans) = 0;
	// List command
	//virtual void SetControlModeList(UINT n) {};
	//virtual void SetStartListPos(int n, unsigned int pos) {};
	//virtual void SetExtStartPosList(unsigned int pos) {};
	//virtual void SetExtStartDelayListX(long delay) {};
	//virtual void SetExtStartDelayListY(long delay) {};
	virtual void SetSmoothContinue(BOOL bFlag) = 0;
	virtual void SetSpeed(double jump, double mark) =0;
	virtual void SetScanParamList() = 0; // {};
	virtual void SetAddMarkSpeed(double dAddSpeed) = 0;
	virtual void SetMatrixList(double m11, double m12, double m21, double m22) = 0; // {};
	virtual void SetInputPointer(unsigned int position) = 0; // {};
	virtual void StartList(int n = 1) = 0; // {};
	virtual void JumpList(double x, double y) = 0; //{};
	virtual void MarkList(double x, double y) = 0; //{};
	virtual void ArcList(double x, double y, double th) = 0; //{};
	virtual void LaserOnList(unsigned int us) = 0; // {};
	virtual void LaserOn(BOOL bFlag) = 0; //{};
	virtual void DelayList(unsigned short u10) = 0; // {};
	virtual void EndList() = 0; // {};
	virtual void WriteOutputList(unsigned short value) = 0; // {};
	virtual void LaserOnHold(unsigned int ms)=0;

//	// Digital I/O
	virtual void AO(unsigned int x, double per) = 0; //{};
	virtual double GetAO(unsigned int x) = 0; //{ return 0.0; };
	virtual void AO_List(unsigned int x, double per) = 0; //{};
	virtual BOOL DI(int pinnum) = 0; //{ return FALSE; };
	virtual void DO(int pinnum, BOOL flag) = 0; //{};
	virtual BOOL GetDO(int pinnum) = 0; //{ return FALSE; };
	virtual int GetDO_Value() = 0; //{ return 0; };
	virtual void DO_Value_List(int value) = 0; // {};

	virtual void LightOn(BOOL bOn);
	virtual BOOL IsLightOn();

	void LaserIOInit();
	void LaserIOTerminate();
	BOOL LaserIOStatus();

	int GetDOEmission();	// INT_BE_EmissionEnable
	BOOL GetDOGuideOn();

	void SetGuideOn(BOOL bFlag);	
	void SetPower(double per, BOOL list = FALSE);

	BOOL GetDIReady();
	BOOL GetDIError();
	BOOL GetDILaserOn(); //INT_DI_PowerOn
	BOOL GetDIEmission(); // INT_DI_EmissionOn
	BOOL GetDILaserAssigned(); // DI_LASER_ASSIGNED

	void SetLaserFault(BOOL bFalut);

	int m_iLaser;
protected:
	virtual void ErrorMessage(short err) = 0; // {};
	virtual BOOL PeekAndPump();

	short GetStatus() { return m_nError; };

	virtual void CorrParamRegisterData(BOOL bFlag = TRUE) {};

};


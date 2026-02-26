#pragma once
#include "MelsecSocket.h"
#include <vector>

//원본
#define PLC_READ_ADDRESS	30100
#define PLC_WRITE_ADDRESS	30000

//사내 테스트
//#define PLC_READ_ADDRESS	31200
//#define PLC_WRITE_ADDRESS	31000

//#define PLC_READ_PT_ADRESS  0x5000 // Vision 좌표 시작주소
//#define PLC_POINT_COUNT		566

// W7100: PLC_WORD_STATUS
#define PLC_BIT_LINK		0x0001	// Link 
#define PLC_BIT_INIT		0x0002	// 초기화 요청
#define PLC_BIT_TIMESYNC	0x0004	// 시간 동기화 요청
#define PLC_BIT_WELDREADY	0x0008	// 용접 준비 요청
#define PLC_BIT_ALARM		0x0010	// 알람 발생 
#define PLC_BIT_AUTO		0x0020	// 홈뷰 메뉴가 다 죽음 PLC에서 신호 변경 시 바로 반응
#define PLC_BIT_MANUAL		0x0040	// PLC 제어가 되지 않음
//#define PLC_BIT_COMPLETE	0x0080	// COMPLETE

//W7101: PLC_WORD_COMMAND
#define PLC_BIT_WELD		0x0001  // 0
#define PLC_BIT_WELDDONE	0x0002	// 1
#define PLC_BIT_WELDRUN		0x0004
#define PLC_BIT_INSPECT		0x0008  // 3

//C-Ring Division blower 개별 제어를 하기 위해 용접패턴 추가 - 21.10.25 PJM
#define PLC_BIT_INNER_AIR_KNIFE		0x0020
#define PLC_BIT_OUTTER_AIR_KNIFE	0x0040
//#define PLC_BIT_INSPECTDONE 0x0010  // 4
//#define PLC_BIT_WELDDONE	0x0002  // 1
//#define PLC_BIT_INSPECTDONE 0x0008  // 3
//#define PLC_BIT_INSPECT_OK  0x0010  // 4

// PLC_WORD_ALARM
#define PLC_ALARM_LASER		0x0001
#define PLC_ALARM_SCANNER	0x0002
#define PLC_ALARM_VISION	0x0004
#define PLC_ALARM_RECIPE	0x0008

// ==============================

#define PLC_WORD_STATUS		0
#define PLC_WORD_COMMAND	1
//#define PLC_WORD_LASER		0x02
//#define PLC_WORD_ALARM		0x03
#define PLC_WORD_PMODEL     5  // PLC Model
#define PLC_WORD_RECIPE		6  // 0=None, 1=77 JigA, 2= 77 JigB
#define PLC_WORD_WORKJIG    7  // 1=JigA, 2=JigB
#define PLC_WORD_WROKWELD	8  // 1=Welding, 2=Rework
#define PLC_WORD_WORKMODE   9  // 1=Welding, 2=Simulation

#define PLC_WORD_TIME		11 // 6 WORD
#define PLC_WORD_LOTID		18 // @ WORD LotID
#define PLC_WORD_LOTID_JIGA		18 // @ WORD LotID
#define PLC_WORD_LOTID_JIGB		30 // @ WORD LotID

#define mBits(x,m) ((x)&(m))
#define mBitsOn(lvx,by) ((lvx)|=(by)) 
#define mBitsOff(lvx,bn) ((lvx)&= ~(bn))
#define mBitsToggle(lvx,b) ((lvx)^= (b)) 

enum PLC_IO{
	PLC_ACT = 0,
	PLC_INIT,
	PLC_TIMESYNC,
	PLC_RDY_WELD,
	PLC_ALARM,
	PLC_AUTO,
	PLC_MANUAL
};
class CMelsecPLC
{
public:
	CMelsecPLC();
	~CMelsecPLC();

	BOOL Close();
	
	BOOL Open(CString strIP, int iPort, int nJigNo = 1, int nRead = PLC_READ_ADDRESS, int nWrite = PLC_WRITE_ADDRESS);
	BOOL UpdateOutput();
	BOOL UpdateInput();
	//BOOL UpdatePosition();

	void SetToggle(WORD wPage, WORD bit, BOOL bUpdate = FALSE);

	void SetRecipe(int i);
	void SetToggleLink(BOOL bUpdate = FALSE);
	void SetLink(BOOL bFlag, BOOL bUpdate = FALSE);
	void SetInit(BOOL bFlag, BOOL bUpdate = FALSE);
	void SetTimeSync(BOOL bFlag, BOOL bUpdate = FALSE);
	void SetWeldReady(BOOL bFlag, BOOL bUpdate = FALSE);
	void SetAlarm(BOOL bFlag, BOOL bUpdate = FALSE);
	void SetAuto(BOOL bFlag, BOOL bUpdate = FALSE);
	void SetManual(BOOL bFlag, BOOL bUpdate = FALSE);
	//void SetComplete(BOOL bFlag, BOOL bUpdate = FALSE);
	void SetWeldReq(BOOL bFlag, BOOL bUpdate = FALSE);
	void SetInspectReq(BOOL bFlag, BOOL bUpdate = FALSE);
	void SetInnerAirknife(BOOL bFlag, BOOL bUpdate = FALSE);
	void SetOutterAirknife(BOOL bFlag, BOOL bUpdate = FALSE);

	void SetWeldComplete(BOOL bFlag, BOOL bUpdate = FALSE);
	void SetWeldRun(BOOL bFlag, BOOL bUpdate = FALSE);
	//void SetInspectComplete(BOOL bFlag, BOOL bUpdate = FALSE);
	//void SetInspectOK(BOOL bOK, BOOL bUpdate = FALSE);
	int  GetRecipe();
	//void SetLaserState(BOOL bReady, BOOL bPowerOn, BOOL bActive, BOOL bEmission);
	void SetLaserPower(double dPower);
	void SetAlarmCode(int iCode);

	BOOL GetLink() { return mBits(m_rData[PLC_WORD_STATUS], PLC_BIT_LINK); };
	BOOL GetReqInit() { return mBits(m_rData[PLC_WORD_STATUS], PLC_BIT_INIT); };
	BOOL GetReqTimeSync() { return mBits(m_rData[PLC_WORD_STATUS], PLC_BIT_TIMESYNC); };
	BOOL GetReadyWelding() { return mBits(m_rData[PLC_WORD_STATUS], PLC_BIT_WELDREADY); };
	BOOL GetAlarm() { return mBits(m_rData[PLC_WORD_STATUS], PLC_BIT_ALARM); };
	BOOL GetAuto() { return mBits(m_rData[PLC_WORD_STATUS], PLC_BIT_AUTO); };
	BOOL GetManual() { return mBits(m_rData[PLC_WORD_STATUS], PLC_BIT_MANUAL); };
	//BOOL GetComplete() { return mBits(m_rData[PLC_WORD_STATUS], PLC_BIT_COMPLETE); };
	int GetWorkJig()     {														// 1=JigA, 2=JigB
		if(m_nJigNo == 1) 
			return 1;
		else
			return int(m_rData[PLC_WORD_WORKJIG]);
	};
	BOOL GetReWeld()     { return (int(m_rData[PLC_WORD_WROKWELD]) == 2); };	// 1=Welding, 2=Rework
	BOOL GetSimulation() { return (int(m_rData[PLC_WORD_WORKMODE]) == 2); };	// 1=Welding, 2=Simulation

	BOOL GetReqWelding() { return mBits(m_rData[PLC_WORD_COMMAND], PLC_BIT_WELD); };
	BOOL GetReqWeldingComplete() { return mBits(m_rData[PLC_WORD_COMMAND], PLC_BIT_WELDDONE); };
	BOOL GetReqInspect() { return mBits(m_rData[PLC_WORD_COMMAND], PLC_BIT_INSPECT); };

	BOOL GetResWelding() { return mBits(m_wData[PLC_WORD_COMMAND], PLC_BIT_WELD); };
	BOOL GetResInspect() { return mBits(m_wData[PLC_WORD_COMMAND], PLC_BIT_INSPECT); };
	
	BOOL GetInnerAirknife() { return mBits(m_wData[PLC_WORD_COMMAND], PLC_BIT_INNER_AIR_KNIFE); };
	BOOL GetOutterAirknife() { return mBits(m_wData[PLC_WORD_COMMAND], PLC_BIT_OUTTER_AIR_KNIFE); };

	void UpdateDateTime();

	//BOOL GetWriteStatusData(int bit)
	//{
	//	return mBits(m_wData[PLC_WORD_STATUS], bit);
	//}

	//void SetWriteStatusData(BOOL bFlag, int bit)
	//{
	//	if (bFlag)
	//		mBitsOn(m_wData[PLC_WORD_STATUS], bit);
	//	else
	//		mBitsOff(m_wData[PLC_WORD_STATUS], bit);
	//}
	
	WORD GetReadData(int iWd) {
		return m_rData[iWd];
	}
	WORD GetWriteData(int iWd) {
		return m_wData[iWd];
	}
	void SetReadData(int iWd, WORD wValue)
	{
		m_rData[iWd] = wValue;
	}

	CString GetLOTID();
public:
	std::vector<WORD> m_PositionList;

protected:
	BOOL m_bConnected;
	CString m_strIP;
	int m_iPort;
	int m_nJigNo;
	int m_nRead, m_nWrite;

	WORD m_wData[MAX_DATA_BUFF];	// WRITE DATA
	WORD m_rData[MAX_DATA_BUFF];	// READ DATA

	CMelsecSocket m_EthPLC;

	CString m_strLOTID;
};


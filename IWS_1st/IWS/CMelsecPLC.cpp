#include "stdafx.h"
#include "CMelsecPLC.h"
#include <stdio.h>

CMelsecPLC::CMelsecPLC()
{
	memset(m_rData, 0, sizeof(WORD)*MAX_DATA_BUFF);
	memset(m_wData, 0, sizeof(WORD)*MAX_DATA_BUFF);

	m_bConnected = FALSE;
	m_bReqInspectOld = FALSE; // sjyi 2023.09.12 이전 값 저장 변수 추가 
	m_nJigNo = 1;
}

CMelsecPLC::~CMelsecPLC()
{
	Close();
}

BOOL CMelsecPLC::Close()
{
	return m_EthPLC.ClosePLCSocket();
}

BOOL CMelsecPLC::Open(CString strIP, int iPort, int nJigNo /*= 1*/, 
						int nRead /*= PLC_READ_ADDRESS*/, int nWrite /*= PLC_WRITE_ADDRESS*/)
{
	m_bConnected = m_EthPLC.OpenPLCSocket(1, iPort,CStringA(strIP));

	m_nJigNo = nJigNo;
	//
	m_nRead = nRead;
	m_nWrite = nWrite;

	return m_bConnected;
}

BOOL CMelsecPLC::UpdateOutput()
{
	if (m_bConnected)
		return m_EthPLC.WriteWord(m_nWrite, m_wData, MAX_DATA_BUFF); //return m_EthPLC.WriteWord(PLC_WRITE_ADDRESS, m_wData, MAX_DATA_BUFF);
	else
		return TRUE;
}

BOOL CMelsecPLC::UpdateInput()
{
	if (m_bConnected)
		return m_EthPLC.ReadWord(m_nRead, m_rData, MAX_DATA_BUFF);//return m_EthPLC.ReadWord(PLC_READ_ADDRESS, m_rData, MAX_DATA_BUFF);
	else
		return TRUE;
}

//BOOL CMelsecPLC::UpdatePosition()
//{
//	if (m_bConnected)
//	{
//		m_PositionList.clear();
//
//		int nStartAddr = PLC_READ_PT_ADRESS;
//		int nPartWord = 100;		
//		int nTotalWord = PLC_POINT_COUNT;
//		int nLoopCount = (int)ceil(nTotalWord / (double)nPartWord);
//		int nTmpWord = nTotalWord;
//		
//		
//		for (int i = 0; i < nLoopCount; i++)
//		{
//			nTmpWord = min(nTotalWord, nPartWord);
//			WORD* wordData = (WORD*)malloc(nTmpWord * sizeof(int));
//			
//
//			if (!m_EthPLC.ReadWord(nStartAddr, wordData, nTmpWord))
//			{
//				free(wordData);
//				return FALSE;
//			}
//
//			for (int j = 0; j < nTmpWord; j++)
//				m_PositionList.push_back(wordData[j]);
//
//			nTotalWord -= nTmpWord;
//			nStartAddr += (nTmpWord * 2);
//
//			free(wordData);
//		}
//
//
//		return m_EthPLC.ReadWord(PLC_READ_ADDRESS, m_rData, MAX_DATA_BUFF);
//	}
//	else
//	{
//		return FALSE;
//	}
//}

void CMelsecPLC::SetToggle(WORD wPage, WORD bit, BOOL bUpdate /*= FALSE*/)
{
	mBitsToggle(m_wData[wPage],0x0002);
	if (bUpdate) UpdateOutput();
}


void CMelsecPLC::SetRecipe(int i)
{
	m_wData[PLC_WORD_RECIPE] = (WORD)i;
}

void CMelsecPLC::SetToggleLink(BOOL bUpdate /*= FALSE*/)
{
	mBitsToggle(m_wData[PLC_WORD_STATUS], PLC_BIT_LINK);
	if (bUpdate) UpdateOutput();
}

void CMelsecPLC::SetLink(BOOL bFlag, BOOL bUpdate /*= FALSE*/)
{
	if (bFlag)
		mBitsOn(m_wData[PLC_WORD_STATUS], PLC_BIT_LINK);
	else
		mBitsOff(m_wData[PLC_WORD_STATUS], PLC_BIT_LINK);
}

void CMelsecPLC::SetInit(BOOL bFlag, BOOL bUpdate /*= FALSE*/)
{
	if (bFlag)
		mBitsOn(m_wData[PLC_WORD_STATUS], PLC_BIT_INIT);
	else
		mBitsOff(m_wData[PLC_WORD_STATUS], PLC_BIT_INIT);

}

void CMelsecPLC::SetTimeSync(BOOL bFlag, BOOL bUpdate /*= FALSE*/)
{
	if (bFlag)
		mBitsOn(m_wData[PLC_WORD_STATUS], PLC_BIT_TIMESYNC);
	else
		mBitsOff(m_wData[PLC_WORD_STATUS], PLC_BIT_TIMESYNC);
}

void CMelsecPLC::SetWeldReady(BOOL bFlag, BOOL bUpdate /*= FALSE*/)
{
	// Logging
	BOOL bOld = mBits(m_wData[PLC_WORD_STATUS], PLC_BIT_WELDREADY);

	if (bOld && bFlag == FALSE)
	{
		_tracer(TRACE_NORMAL, 9999, _T("PC->PLC : SetWeldReady OFF"));
	}
	else if (bOld == FALSE && bFlag == TRUE)
	{
		_tracer(TRACE_NORMAL, 9999, _T("PC->PLC : SetWeldReady ON"));
	}

	if (bFlag)
	{
		mBitsOn(m_wData[PLC_WORD_STATUS], PLC_BIT_WELDREADY);
	}
	else
	{
		mBitsOff(m_wData[PLC_WORD_STATUS], PLC_BIT_WELDREADY);
	}
}

void CMelsecPLC::SetAlarm(BOOL bFlag, BOOL bUpdate /*= FALSE*/)
{
	if(bFlag)
		mBitsOn(m_wData[PLC_WORD_STATUS], PLC_BIT_ALARM);
	else
		mBitsOff(m_wData[PLC_WORD_STATUS], PLC_BIT_ALARM);
}

void CMelsecPLC::SetAlarmCode(int iCode)
{
	switch (iCode)
	{
	case 0:
		//m_wData[PLC_WORD_ALARM] = 0;
		SetAlarm(FALSE);
		break;
	case 1:
		//m_wData[PLC_WORD_ALARM] |= PLC_ALARM_LASER;
		SetAlarm(TRUE);
		break;
	case 2:
		//m_wData[PLC_WORD_ALARM] |= PLC_ALARM_SCANNER;
		SetAlarm(TRUE);
		break;
	case 3:
		//m_wData[PLC_WORD_ALARM] |= PLC_ALARM_VISION;
		SetAlarm(TRUE);
		break;
	case 4:
		//m_wData[PLC_WORD_ALARM] |= PLC_ALARM_RECIPE;
		SetAlarm(TRUE);
		break;
	}
}

void CMelsecPLC::UpdateDateTime(BOOL bUpdate)
{
	int year = int(m_rData[PLC_WORD_TIME]);
	int month = int(m_rData[PLC_WORD_TIME + 1]);
	int day = int(m_rData[PLC_WORD_TIME + 2]);
	int hour = int(m_rData[PLC_WORD_TIME + 3]);
	int minute = int(m_rData[PLC_WORD_TIME + 4]);
	int second = int(m_rData[PLC_WORD_TIME + 5]);

	CTime workTime = CTime(year, month, day, hour, minute, second);
	SYSTEMTIME time;
	::GetLocalTime(&time);
	time.wYear = year;
	time.wMonth = month;
	time.wDay = day;
	time.wHour = hour;
	time.wMinute = minute;
	time.wSecond = second;

	if (bUpdate)
	{
		::SetLocalTime(&time);
	}
	_tracer(TRACE_NORMAL, 1, _T("Set Local Time by PLC : %04d-%02d-%02d %02d:%02d:%02d"), year, month, day, hour, minute, second);
}

void CMelsecPLC::SetAuto(BOOL bFlag, BOOL bUpdate /*= FALSE*/)
{
	if (bFlag)
		mBitsOn(m_wData[PLC_WORD_STATUS], PLC_BIT_AUTO);
	else
		mBitsOff(m_wData[PLC_WORD_STATUS], PLC_BIT_AUTO);

}

void CMelsecPLC::SetManual(BOOL bFlag, BOOL bUpdate /*= FALSE*/)
{
	if (bFlag)
		mBitsOn(m_wData[PLC_WORD_STATUS], PLC_BIT_MANUAL);
	else
		mBitsOff(m_wData[PLC_WORD_STATUS], PLC_BIT_MANUAL);

}

//void CMelsecPLC::SetComplete(BOOL bFlag, BOOL bUpdate)
//{
//	if (bFlag)
//		mBitsOn(m_wData[PLC_WORD_STATUS], PLC_BIT_COMPLETE);
//	else
//		mBitsOff(m_wData[PLC_WORD_STATUS], PLC_BIT_COMPLETE);
//}

void CMelsecPLC::SetWeldReq(BOOL bFlag, BOOL bUpdate /*= FALSE*/)
{
	if (bFlag) {
		_tracer(TRACE_NORMAL, 9999, _T("PC->PLC : WeldingResponse ON"));
		mBitsOn(m_wData[PLC_WORD_COMMAND], PLC_BIT_WELD);
	}
	else {
		_tracer(TRACE_NORMAL, 9999, _T("PC->PLC : WeldingResponse OFF"));
		mBitsOff(m_wData[PLC_WORD_COMMAND], PLC_BIT_WELD);
	}
}

void CMelsecPLC::SetWeldComplete(BOOL bFlag, BOOL bUpdate /*= FALSE*/)
{
	if (bFlag) {
		_tracer(TRACE_NORMAL, 9999, _T("PC->PLC : WeldingComplete ON"));
		mBitsOn(m_wData[PLC_WORD_COMMAND], PLC_BIT_WELDDONE);
	}
	else {
		_tracer(TRACE_NORMAL, 9999, _T("PC->PLC : WeldingComplete OFF"));
		mBitsOff(m_wData[PLC_WORD_COMMAND], PLC_BIT_WELDDONE);
	}
}

void CMelsecPLC::SetWeldRun(BOOL bFlag, BOOL bUpdate /*= FALSE*/)
{
	if (bFlag)
		mBitsOn(m_wData[PLC_WORD_COMMAND], PLC_BIT_WELDRUN);
	else
		mBitsOff(m_wData[PLC_WORD_COMMAND], PLC_BIT_WELDRUN);
}

void CMelsecPLC::SetInspectReq(BOOL bFlag, BOOL bUpdate /*= FALSE*/)
{
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	if (bFlag)
	{
		TRACE(_T("[sjyi TEST] PC->PLC : InepectRes ON - %02d:%02d:%02d.%03d\n"), stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);
		_tracer(TRACE_NORMAL, 9999, _T("PC->PLC : InspectRes ON"));
		mBitsOn(m_wData[PLC_WORD_COMMAND], PLC_BIT_INSPECT);
	}
	else
	{
		TRACE(_T("[sjyi TEST] PC->PLC : InepectRes OFF - %02d:%02d:%02d.%03d\n"), stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);
		_tracer(TRACE_NORMAL, 9999, _T("PC->PLC : InspectRes OFF"));
		mBitsOff(m_wData[PLC_WORD_COMMAND], PLC_BIT_INSPECT);
	}
}

void CMelsecPLC::SetInnerAirknife(BOOL bFlag, BOOL bUpdate)
{
	if (bFlag)
		mBitsOn(m_wData[PLC_WORD_COMMAND], PLC_BIT_INNER_AIR_KNIFE);
	else
		mBitsOff(m_wData[PLC_WORD_COMMAND], PLC_BIT_INNER_AIR_KNIFE);
}

void CMelsecPLC::SetOutterAirknife(BOOL bFlag, BOOL bUpdate)
{
	if (bFlag)
		mBitsOn(m_wData[PLC_WORD_COMMAND], PLC_BIT_OUTTER_AIR_KNIFE);
	else
		mBitsOff(m_wData[PLC_WORD_COMMAND], PLC_BIT_OUTTER_AIR_KNIFE);
}

//void CMelsecPLC::SetInspectComplete(BOOL bFlag, BOOL bUpdate /*= FALSE*/)
//{
//	if (bFlag)
//		mBitsOn(m_wData[PLC_WORD_COMMAND], PLC_BIT_INSPECTDONE);
//	else
//		mBitsOff(m_wData[PLC_WORD_COMMAND], PLC_BIT_INSPECTDONE);
//}

//void CMelsecPLC::SetInspectOK(BOOL bOK, BOOL bUpdate)
//{
//	if (bOK)
//		mBitsOn(m_wData[PLC_WORD_COMMAND], PLC_BIT_INSPECT_OK);
//	else
//		mBitsOff(m_wData[PLC_WORD_COMMAND], PLC_BIT_INSPECT_OK);
//}

int CMelsecPLC::GetRecipe()
{
	return int(m_rData[PLC_WORD_RECIPE]);
}

int CMelsecPLC::GetModelNo()
{
	return int(m_rData[PLC_WORD_PMODEL]);
}

//void CMelsecPLC::SetLaserState(BOOL bReady, BOOL bPowerOn, BOOL bActive, BOOL bEmission)
//{
//	if (bReady)
//		mBitsOn(m_wData[PLC_WORD_LASER], 0x01);
//	else
//		mBitsOff(m_wData[PLC_WORD_LASER], 0x01);
//
//	if (bPowerOn)
//		mBitsOn(m_wData[PLC_WORD_LASER], 0x02);
//	else
//		mBitsOff(m_wData[PLC_WORD_LASER], 0x02);
//
//	if (bActive)
//		mBitsOn(m_wData[PLC_WORD_LASER], 0x04);
//	else
//		mBitsOff(m_wData[PLC_WORD_LASER], 0x04);
//
//	if (bEmission)
//		mBitsOn(m_wData[PLC_WORD_LASER], 0x08);
//	else
//		mBitsOff(m_wData[PLC_WORD_LASER], 0x08);
//
//}

void CMelsecPLC::SetLaserPower(double dPower)
{
	// hslee
	/*int iPower = int(dPower * 10.0);
	m_wData[PLC_WORD_LASER_PWR] = (WORD)iPower;*/
}

// 2022.09.26 jh.kim 추가 - Ford3차 : MES 전송 데이터
void CMelsecPLC::SetPattern(const char *sPtrn1, const char *sPtrn2)
{
	WORD wPtrn1[4], wPtrn2[4];

	memcpy((void *)wPtrn1, sPtrn1, sizeof(sPtrn1)-1);
	memcpy((void *)wPtrn2, sPtrn2, sizeof(sPtrn2)-1);

	for (int i = 0; i < 4; i++) {
		m_wData[PLC_WORD_PATTERN_MAIN    + i] = wPtrn1[i];
		m_wData[PLC_WORD_PATTERN_SUPPORT + i] = wPtrn2[i];
	}
	UpdateOutput();
	
}

// sjyi 2023.09.12 이전 값 저장 변수 및 2D 검사 관련 PLC 추가 
BOOL CMelsecPLC::GetReqInspectOld()
{
	return m_bReqInspectOld;
}

void CMelsecPLC::SetReqInspectOld(BOOL bValue)
{
	m_bReqInspectOld = bValue;
}

void CMelsecPLC::SetDo2DInspect(int nDoInspection)
{
	m_wData[PLC_WORD_WRITE_DO_2DINSPECT] = nDoInspection;
}

void CMelsecPLC::Set2DInspectResultJigA(int nResult)
{
	m_wData[PLC_WORD_WRITE_2D_RESULT] = nResult;
}

void CMelsecPLC::Set2DInspectResultJigB(int nResult)
{
	m_wData[PLC_WORD_WRITE_2D_RESULT + 1] = nResult;
}

int	 CMelsecPLC::GetDo2DInspectJigA()
{
	return int(m_rData[PLC_WORD_2DINSP_JIGA]);
}

int	 CMelsecPLC::GetDo2DInspectJigB()
{
	return int(m_rData[PLC_WORD_2DINSP_JIGB]);
}

// sjyi 2023.07.17 LOT ID 가져오는 함수 추가
CString CMelsecPLC::GetLOTID()
{
	char szLotID[50] = { 0 };

	int nJigNo = GetWorkJig();
	
	// sjyi 2023.08.01 JIG 구분하여 Lot ID 가져오기
	if (nJigNo == 1) // JIG A
	{
		memcpy(szLotID, &m_rData[PLC_WORD_LOTID_JIGA], 10 * sizeof(WORD));
	}
	else if (nJigNo == 2) // JIG B
	{
		memcpy(szLotID, &m_rData[PLC_WORD_LOTID_JIGB], 10 * sizeof(WORD));
	}
	else // Default Jig A
	{
		memcpy(szLotID, &m_rData[PLC_WORD_LOTID_JIGA], 10 * sizeof(WORD));
	}

	USES_CONVERSION;
	m_strLOTID.Format(_T("%s"), A2W(szLotID));

	return m_strLOTID;
}

BOOL CMelsecPLC::GetWeldRun()
{
	return mBits(m_wData[PLC_WORD_COMMAND], PLC_BIT_WELDRUN);
}
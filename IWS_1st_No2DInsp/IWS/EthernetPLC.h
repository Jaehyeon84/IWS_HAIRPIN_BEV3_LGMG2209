#pragma once

#include <afxmt.h>

#define UM_PLC_MSG						WM_USER + 3456

#if !defined(PLC_CMD_TYPE_DEFINED)
#define PLC_CMD_TYPE_DEFINED

enum plc_cmd_type_t
{
	PLC_CMD_TYPE_ERROR_RESET = 0,
	PLC_CMD_TYPE_READ			,
	PLC_CMD_TYPE_WRITE			,
	PLC_CMD_TYPE_END
};
#endif

// CEthernetPLC
class CEthernetPLC : public CWnd
{
	DECLARE_DYNAMIC(CEthernetPLC)

//Attributes
private:

	CWnd*			m_pMainWnd;
	BOOL			m_bSendLog;

	CString			m_strPLCIP;
	int				m_nPLCPort;

	int				m_fdPlcSock;
	CString			m_strErr;

	CCriticalSection	m_cLock;

	CString			m_strReceiveData;
public:
	short			m_rDataInsp[6144];
	short			m_rDataRepair[384];
	BOOL			m_bConnected;
//Operations
private:
	void SendLog(CString strLog, BOOL bRecieve = FALSE);

	BOOL CheckBit(CString strReply, int* iBitData);
	BOOL CheckBitArray(CString strReply, int nCount, int* iBitArray);
	BOOL CheckWord(CString strReply, int* iVal);
	BOOL CheckWordArray(CString strReply, int nCount, int* iValArray);
	BOOL CheckDWord(CString strReply, int* iVal);
	BOOL CheckDWordArray(CString strReply, int nCount, int* iValArray);

	//��켷 Binary üũ�Լ�
	BOOL CheckBinaryWord(CString strReply, short* iVal);
	BOOL CheckBinaryWordArray(CString strReply, int nCount, short* iValArray);
	BOOL CheckBinaryDWord(CString strReply, int* iVal);
	BOOL CheckBinaryDWordArray(CString strReply, int nCount, int* iValArray);



public:

	BOOL PLC_Connect();
	BOOL PLC_SendMsg(CString Message, BOOL isRead);		//isRead - True : PLC Read ���, False : PLC��  Write -- Write�� ���� Ack�� �����Ѵ�.
	BOOL PLC_DisConnect();
	BOOL IsConnectPLC() { return m_bConnected; }
	BOOL PLCErrorReset();
	void SetSendLog(BOOL bSend) { m_bSendLog = bSend;  }

	//Bit
	int   GetBitM(int address); // Return Value  -1: ERROR, 0: BIT OFF, 1: BIT ON,  -1: ERROR
	BOOL  SetBitM(int address, BOOL bON);
	
	//Bit Block Data ó��
	BOOL  GetBitBlockM(int iFirstAddress, int nDataCount, int* iReadArray); // iArray Value  -1: ERROR, 0: BIT OFF, 1: BIT ON,  -1: ERROR
	BOOL  SetBitBlockM(int iFirstAddress, int nDataCount, BOOL* bValArray);

	//Word
	BOOL  GetWord(int iAddress, int* iReadVal); // iReadVal -1: ERROR
	BOOL  SetWord(int iAddress, int ival);
	
	//DWord
	BOOL  GetDoubleWord(int iAddress, int* iReadVal); // iReadVal -1: ERROR
	BOOL  SetDoubleWord(int iAddress, int iVal);

	//Word Block Data ó��
	BOOL  GetWordBlock(int iFirstAddress, int nDataCount, int* iReadArray);
	BOOL  SetWordBlock(int iFirstAddress, int nDataCount, int* iValArray, int iStartOffset = 0);

	//DWord Block Data ó��
	BOOL  SetDoubleWordBlock(int iFirstAddress, int nDataCount, int* iValArray);
	BOOL  GetDoubleWordBlock(int iFirstAddress, int nDataCount, int* iReadArray);

	
public:
	//��켷 2020 05 12

	BOOL PLC_SendBinaryMsg(char * Message, int nLen, BOOL isRead);		//isRead - True : PLC Read ���, False : PLC��  Write -- Write�� ���� Ack�� �����Ѵ�.

	//Word
	BOOL  GetBinaryWord(int iAddress, short* iReadVal); // iReadVal -1: ERROR
	BOOL  SetBinaryWord(int iAddress, short ival);

	//DWord
	BOOL  GetBinaryDoubleWord(int iAddress, int* iReadVal); // iReadVal -1: ERROR
	BOOL  SetBinaryDoubleWord(int iAddress, int iVal);

	//Word Block Data ó��
	BOOL  GetBinaryWordBlock(int iFirstAddress, int nDataCount, short* iReadArray);
	BOOL  SetBinaryWordBlock(int iFirstAddress, int nDataCount, short* iValArray, int iStartOffset = 0);
	BOOL  UpdateInput_InspData(int addr, int nWordCnt);
	BOOL UpdateInput_RepairData(int addr, int nWordCnt);

	short GetRead_Insp(int iWd) {
		return m_rDataInsp[iWd];
	}
	//BOOL GetInpectionResults(short nNo, POINTF* pStartPos, POINTF* pEndPos);
	//DWord Block Data ó��
	BOOL  SetBinaryDoubleWordBlock(int iFirstAddress, int nDataCount, int* iValArray);
	BOOL  GetBinaryDoubleWordBlock(int iFirstAddress, int nDataCount, int* iReadArray);

	void GetWordToBit(short nVal, BOOL * bBit);							//Word�� Bit������ �ɰ��� I/O�� ����� ��� 
	void GetBitToWord(BOOL * bBit, short * nVal);							//Bit�� Word�� ���
	void GetAsciiCode(short * nVal, int nReadCnt, CString * sVal);		//Ascii �ڵ�� ��ȯ, ��Ʈ�� �����͸� �����ö� ���
	void SetDWordParsing(int nDecimalPoint, double dVal, int * nVal);	//����� ������� ���� ���.
	void GetDWordParsing(int nDecimalPoint, int * nVal, double *dVal);  //����� ������� ���� ���.									
public:
	CEthernetPLC(CWnd* pMainWnd, CString strPLCIP, int nPLCPort);
	virtual ~CEthernetPLC();


protected:
	DECLARE_MESSAGE_MAP()
};



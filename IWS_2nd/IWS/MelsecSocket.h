#pragma once

#define UWM_PLC_ERROR		(WM_USER+500)
#define PLC_ERROR_ON		(WM_USER+501)
#define PLC_ERROR_OFF		(WM_USER+502)
#define MAX_DATA_BUFF		32

class CMelsecSocket :	public CWnd
{
	enum Area
	{
		D = 0xA8,
		W = 0xB4,
		r = 0xAF,
		ZR = 0xB0,
		X = 0x9C,
		Y = 0x9D,
		M = 0x90,
		L = 0x92,
		s = 0x98,
		B = 0xA0,
		f = 0x93,
		TS = 0xC1,
		TN = 0xC2,
		CS = 0xC4,
		CC = 0xC3,
		CN = 0xC5
	};

public:
	CMelsecSocket();
	virtual ~CMelsecSocket();
	CWnd* m_pParent;
	HWND m_hRcv;
	int				EthernetType;
	int				EthernetPort;
	CStringA		EthernetIP;
	WSADATA			wsaData;
	SOCKET			ClientSocket;
	SOCKADDR_IN		ServerAddress, Qj71e71;
	BOOL			thread_flag = TRUE;
	BOOL			exit_flag = TRUE;
	int				AddressSize = sizeof(ServerAddress);
	int				get_data;
	short			getdataval;
	WORD			getSdataval;
	WORD			getWdata[MAX_DATA_BUFF];
	BOOL			m_bConnected;
	int				iErrCnt;
protected:
	CWinThread *m_pThread;
	// Implementation
public:
	BOOL ReadWord(int iAddr, WORD *pData, int nData);
	BOOL WriteWord(int iAddr, WORD *pData, int nData);

	static UINT Thread_Ethernet(LPVOID lpVoid);
	int Thread_EthernetBody();

	void SetCWnd(CWnd* pParent);
	BOOL OpenPLCSocket(int Type, int Port, CStringA IP);
	short SendEthernetCommandRead(CStringA cmd, CStringA* reply);
	short SendEthernetCommandWrite(CStringA cmd);
	BOOL ClosePLCSocket();

	void Process(char *data, int nData);
	char ctox(char c);
	int	atox(char *str);
protected:

	void SendEthernet(char* send_message);

};


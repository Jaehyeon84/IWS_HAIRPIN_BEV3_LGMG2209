#pragma once
#include <SetupAPI.h>

#define USE_THREAD_RECIVE

#define COMPORT_BUF_SIZE 4096
#define	ASCII_XON	0x11
#define	ASCII_XOFF	0x13

#define	COMPORT_STOP1	0
#define	COMPORT_STOP1_5	1
#define	COMPORT_STOP2	2

// Parity bits
#define	COMPORT_PARITY_NONE	0	// 'N'
#define	COMPORT_PARITY_ODD	1	// 'O'
#define	COMPORT_PARITY_EVEN	2	// 'E'
#define	COMPORT_PARITY_MARK	3	// 'M'
#define	COMPORT_PARITY_SPACE	4	// 'S'


#define COMPORT_MSG WM_USER+1000
enum {
	COMPORT_EVT_ERROR = -1,
	COMPORT_EVT_OPEN = 1,
	COMPORT_EVT_CLOSE = 2
};
class CHubiSerial
{
public:
	CHubiSerial();
	virtual ~CHubiSerial(void);
	static int		char2wchar(char*	pSrc_char, wchar_t*	pDest_wchar_t);
	static int		wchar2char(wchar_t* pSrc_wchar_t, char*		pDest_char);
	static BOOL		GetComPortList(int** pPortNumber, int* nPort, CStringArray *arrStrDevice);
	static BOOL		GetComPortList(int** pPortNumber, int* nPort);
	virtual void	Thread_ReciveBody() {};
	int				Open(int iPortNumber, DWORD nRate, BYTE nParity, BYTE nSize, BYTE nStop, BOOL bReset = FALSE);
	BOOL			Close(BOOL bStopThreadCheckPort = TRUE);
	int				Send(char* pData, int nDataSize, DWORD dwTimeOut = INFINITE);
	int				Receive(char* pBuffer, int nBufferSize, DWORD dwTimeOut = INFINITE);
	BOOL			IsOpenPort() { return (m_iPortNumber >= 0); };
	int				GetPortNumber() { return m_iPortNumber; };
	int				Thread_CheckOpendPort();
	void			SetEventReceiver(HWND hWnd_EventMsgReceiver) { m_hEventReceiver = hWnd_EventMsgReceiver; }
	CString			m_sMsg;
	int		Callback_DroppedDevice();
	void			Flush();
	static BOOL		CheckExistPort(int iPort);
	CString			GetDeviceName() { return m_sDeviceName; };
	int				GetDeviceID() { return m_iDeviceID; };
	void			SetDeviceInfo(int iID, CString str) {
		m_iDeviceID = iID;
		m_sDeviceName = str;
	};
	
protected:
	CStringA		m_strReciveData;
	CString			m_sDeviceName;
	int				m_iDeviceID;
	int				m_iPortNumber;
	BOOL volatile	m_bDropPort;
	BOOL volatile	m_bCheckOpendPortStop;
	HANDLE			m_hCheckOpendPortThread;
	HANDLE			m_hReciveThread;
	HANDLE			m_hEventReceiveDone;

	HANDLE			m_hComPort;
	OVERLAPPED		m_ioSnd;
	OVERLAPPED		m_ioRcv;
	HWND			m_hEventReceiver;
	BOOL			m_bReciveThread;
};


#include "stdafx.h"
#include "CHubiSerial.h"

#pragma comment(lib, "setupapi.lib")
UINT WINAPI	Thread_CheckOpendPort_Launcher(LPVOID lParam)
{
	if (lParam == NULL) return 0;
	CHubiSerial *pCom = ((CHubiSerial*)lParam);
	char pName[] = "CComPortCtrl::CheckOpendPort()";
	CStringA str;

	TRACE("//=============================================\n");
	TRACE("// %s CheckOpendPort Started\n", str);
	TRACE("//=============================================\n");

	((CHubiSerial*)lParam)->Thread_CheckOpendPort();


	TRACE("//=============================================\n");
	TRACE("// %s CheckOpendPort Finised\n", str);
	TRACE("//=============================================\n");

	return 0;
}

UINT WINAPI Thread_Recive_Launcher(LPVOID lParam) {
	if (lParam == NULL) return 0;
	CHubiSerial *pCom = ((CHubiSerial*)lParam);
	char pName[] = "CComPortCtrl::CheckOpendPort()";
	CStringA str;
	int iID = pCom->GetDeviceID();

	TRACE("//=============================================\n");
	TRACE("// %s Recive Started\n", str);
	TRACE("//=============================================\n");

	((CHubiSerial*)lParam)->Thread_ReciveBody();


	TRACE("//=============================================\n");
	TRACE("// %s Recive Finised\n", str);
	TRACE("//=============================================\n");

	return 0;

}

CHubiSerial::CHubiSerial()
{
	CString sDeviceName = _T("NoName");
	m_sDeviceName = sDeviceName;
	m_iPortNumber = -1;
	m_bDropPort = FALSE;
	m_bCheckOpendPortStop = FALSE;
	m_hCheckOpendPortThread = NULL;
	m_hComPort = INVALID_HANDLE_VALUE;

	m_hReciveThread = NULL;

	memset(&m_ioSnd, 0x00, sizeof(OVERLAPPED));
	memset(&m_ioRcv, 0x00, sizeof(OVERLAPPED));
	m_ioSnd.hEvent = NULL;
	m_ioRcv.hEvent = NULL;
	m_hEventReceiver = NULL;
	m_hEventReceiveDone = NULL;
	m_bReciveThread = FALSE;
#ifdef USE_THREAD_RECIVE
	m_hEventReceiveDone = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif
	m_iDeviceID = -1;
}


CHubiSerial::~CHubiSerial(void)
{
	Close();
#ifdef USE_THREAD_RECIVE
	CloseHandle(m_hEventReceiveDone);
#endif
}

int CHubiSerial::char2wchar(char* pSrc_char, wchar_t* pDest_wchar_t)
{
	int nSrcLen = (int)strlen(pSrc_char);
	return MultiByteToWideChar(CP_ACP, 0, pSrc_char, nSrcLen, pDest_wchar_t, nSrcLen);
}
int CHubiSerial::wchar2char(wchar_t* pSrc_wchar_t, char* pDest_char)
{
	int nSrcLen = (int)wcslen(pSrc_wchar_t);
	return WideCharToMultiByte(CP_ACP, 0, pSrc_wchar_t, nSrcLen, pDest_char, nSrcLen, NULL, NULL);
}

BOOL CHubiSerial::GetComPortList(int** pPortNumber, int* nPort,CStringArray *arrStrDevice)
{
	//REF : http://support.microsoft.com/kb/259695/ko
	// Create a HDEVINFO with all present devices.
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL/*_T("USB")*/, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);
	if (hDevInfo == INVALID_HANDLE_VALUE) { return 1; }

	int iNofPort = 0;
	int pPort[100];
	// Enumerate through all devices in Set.
	SP_DEVINFO_DATA DeviceInfoData;
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
	{
		DWORD DataT;
		LPTSTR buffer = NULL;
		DWORD buffersize = 0;

		//
		// Call function with null to begin with, 
		// then use the returned buffer size (doubled)
		// to Alloc the buffer. Keep calling until
		// success or an unknown failure.
		//
		//  Double the returned buffersize to correct
		//  for underlying legacy CM functions that 
		//  return an incorrect buffersize value on 
		//  DBCS/MBCS systems.
		// 
		while (!SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&DeviceInfoData,
			//SPDRP_DEVICEDESC,
			SPDRP_FRIENDLYNAME,
			&DataT,
			(PBYTE)buffer,
			buffersize,
			&buffersize))
		{
			DWORD dwError = GetLastError();
			if (dwError == ERROR_INSUFFICIENT_BUFFER)
			{
				// Change the buffer size.
				if (buffer) LocalFree(buffer);
				// Double the size to avoid problems on 
				// W2k MBCS systems per KB 888609. 
				buffer = (LPTSTR)LocalAlloc(LPTR, buffersize * 2);
			}
			else
			{
				// Insert error handling here.
				break;
			}
		}

		if (buffer != NULL)
		{
			CString sDevice;
			sDevice.Format(_T("%s"), buffer);
			arrStrDevice->Add(sDevice);
			int iPos = sDevice.Find(_T("(COM"));
			if (iPos >= 0)
			{
				int iEPos = sDevice.Find(_T(")"), iPos + 1);
				if ((iEPos - iPos - 4)<3 && (iEPos - iPos - 4)>0)
				{
					CString sNum = sDevice.Mid(iPos + 4, iEPos - iPos - 4);
					int     iNum = _ttoi(sNum.GetBuffer(sNum.GetLength()));
					if (iNum>0)
					{
						pPort[iNofPort] = iNum;
						iNofPort++;
					}
				}
			}

			//TRACE(_T("\n%s    "),sDevice);
		}

		if (buffer) LocalFree(buffer);
	}

	if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS)
	{
		(*pPortNumber) = NULL;
		(*nPort) = 0;

		return FALSE;
	}


	if (iNofPort<1)
	{
		(*pPortNumber) = NULL;
		(*nPort) = 0;
	}
	else
	{
		(*pPortNumber) = new int[iNofPort];
		(*nPort) = iNofPort;
		memcpy((*pPortNumber), pPort, sizeof(int)*iNofPort);
	}



	//  Cleanup
	SetupDiDestroyDeviceInfoList(hDevInfo);

	return TRUE;
}

BOOL CHubiSerial::GetComPortList(int** pPortNumber, int* nPort)
{
	//REF : http://support.microsoft.com/kb/259695/ko
	// Create a HDEVINFO with all present devices.
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL/*_T("USB")*/, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);
	if (hDevInfo == INVALID_HANDLE_VALUE) { return 1; }

	int iNofPort = 0;
	int pPort[100];
	// Enumerate through all devices in Set.
	SP_DEVINFO_DATA DeviceInfoData;
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
	{
		DWORD DataT;
		LPTSTR buffer = NULL;
		DWORD buffersize = 0;

		//
		// Call function with null to begin with, 
		// then use the returned buffer size (doubled)
		// to Alloc the buffer. Keep calling until
		// success or an unknown failure.
		//
		//  Double the returned buffersize to correct
		//  for underlying legacy CM functions that 
		//  return an incorrect buffersize value on 
		//  DBCS/MBCS systems.
		// 
		while (!SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&DeviceInfoData,
			//SPDRP_DEVICEDESC,
			SPDRP_FRIENDLYNAME,
			&DataT,
			(PBYTE)buffer,
			buffersize,
			&buffersize))
		{
			DWORD dwError = GetLastError();
			if (dwError == ERROR_INSUFFICIENT_BUFFER)
			{
				// Change the buffer size.
				if (buffer) LocalFree(buffer);
				// Double the size to avoid problems on 
				// W2k MBCS systems per KB 888609. 
				buffer = (LPTSTR)LocalAlloc(LPTR, buffersize * 2);
			}
			else
			{
				// Insert error handling here.
				break;
			}
		}

		if (buffer != NULL)
		{
			CString sDevice;
			sDevice.Format(_T("%s"), buffer);
			int iPos = sDevice.Find(_T("(COM"));
			if (iPos >= 0)
			{
				int iEPos = sDevice.Find(_T(")"), iPos + 1);
				if ((iEPos - iPos - 4) < 3 && (iEPos - iPos - 4) > 0)
				{
					CString sNum = sDevice.Mid(iPos + 4, iEPos - iPos - 4);
					int     iNum = _ttoi(sNum.GetBuffer(sNum.GetLength()));
					if (iNum > 0)
					{
						pPort[iNofPort] = iNum;
						iNofPort++;
					}
				}
			}

			//TRACE(_T("\n%s    "),sDevice);
		}

		if (buffer) LocalFree(buffer);
	}

	if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS)
	{
		(*pPortNumber) = NULL;
		(*nPort) = 0;

		return FALSE;
	}


	if (iNofPort < 1)
	{
		(*pPortNumber) = NULL;
		(*nPort) = 0;
	}
	else
	{
		(*pPortNumber) = new int[iNofPort];
		(*nPort) = iNofPort;
		memcpy((*pPortNumber), pPort, sizeof(int)*iNofPort);
	}



	//  Cleanup
	SetupDiDestroyDeviceInfoList(hDevInfo);

	return TRUE;
}

int CHubiSerial::Open(int iPortNumber, DWORD nRate, BYTE nParity, BYTE nSize, BYTE nStop, BOOL bReset/*=FALSE*/)
{
	if (iPortNumber<1 || iPortNumber>99) return -1;

	if (IsOpenPort())
	{
		m_sMsg.Format(_T("COM%d Already opened!"), iPortNumber);
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);
		return -2;
	}


	int nPort = 0;
	int *pPortNum = NULL;
	if (!CHubiSerial::GetComPortList(&pPortNum, &nPort) || nPort == 0)
	{
		m_sMsg.Format(_T("COM Port not exist!!"));
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);
		return -3;
	}

	BOOL bExistPortNumber = FALSE;
	for (int i = 0; i<nPort; i++)
	{
		if (pPortNum[i] == iPortNumber)
		{
			bExistPortNumber = TRUE;
			break;
		}
	}
	if (pPortNum != NULL) delete[] pPortNum;

	if (!bExistPortNumber)
	{
		m_sMsg.Format(_T("COM%d not exist!"), iPortNumber);
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);
		return -4;
	}


	char	cComPort[25];
	sprintf_s(cComPort, 25, "\\\\.\\COM%d", iPortNumber);

	m_hComPort = ::CreateFileA(cComPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0); // NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (m_hComPort == HANDLE(-1))
	{
		DWORD ErrorCode = ::GetLastError();
		::CloseHandle(m_hComPort);
		m_hComPort = INVALID_HANDLE_VALUE;

		m_sMsg.Format(_T("Cannot open COM%d. error code=%d"), iPortNumber, ErrorCode);
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);
		return -5;
	}



	// 이벤트마스크를 설정
	if (::SetCommMask(m_hComPort, EV_RXCHAR) == FALSE)
	{
		::CloseHandle(m_hComPort);
		m_hComPort = INVALID_HANDLE_VALUE;
		m_sMsg.Format(_T("Cannot open COM%d. MaskError"), iPortNumber);
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);

		return -6;
	}

	// 포트의Input/Output Buffer 사이즈를 설정
	if (::SetupComm(m_hComPort, COMPORT_BUF_SIZE, COMPORT_BUF_SIZE) == FALSE)
	{
		DWORD ErrorCode = ::GetLastError();
		::CloseHandle(m_hComPort);
		m_hComPort = INVALID_HANDLE_VALUE;

		m_sMsg.Format(_T("Cannot open COM%d. Buffer size Error"), iPortNumber);
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);

		return -7;
	}

	// 모든 Rx/Tx 동작을 제한하고 또한 Buffer의 내용을 버림
	// Abort 는 Ovelapped 작업 취소
	// Clear 는 입출력 버퍼 청소
	if (::PurgeComm(m_hComPort, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR) == FALSE)
	{
		DWORD ErrorCode = ::GetLastError();
		::CloseHandle(m_hComPort);
		m_hComPort = INVALID_HANDLE_VALUE;

		m_sMsg.Format(_T("Cannot open COM%d. Buffer init error"), iPortNumber);
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);

		return -8;
	}

	// overlapped I/O 와 관련하여 timeout설정
	COMMTIMEOUTS CommTimeouts = { 0xFFFFFFFF,0,1000,0,1000 };
	CommTimeouts.ReadIntervalTimeout = 0xFFFFFFFF;		// 통신 선로상에서 한바이트가 전송되고 또한 바이트가 전송되기까지의 시간
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;				//
	CommTimeouts.ReadTotalTimeoutConstant = 0;//1000
	CommTimeouts.WriteTotalTimeoutMultiplier = 2 * CBR_9600 / nRate;
	CommTimeouts.WriteTotalTimeoutConstant = 0;
	if (::SetCommTimeouts(m_hComPort, &CommTimeouts) == FALSE)		// 통신포트의TimeOut을설정
	{
		DWORD ErrorCode = ::GetLastError();
		::CloseHandle(m_hComPort);
		m_hComPort = INVALID_HANDLE_VALUE;

		m_sMsg.Format(_T("Cannot open COM%d. SetTimeOut error"), iPortNumber);
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);

		return -9;
	}

	//DCB를 설정(DCB: 시리얼통신의 제어 파라메터)
	DCB	Dcb;
	//DCBlength					: DCB구조체의 바이트크기를 지정한다.
	//BaudRate					: 통신장치의 전송속도를 지정한다. 실제 baud속도를 입력하거나 다음의 baud속도 인덱스중에 하나를 선택
	//	- CBR_110  
	//	- CBR_300  
	//	- CBR_600  
	//	- CBR_1200 
	//	- CBR_2400  
	//	- CBR_4800  
	//	- CBR_9600  
	//	- CBR_14400 
	//	- CBR_19200 
	//	- CBR_38400 
	//	- CBR_56000 
	//	- CBR_57600 
	//	- CBR_115200
	//	- CBR_128000
	//	- CBR_256000
	//fBinary						: 이진모드(binary mode) 사용함을 지정. WIN32 API '이진모드'만 지원하므로 이멤버는 반드시 TRUE이어야 한다. FALSE이면 작업을 할수 없다.
	//fParity						: 패리티체크를 사용할것인가를 지정. TRUE이면 패리티체크는 실행되고, 에러도 보고된다.
	//fOutxCtsFlow				: CTS(Clear to Send)신호를 출력조절 컨트롤부터 감시할것인가를 지정한다. 이멤버가 TRUE이고, CTS가 0이면, CTS신호가 보내질때까지(CTS 1일때까지) 출력은 중지된다.
	//fOutxDsrFlow				: DSR(Data set Read)신호를 출력조절 컨트롤부터 감시할것인가를 지정한다. 이멤버가 TRUE이고, DSR이 0이면, DSR신호가 보내질때까지(DSR 1일때까지) 출력은 중지된다. 
	//fDtrControl					: DTR(Data terminal Ready)신호를 사용할것인가 지정한다. 다음값중에 하나를 지정할수 있다. 
	//	- DTR_CONTROL_DISABLE   : 장치를 열거나 DTR이 on되려고 할때, DTR을 OFF한다. 
	//	- DTR_CONTROL_ENABLE    : 장치를 열거나 DTR이 OFF되려고 할때, DTR을 on한다.
	//	- DTR_CONTROL_HANDSHAKE : DTR handshaking을 on한다. 에러가 발생하면 어플리케이션에서 EscapeCommFunction 함수를 이용해서 적용한다.
	//fDsrSensitivity				: 장치가 DSR(Data Set Read) 신호영역에 민감할것을 지정한다. TRUE이면, 모뎀의 DSR 입력선이 고위(1)일때까지 장치가 모든바이트의 수신을 무시한다.
	//fTXContinueOnXoff			: 수신버퍼가 가득차거나 장치에 XoffChar 가 전송되면 송신을 중단할것인가를 지정한다.
	//fOutX						: 송신동안 XON/XOFF 흐름제어를 할것인가를 지정한다. TRUE이면, XoffChar 문자를 받았을때 송신을 중지하고, 다시 XonChar 문자를 받았을때 송신한다.
	//fInX						: 수신동안 XON/XOFF 흐름제어를 할것인가를 지정한다. TRUE이면, XoffLim 바이트로 버퍼가 가득찼을때 XoffChar 문자가 송신되고
	//XonLim						: 바이트 이하로 비었을때 XonChar 문자가 송신된다.
	//fErrorChar					: 패리티에러가 수신된 바이트들은 ErrorChar 문자로 대치할것인가를 지정한다. TRUE이면 fParity 멤버가 TRUE이면 변환이 일어난다.
	//fNull						: NULL바이트를 제외할 것인가를 지정한다. TRUE이면 NULL바이트가 수신되었을때 버려진다.
	//fRtsControl					: RTS(Request to Send) 흐름제어를 사용할것인지 지정한다. 0이면 디폴트 RTS_CONTROL_HANDSHAKE 이다. 다음값중 하나를 지정할수 있다.
	//	- RTS_CONTROL_DISABLE	: 장치를 열거나 RTS신호가 on이 되려할때, RTS를 OFF한다.
	//	- RTS_CONTROL_ENABLE		: 장치를 열거나 RTS신호가 OFF되려 할때, RTS를 on한다.
	//	- RTS_CONTROL_HANDSHAKE	: RTS handshaking을 on한다.
	//	- RTS_CONTROL_TOGGLE		: 송신할 바이트 있다면 RTS 라인은 1이고 모든버퍼에 바이트를 보내고, RTS 라인은 0이된다.
	//fAbortOnError				: 에러가 발생하면 송신,수신 작업이 종료할것인가를 지정한다. TRUE이면, 에러가 발생하면 모든송수신 작업은 에러상태와 함께 종료된다. 장치는 어떤통신운영도 ClearCommError 함수를 호출하기까지 수락하지 않는다.
	//fDummy2						: 사용안함.
	//wReserved					: 사용안함. 항상 0이어야한다.
	//XonLim						: XON문자가 보내기전에, 수신버퍼에 허용되는 최소바이트.
	//XoffLim						: XOFF문자가 보내기전에, 수신버퍼에 허용되는 최대바이트. 허용된 최대바이트 = 수신버퍼(byte) - XoffLin
	//ByteSize					: 송신,수신 바이트의 비트수. (4-8)
	//Parity						: 사용될 패리티를 설정한다. 다음값중에 하나를 지정할수 있다.
	//	- EVENPARITY				: 짝수
	//	- MARKPARITY				: 마크
	//	- NOPARITY				: 없음
	//	- ODDPARITY				: 홀수
	//	- SPACEPARITY			: 간격
	//StopBits					: 사용될 STOP비트수를 설정한다. 다음값중에 하나를 지정할수 있다.
	//	- ONESTOPBIT				:   1비트
	//	- ONE5STOPBITS			:  1.5비트
	//	- TWOSTOPBITS			:    2비트
	//XonChar						: 송신,수신에 사용될 XON문자를 지정한다.
	//XoffChar					: 송신,수신에 사용될 XOFF문자를 지정한다.
	//ErrorChar					: Parity 에러와 함께수신된 바이트에 치환할 문자를 지정한다.
	//EofChar						: binary모드가 아닐경우, 데이터 끝을 나타내는 문자를 지정한다.
	//EvtChar						: 신호 이벤트에 사용될 문자를 지정한다.
	//wReserved1					: 사용안함.

	Dcb.DCBlength = sizeof(Dcb);
	if (::GetCommState(m_hComPort, &Dcb) == FALSE)
	{
		DWORD ErrorCode = ::GetLastError();
		::CloseHandle(m_hComPort);
		m_hComPort = INVALID_HANDLE_VALUE;

		m_sMsg.Format(_T("Cannot open COM%d. DCBGet Error"), iPortNumber);
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);

		return -10;
	}
	
// 	Dcb.BaudRate = CBR_4800;     //  baud rate
// 	Dcb.ByteSize = 8;             //  data size, xmit and rcv
// 	Dcb.Parity = NOPARITY;      //  parity bit
// 	Dcb.StopBits = ONESTOPBIT;    //  stop bit

	
// 
	BOOL bUseDTRDSR = FALSE;
	BOOL bUseDRSCTS = FALSE;
	BOOL bUseXONXOFF = FALSE;

	//===================
	Dcb.BaudRate = nRate;
	Dcb.ByteSize = nSize;
	Dcb.StopBits = nStop;
	switch (nParity)
	{
	case COMPORT_PARITY_NONE: Dcb.fParity = FALSE; Dcb.Parity = NOPARITY; break;
	case COMPORT_PARITY_ODD: Dcb.fParity = TRUE; Dcb.Parity = ODDPARITY; break;
	case COMPORT_PARITY_EVEN: Dcb.fParity = TRUE; Dcb.Parity = EVENPARITY; break;
	case COMPORT_PARITY_MARK: Dcb.fParity = TRUE; Dcb.Parity = MARKPARITY; break;
	case COMPORT_PARITY_SPACE: Dcb.fParity = TRUE; Dcb.Parity = SPACEPARITY; break;
	}
 
 
 	//===================
 	if (bReset)
 	{
 		Dcb.fOutxDsrFlow = TRUE;
 		Dcb.fDtrControl = DTR_CONTROL_ENABLE;
 	}
 	else
 	{
 		Dcb.fOutxDsrFlow = bUseDTRDSR;
 		Dcb.fDtrControl = DTR_CONTROL_DISABLE;//bUseDTRDSR ? DTR_CONTROL_HANDSHAKE : DTR_CONTROL_ENABLE;
 	}
 	Dcb.fOutxCtsFlow = bUseDRSCTS;
 	Dcb.fRtsControl = bUseDRSCTS ? RTS_CONTROL_HANDSHAKE: RTS_CONTROL_ENABLE;
 										  //===================
 	Dcb.fInX = bUseXONXOFF;
 	Dcb.fOutX = bUseXONXOFF;
 	Dcb.XonLim = 2048;
 	Dcb.XoffLim = 1024;
 	Dcb.XonChar = ASCII_XON;
 	Dcb.XoffChar = ASCII_XOFF;
 	//===================
 	Dcb.fBinary = TRUE;
 	Dcb.fParity = TRUE;
	




	if (::SetCommState(m_hComPort, &Dcb) == FALSE)
	{
		DWORD ErrorCode = ::GetLastError();
		::CloseHandle(m_hComPort);
		m_hComPort = INVALID_HANDLE_VALUE;
		m_sMsg.Format(_T("Cannot open COM%d. DCBSEt Error"), iPortNumber);
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);


		return -11;
	}


	// 통신포트의 모든 에러를 리셋
	DWORD ErrorCode;
	if (::ClearCommError(m_hComPort, &ErrorCode, NULL) == FALSE)
	{
		::CloseHandle(m_hComPort);
		m_hComPort = INVALID_HANDLE_VALUE;

		m_sMsg.Format(_T("Cannot open COM%d. Error-clear error"), iPortNumber);
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);

		return -12;
	}


	m_ioSnd.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);;
	if (m_ioSnd.hEvent == NULL)
	{
		DWORD ErrorCode = ::GetLastError();
		::CloseHandle(m_hComPort);
		m_hComPort = INVALID_HANDLE_VALUE;

		m_sMsg.Format(_T("Cannot open COM%d. IOW(S) error"), iPortNumber);
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);

		return -13;
	}

	m_ioRcv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);;
	if (m_ioRcv.hEvent == NULL)
	{
		DWORD ErrorCode = ::GetLastError();
		::CloseHandle(m_hComPort);
		m_hComPort = INVALID_HANDLE_VALUE;

		m_sMsg.Format(_T("Cannot open COM%d. IOW(R) error"), iPortNumber);
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);
		return -14;
	}



	m_iPortNumber = iPortNumber;
	m_bDropPort = FALSE;
	m_bCheckOpendPortStop = FALSE;
	m_hCheckOpendPortThread = (HANDLE)_beginthreadex(NULL, 0, Thread_CheckOpendPort_Launcher, this, 0, NULL);
	TRACE("Success port open( Com%d )", iPortNumber);
	m_sMsg.Empty();
	if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_OPEN, 0);
#ifdef USE_THREAD_RECIVE
	m_bReciveThread = TRUE;
	m_hReciveThread = (HANDLE)_beginthreadex(NULL, 0, Thread_Recive_Launcher, this, 0, NULL);
#endif // USE_THREAD_RECIVE
	return 0;
}

BOOL CHubiSerial::Close(BOOL bStopThreadCheckPort/*=TRUE*/)
{

	if (!IsOpenPort())
	{
		m_sMsg.Format(_T("not opened comport.."));
		if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);
		return FALSE;
	}

	m_bReciveThread = FALSE;
	
	if (bStopThreadCheckPort)
	{
		if (m_hCheckOpendPortThread != NULL)
		{
			m_bCheckOpendPortStop = TRUE;
			if (WaitForSingleObject(m_hCheckOpendPortThread, 2000) != WAIT_OBJECT_0)
			{
				::TerminateThread(m_hCheckOpendPortThread, 0);
			}
			m_hCheckOpendPortThread = NULL;
		}
	}

	if (m_ioSnd.hEvent != NULL)
	{
		CloseHandle(m_ioSnd.hEvent);
		m_ioSnd.hEvent = NULL;
	}
	if (m_ioRcv.hEvent != NULL)
	{
		CloseHandle(m_ioRcv.hEvent);
		m_ioRcv.hEvent = NULL;
	}
	memset(&m_ioSnd, 0x00, sizeof(OVERLAPPED));
	memset(&m_ioRcv, 0x00, sizeof(OVERLAPPED));


	if (::SetCommMask(m_hComPort, 0) == FALSE)
	{
		/*
		if (::CloseHandle(m_hComPort) == FALSE)
		{
		m_hComPort = INVALID_HANDLE_VALUE;
		return COMPORT_RET_FAILED;
		}
		m_hComPort = INVALID_HANDLE_VALUE;

		return COMPORT_RET_FAILED;
		*/
	}
	if (::EscapeCommFunction(m_hComPort, CLRDTR) == FALSE)
	{
		/*
		if (::CloseHandle(m_hComPort) == FALSE)
		{
		m_hComPort = INVALID_HANDLE_VALUE;
		return COMPORT_RET_FAILED;
		}
		m_hComPort = INVALID_HANDLE_VALUE;

		return COMPORT_RET_FAILED;
		*/
	}
	if (::PurgeComm(m_hComPort, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR) == FALSE)
	{
		/*
		if (::CloseHandle(m_hComPort) == FALSE)
		{
		m_hComPort = INVALID_HANDLE_VALUE;
		return COMPORT_RET_FAILED;
		}
		m_hComPort = INVALID_HANDLE_VALUE;

		return COMPORT_RET_FAILED;
		*/
	}
	if (::CloseHandle(m_hComPort) == FALSE)
	{
		/*
		m_hComPort = INVALID_HANDLE_VALUE;
		return COMPORT_RET_FAILED;
		*/
	}
	TRACE("Success port close( Com%d )", m_iPortNumber);
	m_hComPort = INVALID_HANDLE_VALUE;
	m_iPortNumber = -1;
	if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_CLOSE, 0);
#ifdef USE_THREAD_RECIVE
	if (m_hReciveThread) {
		Sleep(100);
		::TerminateThread(m_hReciveThread, 0);
	}
		
#endif // USE_THREAD_RECIVE
	return TRUE;
}



int CHubiSerial::Send(char* pData, int nDataSize, DWORD dwTimeOut/*=INFINITE*/)
{
	if (!IsOpenPort()) return -1;
	DWORD dwErrorFlags;
	DWORD dwErrorCode;
	COMSTAT ComStat;
	DWORD dwSend = 0;
	DWORD dwSendCurrent = 0;

	DWORD dwTickTime = GetTickCount();
	BOOL bTimeout = FALSE;
	// 통신 포트의 에러를 리셋
	//ClearCommError(m_hComPort, &dwErrorFlags, &ComStat);

	// overlapped I/O를 통하여 데이터 전송
#ifdef USE_THREAD_RECIVE
	ResetEvent(m_hEventReceiveDone);
#endif // USE_THREAD_RECIVE

	if (!WriteFile(m_hComPort, pData, nDataSize, &dwSendCurrent, &m_ioSnd))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			//CommTimeOuts.WriteTotalTimeoutMultiplier 에 따라 타임아웃될 수 있음에 주의

			//			int nCount=0;
			while (!GetOverlappedResult(m_hComPort, &m_ioSnd, &dwSendCurrent, FALSE))
			{
				dwErrorCode = GetLastError();
				if (dwErrorCode != ERROR_IO_INCOMPLETE)
				{
					ClearCommError(m_hComPort, &dwErrorFlags, &ComStat);
					TRACE("Com error : code = %u", dwErrorCode);
					if (dwErrorFlags>0)
						TRACE("Com error : CommErrorFlag = %u", dwErrorFlags);
					return -1;
					break;
				}
				else
				{
					//VisT(TRACE_NORMAL,0,L"Send Data =%d\n ",dwSendCurrent);
					dwSend += dwSendCurrent;
				}

				if (GetTickCount() - dwTickTime > dwTimeOut)
				{
					bTimeout = TRUE;
					break;
				}
			}
			dwSend += dwSendCurrent;
			if (dwSend != nDataSize)
			{
				TRACE("Send timeout(Total of %lf byte sent)", dwSend);
			}
			if (bTimeout) ClearCommError(m_hComPort, &dwErrorFlags, &ComStat);

		}
		else
		{
			ClearCommError(m_hComPort, &dwErrorFlags, &ComStat);
			if (dwErrorFlags>0)
				TRACE("Com error : CommErrorFlag = %u", dwErrorFlags);
			dwSend = 0;
			return -1;
		}
	}
	else
	{
		dwSend = dwSendCurrent;
		//TRACE("SEND OK [%s](%d)\n", pData, dwSend);
	}



	return dwSend;
}

int CHubiSerial::Receive(char* pBuffer, int nBufferSize, DWORD dwTimeOut/*=INFINITE*/)
{
	if (!IsOpenPort()) return 0;

	DWORD dwRead, dwError, dwErrorFlags;
	COMSTAT comstat;

	//ClearCommError를 이용해 읽을 내용이 있는지 체크.
	if (!ClearCommError(m_hComPort, &dwErrorFlags, &comstat))
	{
		DWORD dwErrCode = GetLastError();
		//_tracer(TRACE_ERROR, 0, L"Com error : code = %u", dwErrCode);
		return 0;
	}
	switch (dwErrorFlags)
	{
	case 0: break;
	case CE_BREAK:TRACE("ReceiveEX:ClearCommError:dwErrorFlags = CE_BREAK	"); break;
	case CE_FRAME:TRACE("ReceiveEX:ClearCommError:dwErrorFlags = CE_FRAME	"); break;
	case CE_OVERRUN:TRACE("ReceiveEX:ClearCommError:dwErrorFlags = CE_OVERRUN	"); break;
	case CE_RXOVER:TRACE("ReceiveEX:ClearCommError:dwErrorFlags = CE_RXOVER	"); break;
	case CE_RXPARITY:TRACE("ReceiveEX:ClearCommError:dwErrorFlags = CE_RXPARITY"); break;
	default:TRACE("ReceiveEX:ClearCommError:dwErrorFlags = %u", dwErrorFlags); break;
	}


	dwRead = comstat.cbInQue;
	if ((int)dwRead >= nBufferSize)
	{
		TRACE("Buffer size too small (buf%d <= read%d)", nBufferSize, dwRead);
	}
	if (dwRead > 0)
	{
		if (!ReadFile(m_hComPort, pBuffer, nBufferSize, &dwRead, &m_ioRcv))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				//CommTimeOuts.ReadTotalTimeoutConstant 에 따라 타임아웃될 수 있음에 주의
				while (!GetOverlappedResult(m_hComPort, &m_ioRcv, &dwRead, TRUE))
				{
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE)
					{// 뭔가 에러가 발생한 상태임. 컴포트 복구 시도.

						ClearCommError(m_hComPort, &dwErrorFlags, &comstat);
						TRACE("Com error : code = %u", dwError);
						if (dwErrorFlags>0)
							TRACE("Com error : CommErrorFlag = %u", dwErrorFlags);
						break;
					}
				}
			}
			else
			{// 현 시퀀스에서 원치 않는 에러가 발생한 경우 임.
				dwRead = 0;
				ClearCommError(m_hComPort, &dwErrorFlags, &comstat);
				if (dwErrorFlags>0)
					TRACE("Com error : CommErrorFlag = %u", dwErrorFlags);
			}
		}
	}
	return dwRead;
}

int	CHubiSerial::Thread_CheckOpendPort()
{
	while (!m_bCheckOpendPortStop)
	{
		if (!IsOpenPort())// || !CheckExistPort(m_iPortNumber))
		{
			m_sMsg.Format(_T("Dropped device!! Com%d"), m_iPortNumber);
			if (m_hEventReceiver) ::SendMessage(m_hEventReceiver, COMPORT_MSG, COMPORT_EVT_ERROR, 0);

			m_bDropPort = TRUE;
			TRACE("Dropped device!!");
			//Close(FALSE);
			Callback_DroppedDevice();
			break;
		}

		//통신중에는 절전모드 또는 화면보호기 진입을 금지한다( 절전모드/화면보호기 방지(타이머 리셋) )
		SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
		Sleep(200);
	}
	SetThreadExecutionState(ES_CONTINUOUS); //절전모드/화면보호기 설정 원복
	TRACE("Stop thread(Thread_CheckOpendPort)");
	return 0;
}

int CHubiSerial::Callback_DroppedDevice()
{
	return Close(FALSE);
}

void CHubiSerial::Flush()
{
	if (!IsOpenPort()) return;
	Sleep(10);
	PurgeComm(m_hComPort, PURGE_RXCLEAR | PURGE_RXABORT);
}

BOOL CHubiSerial::CheckExistPort(int iPort)
{
	//	if(!IsOpenPort())
	//	{
	//		VisT(TRACE_ERROR,0,L"Not opend port..");
	//		return FALSE;
	//	}


	HDEVINFO hDevInfo = SetupDiGetClassDevsA(NULL, ("USB"), 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);
	if (hDevInfo == INVALID_HANDLE_VALUE) { return FALSE; }

	CString sComPort;
	sComPort.Format(_T("(COM%d)"), iPort);//m_iPortNumber);

	BOOL bFind = FALSE;
	SP_DEVINFO_DATA DeviceInfoData;
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
	{
		DWORD DataT;
		LPTSTR buffer = NULL;
		DWORD buffersize = 0;


		while (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, &DataT, (PBYTE)buffer, buffersize, &buffersize))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				if (buffer) LocalFree(buffer);
				buffer = (LPTSTR)LocalAlloc(LPTR, buffersize * 2);
			}
			else
			{
				break;
			}
			Sleep(1);
		}

		if (buffer != NULL)
		{
			CString sDevice;
			sDevice.Format(_T("%s"), buffer);
			int iPos = sDevice.Find(sComPort);
			if (iPos >= 0)
			{
				bFind = TRUE;
				break;
			}
		}

		if (buffer) LocalFree(buffer);
		Sleep(1);
	}

	if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS)
	{
		TRACE("Dropped Com%d", iPort);
		return FALSE;
	}

	//  Cleanup
	SetupDiDestroyDeviceInfoList(hDevInfo);
	if (!bFind) {
		TRACE("Dropped Com%d", iPort);
		TRACE(L"Dropped Com%d\n", iPort);
		return FALSE;  
	}
		
	return bFind;
}


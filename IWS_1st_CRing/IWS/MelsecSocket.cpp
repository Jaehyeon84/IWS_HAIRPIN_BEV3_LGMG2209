#include "stdafx.h"
#include "MelsecSocket.h"

#define			EVENT_RECEIVE_COMPLETE		"E000"
#define		PLC_IP		"192.168.0.254"
#define		PLC_PORT	0x2000
CCriticalSection g_csPlcWrite;

CMelsecSocket::CMelsecSocket()
{
	m_pParent = nullptr;
	get_data = 0;
	m_hRcv = NULL;
	m_bConnected = FALSE;
	iErrCnt = 0;
}


CMelsecSocket::~CMelsecSocket()
{
	thread_flag = FALSE;
	Sleep(1000);
}

BOOL CMelsecSocket::ReadWord(int iAddr, WORD *pData, int nData)
{
	if (!m_bConnected) return FALSE;

	int ReadCnt = nData;
	CStringA strData;
	char SendFrame[256] = {};
	int StartAddr = iAddr;

	memset(SendFrame, 0, 256);
	SendFrame[0] = 0x50; //서브헤더
	SendFrame[1] = 0x00; //서브헤더
	SendFrame[2] = 0x00; //네트워크 번호
	SendFrame[3] = 0xFFu; //&HFF 'PLC 번호
	SendFrame[4] = 0xFFu; //요구상대 모듈 IO번호_L
	SendFrame[5] = 0x03; //요구상대 모듈 IO번호_H
	SendFrame[6] = 0x00; //요구상대 모듈 국번호
	SendFrame[7] = 0x0C; //요구 데이터 길이_L (12바이트)
	SendFrame[8] = 0x00; //요구 데이터 길이_H (12바이트)
	SendFrame[9] = 0x10; //CPU 감시 타이머_L
	SendFrame[10] = 0x00; //CPU 감시 타이머_H
	//Word Read :::: Command = 0401
	SendFrame[11] = 0x01; //커맨드_L
	SendFrame[12] = 0x04; //커맨드_H
	SendFrame[13] = 0x00; //서브 커맨드_L
	SendFrame[14] = 0x00; //서브 커맨드_H
	//읽기 주소 설정
	SendFrame[15] = (byte)(StartAddr % 256); //선두 디바이스_L
	SendFrame[16] = (byte)(StartAddr / 256); //선두 디바이스_H
	SendFrame[17] = 0x00;//선두 디바이스
	SendFrame[18] = (byte)Area::D;
	SendFrame[19] = (byte)(ReadCnt % 256); //데이터 개수_L
	SendFrame[20] = (byte)(ReadCnt / 256); //데이터 개수_H
	int offset = 21;
	int n = offset;// +WordCnt * 2;
	SendFrame[offset] = 0;
	int nError;
	get_data = FALSE;
	if (!EthernetType) {
		SOCKADDR_IN SendAddress;
		// Socket 생성
		SendAddress.sin_family = AF_INET;
		SendAddress.sin_addr.s_addr = inet_addr(PLC_IP);
		SendAddress.sin_port = htons(PLC_PORT);

		nError = sendto(ClientSocket, SendFrame, n, MSG_DONTROUTE, (const struct sockaddr *)&SendAddress, sizeof(SendAddress));
	}
	else				
		nError = send(ClientSocket, SendFrame, n, MSG_DONTROUTE);

	//	TRACE("message sent length is %d\n", nError);
	if (nError == -1)
	{
		nError = WSAGetLastError();
		TRACE("WSAGetLastError is %d\n", nError);
		if (iErrCnt > 20)
			m_bConnected = FALSE;
		iErrCnt++;
		return FALSE;
	}
	iErrCnt = 0;
	int iwait = 0;
	do {
		Sleep(1);
		if (get_data == TRUE) break;
	} while (iwait++ < 100);

	memcpy(pData, getWdata, sizeof(WORD)*nData);
	return get_data;
}

BOOL CMelsecSocket::WriteWord(int iAddr, WORD *pData, int nData)
{
	if (!m_bConnected) return FALSE;
	int WordCnt = nData;
	char SendFrame[256];
	int StartAddr = iAddr;
	memset(SendFrame, 0, 256);
	SendFrame[0] = 0x50;//'서브헤더
	SendFrame[1] = 0x00;//'서브헤더
	SendFrame[2] = 0x00;//'네트워크 번호
	SendFrame[3] = 0xFFu;//FF'PLC 번호
	SendFrame[4] = 0xFFu;//'요구상대 모듈 IO번호_L
	SendFrame[5] = 0x03;//'요구상대 모듈 IO번호_H
	SendFrame[6] = 0x00;//'요구상대 모듈 국번호
	SendFrame[7] = (byte)(12 + (WordCnt * 2));//'요구 데이터 길이_L (12바이트)
	SendFrame[8] = 0x00;//'요구 데이터 길이_H (12바이트)
	SendFrame[9] = 0x10;//'CPU 감시 타이머_L
	SendFrame[10] = 0x00;//'CPU 감시 타이머_H
	//'Word Write :::: Command = 1401
	SendFrame[11] = 0x01;//'커맨드_L
	SendFrame[12] = 0x14;//'커맨드_H
	SendFrame[13] = 0x00;//'서브 커맨드_L
	SendFrame[14] = 0x00;//'서브 커맨드_H
	//'읽기 주소 설정
	SendFrame[15] = (byte)(StartAddr % 256);//'선두 디바이스_L
	SendFrame[16] = (byte)(StartAddr / 256);//'선두 디바이스_H
	SendFrame[17] = 0x00;//'선두 디바이스
	SendFrame[18] = (byte)Area::D;
	SendFrame[19] = (byte)(WordCnt % 256);//'데이터 개수_L
	SendFrame[20] = (byte)(WordCnt / 256);//'데이터 개수_H
	int offset = 21;
	for (int i = 0; i < nData;i++) {
		SendFrame[offset + i * 2] = (byte)LOBYTE(pData[i]);
		SendFrame[offset + i * 2 + 1] = (byte)HIBYTE(pData[i]);
	}
	int nError;
	int n = offset + 2* nData;
	g_csPlcWrite.Lock(); //INHO 2009.07.23 
	get_data = FALSE;

	if (!EthernetType) {
		SOCKADDR_IN SendAddress;
		// Socket 생성
		SendAddress.sin_family = AF_INET;
		SendAddress.sin_addr.s_addr = inet_addr(PLC_IP);
		SendAddress.sin_port = htons(PLC_PORT);

		nError = sendto(ClientSocket, SendFrame, n, MSG_DONTROUTE, (const struct sockaddr *)&SendAddress, sizeof(SendAddress));
	}
	else
		nError = send(ClientSocket, SendFrame, n, MSG_DONTROUTE);

	//	TRACE("message sent length is %d\n", nError);
	if (nError == -1)
	{
		if (iErrCnt > 30)
			m_bConnected = FALSE;
		iErrCnt++;
		nError = WSAGetLastError();
		TRACE("WSAGetLastError is %d\n", nError);
		g_csPlcWrite.Unlock();
		return FALSE;
	}
	iErrCnt = 0;
	g_csPlcWrite.Unlock(); 

	int iwait = 0;
	do {
		Sleep(10);
		if (get_data == TRUE) break;
	} while (iwait++ < 100);
	return TRUE;
}

UINT CMelsecSocket::Thread_Ethernet(LPVOID lpVoid)
{
	CMelsecSocket* pMe = (CMelsecSocket*)lpVoid;
	return pMe->Thread_EthernetBody();
}

int CMelsecSocket::Thread_EthernetBody()
{
	// Thread 변수
	int j = 0;
	int k = 0;
	int read_size;
	char buffer[256];
	int size;
	memset(buffer, (char)NULL, 256);

	// Winsock Start up
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)	// header만 include 해서는 안된다.
													// Project -> settings -> Link -> Objct/Libraries에 ws2_32.lib 를 추가해야 한다. 
	{
		AfxMessageBox(L"Error to start socket\n");
		WSACleanup();
		return 0;
		//exit(0);
	}

	// Socket 생성
	if (!EthernetType)			//UDP Socket
	{
		ClientSocket = socket(AF_INET, SOCK_DGRAM, 0);
		ServerAddress.sin_family = AF_INET;
		ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		ServerAddress.sin_port = htons(EthernetPort);
	}
	else
	{
		ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
		ServerAddress.sin_family = AF_INET;
		ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		ServerAddress.sin_port = htons(EthernetPort);
	}
	closesocket(ClientSocket);

	// Socket  다시 생성(초기 소켓이상시 접속안됨)
	if (EthernetType)
	{
		ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
		ServerAddress.sin_family = AF_INET;
		ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		ServerAddress.sin_port = htons(EthernetPort);
	}

	if (ClientSocket == INVALID_SOCKET)
	{
		AfxMessageBox(L"Socket Error\n");
		closesocket(ClientSocket);
		TRACE("closesocket \n");
		WSACleanup();
		return 0;
		//exit(0);
	}

	// Bind 실시
	BOOL bReUseAddr = TRUE;

	//---------------------TCP/IP OPEN  CONNECTION -------------------------
	Qj71e71.sin_family = AF_INET;
	Qj71e71.sin_addr.s_addr = inet_addr(EthernetIP);
	Qj71e71.sin_port = htons(EthernetPort);

	//-----------------------------------------------------------------------
	// 소켓 이벤트 생성
	WSANETWORKEVENTS event;
	WSAEVENT hRecvEvent = WSACreateEvent();
	memset(&event, 0, sizeof(event));
	read_size = WSAEventSelect(ClientSocket, hRecvEvent, FD_CONNECT | FD_ACCEPT | FD_READ | FD_CLOSE | FD_OOB);
	if (read_size != 0)
	{
		AfxMessageBox(L"FD_ACCEPT Failure!!!");
		return 0;
	}

	if (connect(ClientSocket, (LPSOCKADDR)&Qj71e71, sizeof(Qj71e71)) != 0)
	{
		int err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK)
		{
			AfxMessageBox(L"OPEN ERROR", MB_OK, NULL);
			TRACE("open error\n");
		}
		TRACE(" connect error\n");
	}
	m_bConnected = TRUE;
	size = sizeof(ServerAddress);

	while (thread_flag)
	{
		DWORD dw = WSAWaitForMultipleEvents(1, &hRecvEvent, FALSE, 100, FALSE);	// 100 msec

		switch (dw)
		{
		case WSA_WAIT_TIMEOUT:
			break;

		default:
			WSAEnumNetworkEvents(ClientSocket, hRecvEvent, &event);

			if (event.lNetworkEvents & FD_CONNECT)
				//TRACE("PLC network is connected\n");
				if (event.iErrorCode[4] == 0)
				{
					TRACE("PLC network is connected\n");
					_tracer(TRACE_NORMAL, 9999, L"PLC Connected(IP : %s, port : %d)", CString(EthernetIP), EthernetPort);
				}
				else
				{
					_tracer(TRACE_NORMAL, 9999, L"PLC Connect Error(IP : %s, port : %d)", CString(EthernetIP), EthernetPort);
					//AfxMessageBox(L"PLC : Connect Error", MB_OK | MB_TOPMOST);
				}

			if (event.lNetworkEvents & FD_ACCEPT)
				TRACE("FD_ACCEPT\n");

			if (event.lNetworkEvents & FD_READ)
			{
				//				read_size = recvfrom(ClientSocket, buffer, sizeof(buffer), 0, (sockaddr*)&ServerAddress, &size);
				//				read_size = recv(ClientSocket, buffer, sizeof(buffer), MSG_PEEK);
				if (!EthernetType)	read_size = recvfrom(ClientSocket, buffer, sizeof(buffer), 0, (sockaddr*)&ServerAddress, &size);
				else				read_size = recv(ClientSocket, buffer, sizeof(buffer), 0);

				//				TRACE("FD_READ %s\n", buffer);//////must

				if (read_size != SOCKET_ERROR)
				{
					//Process(buffer,pEtnPLC->m_pParent->m_hWnd);//INHO 2009.06.08 
					Process(buffer, read_size);

					memset(buffer, (char)NULL, 256);
				}
			}

			if (event.lNetworkEvents & FD_CLOSE)
			{
				TRACE("FD_CLOSE\n");

				thread_flag = FALSE;
				closesocket(ClientSocket);
				TRACE("closesocket \n");
				WSACloseEvent(hRecvEvent);
				TRACE("WSACloseEvent \n");
			}

			break;
		}
	}

	exit_flag = 0;


	if (WSACleanup())
		TRACE("WSACleanup failed\n");
	else
		TRACE("WSACleanup OK \n");
	m_bConnected = FALSE;
	return 0;
}

void CMelsecSocket::SetCWnd(CWnd* pParent)
{
	m_pParent = pParent;
}

BOOL CMelsecSocket::OpenPLCSocket(int Type, int Port, CStringA IP)
{
	EthernetType = Type;
	EthernetPort = Port;
	EthernetIP = IP;
	AfxBeginThread(Thread_Ethernet, this/*&AddressSize*/);
	return TRUE;
}

short CMelsecSocket::SendEthernetCommandRead(CStringA cmd, CStringA* reply)
{
	get_data = FALSE;
	char thread_message[255];

	memset(thread_message, (char)NULL, 255);


	sprintf_s(thread_message, "%s", cmd.GetBuffer());

	SendEthernet(thread_message);
	int iwait = 0;
	do {
		Sleep(1);
		if (get_data == TRUE) break;
	} while (iwait++ < 100);
	//*reply = getSdataval;

	if (!get_data)
		AfxMessageBox(L"SendEthernetCommandRead Fail!!");

	return TRUE;
}

short CMelsecSocket::SendEthernetCommandWrite(CStringA cmd)
{
	g_csPlcWrite.Lock(); //INHO 2009.07.23 

	char thread_message[255];

	memset(thread_message, (char)NULL, 255);

	sprintf_s(thread_message, "%s", cmd.GetBuffer());

	SendEthernet(thread_message);
	return TRUE;

	g_csPlcWrite.Unlock(); //INHO 2009.07.23 
}

BOOL CMelsecSocket::ClosePLCSocket()
{
	closesocket(ClientSocket);
	_tracer(TRACE_NORMAL, 9999, L"PLC Disconnected(IP : %s, port : %d)", CString(EthernetIP), EthernetPort);

	return TRUE;
}

void CMelsecSocket::Process(char *data, int nData)
{
	char iData[256] = "";
	char temp[256] = "";
	char templen[2] = "";
	int  len = nData;

	memcpy(iData, data, nData);

	if (BYTE(iData[0]) == 0xD0
		&& BYTE(iData[1]) == 0x00 
		&& BYTE(iData[2]) == 0x00 
		&& BYTE(iData[3]) == 0xFF)
	{
		memset(getWdata, 0, sizeof(WORD)*MAX_DATA_BUFF);
		int nRcv = iData[7];
		int is = 11;
		CStringA strRcv, strTmp;
		for (int i = 0; i < nData; i++) {
			strTmp.Format("%x ", (BYTE)iData[i]);
			strRcv += strTmp;
		}
		//TRACE("RCV:%s\n", strRcv);
		strRcv = "";

		if(iData[7] == 2){// send ok
			get_data = TRUE;
		}
		for (int i = 0; i < MAX_DATA_BUFF; i++) {
			templen[0] = iData[is + i * 2];
			templen[1] = iData[is + i * 2 + 1];
			getWdata[i] = MAKEWORD(templen[0], templen[1]);
			strTmp.Format("%02d", getWdata[i]);
			strRcv += strTmp;
		}
		//TRACE("DATA:%s\n", strRcv);
		get_data = TRUE;
	}
}

char CMelsecSocket::ctox(char c)
{
	if (c >= '0' && c <= '9') {
		return (c - '0');
	}
	else {
		return (toupper(c) - 'A' + 0x0A);
	}
}

int CMelsecSocket::atox(char *str)
{
	int tmp = 0, i;
	BYTE	nible;

	for (i = 0;; i++) {
		if (*str == NULL) break;
		nible = (BYTE)ctox(*str++);
		tmp <<= 4;
		tmp += nible;
	}
	return tmp;
}

void CMelsecSocket::SendEthernet(char* send_message)
{
	char thread_message[255];
	char thread_read_header[50];

	// Socket 구조체
	SOCKADDR_IN SendAddress;

	memset(thread_message, (char)NULL, 255);
	memset(thread_read_header, (char)NULL, 50);

	// Socket 생성
	SendAddress.sin_family = AF_INET;
	SendAddress.sin_addr.s_addr = inet_addr(PLC_IP);
	SendAddress.sin_port = htons(PLC_PORT);
	sprintf_s(thread_message, "%s", send_message);

	//	TRACE("message send : %s\n", thread_message);
	int nError;
	if (!EthernetType)	nError = sendto(ClientSocket, thread_message, (int)strlen(thread_message), MSG_DONTROUTE, (const struct sockaddr *)&SendAddress, sizeof(SendAddress));
	else				nError = send(ClientSocket, thread_message, (int)strlen(thread_message), MSG_DONTROUTE);

	//	TRACE("message sent length is %d\n", nError);
	if (nError == -1)
	{
		nError = WSAGetLastError();
		TRACE("WSAGetLastError is %d\n", nError);

	}
}

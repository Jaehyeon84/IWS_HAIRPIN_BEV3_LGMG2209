#include "stdafx.h"
#include "CEMiniSerial.h"



CEMiniSerial::CEMiniSerial()
{
	m_bOnOff = false;
}

CEMiniSerial::~CEMiniSerial()
{
	if (IsOpenPort()) {
		SetOnOff(FALSE);
		Close();
	}
}

bool CEMiniSerial::ConnectEMini(int nPort)
{
	if (IsOpenPort()) {
		Close();
	}
	int iret = Open(nPort, EMINI_BAUD, COMPORT_PARITY_NONE, EMINI_DATABIT, EMINI_STOPBIT, FALSE);
	if (iret == 0) {
		m_iPortNumber = nPort;
		return true;
	}

	return false;
}

void CEMiniSerial::SetLight(int nValue)
{
	CStringA strSentence;
	strSentence.Format("# %d %d\r", 0, nValue);
	this->Send(strSentence.GetBuffer(), strSentence.GetLength());

	strSentence.Format("# %d %d\r", 1, nValue);
	this->Send(strSentence.GetBuffer(), strSentence.GetLength());
}

void CEMiniSerial::SetOnOff(BOOL bOn)
{
	CStringA strSentence;

	if (bOn)
		strSentence = "setonex 0000000F\r";
	else
		strSentence = "setonex 00000000\r";

// 	char SendBuf[MAX_PATH] = { 0, };
// 
// 	strcpy_s(SendBuf, strSentence);
// 	SendPort(SendBuf);
	this->Send(strSentence.GetBuffer(), strSentence.GetLength());
	m_bOnOff = bOn;
}


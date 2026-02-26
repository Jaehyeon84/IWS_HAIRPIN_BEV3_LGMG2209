#pragma once
#include "CHubiSerial.h"

#define EMINI_STX	0x02
#define EMINI_ETX  0x03
#define EMINI_BLANK 0x20
#define EMINI_BAUD  38400
#define EMINI_DATABIT 8  
#define EMINI_STOPBIT ONESTOPBIT
class CEMiniSerial :public CHubiSerial
{
public:
	CEMiniSerial();   // ǥ�� �������Դϴ�.
	~CEMiniSerial();

public:
	bool m_bOnOff;

public:
	bool ConnectEMini(int nPort);
	void SetLight(int nValue);
	void SetOnOff(BOOL bOn);

};


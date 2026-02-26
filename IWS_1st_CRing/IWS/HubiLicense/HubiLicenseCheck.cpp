#include "stdafx.h"
#include "HubiLicenseCheck.h"
#include "Blowfish.h"
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

#define DEC_OPT1 1
#define DEC_OPT2 0
#define DEC_OPT3 0
#define DEC_OPT4 0
#define DEC_OPT5 1

#define DEC_LICENSE ((DEC_OPT1 << 16) + (DEC_OPT2 << 12) + (DEC_OPT3 << 8) + (DEC_OPT4 << 4) + (DEC_OPT5))
#define MAX_LANCARD 10
CHubiLicenseCheck::CHubiLicenseCheck()
{
}


CHubiLicenseCheck::~CHubiLicenseCheck()
{
}

// Optimized Function to convert an unsigned char to a Hex string of length 2
void CHubiLicenseCheck::Char2Hex(unsigned char ch, char* szHex)
{
	static unsigned char saucHex[] = "0123456789ABCDEF";
	szHex[0] = saucHex[ch >> 4];
	szHex[1] = saucHex[ch & 0xF];
	szHex[2] = 0;
}

//Function to convert a Hex string of length 2 to an unsigned char
bool CHubiLicenseCheck::Hex2Char(char const* szHex, unsigned char& rch)
{
	if (*szHex >= '0' && *szHex <= '9') rch = (unsigned char)(*szHex - '0');
	else if (*szHex >= 'A' && *szHex <= 'F') rch = (unsigned char)(*szHex - 55); //-'A' + 10
	else	return false;

	szHex++;
	rch <<= 4;
	if (*szHex >= '0' && *szHex <= '9') rch = (unsigned char)(rch + (*szHex - '0'));
	else if (*szHex >= 'A' && *szHex <= 'F') rch = (unsigned char)(rch + (*szHex - 55)); //-'A' + 10;
	else	return false;

	return true;
}

//Function to convert binary string to hex string
void CHubiLicenseCheck::Binary2Hex(unsigned char const* pucBinStr, int iBinSize, char* pszHexStr)
{
	int i;
	char szHex[3];
	unsigned char const* pucBinStr1 = pucBinStr;
	*pszHexStr = 0;
	CStringA strHex;
	for (i = 0; i < iBinSize; i++, pucBinStr1++)
	{
		Char2Hex(*pucBinStr1, szHex);
		strHex += szHex;
	}
	memcpy(pszHexStr, strHex, sizeof(char)* strHex.GetLength());
}

//Function to convert hex string to binary string
bool CHubiLicenseCheck::Hex2Binary(char const* pszHexStr, unsigned char* pucBinStr, int iBinSize)
{
	int i;
	unsigned char ch;
	for (i = 0; i < iBinSize; i++, pszHexStr += 2, pucBinStr++)
	{
		if (false == Hex2Char(pszHexStr, ch))
			return false;
		*pucBinStr = ch;
	}
	return true;
}

BOOL CHubiLicenseCheck::GetMACAddr(BYTE *mac, int &nCard)
{
	BYTE		data[4096];
	ZeroMemory(data, 4096);

	unsigned long		len = sizeof(data);
	PIP_ADAPTER_INFO	pinfo = (PIP_ADAPTER_INFO)data;

	unsigned long nError = GetAdaptersInfo(pinfo, &len);
	switch (nError)
	{
	case 0:						break;
	case ERROR_NO_DATA:			break;
	case ERROR_NOT_SUPPORTED:	break;

	case ERROR_BUFFER_OVERFLOW:
		nError = GetAdaptersInfo(pinfo, &len);
		break;
	}

	if (nError == 0)
	{
		nCard = 1;
		memcpy(mac, pinfo->Address, sizeof(BYTE)*MAX_ADAPTER_ADDRESS_LENGTH);
		PIP_ADAPTER_INFO p = pinfo->Next;
		while (p) {
			memcpy(mac+nCard* MAX_ADAPTER_ADDRESS_LENGTH, p->Address, sizeof(BYTE)*MAX_ADAPTER_ADDRESS_LENGTH);
			nCard++;
			p = p->Next;
		}
		return TRUE;
	}
	else {
		memset(mac, 0, sizeof(BYTE)*MAX_ADAPTER_ADDRESS_LENGTH);
		return FALSE;
	}
}

CStringA  CHubiLicenseCheck::GenLicense(CString strFilename)
{
	int iMode = IMethod::ECB;
	int iPadding = IMethod::ZEROES;

	CBlowFish *m_pBlowFish = new CBlowFish();
	CStringA strKeyCode;

	char acKey[56];
	strKeyCode = "HUBIS777";
	/*strKeyCode.Format("%02X%02X%02X%02X%02X%02X", mac_add[0], mac_add[1], mac_add[2],
		mac_add[3], mac_add[4], mac_add[5]);*/

	int iLength = strKeyCode.GetLength() / 2;
	Hex2Binary(strKeyCode, (unsigned char*)acKey, iLength);

	// Initialize BlowFish
	m_pBlowFish->Initialize(acKey, iLength, SBlock(0UL, 0UL), iMode, iPadding);
	// Encode
	int iLen, iLen1;
	CStringA strFtnCode;
	strFtnCode.Format("%s%08xAA", strKeyCode, DEC_LICENSE);
	iLen = strFtnCode.GetLength() / 2;

	//Estimate Padding
	int iBlockSize = m_pBlowFish->GetBlockSize();
	if (iBlockSize > -1)
	{
		if (iLen%iBlockSize != 0)
			iLen1 = (iLen / iBlockSize + 1) * iBlockSize;
		else
			iLen1 = iLen;
	}
	else
		iLen1 = iLen;

	//
	//char* pcIn = new char[iLen1];// (char*)(_alloca(iLen1));
	char pcIn[1024] = { "" };
	Hex2Binary(strFtnCode, (unsigned char*)(pcIn), iLen);

	m_pBlowFish->ResetChain();
	if (iBlockSize > -1)
		m_pBlowFish->Pad(pcIn, iLen);

	//
	//char* pcOut = new char[iLen1];// (char*)(_alloca(iLen1));
	char pcOut[1024] = { "" };
	m_pBlowFish->Encrypt(pcIn, pcOut, iLen1);

	//
	iLen = (iLen1 << 1) + 1;
	// char* pcHex = new char[iLen];// (char*)(_alloca(iLen));
	char pcHex[1024] = { "" };
	Binary2Hex((unsigned char*)(pcOut), iLen1, pcHex);
	pcHex[iLen] = 0;

	CStringA strSerial = pcHex;

	if (strFilename.GetLength()) {
		FILE *pf;
		CStringA strFilenameA = CStringA(strFilename);
		fopen_s(&pf, strFilenameA, "w+");
		if (pf) {
			fprintf_s(pf, "%s", pcHex);
			fclose(pf);
		}
	}
	delete m_pBlowFish;
	return strSerial;
}

int CHubiLicenseCheck::CheckLicense(CStringA strLicense)
{
	int iMode = IMethod::ECB;
	int iPadding = IMethod::ZEROES;
	CStringA strKeyCode;

	int ret = 0;
	BYTE mac[MAX_ADAPTER_ADDRESS_LENGTH*MAX_LANCARD];
	int nCard = 0;
	if (!GetMACAddr(mac, nCard)) {
		return -1;
	}

	BOOL bMac = FALSE;
	int iFct = 0;
	for (int icard = 0; icard < nCard; icard++) {
		BYTE mac_add[MAX_ADAPTER_ADDRESS_LENGTH];
		memcpy(mac_add, mac + icard * MAX_ADAPTER_ADDRESS_LENGTH, sizeof(BYTE)*MAX_ADAPTER_ADDRESS_LENGTH);
		char acKey[56];
		strKeyCode.Format("%02X%02X%02X%02X%02X%02X", mac_add[0], mac_add[1], mac_add[2],
			mac_add[3], mac_add[4], mac_add[5]);
		int iLength = strKeyCode.GetLength() / 2;
		Hex2Binary(strKeyCode, (unsigned char*)acKey, iLength);
		CBlowFish *m_pBlowFish = new CBlowFish();
		m_pBlowFish->Initialize(acKey, iLength, SBlock(0UL, 0UL), iMode, iPadding);

		char pcIn[1024] = { "" };
		int iLen = strLicense.GetLength() / 2;
		Hex2Binary(strLicense, (unsigned char*)(pcIn), iLen);
		char pcOut[1024] = { "" };
		m_pBlowFish->ResetChain();
		m_pBlowFish->Decrypt(pcIn, pcOut, iLen);
		bMac = TRUE;
		for (int i = 0; i < 6; i++) {
			if (mac_add[i] != (BYTE)pcOut[i]) {
				bMac = FALSE;
				break;
			}
		}
		delete m_pBlowFish;
		if (bMac) {
			iFct = (pcOut[6] << 31) + (pcOut[7] << 16) + (pcOut[8] << 8) + (pcOut[9]);
			break;
		}
		
	}
	if (!bMac)
		return -2;
	if (iFct != DEC_LICENSE)
		return -3;

	return 1;
}

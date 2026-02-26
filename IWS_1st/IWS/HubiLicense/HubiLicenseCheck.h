#pragma once
class CHubiLicenseCheck
{
public:
	CHubiLicenseCheck();
	~CHubiLicenseCheck();

	int CheckLicense(CStringA strLicense);
	CStringA  GenLicense(CString strFilename);
private:

	BOOL GetMACAddr(BYTE *mac, int &nCard);
	void Char2Hex(unsigned char ch, char* szHex);
	bool Hex2Char(char const* szHex, unsigned char& rch);
	void Binary2Hex(unsigned char const* pucBinStr, int iBinSize, char* pszHexStr);
	bool Hex2Binary(char const* pszHexStr, unsigned char* pucBinStr, int iBinSize);
};

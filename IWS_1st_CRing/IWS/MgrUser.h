#pragma once

enum USER_TYPE { USER_OPERATOR, USER_ADMINISTRATOR, USER_MASTER, USER_TESTER };

class CMgrUser
{
public:
	CMgrUser(void);
	~CMgrUser(void);

protected:
	WORD	m_nUser;
	CString m_strPassword;

public:
	WORD GetUser();
	BOOL ChangePassword(CString new_pass);
	BOOL LogOn(CString password);
	void LogOut();

	//
	void SetPassword(CString sPwd) { m_strPassword = sPwd; };

	void RegisterData(BOOL bFlag=TRUE);
};

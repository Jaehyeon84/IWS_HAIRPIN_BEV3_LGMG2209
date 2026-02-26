#include "StdAfx.h"
#include "IWS.h"
#include "MgrUser.h"

CMgrUser::CMgrUser(void)
{
	m_nUser = USER_OPERATOR;

	//RegisterData(FALSE);
}

CMgrUser::~CMgrUser(void)
{
	//RegisterData(TRUE);
}

WORD CMgrUser::GetUser()
{
	return m_nUser;
}

BOOL CMgrUser::ChangePassword(CString new_pass)
{
	new_pass.MakeUpper();
	m_strPassword = new_pass;
	return TRUE;

	/*cur_pass.MakeUpper();
	if(cur_pass == m_strPassword){
		new_pass.MakeUpper();
		m_strPassword = new_pass;
		RegisterData(TRUE);
		return TRUE;
	}
	else{
		return FALSE;
	}*/
}

BOOL CMgrUser::LogOn(CString password)
{
	BOOL bRes;
	password.MakeUpper();
	if(password == m_strPassword)
	{
		m_nUser = USER_ADMINISTRATOR;
		bRes = TRUE;
	}
	else if(password == _T("HUBIS.CO.KR"))
	{
		m_nUser = USER_MASTER;
		bRes = TRUE;
	}
	else if(password == _T("TEST_MODE"))
	{
		m_nUser = USER_TESTER;
		bRes = TRUE;
	}
	else
	{
		bRes = FALSE;
	}
	return bRes;
}

void CMgrUser::LogOut()
{
	m_nUser = USER_OPERATOR;
}

void CMgrUser::RegisterData(BOOL bFlag)
{
	//CString pname;
	//CIni ini;
	//pname.Format(_T("%s\\configIWS.ini"), g_File.GetPropPath());
	//ini.SetPathName(pname);
	//TCHAR temp[MAX_PATH];
	//if(bFlag){
	//	// save data
	//	ini.WriteString(_T("User"), _T("Password"), m_strPassword);
	//}
	//else{
	//	// load data
	//	ini.GetString(_T("User"), _T("Password"), temp, MAX_PATH, _T(""));	
	//	m_strPassword = temp;
	//}
}
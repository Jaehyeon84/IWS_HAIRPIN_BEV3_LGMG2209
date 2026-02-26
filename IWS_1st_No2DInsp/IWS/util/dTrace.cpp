// dTrace.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"
#include "dTrace.h"
#include <sstream>
#include <process.h>
#include <iostream>
#include <fstream>
#include <iomanip>

//#pragma data_seg("SHAREDATA")
CTraceCtrl* g_TraceCtrl = NULL;
//#pragma data_seg()
//#pragma comment(linker,"/Section:SHAREDATA,rws")


//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


void vTRACE(vTRACE_TYPE vType, int iCode, char *pFunc, int iLine, const TCHAR * format,...)
{
	if(g_TraceCtrl==NULL) return;

    TCHAR *buffer;
	va_list argptr;
    va_start(argptr, format);
    size_t numberOfElements = _vsctprintf(format,argptr)+20;
    buffer = (TCHAR *)malloc(numberOfElements*sizeof(TCHAR));
	memset(buffer, 0x00, numberOfElements*sizeof(TCHAR));
    _vstprintf_s(buffer,numberOfElements,format,argptr);
    va_end(argptr);	

	//g_TraceCtrl.Trace(vType,iCode, pFunc, iLine, buffer);
	g_TraceCtrl->Trace(vType,iCode, pFunc, iLine, buffer);
	free(buffer);

//	va_list args;
//    va_start( args, format );
//	g_TraceCtrl.Trace(vType,iCode, pFunc, iLine, format,args);
//	va_end( args );
}
void vTRACE_INIT(HWND hParentWnd,CString sTraceFilePath,int iFileMethod, CString sLogPrefix, int iDebugLevel)
{
	//g_TraceCtrl.Init(hParentWnd, sTraceFilePath, iFileMethod, sLogPrefix, iDebugLevel);


	if(g_TraceCtrl==NULL)
		g_TraceCtrl = new CTraceCtrl();
	g_TraceCtrl->Init(hParentWnd, sTraceFilePath, iFileMethod, sLogPrefix, iDebugLevel);
}
void vTRACE_Close()
{
	if(g_TraceCtrl!=NULL)
	{
		g_TraceCtrl->Destroy();
		delete g_TraceCtrl;
		g_TraceCtrl = NULL;
	}
	
}

void vTRACE_ShowHandle(CWnd *pWnd)
{
	g_TraceCtrl->m_pMsgWnd = pWnd;
}

using std::setw;
using std::setfill;

int g_LodeTraceCtrlCount = 0;

#define CLASSNAME	_T("TracerView")
#define WINDOWNAME	_T("TracerView")

static UINT	TRACE_Thread_Launcher(LPVOID pParam)
{
	TRACE("//=============================================\n");
	TRACE("// TRACE log Started\n");
	TRACE("//=============================================\n");

	CTraceCtrl* pTraceLog = (CTraceCtrl*)pParam;
	//SetTraceThreadName(::GetCurrentThreadId(), "Trace_Thread");
	SetThreadName(-1, "Trace_Thread");
	BOOL bRet = pTraceLog->Trace_Thread();

	TRACE("//=============================================\n");
	TRACE("// TRACE log end\n");
	TRACE("//=============================================\n");

	return bRet;
}



CTraceCtrl::CTraceCtrl()
{
	if (g_LodeTraceCtrlCount == 0)
	{
		//::GetModuleFileName의 리턴값이.. 가끔 오동작해서 경로 중간에 \..\등으로 나타는 경우가 있음...
		// 그러면 탐색기에서는 문제 없으나 프로그램이 문제를 일으킬 때가 있어서 QueryFullProcessImageName로 바꿉니다...

		DWORD nBuff = 2048;
		TCHAR szBuff[2048] = { 0 };
		HANDLE hCurrP = ::GetCurrentProcess();
		QueryFullProcessImageName(hCurrP, 0, szBuff, &nBuff);
		CloseHandle(hCurrP); //pseudo handle이라 close할 필요는 없으나.. 안하면 나중에 햇갈릴것 같아..ㅠㅠ

		TCHAR pDrv[256] = { 0, };
		TCHAR pDir[256] = { 0, };
		TCHAR pName[256] = { 0, };
		TCHAR pExt[256] = { 0, };

		_tsplitpath_s(szBuff, pDrv, 256, pDir, 256, pName, 256, pExt, 256);
		//m_sTracePath.Format(_T("%s%sLog\\"),pDrv,pDir); //20141128 cdh edit
		m_sTracePath.Empty(); //20141128 cdh add

		m_sTraceBuffer = new deque<TRACE_DATA>;

		m_hMutex = ::CreateMutex(NULL, false, NULL);
		m_bStopThread = FALSE;

		DWORD dwThreadID;
		m_hThread = NULL;
		m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TRACE_Thread_Launcher, (LPDWORD)this, 0, &dwThreadID);
		if (m_hThread == NULL)
		{
			AfxMessageBox(_T("Cannot execute Log-threa.."));
		}
		m_bInit = FALSE;
		m_hParentWnd = NULL;

		m_iFileMethode = 0;
		m_iDebugLevel = 0;
		m_sPrefix = _T("");
		m_bDestroyTraceStart = FALSE;
	}
	m_pMsgWnd = NULL;
	g_LodeTraceCtrlCount++;
}
CTraceCtrl::~CTraceCtrl()
{
	g_LodeTraceCtrlCount--;

	if (g_LodeTraceCtrlCount == 0)
	{
		delete m_sTraceBuffer;
	}
}
void CTraceCtrl::Destroy()
{
	m_bDestroyTraceStart = TRUE;
	if (m_hThread != NULL)
	{
		m_bStopThread = TRUE;
		DWORD dwResult = WaitForSingleObject(m_hThread, 6000);
		if (dwResult != WAIT_OBJECT_0)
		{
			TerminateThread(m_hThread, 0);
		}
		if (m_hThread != NULL) { CloseHandle(m_hThread); m_hThread = NULL; }
	}
	m_bStopThread = FALSE;
	Trace_Thread_Write();
	lock();
	if (m_sTraceBuffer != NULL)
	{
		m_sTraceBuffer->clear();
		delete m_sTraceBuffer;
		m_sTraceBuffer = NULL;
	}
	unlock();
	if (m_hMutex != NULL) { ::CloseHandle(m_hMutex); m_hMutex = NULL; }
}
bool CTraceCtrl::lock()
{
	if (m_hMutex == NULL) return false;

	WaitForSingleObject(m_hMutex, INFINITE);
	return TRUE;
}
void CTraceCtrl::unlock()
{
	ReleaseMutex(m_hMutex);
}

void  CTraceCtrl::BufferIN(TRACE_DATA sLog)
{
	if (m_sTraceBuffer == NULL) return;
	if (!lock()) return;
	if (m_sTraceBuffer == NULL) { unlock(); return; }
	m_sTraceBuffer->push_back(sLog);
	unlock();
}
TRACE_DATA CTraceCtrl::BufferOut()
{
	TRACE_DATA sLog;
	if (!lock())
	{
		sLog.Type = TRACE_ERROR;
		return sLog;
	}

	sLog = m_sTraceBuffer->front();
	m_sTraceBuffer->pop_front();
	unlock();
	return sLog;
}
BOOL CTraceCtrl::BufferEmpty()
{
	if (!lock()) return TRUE;
	BOOL bEmpty = m_sTraceBuffer->empty();
	unlock();
	return bEmpty;
}
void CTraceCtrl::Init(HWND pParent, CString sPath, int iMethod, CString sLogPrefix, int iDebugLevel)
{
	m_hParentWnd = pParent;
	m_sTracePath = sPath;
	m_iFileMethode = iMethod;
	m_iDebugLevel = iDebugLevel;
	m_bInit = TRUE;
	m_sPrefix = sLogPrefix;
	TRACE(_T("TRACE init : %s,%d,%d\n"), sPath, iMethod, iDebugLevel);
}

void CTraceCtrl::Trace(vTRACE_TYPE vType, int iCode, char* pFunc, int iLine, const TCHAR* pFormat, ...)
{
	if (vType < m_iDebugLevel) return;

	if (!m_bInit)
	{
		if (m_sTraceBuffer->size() > 200) return;
	}
	CString sFuncInfo;
	if (m_iDebugLevel == 0)
	{
		USES_CONVERSION;
		CString sFuncName = A2W(pFunc);
		sFuncInfo.Format(_T("%s(%d)>> "), sFuncName, iLine);
	}

	SYSTEMTIME curTime;
	GetLocalTime(&curTime);

	tostringstream ostr;
	va_list argptr;
	TCHAR *buffer;
	va_start(argptr, pFormat);
	size_t numberOfElements = _vsctprintf(pFormat, argptr) + 20;
	buffer = (TCHAR *)malloc((numberOfElements * sizeof(TCHAR)));
	memset(buffer, 0x00, (numberOfElements * sizeof(TCHAR)));
	_vstprintf_s(buffer, numberOfElements, pFormat, argptr);
	va_end(argptr);

	if (m_sPrefix.GetLength() > 0)
	{
		ostr << _T("[");
		ostr << m_sPrefix.GetBuffer(m_sPrefix.GetLength());
		ostr << _T("] ");
	}

	if (m_iDebugLevel == 0)
	{
		ostr << sFuncInfo.GetBuffer(sFuncInfo.GetLength());
	}

	ostr << buffer << std::endl;
	free(buffer);

	TRACE_DATA sLog;
	sLog.iCode = iCode;
	sLog.Type = vType;
	sLog.Time = curTime;
	sLog.sMsg = ostr.str();

	BufferIN(sLog);
	//TRACE(_T("Add Buffer(%d) : %s"),m_sTraceBuffer->size(),ostr.str().c_str());
}
CString CTraceCtrl::MakeLogFileName(SYSTEMTIME *Time)
{
	if (m_iFileMethode == 1)
	{
		return m_sTracePath;
	}

	CString sLogFile;
	SYSTEMTIME curTime;
	if (Time == NULL)
		GetLocalTime(&curTime);
	else
		curTime = *Time;

	// BSI 2025.02.24 로그파일명 변경(월.일 -> 년.원.일)
	sLogFile.Format(_T("TRACE_%04d%02d%02d.txt"), curTime.wYear, curTime.wMonth, curTime.wDay);
	return (m_sTracePath + sLogFile);
}
int	CTraceCtrl::Trace_Thread_Write()
{
	TRACE_DATA sLog;
	CString sFileName;
	while (!BufferEmpty() && !m_bStopThread)
	{
		if (m_sTracePath.IsEmpty())
		{
			if (m_bDestroyTraceStart) return 0;
			Sleep(10);
			continue;
		}
		sLog = BufferOut();

		if (sLog.Type == TRACE_ERROR)
		{
			TRACE_PARAM pErrData;
			pErrData.iCode = sLog.iCode;
			pErrData.Type = sLog.Type;
			pErrData.Time = sLog.Time;
			pErrData.sMsg = sLog.sMsg.c_str();
			::SendMessage(m_hParentWnd, UWM_TRACE_MSG_ERROR, 0, (LPARAM)&pErrData);
		}

		CString sFileName = MakeLogFileName(&(sLog.Time));
		tostringstream ostr;

		switch (sLog.Type)
		{
			case TRACE_DEBUG:   ostr << _T("[D:"); break;
			case TRACE_NORMAL:  ostr << _T("[N:"); break;
			case TRACE_WARNING: ostr << _T("[W:"); break;
			case TRACE_ERROR:   ostr << _T("[E:"); break;
		}

		ostr << sLog.Time.wYear
			<< setw(2) << setfill(_T('0')) << sLog.Time.wMonth
			<< setw(2) << setfill(_T('0')) << sLog.Time.wDay << _T(" ")
			<< setw(2) << setfill(_T('0')) << sLog.Time.wHour << _T(":")
			<< setw(2) << setfill(_T('0')) << sLog.Time.wMinute << _T(":")
			<< setw(2) << setfill(_T('0')) << sLog.Time.wSecond << _T(".")
			<< setw(3) << setfill(_T('0')) << sLog.Time.wMilliseconds << _T("] ");
		ostr << _T("[") << setw(5) << setfill(_T('0')) << sLog.iCode << _T("] ");
		ostr << sLog.sMsg;

		HWND hTraceWnd = ::FindWindow(CLASSNAME, WINDOWNAME);
		if (hTraceWnd)
		{
			TCHAR pBuf[1024] = { 0, };
			int nLng = _stprintf_s(pBuf, 1022, _T("%s"), ostr.str().c_str());
			//	pBuf[nLng]='\0';
			//	TRACE(_T("%s"),pBuf);
			COPYDATASTRUCT cds;
			cds.dwData = sLog.Type;
			cds.cbData = (nLng + 1) * sizeof(TCHAR);
			cds.lpData = pBuf;
			SendMessage(hTraceWnd, WM_COPYDATA, 0, (LPARAM)&cds);
		}

		if (m_iFileMethode == 1)
		{
			wchar_t pLog[1024] = { 0 };
			_stprintf_s(pLog, 1022, _T("%s"), ostr.str().c_str());
			size_t szFileSizeMax = 1024 * 1024 * 2;
			CFile pLogFile;
			CFileException e;
			if (pLogFile.Open(sFileName, CFile::modeReadWrite | CFile::modeNoTruncate | CFile::modeCreate, &e))
			{

				size_t nLogCharCount = wcslen(pLog);
				size_t nLogSize = nLogCharCount * sizeof(wchar_t);
				size_t nFileSize = (size_t)pLogFile.GetLength();
				size_t nFileCharCount = nFileSize / sizeof(wchar_t);

				size_t nMemSize = nLogSize + nFileSize;

				USHORT ByteOrderMark = 0xfeff;
				wchar_t wcLineEnd[3];
				wcLineEnd[0] = L'\r';
				wcLineEnd[1] = L'\n';
				wcLineEnd[2] = L'\0';

				if (nMemSize > szFileSizeMax)
				{
					wchar_t* pCurData = (wchar_t*)malloc(nMemSize + (3 * sizeof(wchar_t)));
					pLogFile.Read(pCurData, (int)nFileSize);
					swprintf_s(pCurData + nFileCharCount, nLogCharCount + 3, L"%s%s", pLog, wcLineEnd);

					int iStartPos = 0;
					for (int i = 0; i < nFileCharCount; i++)
					{
						if (pCurData[i] == L'\n' && (nMemSize - (i * sizeof(wchar_t)) < szFileSizeMax))
						{
							iStartPos = (i + 1);
							break;
						}
					}
					pLogFile.SeekToBegin();
					pLogFile.Write(&ByteOrderMark, 2);
					int nWriteDataSize = (int)nMemSize + (2 * sizeof(wchar_t)) - (iStartPos * sizeof(wchar_t));
					pLogFile.Write(pCurData + iStartPos, ((UINT)nMemSize + (2 * sizeof(wchar_t)) - (iStartPos * sizeof(wchar_t))));
					pLogFile.SetLength(nWriteDataSize + 2);

					free(pCurData);
				}
				else
				{
					if (nFileSize == 0)
					{
						pLogFile.Write(&ByteOrderMark, 2);
					}
					pLogFile.SeekToEnd();
					pLogFile.Write(pLog, (int)nLogSize);
					pLogFile.Write(wcLineEnd, 2 * sizeof(wchar_t));

				}

				pLogFile.Close();



			}
			else
			{
				TRACE("Cannot Open LogFile[%s]\n", sFileName);
			}
		}
		else
		{

			tofstream fout(sFileName.GetBuffer(sFileName.GetLength() + 1),
				std::ios::out | std::ios::app);
			if (fout.is_open())
			{
#ifdef _UNICODE 
				fout.imbue(std::locale(_LOG_LOCAL_));
#endif

				fout << ostr.str();
				fout.close();
				//TRACE(_T("Trace_Thread_Write (%d) :: %s"),m_sTraceBuffer->size(), ostr.str().c_str());

			}
			else
			{
				TRACE("Cannot Open LogFile[%s]\n", sFileName);
			}
		}

		if (m_pMsgWnd->GetSafeHwnd()) {
			SendMessage(m_pMsgWnd->GetSafeHwnd(), UWM_TRACE_MSG, 0, (LPARAM)&sLog);
		}
	}

	return 0;
}
int	CTraceCtrl::Trace_Thread()
{
	while (!m_bStopThread)
	{
		Trace_Thread_Write();
		Sleep(10);
	}
	return 0;
}
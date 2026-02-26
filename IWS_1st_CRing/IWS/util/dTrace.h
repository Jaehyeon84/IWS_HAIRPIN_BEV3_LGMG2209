#pragma once

typedef enum enum_vTRACE_TYPE
{
	TRACE_DEBUG = 0,
	TRACE_NORMAL,
	TRACE_WARNING,
	TRACE_ERROR
}vTRACE_TYPE;

typedef struct struct_TRACE_PARAM
{
	SYSTEMTIME Time;
	vTRACE_TYPE Type;
	CString  sMsg;
	int		iCode;
} TRACE_PARAM;

#define UWM_TRACE_MSG_ERROR WM_USER+3000
#define UWM_TRACE_MSG WM_USER+3001
void vTRACE(vTRACE_TYPE vType, int iCode, char *pFunc, int iLine, const TCHAR * format,...);
void vTRACE_INIT(HWND hParentWnd,CString sTraceFilePath,int iFileMethod, CString sLogPrefix, int iDebugLevel);
void vTRACE_Close();
void vTRACE_ShowHandle(CWnd *pWnd);
#define _tracer(vType, iCode, pFormat, ...)  vTRACE(vType, iCode,__FUNCTION__,__LINE__,pFormat,__VA_ARGS__)

/*

void AFX_EXT_CLASS CustomTrace(const TCHAR* format, ...);
void AFX_EXT_CLASS CustomTrace(const char* format, ...);

#ifdef _DEBUG
	#ifdef ATLTRACE 
		#undef ATLTRACE
		#undef ATLTRACE2

		#define ATLTRACE CustomTrace
		#define ATLTRACE2 ATLTRACE
	#endif // ATLTRACE
#endif // _DEBUG
*/


#include <deque>
#include <string>

using std::deque;
using std::wstring;

#ifdef _UNICODE 
typedef std::wstring		tstring;
typedef std::wostringstream tostringstream;
typedef std::wofstream		tofstream;
#define _LOG_LOCAL_			"korean"
#else
typedef std::string			tstring;
typedef std::ostringstream	tostringstream;
typedef std::ofstream		tofstream;
#endif 

typedef struct struct_TRACE_DATA
{
	SYSTEMTIME Time;
	vTRACE_TYPE Type;
	tstring  sMsg;
	int		iCode;
} TRACE_DATA;

class CTraceCtrl
{
public:
	CTraceCtrl();
	virtual ~CTraceCtrl();

	void		Init(HWND pParent, CString sPath, int iMethod, CString sLogPrefix, int iDebugLevel);
	void		Destroy();
	void		Trace(vTRACE_TYPE vType, int iCode, char* pFunc, int iLine, const TCHAR* pFormat, ...);
	HWND		m_hParentWnd;
	CString		m_sTracePath;
	int			m_iFileMethode;
	int			m_iDebugLevel;
	BOOL		m_bInit;
	CString		MakeLogFileName(SYSTEMTIME *Time = NULL);

	deque<TRACE_DATA> *m_sTraceBuffer;
	CWnd		*m_pMsgWnd;
	void		BufferIN(TRACE_DATA sLog);
	TRACE_DATA	BufferOut();
	BOOL		BufferEmpty();
	void		unlock();
	bool		lock();
	HANDLE		m_hMutex;

	int			Trace_Thread();
	int			Trace_Thread_Write();
	BOOL volatile m_bStopThread;
	HANDLE		m_hThread;
	BOOL volatile m_bDestroyTraceStart;
	CString		m_sPrefix;
};


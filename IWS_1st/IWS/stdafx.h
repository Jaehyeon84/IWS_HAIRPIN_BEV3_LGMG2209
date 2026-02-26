// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해
// 추가적으로 제공되는 내용입니다.
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.


#include <afxdisp.h>        // MFC 자동화 클래스입니다.



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원
const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
#include "./util/Color.h"
void SetThreadName(DWORD dwThreadID, char* threadName);

#include "./API/litecad.h"
#ifdef _WIN64
#pragma comment (lib, "./API/x64/litecad64.lib")
#pragma comment (lib, "./ChartDirect/x64/chartdir60.lib")
#else
#pragma comment (lib, "./API/litecad.lib")    // only for C/C++ project (see file "LiteCAD.c")
#pragma comment (lib, "./ChartDirect/x86/chartdir60.lib")
#endif


#include "IWS_define.h"
#include "timeapi.h"
#pragma comment (lib, "Winmm.lib")

#include "./util/dTrace.h"


#include <afxcontrolbars.h>

#include "Ini.h"
#include <afxcontrolbars.h>
//#include <afxcontrolbars.h>
//#include <afxcontrolbars.h>
//#include <afxcontrolbars.h>
#include <afxwin.h>
//#include <afxcontrolbars.h>
//#include <afxcontrolbars.h>
//#include <afxcontrolbars.h>
//#include <afxcontrolbars.h>
//#include <afxcontrolbars.h>
//#include <afxcontrolbars.h>
#include <afxwin.h>
#include "alglib/src/optimization.h"	// 2022-07-07 Matrix 관련 코드 추가(MOB 참고)--->
#include <afxcontrolbars.h>				// 2022-07-07 Matrix 관련 코드 추가(MOB 참고)--->
#include <afxcontrolbars.h>
//#include <afxcontrolbars.h>
//#include <afxcontrolbars.h>

//#include "alglib/src/optimization.h"
//#include <afxcontrolbars.h>
//#include <afxcontrolbars.h>

#pragma comment(lib, "./VRSaux/NIVision/Lib/MSVC64/nivision.lib")

#pragma warning (disable:4819)
#pragma comment(lib, "./UltimateGridLib/DLLs/UGMFCU.lib")
//#define _CD_NAMESPACE




#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


 // ehjang 2D Insp. 추가
#define IsSHIFTPressed() ((GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT) * 8 - 1))) != 0)
#define IsCTRLPressed()  ((GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT) * 8 - 1))) != 0)

typedef struct SResult
{
	long nErrorCode;
	float fC;
	float fT;
	float fJ;

	void Init()
	{
		nErrorCode = 1;
		fC = 0.f;
		fT = 0.f;
		fJ = 0.f;
	}
}SResult;

enum
{
	INSP_ERR_OK = 1,	//OK
	INSP_ERR = 0,
	INSP_ERR_F1 = -1,	// Not found HP1
	INSP_ERR_F2 = -2,	// Not found HP2
	INSP_ERR_F12 = -3,	// Not found HP1&HP2 => -3 = INSP_ERR_F1 + INSP_ERR_F2 = -1 -2 
	INSP_ERR_H1 = -4,	// Inner hairpin spec. NG
	INSP_ERR_H2 = -5,	// Outer hairpin spec. NG
	INSP_ERR_H3 = -6,	// Inner-Outer step-height spec. NG
	INSP_ERR_C = -7,	// Inner-Outer distance spec.NG
	INSP_ERR_T = -8,	//
	INSP_ERR_J = -9,	// [curruent layer-pair Outer]  -- [next layer pair Inner] distance spec.NG
	INSP_ERR_A1 = -10,	//
	INSP_ERR_A2 = -11	//
};
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

// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// IWS.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"


void SetThreadName(DWORD dwThreadID, char* threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;//threadName.GetBuffer(threadName.GetLength()+1);
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		TRACE("Error Set thread name(%s)!!!!!!!!!!!!!!!!!!!!!!!!!! \n", threadName);
	}
#pragma warning(pop)
}

TCHAR g_dscLaserName[4][64] = {	
	{L"YLR2000SM"},
	{L"YLR6000QCW_CW"},
	{L"YLR6000QCW_PULSE"}, 
	{L"TRUMPF_TRUDISK"}
};

COLORREF g_clrPen[16] = {
	RGB(107,142,35),
	COLOR_OLIVEDRAB1,
	COLOR_BLUE1,
	COLOR_ORANGERED,
	COLOR_DARKGREEN,
	COLOR_SLATEBLUE,
	COLOR_FIREBRICK,
	COLOR_YELLOW,
	COLOR_MEDIUMVIOLETRED,
	COLOR_MAGENTA,
	COLOR_BROWN,
	COLOR_DEEPPINK,
	COLOR_SADDLEBROWN,
	COLOR_YELLOWGREEN,
	COLOR_INDIANRED,
	COLOR_GREY
};

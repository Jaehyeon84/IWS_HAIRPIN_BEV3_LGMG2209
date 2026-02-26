/*! \file SplashWnd.cpp
\brief Класс, реализующий splash-окно в отдельном потоке. Без MFC.

Created 2006-06-08 by Kirill V. Lyadvinsky \n
Modified: 
2007-05-25 Добавлена дополнительная синхронизация. Поправлена работа с таймером. \n
2008-06-20 Добавлен вывод текста. Поддержка нескольких мониторов. Поддержка подгружаемых ресурсов (DLL) \n

* The contents of this file are subject to the terms of the Common Development and    \n
* Distribution License ("CDDL")(collectively, the "License"). You may not use this    \n
* file except in compliance with the License. You can obtain a copy of the CDDL at    \n
* http://www.opensource.org/licenses/cddl1.php.                                       \n

*/
#include "stdafx.h"

#include <Windows.h>
#include <CommCtrl.h>
#include <GdiPlus.h>
#include <string>
#include <process.h>

#include "SplashWnd.h"

CSplashWnd::CSplashWnd( HWND hParent )
{
	m_hThread = NULL;
	m_pImage = NULL;
	m_hSplashWnd = NULL;
	m_ThreadId = 0;
	m_hEvent = NULL;
	m_hParentWnd = hParent;

	_current_frame = 0;
	_frame_count = 0;
	_is_playing = false;
	_property_item = nullptr;
	_dimension_ids = nullptr;
}

CSplashWnd::~CSplashWnd()
{
	Close();
	if (m_pImage) delete m_pImage;
}

void CSplashWnd::SetImage(CString strGifPath)
{
	m_pImage = Gdiplus::Image::FromFile(strGifPath);

	UINT frame_dimension_count = m_pImage->GetFrameDimensionsCount();
	if (frame_dimension_count <= 0) {
		return;
	}
	_dimension_ids = (GUID*)new GUID[frame_dimension_count];
	if (nullptr == _dimension_ids) {
		return;
	}
	m_pImage->GetFrameDimensionsList(_dimension_ids, frame_dimension_count);
	wchar_t guid_string[39] = L"";
	StringFromGUID2(_dimension_ids[0], guid_string, 39);
	_frame_count = m_pImage->GetFrameCount(&_dimension_ids[0]);
	UINT total_buffer = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
	_property_item = (Gdiplus::PropertyItem *)new char[total_buffer];
	m_pImage->GetPropertyItem(PropertyTagFrameDelay, total_buffer, _property_item);
}

void CSplashWnd::Show()
{
	if (m_hThread == NULL)
	{
		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, SplashThreadProc, static_cast<LPVOID>(this), 0, &m_ThreadId);
		if (WaitForSingleObject(m_hEvent, 5000) == WAIT_TIMEOUT)
		{
			OutputDebugString(L"Error starting SplashThread\n");
		}
	}
	else
	{
		PostThreadMessage( m_ThreadId, WM_ACTIVATE, WA_CLICKACTIVE, 0 );
	}
}

void CSplashWnd::Close()
{
	if (m_hThread)
	{
		PostThreadMessage(m_ThreadId, WM_QUIT, 0, 0);
		if (WaitForSingleObject(m_hThread, 9000) == WAIT_TIMEOUT)
		{
			::TerminateThread(m_hThread, 2222);
		}
		CloseHandle(m_hThread);
		CloseHandle(m_hEvent);
	}
	m_hThread = NULL;
	if (m_pImage) delete m_pImage;
	m_pImage = NULL;

	if (nullptr != _property_item) {
		delete _property_item;
		_property_item = nullptr;
	}

	if (nullptr != _dimension_ids) {
		delete _dimension_ids;
		_dimension_ids = nullptr;
	}


}

void CSplashWnd::Play()
{
	do {
		if (_is_playing) {
			break;
		}
		if (nullptr == m_pImage) {
			break;
		}
		_current_frame = 0;
		GUID guid = Gdiplus::FrameDimensionTime;
		m_pImage->SelectActiveFrame(&guid, _current_frame);
		UINT elapse = ((UINT*)(_property_item->value))[_current_frame] * 10;
		SetTimer(m_hSplashWnd, 1, elapse, nullptr);
		_current_frame++;
		/*Invalidate(FALSE);*/
		_is_playing = true;
	} while (false);
}



void CSplashWnd::Stop()
{
	do {
		if (false == _is_playing) {
			break;
		}
		KillTimer(m_hSplashWnd, 1);
		_is_playing = false;
	} while (false);
}

unsigned int __stdcall CSplashWnd::SplashThreadProc( void* lpParameter )
{
	CSplashWnd* pThis = static_cast<CSplashWnd*>(lpParameter);
	if (pThis->m_pImage == NULL) return 0;

	// Register your unique class name
	WNDCLASS wndcls = {0};

	wndcls.style = CS_HREDRAW | CS_VREDRAW;
	wndcls.lpfnWndProc = SplashWndProc; 
	wndcls.hInstance = GetModuleHandle(NULL);
	wndcls.hCursor = LoadCursor(NULL, IDC_APPSTARTING);
	wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndcls.lpszClassName = L"SplashWnd";
	wndcls.hIcon = LoadIcon(wndcls.hInstance,   (IDI_APPLICATION));

	if (!RegisterClass(&wndcls))
	{
		if (GetLastError() != 0x00000582) // already registered)
		{
			OutputDebugString(L"Unable to register class SplashWnd\n");
			return 0;
		}
	}

  // try to find monitor where mouse was last time
  POINT point = { 0 };
  MONITORINFO mi = { sizeof(MONITORINFO), 0 };
  HMONITOR hMonitor = 0;
  RECT rcArea = { 0 };

  ::GetCursorPos( &point );
  hMonitor = ::MonitorFromPoint( point, MONITOR_DEFAULTTONEAREST );
  if ( ::GetMonitorInfo( hMonitor, &mi ) )
  {
    rcArea.left = ( mi.rcMonitor.right + mi.rcMonitor.left - static_cast<long>(pThis->m_pImage->GetWidth()) ) /2;
    rcArea.top = ( mi.rcMonitor.top + mi.rcMonitor.bottom - static_cast<long>(pThis->m_pImage->GetHeight()) ) /2;
  }
  else
  {
    SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
    rcArea.left = (rcArea.right + rcArea.left - pThis->m_pImage->GetWidth())/2;
    rcArea.top = (rcArea.top + rcArea.bottom - pThis->m_pImage->GetHeight())/2;
  }  

	//
	pThis->m_hSplashWnd = CreateWindowEx(pThis->m_WindowName.length()?0:WS_EX_TOOLWINDOW, L"SplashWnd", pThis->m_WindowName.c_str(), 
		WS_CLIPCHILDREN|WS_POPUP, rcArea.left, rcArea.top, pThis->m_pImage->GetWidth(), pThis->m_pImage->GetHeight(), 
		pThis->m_hParentWnd, NULL, wndcls.hInstance, NULL);
	if (!pThis->m_hSplashWnd)
	{
		OutputDebugString(L"Unable to create SplashWnd\n");
		return 0;
	}
  
	SetWindowLongPtr(pThis->m_hSplashWnd, GWLP_USERDATA, reinterpret_cast<DWORD_PTR>(pThis) );
	ShowWindow(pThis->m_hSplashWnd, SW_SHOWNOACTIVATE);

	MSG msg;
	BOOL bRet;
	LONG timerCount = 0;

	PeekMessage(&msg, NULL, 0, 0, 0); // invoke creating message queue
	SetEvent(pThis->m_hEvent);
	pThis->Play();
	while ((bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
	{ 
		if (msg.message == WM_QUIT) 
			break;
		
		if (bRet == -1)
		{
			// handle the error and possibly exit
		}
		else
		{
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
	}
	pThis->Stop();
	DestroyWindow(pThis->m_hSplashWnd);

	return 0;
}

LRESULT CALLBACK CSplashWnd::SplashWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CSplashWnd* pInstance = reinterpret_cast<CSplashWnd*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (pInstance == NULL)
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

  switch (uMsg)
	{
		case WM_PAINT:
		{
			if (pInstance->m_pImage)
			{
				Gdiplus::Graphics gdip(hwnd);
				gdip.DrawImage(pInstance->m_pImage, 0, 0, pInstance->m_pImage->GetWidth(), pInstance->m_pImage->GetHeight());

			}
			ValidateRect(hwnd, NULL);
			return 0;
		} break;
		
		case WM_TIMER:
		{
			UINT_PTR  nIDEvent = (UINT_PTR)wParam;
			if (nIDEvent == 1)
			{
				KillTimer(hwnd, nIDEvent);
				GUID guid = Gdiplus::FrameDimensionTime;
				pInstance->m_pImage->SelectActiveFrame(&guid, pInstance->_current_frame);
				UINT elapse = ((UINT*)(pInstance->_property_item->value))[pInstance->_current_frame] * 10;
				SetTimer(hwnd,1, elapse, nullptr);
				pInstance->_current_frame = (++pInstance->_current_frame) % pInstance->_frame_count;
				InvalidateRect(hwnd, NULL, FALSE);
			}

			break;
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CSplashWnd::SetWindowName( const wchar_t* windowName )
{
	m_WindowName = windowName;
}

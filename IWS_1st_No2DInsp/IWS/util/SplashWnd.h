/*! \file SplashWnd.h
\brief Класс, реализующий splash-окно в отдельном потоке. Без MFC.

Created 2006-06-08 by Kirill V. Lyadvinsky \n

* The contents of this file are subject to the terms of the Common Development and    \n
* Distribution License ("CDDL")(collectively, the "License"). You may not use this    \n
* file except in compliance with the License. You can obtain a copy of the CDDL at    \n
* http://www.opensource.org/licenses/cddl1.php.                                       \n

*/
#ifndef __SPLASHWND_H_
#define __SPLASHWND_H_

class CSplashWnd
{
private:
	CSplashWnd(const CSplashWnd&) {};
	CSplashWnd& operator=(const CSplashWnd&) {};
protected:
	HANDLE							m_hThread;
	unsigned int				m_ThreadId;
	HANDLE							m_hEvent;
	
	Gdiplus::Image*			m_pImage;					
	HWND								m_hSplashWnd;	
	std::wstring m_WindowName;			
	HWND								m_hParentWnd;
  UINT _frame_count;
  UINT _current_frame;
  GUID* _dimension_ids;
  Gdiplus::PropertyItem* _property_item;
  bool _is_playing;

public:
	CSplashWnd( HWND hParent = NULL );
	~CSplashWnd();														

	void SetImage(CString strGifPath);
	void SetWindowName(LPCWSTR windowName);		
	void Show();								
	void Close();	
	void Play();
	void Stop();
	HWND GetWindowHwnd() const								
	{
		return m_hSplashWnd;
	};

protected:
	static LRESULT CALLBACK SplashWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static unsigned int __stdcall SplashThreadProc(void* lpParameter);
};

#endif//__SPLASHWND_H_

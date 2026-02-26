#include "../Stdafx.h"
#include "WorkImg.h"
#include "Utility.h"
#include "DispImgDlg.h"

extern CUtility g_Util;

/* */
/* CBaseImg */
CBaseImg::CBaseImg()
{
	m_bWndAttached = FALSE;
	m_hNiWnd = NULL;
	m_iNiWnd = -1;
	m_hParent = NULL;
}

CBaseImg::~CBaseImg()
{
	if (m_bWndAttached)
		Detach();
	if (m_iNiWnd != -1)
		imaqCloseWindow(m_iNiWnd);
}


// Use this function for preparing IMAQ image display window.
// But, make sure to resize window after calling this function. 
BOOL CBaseImg::AttachToParentWnd(CWnd* pParentWnd, BOOL bResample, RGBValue bgColor)
{
	m_hParent = pParentWnd->GetSafeHwnd();

	RECT rc;
	pParentWnd->GetWindowRect(&rc);
	int offSet = 2;
	int wd_parent = rc.right - rc.left;
	int ht_parent = rc.bottom - rc.top;
	int wd_niwnd = wd_parent - offSet;
	int ht_niwnd = ht_parent - offSet;

	m_nWdWnd = wd_parent; // YJKO add
	m_nHtWnd = ht_parent;

	if (m_iNiWnd == -1) // not created yet
	{
		if (imaqGetWindowHandle(&m_iNiWnd) == 0) {
			char* err = imaqGetErrorText(imaqGetLastError());
			MessageBoxW((LPCTSTR)(LPSTR)err, MB_OK);
			imaqDispose(err);
			return FALSE;
		}
		imaqSetupWindow(m_iNiWnd, 0);

		// set Ni window size just a bit smaller than parent-control window
//		imaqSetWindowSize(m_iNiWnd, wd_niwnd, ht_niwnd);
	}

	//	int wd, ht;
	//	imaqGetWindowSize(m_iNiWnd, &wd, &ht);

	if (!m_hNiWnd)
	{
		m_hNiWnd = (HWND)imaqGetSystemWindowHandle(m_iNiWnd);
		m_bWndAttached = TRUE;

		::SetParent(m_hNiWnd, m_hParent);
		::MoveWindow(m_hNiWnd, offSet / 2, offSet / 2, wd_niwnd, ht_niwnd, FALSE);
	}


	// if image is larger then window, image is properly zoomed to fit to the Ni Window
	// image's original aspect ratio is maintained
	if (!bResample)
	{
		imaqSetWindowZoomToFit(m_iNiWnd, TRUE);
		imaqSetWindowBackground(m_iNiWnd, IMAQ_FILL_STYLE_SOLID, IMAQ_HATCH_STYLE_HORIZONTAL, &bgColor, &bgColor);
	}
	return TRUE;
}

void CBaseImg::MoveWindowTo(int x, int y, int width, int height, BOOL bRapaint)
{
	m_nWdWnd = width;
	m_nHtWnd = height;

	::MoveWindow(m_hParent, x, y, width, height, bRapaint);
	::MoveWindow(m_hNiWnd, 0, 0, width, height, bRapaint);
}
void CBaseImg::AdjustImgSize(Image* pImg, Image* pImgOut)
{
	imaqResample(pImgOut, pImg, m_nWdWnd-1, m_nHtWnd-1, IMAQ_ZERO_ORDER, IMAQ_NO_RECT);
}

int CBaseImg::GetNiWndId()
{
	return m_iNiWnd;
}

// Use this function for preparing IMAQ image display window.
// But, make sure to resize window after calling this function. 
BOOL CBaseImg::AttachToParentWndNew(CWnd* pParentWnd, BOOL bResample, RGBValue bgColor)
{
	m_hParent = pParentWnd->GetSafeHwnd();

	RECT rc;
	pParentWnd->GetWindowRect(&rc);
	int offSet = 0;
	int wd_parent = rc.right - rc.left;
	int ht_parent = rc.bottom - rc.top;
	int wd_niwnd = wd_parent - offSet;
	int ht_niwnd = ht_parent - offSet;

	if (m_iNiWnd == -1) // not created yet
	{
		if (imaqGetWindowHandle(&m_iNiWnd) == 0) {
			g_Util.GetImaqLastError(L"Getting ni window number");
			return FALSE;
		}

		imaqSetupWindow(m_iNiWnd, 0);

		// set Ni window size just a bit smaller than parent-control window
		imaqSetWindowSize(m_iNiWnd, wd_niwnd, ht_niwnd);
	}
	//	int wd, ht;
	//	imaqGetWindowSize(m_iNiWnd, &wd, &ht);


	if (!m_hNiWnd)
	{
		m_hNiWnd = (HWND)imaqGetSystemWindowHandle(m_iNiWnd);

		// cf. CWorkImage::AttachToParentWnd
		// Use SubclassWindow instead of Attach for CWorkImage
		SubclassWindow(m_hNiWnd);

		::SetParent(m_hNiWnd, pParentWnd->GetSafeHwnd());
		::MoveWindow(m_hNiWnd, offSet / 2, offSet / 2, wd_niwnd, ht_niwnd, TRUE);
		//		::SetWindowLong(m_hNiWnd, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPED);
	}

	// set background color
//	const RGBValue bc = {170, 247, 199, 0 }; 
	if (!bResample)
	{
		imaqSetWindowBackground(m_iNiWnd, IMAQ_FILL_STYLE_SOLID, IMAQ_HATCH_STYLE_HORIZONTAL, &bgColor, &bgColor);
		imaqSetWindowZoomToFit(m_iNiWnd, TRUE);
	}

	return TRUE;
}

/* */
/* CWorkImg */
CWorkImg::CWorkImg(CWnd* parent)//CDispImgDlg* pDispDlg)
	:m_parent(NULL)
{
	//m_pDispDlg = (CDispImgDlg*)pDispDlg;
	m_parent = parent;
}

CWorkImg::~CWorkImg()
{
}

BEGIN_MESSAGE_MAP(CWorkImg, CBaseImg)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CWorkImg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//DHChoi 20200623
	//m_pDispDlg->OnLButtonDown();
	if (m_parent != NULL)
		::SendMessage(m_parent->GetSafeHwnd(), UWM_WORKIMG_EVENT, GetNiWndId(), (LPARAM)WORKIMG_EVENT_LBUTTON_DOWN);


	CBaseImg::OnLButtonDown(nFlags, point);
}


void CWorkImg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// Prevent from calling default OnMouseMove handler at CWorkImage.
	// Otherwise, cursor is reset by IMAQ OnMouseMove handler.

	//DHChoi 20200623
	//m_pDispDlg->OnMouseMove();
	if (m_parent != NULL)
		::SendMessage(m_parent->GetSafeHwnd(), UWM_WORKIMG_EVENT, GetNiWndId(), (LPARAM)WORKIMG_EVENT_MOUSE_MOVE);


	CBaseImg::OnMouseMove(nFlags, point);
}

void CWorkImg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//DHChoi 20200623
	//m_pDispDlg->OnLButtonUp();
	if (m_parent != NULL)
		::SendMessage(m_parent->GetSafeHwnd(), UWM_WORKIMG_EVENT, GetNiWndId(), (LPARAM)WORKIMG_EVENT_LBUTTON_UP);

	CBaseImg::OnLButtonUp(nFlags, point);
}


// Use this function for preparing IMAQ image display window.
// But, make sure to resize window after calling this function. 
BOOL CWorkImg::AttachToParentWnd(CWnd* pParentWnd, BOOL bResample, RGBValue bgColor)
{
	m_hParent = pParentWnd->GetSafeHwnd();

	RECT rc;
	pParentWnd->GetWindowRect(&rc);
	int offSet = 0;
	int wd_parent = rc.right - rc.left;
	int ht_parent = rc.bottom - rc.top;
	int wd_niwnd = wd_parent - offSet;
	int ht_niwnd = ht_parent - offSet;

	CBaseImg::m_nWdWnd = wd_niwnd; // YJKO add
	CBaseImg::m_nHtWnd = ht_niwnd;

	if (m_iNiWnd == -1) // not created yet
	{
		if (imaqGetWindowHandle(&m_iNiWnd) == 0) {
			g_Util.GetImaqLastError(L"Getting ni window number");
			return FALSE;
		}

		imaqSetupWindow(m_iNiWnd, 0);

		// set Ni window size just a bit smaller than parent-control window
		imaqSetWindowSize(m_iNiWnd, wd_niwnd, ht_niwnd);
	}
	//	int wd, ht;
	//	imaqGetWindowSize(m_iNiWnd, &wd, &ht);

	if (!m_hNiWnd)
	{
		m_hNiWnd = (HWND)imaqGetSystemWindowHandle(m_iNiWnd);

		// cf. CWorkImage::AttachToParentWnd
		// Use SubclassWindow instead of Attach for CWorkImage
		SubclassWindow(m_hNiWnd);

		::SetParent(m_hNiWnd, pParentWnd->GetSafeHwnd());
		::MoveWindow(m_hNiWnd, offSet / 2, offSet / 2, wd_niwnd, ht_niwnd, TRUE);
		//		::SetWindowLong(m_hNiWnd, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPED);
	}

	// set background color
//	const RGBValue bc = {170, 247, 199, 0 }; 
	if (!bResample)
	{
		imaqSetWindowBackground(m_iNiWnd, IMAQ_FILL_STYLE_SOLID, IMAQ_HATCH_STYLE_HORIZONTAL, &bgColor, &bgColor);
		imaqSetWindowZoomToFit(m_iNiWnd, TRUE);
	}

	return TRUE;
}


int CWorkImg::_imaqShowScrollbars(BOOL bShow)//DHChoi 20200629
{
	if (m_iNiWnd < 0) { return -1; }
	if (imaqShowScrollbars(m_iNiWnd, bShow) == 0)
	{
		int iErr = imaqGetLastError();
		TRACE(_T("\nError : Show scrollbar = %d\t\t"), iErr);
		return -2;
	}

	return 0;
}
int CWorkImg::_imaqDisplayImage(Image* pImg)//DHChoi 20200629
{
	if (m_iNiWnd < 0) { return -1; }

	if (imaqDisplayImage(pImg, m_iNiWnd, FALSE) == 0)
	{
		int iErr = imaqGetLastError();
		TRACE(_T("\nError : Show scrollbar = %d\t\t"), iErr);
		return -2;
	}

	return 0;
}


BOOL CWorkImg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		BOOL bUsed = FALSE;
		if (m_parent != NULL)
		{
			
			switch (pMsg->wParam) {
			case VK_UP   : ::SendMessage(m_parent->GetSafeHwnd(), UWM_WORKIMG_EVENT, GetNiWndId(), (LPARAM)WORKIMG_EVENT_KB_VK_UP	); bUsed = TRUE; break;
			case VK_DOWN : ::SendMessage(m_parent->GetSafeHwnd(), UWM_WORKIMG_EVENT, GetNiWndId(), (LPARAM)WORKIMG_EVENT_KB_VK_DOWN	); bUsed = TRUE; break;
			case VK_LEFT : ::SendMessage(m_parent->GetSafeHwnd(), UWM_WORKIMG_EVENT, GetNiWndId(), (LPARAM)WORKIMG_EVENT_KB_VK_LEFT	); bUsed = TRUE; break;
			case VK_RIGHT: ::SendMessage(m_parent->GetSafeHwnd(), UWM_WORKIMG_EVENT, GetNiWndId(), (LPARAM)WORKIMG_EVENT_KB_VK_RIGHT); bUsed = TRUE; break;
			}
		}
		if (bUsed) return TRUE;
	}


	return CBaseImg::PreTranslateMessage(pMsg);
}

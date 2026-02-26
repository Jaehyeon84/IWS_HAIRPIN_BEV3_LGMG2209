#pragma once
#include "afxwin.h"
#include "./NIVision/Include/nivision.h"
using namespace std;

class CDispImgDlg;
class CBaseImg :public CWnd
{
public:
	CBaseImg();
	virtual ~CBaseImg();
	// Attributes
public:
	BOOL	m_bWndAttached;
	int		m_iNiWnd;		// Ni Window id
	HWND	m_hNiWnd;		// Ni window handle
	HWND	m_hParent;		// Parent handle
	int		m_nWdWnd;
	int		m_nHtWnd;

public:
	BOOL AttachToParentWnd(CWnd* pParentWnd, BOOL bResample, RGBValue bgColor);
	int		GetNiWndId();
	void	MoveWindowTo(int x, int y, int width, int height, BOOL bRapaint = FALSE);
	void	AdjustImgSize(Image* pImg, Image* pImgOut);

	//
	BOOL AttachToParentWndNew(CWnd* pParentWnd, BOOL bResample, RGBValue bgColor);
};

#define UWM_WORKIMG_EVENT (WM_USER+4000)//DHChoi 20200623

class CWorkImg: public CBaseImg
{
public:
	typedef enum enNI_EVENT
	{
		WORKIMG_EVENT_LBUTTON_DOWN = 1,
		WORKIMG_EVENT_LBUTTON_UP = 2,
		WORKIMG_EVENT_MOUSE_MOVE = 3,
		WORKIMG_EVENT_LBUTTON_CLICK = 4,
		WORKIMG_EVENT_KB_VK_UP		= 101,
		WORKIMG_EVENT_KB_VK_DOWN	= 102,
		WORKIMG_EVENT_KB_VK_LEFT	= 103,
		WORKIMG_EVENT_KB_VK_RIGHT	= 104

	}NI_EVENT;//DHChoi 20200623
	CWorkImg(CWnd* parent);//CDispImgDlg* parent);//DHChoi 20200623
	virtual ~CWorkImg();

	//CDispImgDlg* m_pDispDlg;

	CWnd* m_parent;  //DHChoi 20200623
	int _imaqShowScrollbars(BOOL bShow);
	int _imaqDisplayImage(Image* pImg);

	// Implementation
public:
	virtual BOOL AttachToParentWnd(CWnd* pParentWnd, BOOL bResample, RGBValue bgColor);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


#pragma once

// CCheckBoxST Bitmap Resource
#define EACH_BITMAP_SIZE     13 /* 크기 보정 값 */
#define FIRST_RADIO_BMP       6 /* 라디오 버튼의 처음 이미지 */
#define FIRST_CHECK_BMP       0 /*   체크 버튼의 처음 이미지 */
static const UINT UWM_CHECKST_TIP = ::RegisterWindowMessage(_T("CCheckBoxSTToolTip"));
// CCheckBoxST

class CCheckBoxST : public CButton
{
	DECLARE_DYNAMIC(CCheckBoxST)
	enum	{
		CMB_COLOR_BK_IN = 0,		// Background color when mouse is INside
		CMB_COLOR_FG_IN,				// Text color when mouse is INside
		CMB_COLOR_BK_OUT,				// Background color when mouse is OUTside
		CMB_COLOR_FG_OUT,				// Text color when mouse is OUTside
		CMB_COLOR_BK_FOCUS,			// Background color when the button is focused
		CMB_COLOR_FG_FOCUS,			// Text color when the button is focused

		CMB_MAX_COLORS
	};
public:
	CCheckBoxST();
	virtual ~CCheckBoxST();
public:
	unsigned long     m_dwHighLight;
	unsigned long     m_dwCheckColour;

	HIMAGELIST        m_himl;
	HBITMAP           hBmp;

private:
	CWnd*		m_pTipRcvWnd;
	CString		m_strToolTip;
	CBitmap		m_bmToolTip;
	int  m_bHover;
	int  m_nCheckState;
	int  m_b3State;
	int  m_bRadioButton;
	int  m_bCheckButton;
	int  m_bPushed;
	COLORREF m_crColors[CMB_MAX_COLORS];
private:
	/** This method draws a three-dimensional rectangle with the top and left sides in the color specified by clrTopLeft and the bottom and right sides in the color specified by clrBottomRight. */
	void Draw3dRect(HDC hDC, LPCRECT lpRect, unsigned long clrTopLeft, unsigned long clrBottomRight);

	/** Draw a Check Symbol. */
	void DrawCheck(HDC hDC, LPCRECT lpRect);

	/** Draw a Rectangle Symbol. */
	void DrawIndeterminateRect(HDC hDC, LPCRECT lpRect, long dwColour = -1);

	/** Draw the orange style border. */
	void DrawOrangeBoarder(HDC hDC, LPCRECT lpRect);

	/** Set the check state of a check box. */
	void SetChangeStates();

	/** Set the uncheck state of a Radio Button. */
	void UnCheckOtherRadio();

public:
	void SetTipData(CWnd *pWnd, CString strDsc, UINT nIDBitmap = NULL);

	/** This method is called to draw an owner-drawn CButton object. */
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	/** This method is called by the framework to allow other necessary subclassing to occur before the window is subclassed. Overriding this method allows for dynamic subclassing of controls. It is an advanced overridable. */
	virtual void PreSubclassWindow();

	/** Sets or resets the check state of a radio button or check box.*/
	void SetCheck(int nCheck);

	/** Retrieves the check state of a radio button or check box. */
	int GetCheck() const;

	/** Determines whether the specified window is the RadioButton. */
	int  IsRadioButton();

	/** Determines whether the specified window is the CheckBox. */
	int  IsCheckBox();

	/** Specifies that the control displays a bitmap instead of the default. */
	void SetBitmapFile(const TCHAR* lpszBmpFile, unsigned int nMaskColour = 0x00FFFFFF);

	void SetBitmapResource(unsigned int nResID, unsigned int nMaskColour = 0x00FFFFFF);

	void SetDefaultColors(BOOL bRepaint = TRUE);
protected:
	/** This method is called by the framework when the CWnd object background needs erasing. For example, when a window is resized. It is called to prepare an invalidated region for painting. */
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	/** This method is called by the framework when the mouse cursor or stylus moves. */
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	/** The framework calls this member function when the user presses the left mouse button. */
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	/** The framework calls this member function when the user releases the left mouse button. */
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	/** This method is called by the framework when the cursor hovers over the client area of the window for the period of time specified in a prior call to TrackMouseEvent.*/
	LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);

	/** This method is called by the framework when the cursor leaves the client area of the window specified in a prior call to TrackMouseEvent. */
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()
};



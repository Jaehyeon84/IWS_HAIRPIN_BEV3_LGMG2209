#if !defined(__SXGROUPBOX_H)
#define __SXGROUPBOX_H

//===================================================================
// SxGroupBox Custom Groupbox Class
//                                         By Toby Norris, 1999
//
// This class provides functions for customizing the font, colors, 
// and style of the GroupBox control.  
//====================================================================
//

// box line styles
#define BS_3D     11
#define BS_RECT   12

struct CSxLogFont : public LOGFONT
{
public:
//
// constructor 
//
	CSxLogFont
	(
		LONG fontHeight = 80, 
		LONG fontWeight = FW_NORMAL,
		BYTE fontItalic = false,
		LPCTSTR faceName = L"MS Sans Serif"
	)
	{
		// clear the current object
		memset(this, 0, sizeof(*this));

		// store the height, weight, and italics
		lfHeight = fontHeight;
		lfWeight = fontWeight;
		lfItalic = fontItalic;

		// make sure that font name is not too long
		ASSERT( wcslen(faceName) < LF_FACESIZE );
		_tcscpy_s(lfFaceName, faceName);

		// set other default parameters
		lfOutPrecision   = OUT_STROKE_PRECIS;
		lfClipPrecision  = CLIP_STROKE_PRECIS;
		lfQuality        = PROOF_QUALITY;
		lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	}
//
// sets a new font name
//
	void SetFaceName(LPCTSTR faceName)
	{
		if(!faceName) return;

		ASSERT(_tcslen(faceName) < LF_FACESIZE );
		lstrcpy(lfFaceName, faceName); 		 
	}
//
// sets a new font rotation angle
//
	int SetFontAngle(int angle)
	{
		int oldangle = lfEscapement;
		lfEscapement = angle;
		return oldangle;
	}
//
// sets the underline style
//
	int SetUnderline(int state)
	{
		int oldstate = lfUnderline;
		lfUnderline = state;
		return oldstate;
	}
//
// sets the strikeout style
//
	int SetStrikeOut(int state)
	{
		int oldstate = lfStrikeOut;
		lfStrikeOut = state;
		return oldstate;
	}
};

class CSxGroupBox : public CButton
{
public:
	CSxGroupBox();
	virtual ~CSxGroupBox();

	void SetBoxColors(COLORREF color1 = NULL, 
		COLORREF color2 = NULL, COLORREF color3 = NULL, bool redraw = false);
	void GetBoxColors(COLORREF& color1, COLORREF& color2);

	void SetText(CString textStr, bool redraw = false);
	CString GetText()
		{return m_txtString;};

	void SetTextColor(COLORREF color, bool redraw = false);
	COLORREF GetTextColor()
		{return m_txtColor;};

	void SetFont(const LOGFONT* lpLogFont = (LOGFONT*)NULL);
	CFont* GetFont()
		{return m_txtFont;};

	void SetTextAlign(int TextAlign);
	int GetTextAlign()
		{return m_txtAlignment;};

	void SetTextOffset(int Voffset, int Hoffset);

	void SetLineStyle(int StyleLine = BS_3D);
	int GetLineStyle()
		{return m_LineStyle;};

	void SetLineThickness(int thickness, bool redraw = false)
		{m_boxThickness=thickness;if(redraw)Invalidate();};
	int GetLineThickness()
		{return m_boxThickness;};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSxGroupBox)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

private:
	COLORREF m_boxColor1;
	COLORREF m_boxColor2;
	COLORREF m_boxColor1_d;
	COLORREF m_boxColor2_d;
	bool m_StyleBitmap;
	int m_boxThickness;

	COLORREF m_txtColor;
	CFont *m_txtFont;
	CString m_txtString;
	int m_txtAlignment;    // text alignment (BS_LEFT, BS_CENTER, BS_RIGHT)
	int m_txtHOffset;      // horizontal distance from left/right side to beginning/end of text
	int m_txtVOffset;      // vertical distance to move font up
	int m_LineStyle;       // line style (BS_FLAT, BS_3D, BS_RECT)

	CRect m_boxRect;

	//{{AFX_MSG(CSxGroupBox)
	afx_msg void OnPaint();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

#include "stdafx.h"
#include "CStaticCadPathView.h"
#include "EntityHatch.h"
DECLARE_USER_MESSAGE(UWM_CSTATICCADCCDVIEW)

// LiteCAD event procedure
//-----------------------------------------------
void CALLBACK EventProcCadCCD(HANDLE hEvent)
{
	int EventType;
	CStaticCadPathView* pApp = (CStaticCadPathView*)lcPropGetHandle(hEvent, LC_PROP_EVENT_APPPRM2);
	if (!pApp->GetSafeHwnd()) return;
	if (pApp) {
		EventType = lcPropGetInt(hEvent, LC_PROP_EVENT_TYPE);
		switch (EventType) {
		case LC_EVENT_PAINT:       pApp->OnPaint(hEvent);  break;
		case LC_EVENT_MOUSEMOVE:   pApp->OnMouseMove(hEvent);  break;
		case LC_EVENT_LBDOWN:      pApp->OnLButtonDown(hEvent);  break;
		}
	}
}


CStaticCadPathView::CStaticCadPathView()
{
	m_hRcvWnd = NULL;
	m_hImage = NULL;
	m_hPtbuf = NULL;
	::InitializeCriticalSection(&m_csImage);
	m_dPenX0 = 0.0;         // Pen start point
	m_dPenY0 = 0.0;
	m_dPenSpeed = 100.0;     // Pen speed, mm/sec
	m_dPenWidth = 0.35;     // Pen speed, mm/sec
}


CStaticCadPathView::~CStaticCadPathView()
{
}
BEGIN_MESSAGE_MAP(CStaticCadPathView, CStatic)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CStaticCadPathView::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);
}

bool CStaticCadPathView::Init()
{
#ifdef _STATICCADCCD_CALLBACK_USE
	lcEventSetProc(LC_EVENT_PAINT, EventProcCadCCD, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_MOUSEMOVE, EventProcCadCCD, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_LBDOWN, EventProcCadCCD, 0, (HANDLE)this);
#endif // _STATICCADCCD_CALLBACK_USE
	CString str = lcPropGetStr(NULL, LC_PROP_G_DIRCFG);
	CString str16 = str + L"\\iws16.ico";
	CString str32 = str + L"\\iws32.ico";
	lcPropPutStr(0, LC_PROP_G_ICON16, str16);
	lcPropPutStr(0, LC_PROP_G_ICON32, str32);
	
	lcPropPutStr(0, LC_PROP_G_MSGTITLE, L"IWS");

	lcEventSetProc(LC_EVENT_PAINT, EventProcCadCCD, 0, (HANDLE)this);
	lcPropPutStr(0, LC_PROP_G_REGCODE, L"c4b8-467d-91b9-9e63");

// 	BOOL bInit = lcInitialize(); // EventProc, (HANDLE)this, 0 );
// 	if (bInit == false) {
// 		AfxMessageBox( L"Can not initialize LiteCAD DLL", MB_OK | MB_ICONSTOP);
// 		return false;
// 	}
	int Style;
	Style = /*LC_WS_HSCROLL | LC_WS_VSCROLL |*/ LC_WS_BORDER | LC_WS_DEFAULT /*| LC_WS_RULERS*/;
	Style &= ~(LC_WS_HSCROLL | LC_WS_VSCROLL);
	m_hLcWnd = lcCreateWindow(this->GetSafeHwnd(), Style);  // real size will be set in the LcAppPaint::Resize()
	m_hLcDrw = lcCreateDrawing();
	lcDrwNew(m_hLcDrw, L"", NULL);

	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_COLORBACKM, RGB(255, 255, 255));
	// Set foreground color for Model space
	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_COLORFOREM, RGB(0, 0, 0));
	// Set cursor color for Model space
	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_COLORCURSORM, RGB(50, 130, 100));

	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_LWMODE, LC_LW_REAL);
	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_LWIDTH, LC_LWEIGHT_DEFAULT);
	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_LWDEFAULT, 100);  // 0.25 mm 
	lcPropPutBool(m_hLcDrw, LC_PROP_DRW_DIRTY, false);
	lcPropPutBool(m_hLcDrw, LC_PROP_DRW_BARCSOLID, false);  // draw barcodes without filling

	// Get Model block
	HANDLE hBlock = lcPropGetHandle(m_hLcDrw, LC_PROP_DRW_BLOCK_MODEL);
	// Link the block with design window. The window will display graphics objects of the block.
	lcWndSetBlock(m_hLcWnd, hBlock);

	return true;
}


void CStaticCadPathView::Update()
{
	RECT rect;
	int W, H;

	GetClientRect(&rect);
	W = rect.right;
	H = rect.bottom;
	lcWndResize(m_hLcWnd, 0, 0, W, H);
}

#define _NEWBEAM 1

void CStaticCadPathView::CreatePointBuff(HANDLE hBlock)
{
	HANDLE  hEnt;
	int    iPline, nPlines, iVer, nVers, nPnts;
	double X, Y, Xprev, Yprev, Dist, Len;
	BOOL   bRet, bPenDown;

	if (m_hPtbuf) {
		lcPaint_DeletePtbuf(m_hPtbuf);
	}

	// save current colors
	m_ColorBack = lcPropGetInt(m_hLcDrw, LC_PROP_DRW_COLORBACKM);
	m_ColorFore = lcPropGetInt(m_hLcDrw, LC_PROP_DRW_COLORFOREM);
	m_ColorCurs = lcPropGetInt(m_hLcDrw, LC_PROP_DRW_COLORCURSORM);
	// Set background color for Model space
	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_COLORBACKM, RGB(255, 255, 255));
	// Set foreground color for Model space
	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_COLORFOREM, RGB(0, 0, 0));
	// Set cursor color for Model space
	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_COLORCURSORM, RGB(50, 130, 100));

	lcWndRedraw(m_hLcDrw);

	// create points buffer
	m_hPtbuf = lcPaint_CreatePtbuf();

	// start pen position
	bPenDown = false;
	X = m_dPenX0;
	Y = m_dPenY0;
	Dist = 0.0;
	lcPaint_PtbufAddPoint(m_hPtbuf, X, Y, Dist, 0.0, bPenDown);
	Xprev = X;
	Yprev = Y;
	nPnts = 1;

	// get the block linked to CAD window
	//hBlock = lcPropGetHandle(m_hLcWnd, LC_PROP_WND_VIEWBLOCK);

	// retrieve all entities of the block
	hEnt = lcBlockGetFirstEnt(hBlock);
	while (hEnt) {
		// explode entity to single polylines
		int iLayer = lcPropGetInt(hEnt, LC_PROP_LAYER_ID);
		TRACE("Layr = %d\n", iLayer);
		iPline = 0;
		nPlines = lcExpEntity(hEnt, LC_EXP_ALL, false); // creates a list of polylines
		while (1) {
			nVers = lcExpGetPline(0.0);
			if (nVers > 0) {
				iPline++;
				iVer = 0;
				while (1) {
					bRet = lcExpGetVertex(&X, &Y);
					if (bRet == FALSE) {
						// exit vertices cycle
						bPenDown = false;
						break;
					}
					// here X,Y has vertex value
					if (X != Xprev || Y != Yprev) {
						Len = _hypot(Xprev - X, Yprev - Y);  // distance from previous position
						Dist += Len;
						lcPaint_PtbufAddPoint(m_hPtbuf, X, Y, Dist, 0.0, bPenDown);
						nPnts++;
						Xprev = X;
						Yprev = Y;
					}
					iVer++;
					bPenDown = true;
				}
			}
			else {
				// exit polylines cycle
				break;
			}
		}
		hEnt = lcBlockGetNextEnt(hBlock, hEnt);
	}

	if (nPnts >= 2) {
		// go to start position
		bPenDown = false;
		X = m_dPenX0;
		Y = m_dPenY0;
		if (X != Xprev || Y != Yprev) {
			Len = _hypot(Xprev - X, Yprev - Y);  // distance from previous position
			Dist += Len;
			lcPaint_PtbufAddPoint(m_hPtbuf, X, Y, Dist, 0.0, bPenDown);
		}
	}

	// disable selection
	lcPropPutBool(m_hLcWnd, LC_PROP_WND_SELECT, false);
	lcPropPutBool(m_hLcWnd, LC_PROP_WND_SELBYRECT, false);
	// disable cross-cursor
	lcPropPutBool(m_hLcWnd, LC_PROP_WND_CURSORCROSS, false);

	lcWndRedraw(m_hLcWnd);
	_Dist = 0.0;
	_TickPrev = ::GetTickCount();
}

void CStaticCadPathView::CreatePointBuffV()
{
	int    iPline, nPlines, iVer, nVers, nPnts;
	double X, Y, Xprev, Yprev, Dist, Len;
	BOOL   bRet, bPenDown;

	if (m_hPtbuf) {
		lcPaint_DeletePtbuf(m_hPtbuf);
	}

	// save current colors
	m_ColorBack = lcPropGetInt(m_hLcDrw, LC_PROP_DRW_COLORBACKM);
	m_ColorFore = lcPropGetInt(m_hLcDrw, LC_PROP_DRW_COLORFOREM);
	m_ColorCurs = lcPropGetInt(m_hLcDrw, LC_PROP_DRW_COLORCURSORM);
	// Set background color for Model space
	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_COLORBACKM, RGB(255, 255, 255));
	// Set foreground color for Model space
	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_COLORFOREM, RGB(0, 0, 0));
	// Set cursor color for Model space
	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_COLORCURSORM, RGB(50, 130, 100));

	lcWndRedraw(m_hLcDrw);

	// create points buffer
	m_hPtbuf = lcPaint_CreatePtbuf();

	// start pen position
	bPenDown = false;
	X = m_dPenX0;
	Y = m_dPenY0;
	Dist = 0.0;
	lcPaint_PtbufAddPoint(m_hPtbuf, X, Y, Dist, 0.0, bPenDown);
	Xprev = X;
	Yprev = Y;
	nPnts = 1;

	// get the block linked to CAD window
	//hBlock = lcPropGetHandle(m_hLcWnd, LC_PROP_WND_VIEWBLOCK);

	// retrieve all entities of the block
	for (auto hEnt : m_entities) {
		int iLayer = lcPropGetInt(hEnt, LC_PROP_LAYER_ID);
		iPline = 0;
		nPlines = lcExpEntity(hEnt, LC_EXP_ALL, false); // creates a list of polylines
		while (1) {
			nVers = lcExpGetPline(0.0);
			if (nVers > 0) {
				iPline++;
				iVer = 0;
				while (1) {
					bRet = lcExpGetVertex(&X, &Y);
					if (bRet == FALSE) {
						// exit vertices cycle
						bPenDown = false;
						break;
					}
					// here X,Y has vertex value
					if (X != Xprev || Y != Yprev) {
						Len = _hypot(Xprev - X, Yprev - Y);  // distance from previous position
						Dist += Len;
						lcPaint_PtbufAddPoint(m_hPtbuf, X, Y, Dist, 0.0, bPenDown);
						nPnts++;
						Xprev = X;
						Yprev = Y;
					}
					iVer++;
					bPenDown = true;
				}
			}
			else {
				// exit polylines cycle
				break;
			}
		}
	}

	if (nPnts >= 2) {
		// go to start position
		bPenDown = false;
		X = m_dPenX0;
		Y = m_dPenY0;
		if (X != Xprev || Y != Yprev) {
			Len = _hypot(Xprev - X, Yprev - Y);  // distance from previous position
			Dist += Len;
			lcPaint_PtbufAddPoint(m_hPtbuf, X, Y, Dist, 0.0, bPenDown);
		}
	}

	// disable selection
	lcPropPutBool(m_hLcWnd, LC_PROP_WND_SELECT, false);
	lcPropPutBool(m_hLcWnd, LC_PROP_WND_SELBYRECT, false);
	// disable cross-cursor
	lcPropPutBool(m_hLcWnd, LC_PROP_WND_CURSORCROSS, false);

	lcWndRedraw(m_hLcWnd);
	_Dist = 0.0;
	_TickPrev = ::GetTickCount();
}

void CStaticCadPathView::DrawPointBuff(HANDLE hEvent)
{
	HANDLE hLcWnd = lcPropGetHandle(hEvent, LC_PROP_EVENT_WND);
	HDC hDC = (HDC)lcPropGetHandle(hEvent, LC_PROP_EVENT_HDC);
	int Level = lcPropGetInt(hEvent, LC_PROP_EVENT_INT1);
	if (Level != 0) {// before the drawing (Level==1 after the drawing)
		return;
	}
	double X=0, Y = 0, Xprev = 0, Yprev = 0, Xpen = 0, Ypen = 0, dx = 0, dy = 0, D = 0, Dprev = 0, Len = 0, Coef = 0;
	int    PtMode = LC_POINT_NONE;
#ifndef _NEWBEAM
	int    Xwnd, Ywnd;
#endif
	BOOL   bPenDown = false;
	HANDLE hPen1, hPen2, hPen3;
	DWORD  dTick, t2;

	t2 = ::GetTickCount();
	dTick = t2 - _TickPrev;
	_TickPrev = t2;
	Len = m_dPenSpeed * (dTick / 1000.0);  // distance that pen have passed since previous ProcDraw()
	_Dist += Len;   // total distance since start point

	hPen1 = lcPaint_PenCreate(m_hLcWnd, 0, RGB(0, 0, 0), m_dPenWidth, LC_PS_SOLID);
	hPen2 = NULL;
	lcPaint_PenSelect(m_hLcWnd, hPen1);

	if (lcPaint_PtbufGetPoint2(m_hPtbuf, 0, &Xprev, &Yprev, &Dprev, 0, 0)) {  // first point
		for (;;) {
			// next point
			if (lcPaint_PtbufGetPoint2(m_hPtbuf, 1, &X, &Y, &D, 0, &bPenDown)) {
				if ((Dprev <= _Dist) && (_Dist <= D)) {
					Len = D - Dprev;
					Coef = (_Dist - Dprev) / Len;
					dx = X - Xprev;
					dy = Y - Yprev;
					Xpen = Xprev + (dx * Coef);
					Ypen = Yprev + (dy * Coef);
					// draw line before Pen
					if (bPenDown) {
						lcPaint_DrawLine(m_hLcWnd, Xprev, Yprev, Xpen, Ypen);
					}
					// draw line after Pen
					if (hPen2 == NULL) {
						// not yet burned part
						hPen2 = lcPaint_PenCreate(m_hLcWnd, 0, RGB(210, 210, 210), 0.0, LC_PS_SOLID);
						lcPaint_PenSelect(m_hLcWnd, hPen2);
					}
					if (bPenDown) {
						lcPaint_DrawLine(m_hLcWnd, Xpen, Ypen, X, Y);
					}
					PtMode = bPenDown ? LC_POINT_BEAM1 : LC_POINT_BEAM0;
				}
				else {
					if (bPenDown) {
						lcPaint_DrawLine(m_hLcWnd, Xprev, Yprev, X, Y);
					}
				}
				Xprev = X;
				Yprev = Y;
				Dprev = D;
			}
			else {
				break;
			}
		}
		// draw laser beam
#ifdef _NEWBEAM
		lcPaint_DrawPoint(hLcWnd, Xpen, Ypen, PtMode, 0.0);
#else
		lcCoordDrwToWnd(hLcWnd, Xpen, Ypen, &Xwnd, &Ywnd);
		DrawBeamPoint(hDC, Xwnd, Ywnd, (PtMode == LC_POINT_BEAM1) ? true : false);
#endif
	}

	if (hPen2 == NULL) {
		_Dist = 0.0;
	}

	// draw start point
	hPen3 = lcPaint_PenCreate(hLcWnd, 0, RGB(0, 255, 255), 0.0, LC_PS_SOLID);
	lcPaint_PenSelect(hLcWnd, hPen3);
	lcPaint_DrawPoint(hLcWnd, m_dPenX0, m_dPenY0, LC_POINT_PLUS | LC_POINT_SQUARE, -2);

	// draw prompt text at the top of CAD window
	CString szText;

	//swprintf(szText, L"Emulation mode, <Esc> - exit, <PgUp>/<PgDow> - increase/decrease speed (%.3f)", m_dPenSpeed);
	szText.Format(L"Emulation mode, <Esc> - exit, <PgUp>/<PgDow> - increase/decrease speed (%.3f)", m_dPenSpeed);
	::SetTextAlign(hDC, TA_LEFT | TA_TOP);
	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, RGB(255, 0, 0));
	::TextOut(hDC, 4, 4, szText, (int)wcslen(szText));

	lcWndRedraw(hLcWnd);
	lcEventReturnCode(1);
}

void CStaticCadPathView::AddEntity(HANDLE hEnt)
{
	m_entities.push_back(hEnt);
}

void CStaticCadPathView::SetWorkSize(double x, double y, double w, double h)
{
	double dx = w * 0.5;
	double dy = h * 0.5;
	lcWndZoomRect(m_hLcWnd, x-dx, y - dy,  x+dx, y+dy);
	lcWndRedraw(m_hLcWnd);
}

void CStaticCadPathView::SetImageU8(BYTE* pImg, int w, int h)
{
	m_imgSizeH = h;
	m_imgSizeW = w;
	if (m_hImage)		lcPaint_ImageDelete(m_hImage);
	m_hImage = lcPaint_ImageAdd(41);
	BOOL bRet = lcPaint_ImageCreate(m_hImage, w, h);
	size_t imgSize = w * h;
	int x;
	int y;
	int R, G, B;
	BYTE *ptr = pImg;
	for (size_t i = 0; i < imgSize; i++) {
		x = i % w;
		y = int(i / w);
		R = G = B = *(ptr++);
		lcPaint_ImageSetPixel(m_hImage, x, y, R, G, B);
	}
	//  lcPaint_ImageRotate( hImage2, Ang );
	//  lcPaint_ImageResize( hImage2, W, H, LC_IMGRES_BICUBIC );
	// lcPaint_ImageFlip( hImage2, true, false );
}


void CStaticCadPathView::SetImageRGB(BYTE* pImg, int w, int h)
{
	m_imgSizeH = h;
	m_imgSizeW = w;
	::EnterCriticalSection(&m_csImage);
	if (m_hImage)		lcPaint_ImageDelete(m_hImage);
	m_hImage = lcPaint_ImageAdd(41);
	BOOL bRet = lcPaint_ImageCreate(m_hImage, w, h);
	size_t imgSize = w * h;
	int x;
	int y;
	int R, G, B;
	BYTE *ptr = pImg;
	for (size_t i = 0; i < imgSize; i++) {
		x = i % w;
		y = int(i / w);
		B = *(ptr++);
		G = *(ptr++);
		R = *(ptr++);
		lcPaint_ImageSetPixel(m_hImage, x, y, R, G, B);
	}
	::LeaveCriticalSection(&m_csImage);
	//  lcPaint_ImageRotate( hImage2, Ang );
	//  lcPaint_ImageResize( hImage2, W, H, LC_IMGRES_BICUBIC );
	// lcPaint_ImageFlip( hImage2, true, false );
	
	//Update();
}

void CStaticCadPathView::OnPaint(HANDLE hEvent)
{
	if (m_hImage) {
		::EnterCriticalSection(&m_csImage);
		int w = lcPropGetInt(m_hImage, LC_PROP_IMAGE_W);
		int h = lcPropGetInt(m_hImage, LC_PROP_IMAGE_H);
		lcPaint_DrawImage2(m_hLcWnd, m_hImage, 0, 0, w, h, 0, 0, NULL);
		::LeaveCriticalSection(&m_csImage);
	}
	if (m_hPtbuf)
		DrawPointBuff(hEvent);
}

void CStaticCadPathView::OnMouseMove(HANDLE hEvent)
{
	HANDLE hLcWnd = lcPropGetHandle(hEvent, LC_PROP_EVENT_WND);
	/*
	  double X1, Y1;
	 X1 = lcPropGetFloat( hEvent, LC_PROP_EVENT_FLOAT1 );
	 Y1 = lcPropGetFloat( hEvent, LC_PROP_EVENT_FLOAT2 );
	*/
	m_dCurrX = lcPropGetFloat(hLcWnd, LC_PROP_WND_CURX);
	m_dCurrY = lcPropGetFloat(hLcWnd, LC_PROP_WND_CURY);
	if (m_hRcvWnd)
		::SendMessage(m_hRcvWnd, UWM_CSTATICCADCCDVIEW, CAD_EVENT_MOUSEMOVE, 0);
}

void CStaticCadPathView::OnLButtonDown(HANDLE hEvent)
{

}



void CStaticCadPathView::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
	Init();
	Update();
}


void CStaticCadPathView::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	lcWndResize(m_hLcWnd, 0, 0, cx,cy);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CStaticCadPathView::OnDestroy()
{
	lcEventSetProc(LC_EVENT_PAINT, EventProcCadCCD, 0, NULL);
	::DeleteCriticalSection(&m_csImage);
	if (m_hImage)
		lcPaint_ImageDelete(m_hImage);
	m_hImage = NULL;
	//lcUninitialize(false);
	lcPropPutBool(m_hLcWnd, LC_PROP_WND_SELECT, true);
	lcPropPutBool(m_hLcWnd, LC_PROP_WND_SELBYRECT, true);
	// Eisable cross-cursor
	lcPropPutBool(m_hLcWnd, LC_PROP_WND_CURSORCROSS, true);
	// restore current colors
	lcPropPutInt(m_hLcWnd, LC_PROP_DRW_COLORBACKM, m_ColorBack);
	lcPropPutInt(m_hLcWnd, LC_PROP_DRW_COLORFOREM,m_ColorFore);
	lcPropPutInt(m_hLcWnd, LC_PROP_DRW_COLORCURSORM, m_ColorCurs);
	lcDeleteDrawing(m_hLcDrw);
	lcDeleteWindow(m_hLcWnd);
	if (m_hPtbuf)
		lcPaint_DeletePtbuf(m_hPtbuf);

	CStatic::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
void CStaticCadPathView::CreatePointBuffFromJobs()
{
	int    nPnts;
	double X, Y, Xprev, Yprev, Dist, Len;
	BOOL   bPenDown;

	if (m_hPtbuf) {
		lcPaint_DeletePtbuf(m_hPtbuf);
	}

	// save current colors
	m_ColorBack = lcPropGetInt(m_hLcDrw, LC_PROP_DRW_COLORBACKM);
	m_ColorFore = lcPropGetInt(m_hLcDrw, LC_PROP_DRW_COLORFOREM);
	m_ColorCurs = lcPropGetInt(m_hLcDrw, LC_PROP_DRW_COLORCURSORM);
	// Set background color for Model space
	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_COLORBACKM, RGB(255, 255, 255));
	// Set foreground color for Model space
	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_COLORFOREM, RGB(0, 0, 0));
	// Set cursor color for Model space
	lcPropPutInt(m_hLcDrw, LC_PROP_DRW_COLORCURSORM, RGB(50, 130, 100));

	lcWndRedraw(m_hLcDrw);

	// create points buffer
	m_hPtbuf = lcPaint_CreatePtbuf();

	// start pen position
	bPenDown = false;
	X = m_dPenX0;
	Y = m_dPenY0;
	Dist = 0.0;
	lcPaint_PtbufAddPoint(m_hPtbuf, X, Y, Dist, 0.0, bPenDown);
	Xprev = X;
	Yprev = Y;
	nPnts = 1;

	// get the block linked to CAD window
	//hBlock = lcPropGetHandle(m_hLcWnd, LC_PROP_WND_VIEWBLOCK);

	// retrieve all entities of the block
	for (auto job : (*m_pJobs)) {
		bPenDown = false;
		if (job->GetEntity()->GetEntityType() == LC_ENT_HATCH) {
			CEntityHatch *pHatch = (CEntityHatch *)job->GetEntity();
			int nPath = (int)pHatch->hatchPath->size();
			for (int i = 0; i < nPath; i++) {
				bPenDown = false;
				X = pHatch->hatchPath[0][i].x;
				Y = pHatch->hatchPath[0][i].y;
				if (X != Xprev || Y != Yprev) {
					Len = _hypot(Xprev - X, Yprev - Y);  // distance from previous position
					Dist += Len;
					lcPaint_PtbufAddPoint(m_hPtbuf, X, Y, Dist, 0.0, bPenDown);
					nPnts++;
					Xprev = X;
					Yprev = Y;
				}
				bPenDown = true;
				X = pHatch->hatchPath[1][i].x;
				Y = pHatch->hatchPath[1][i].y;
				if (X != Xprev || Y != Yprev) {
					Len = _hypot(Xprev - X, Yprev - Y);  // distance from previous position
					Dist += Len;
					lcPaint_PtbufAddPoint(m_hPtbuf, X, Y, Dist, 0.0, bPenDown);
					nPnts++;
					Xprev = X;
					Yprev = Y;
				}
			}
		}
		else {

			for (auto p : job->GetEntity()->m_path) {
				X = p.x;
				Y = p.y;
				if (X != Xprev || Y != Yprev) {
					Len = _hypot(Xprev - X, Yprev - Y);  // distance from previous position
					Dist += Len;
					lcPaint_PtbufAddPoint(m_hPtbuf, X, Y, Dist, 0.0, bPenDown);
					nPnts++;
					Xprev = X;
					Yprev = Y;
				}
				bPenDown = true;
			}
		}
	}

	if (nPnts >= 2) {
		// go to start position
		bPenDown = false;
		X = m_dPenX0;
		Y = m_dPenY0;
		if (X != Xprev || Y != Yprev) {
			Len = _hypot(Xprev - X, Yprev - Y);  // distance from previous position
			Dist += Len;
			lcPaint_PtbufAddPoint(m_hPtbuf, X, Y, Dist, 0.0, bPenDown);
		}
	}

	// disable selection
	lcPropPutBool(m_hLcWnd, LC_PROP_WND_SELECT, false);
	lcPropPutBool(m_hLcWnd, LC_PROP_WND_SELBYRECT, false);
	// disable cross-cursor
	lcPropPutBool(m_hLcWnd, LC_PROP_WND_CURSORCROSS, false);

	lcWndRedraw(m_hLcWnd);
	_Dist = 0.0;
	_TickPrev = ::GetTickCount();
}



BOOL CStaticCadPathView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam)
		{
		case VK_PRIOR:
			m_dPenSpeed *= 1.1;
			break;
		case VK_NEXT:
			m_dPenSpeed *= 0.9;
			break;
		}
	}
	return CStatic::PreTranslateMessage(pMsg);
}

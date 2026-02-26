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

// IWSView.cpp: CIWSView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "IWS.h"
#endif

#include "IWSDoc.h"
#include "IWSView.h"
#include "MainFrm.h"
#include "CDlgLaserPen.h"
#include "CDlgLaserPenCW.h"
#include "CDlgLaserPenPulseShape.h"
#include "EntitySpiral.h"
#include "CDlgGenHaipin.h"
#include "CDlgHatchPattern.h"
#include "CDlgWeldPathView.h"
#include "DlgManualWork.h"
#include "CDlgHairpinPattern.h"
#include "CDlgHairpinOffset.h"

#include <cmath>

#include <typeinfo>
#define CMD_SKETCH  LC_CMD_CUSTOM+1

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#include "VRSaux/HubiVRSDlg.h"
DECLARE_USER_MESSAGE(UWM_CIWSVIEW)

void CALLBACK eventHatchChange(HANDLE hEvent) {
	if (!g_bEventEnable) return;
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	if(pDoc)
		if (pDoc->m_pDrawView->m_bDisableCadEvent) return;
	HANDLE hEntity = lcPropGetHandle(hEvent, LC_PROP_EVENT_ENTITY);
	HANDLE hBlock = lcPropGetHandle(g_hLcWnd, LC_PROP_WND_BLOCK);
	HANDLE hSel = lcBlockGetFirstSel(hBlock);
	while (hSel != 0) {
		int iType = lcPropGetInt(hSel, LC_PROP_ENT_TYPE);
		if (iType == LC_ENT_HATCH)
			pDoc->DrawChangedEntity(hSel);
		hSel = lcBlockGetNextSel(hBlock);
	}	
}

void CALLBACK eventMouseMove(HANDLE hEvent) {

	HANDLE hLcWnd = lcPropGetHandle(hEvent, LC_PROP_EVENT_WND);
	double m_dCurrX = lcPropGetFloat(hLcWnd, LC_PROP_WND_CURX);
	double m_dCurrY = lcPropGetFloat(hLcWnd, LC_PROP_WND_CURY);

	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	pFrm->CadMousePos(m_dCurrX, m_dCurrY);
}

void CALLBACK eventOthers(HANDLE hEvent) {
	if (!g_bEventEnable) return;
	HANDLE hEntity = lcPropGetHandle(hEvent, LC_PROP_EVENT_ENTITY);
	int EventType = lcPropGetInt(hEvent, LC_PROP_EVENT_TYPE);
}

void CALLBACK eventAddEntity(HANDLE hEvent) {
	if (!g_bEventEnable) return;
	static HANDLE prevEnt = NULL;

	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	HANDLE hEntity = lcPropGetHandle(hEvent, LC_PROP_EVENT_ENTITY);
	if (prevEnt != hEntity)
		pDoc->DrawAddEntity(hEntity);
	prevEnt = hEntity;

}

void CALLBACK eventEraseEntity(HANDLE hEvent) {
	if (!g_bEventEnable) return;
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	HANDLE hEntity = lcPropGetHandle(hEvent, LC_PROP_EVENT_ENTITY);
	pDoc->DrawDeleteEntity(hEntity);
	int i = lcPropGetInt(hEvent, LC_PROP_EVENT_INT1);
}

void CALLBACK eventSelectEntity(HANDLE hEvent) {
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	pFrm->m_wndWorkView.CadSelectEvent(hEvent);
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	HANDLE hEntity = lcPropGetHandle(hEvent, LC_PROP_EVENT_ENTITY);
	pDoc->DrawSelectedEntity(hEntity);
}

void CALLBACK eventEntPropChanged(HANDLE hEvent) {
	if (!g_bEventEnable) return;
	static DWORD prevEvent = 0;
	static HANDLE prevEnt = NULL;
	DWORD t1 = ::timeGetTime();
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	HANDLE hEntity = lcPropGetHandle(hEvent, LC_PROP_EVENT_ENTITY);

 	if (prevEnt == hEntity && (t1 - prevEvent) < 50) {
		prevEvent = t1;
		prevEnt = hEntity;
		pDoc->m_pDrawView->PostMessageW(UWM_CIWSVIEW, CIWSVIEWMSG_ENT_UPDATE, (LPARAM)hEntity);
		return;
	}
	prevEvent = t1;
	prevEnt = hEntity;


	lcEventsEnable(FALSE);
	pDoc->DrawChangedEntity(hEntity);
	lcEventsEnable(TRUE);

}

void CALLBACK eventKeyDown(HANDLE hEvent) {
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	int iKey = lcPropGetInt(hEvent, LC_PROP_EVENT_INT1);
	int nCtrl = lcPropGetInt(hEvent, LC_PROP_EVENT_INT4);
	HANDLE hBlockAct = lcPropGetHandle(hEvent, LC_PROP_EVENT_BLOCK);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	if (hBlock != hBlockAct) return;

	int nSelEnts = lcPropGetInt(hBlock, LC_PROP_BLOCK_NSELENTS);
	TRACE("Keyval = %d, %d\n", iKey, GetKeyState('c'));
	 
	if (!nCtrl) return;
	if (iKey == 'c' || iKey == 'C') {
		lcWndExeCommand(g_hLcWnd, LC_CMD_COPY, LC_COPY_MOVE);
	}
	else if(iKey == 'V' || iKey == 'v')
	{
		lcWndExeCommand(g_hLcWnd, LC_CMD_CBPASTE, 0);
	}
}
// CIWSView
IMPLEMENT_DYNCREATE(CIWSView, CView)

BEGIN_MESSAGE_MAP(CIWSView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CIWSView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_BUTTON_CAD_LINE, &CIWSView::OnButtonCadLine)
	ON_COMMAND(ID_BUTTON_CAD_POLYLINE, &CIWSView::OnButtonCadPolyline)
	ON_COMMAND(ID_BUTTON_CAD_CIRCLE, &CIWSView::OnButtonCadCircle)
	ON_COMMAND(ID_BUTTON_CAD_ARC, &CIWSView::OnButtonCadArc)
	ON_COMMAND(ID_BUTTON_CAD_EXTEND, &CIWSView::OnButtonCadExtend)
	ON_COMMAND(ID_BUTTON_CAD_TRIM, &CIWSView::OnButtonCadTrim)
	ON_COMMAND(ID_BUTTON_CAD_OFFSET, &CIWSView::OnButtonCadOffset)
	ON_COMMAND(ID_BUTTON_CAD_ROUND, &CIWSView::OnButtonCadRound)
	ON_COMMAND(ID_BUTTON_ZOOM_EXTENTS, &CIWSView::OnButtonZoomExtents)
	ON_COMMAND(ID_BUTTON_CAD_UNDO, &CIWSView::OnButtonCadUndo)
	ON_COMMAND(ID_BUTTON_CAD_REDO, &CIWSView::OnButtonCadRedo)
	ON_COMMAND(ID_BUTTON_ZOOM_BOX, &CIWSView::OnButtonZoomBox)
	ON_COMMAND(ID_BUTTON_ZOOM_PREV, &CIWSView::OnButtonZoomPrev)
	ON_COMMAND(ID_BUTTON_CAD_POINT, &CIWSView::OnButtonCadPoint)
	ON_COMMAND(ID_BUTTON_LASER_PENS, &CIWSView::OnButtonLaserPens)
	ON_COMMAND(ID_BUTTON_LOAD_SCOUTIMG, &CIWSView::OnButtonLoadScoutimg)
	ON_COMMAND(ID_BUTTON_CAD_SPIRAL, &CIWSView::OnButtonCadSpiral)
	ON_COMMAND(ID_BUTTON_CAD_RECT, &CIWSView::OnButtonCadRect)
	ON_COMMAND(ID_BUTTON_ZOOM_FITTOWORK, &CIWSView::OnButtonZoomFittowork)
	ON_COMMAND(ID_BUTTON_ELLIPSE, &CIWSView::OnButtonEllipse)
	ON_COMMAND(ID_BUTTON_CAD_HATCH, &CIWSView::OnButtonCadHatch)
	ON_COMMAND(ID_BUTTON_WELDPATH, &CIWSView::OnButtonWeldpath)
	ON_COMMAND(ID_BUTTON_HAIRPIN_OFFSET, &CIWSView::OnHairpinOffset)
	//ON_COMMAND(ID_BUTTON_WELD, &CIWSView::OnWeld)
	//ON_COMMAND(ID_BUTTON_CAD_MERGE, &CIWSView::OnButtonCadMerge)
	ON_COMMAND(ID_BUTTON_CAD_JOIN, &CIWSView::OnButtonCadJoin)
	ON_COMMAND(ID_BUTTON_CAD_COPY_MOVE, &CIWSView::OnButtonCadCopy)
	ON_COMMAND(ID_BUTTON_CAD_SNAP, &CIWSView::OnButtonCadSnap)
	ON_COMMAND(ID_BUTTON_COPYMENU, &CIWSView::OnButtonCopymenu)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_COPYMENU, &CIWSView::OnUpdateButtonCopymenu)
	ON_COMMAND(ID_BUTTON_CAD_COPY_CIRCLE, &CIWSView::OnButtonCadCopyCircle)
	ON_COMMAND(ID_BUTTON_CAD_COPY_RECT, &CIWSView::OnButtonCadCopyRect)
	ON_COMMAND(ID_BUTTON_CAD_ALIGN_BOTTOM, &CIWSView::OnButtonCadAlignBottom)
	ON_COMMAND(ID_BUTTON_CAD_ALIGN_CENTER, &CIWSView::OnButtonCadAlignCenter)
	ON_COMMAND(ID_BUTTON_CAD_ALIGN_LEFT, &CIWSView::OnButtonCadAlignLeft)
	ON_COMMAND(ID_BUTTON_CAD_ALIGN_RIGHT, &CIWSView::OnButtonCadAlignRight)
	ON_COMMAND(ID_BUTTON_CAD_ALIGN_TOP, &CIWSView::OnButtonCadAlignTop)
	ON_COMMAND(ID_BUTTON_CAD_KBMOVE, &CIWSView::OnButtonCadKbmove)
	ON_COMMAND(ID_BUTTON_CAD_ALIGNMENT, &CIWSView::OnButtonCadAlignment)
	ON_REGISTERED_MESSAGE(UWM_CIWSVIEW, &CIWSView::OnUwmCiwsview)
	ON_COMMAND(ID_BUTTON_HAIRPIN, &CIWSView::OnButtonHairpin)	// hslee
	ON_COMMAND(ID_BUTTON_HAIRPIN_CONFIG, &CIWSView::OnFileHpConfig)	// hslee
	ON_COMMAND(ID_BUTTON_PATTERN, &CIWSView::OnButtonPattern)	// hslee
END_MESSAGE_MAP()

// CIWSView 생성/소멸
CIWSView::CIWSView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_hRefImg = NULL;
	m_hLayerImage = NULL;
	m_hWorkArea = NULL;
	m_bModeAddSpiral = FALSE;
	m_bDisableCadEvent = FALSE;
	m_alignLast = LC_ALIGN_LEFT;
}

CIWSView::~CIWSView()
{
	lcUninitialize(true);
	lcDeleteDrawing(g_hLcDrw);
	lcDeleteProps(g_hLcProp);
	lcDeleteWindow(g_hLcWnd);
}

BOOL CIWSView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CIWSView 그리기

void CIWSView::OnDraw(CDC* /*pDC*/)
{
	CIWSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	//lcDrwRegenViews(g_hLcDrw, 0);
	lcWndRedraw(g_hLcWnd);
	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CIWSView 인쇄


void CIWSView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

void CIWSView::GotoScaner(double x, double y)
{
	CScanner *pScanner = GetDocument()->m_pScanner;
	CLaserPen *pOldPen = pScanner->GetLaserPen();
	CLaserPen *pPenMove = &(GetDocument()->m_pens[14]);
	pScanner->SetLaserPen(pPenMove);
	pScanner->SetLaserPenParam();
	pScanner->WaitMoving(TRUE);

	pScanner->Move(x, y);
	pScanner->WaitMoving(TRUE);

	pScanner->SetLaserPen(pOldPen);
	pScanner->SetLaserPenParam();
}

int CIWSView::ShowRefImage()
{
	CIWSDoc* pDoc = GetDocument();
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	CString strRefImg = pDoc->m_strImgPath + STR_REFIMG_FILE;
	if (!m_hLayerImage) {
		m_hLayerImage = lcDrwAddLayer(g_hLcDrw, STR_LAYER_IMAGE_BK, L"100,100,100", 0, LC_LWEIGHT_DEFAULT);
	}
	lcPropPutHandle(g_hLcDrw, LC_PROP_DRW_LAYER, m_hLayerImage);  // set active layer
	lcPropPutBool(m_hLayerImage, LC_PROP_LAYER_LOCKED, FALSE);
	HANDLE hOldImg = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_IMAGE, STR_REFIMG_FILE);
	if (hOldImg)
	{
		BOOL bret = lcDrwDeleteObject(g_hLcDrw, hOldImg);
		lcBlockPurge(hBlock);
		
	}
	BOOL bRet = lcLayerClear(m_hLayerImage, NULL);
	HANDLE hImg = lcDrwAddImage(g_hLcDrw, STR_REFIMG_FILE, strRefImg);
	if (!hImg) return -1;

	if (m_hRefImg) {
		lcDrwDeleteObject(g_hLcDrw, m_hRefImg);
		lcBlockPurge(hBlock);
		m_hRefImg = NULL;
	}
	
	double sx, sy;
	double imgx, imgy, imgcx, imgcy;
	pDoc->m_config.GetRefImgCnt(&imgcx, &imgcy);
	//pDoc->m_config.GetRefImgRng(&imgx, &imgy);

	pDoc->GetRefImgSize(&imgx, &imgy);
	POINTF sc = pDoc->GetScanCenter();
	
	//double scaleX, scaleY;
	//pDoc->m_config.GetRefImgScale(&scaleX, &scaleY);
	//imgcx *= scaleX;
	//imgcy *= scaleY;

	sx = -imgx / 2.0 + imgcx + double(sc.x);
	sy = -imgy / 2.0 + imgcy + double(sc.y);

//	sx = -pDoc->m_work.m_fRefImgSizeW / 2.0 + pDoc->m_config.m_WorkCenter.x;
//	sy = -pDoc->m_work.m_fRefImgSizeH / 2.0 + pDoc->m_config.m_WorkCenter.y;
	m_hRefImg = lcBlockAddImageRef(hBlock, hImg, sx, sy, imgx,imgy, TRUE);

//	sx = -197.5 / 2.0 + pDoc->m_config.m_WorkCenter.x-0.10;
//	sy = - 202.5 / 2.0 + pDoc->m_config.m_WorkCenter.y+0.15;
//	m_hRefImg = lcBlockAddImageRef(hBlock, hImg,sx,sy, 197.5,202.5, TRUE);
	lcEntToAbove(m_hRefImg, g_hDrwWorkArea);
	lcPropPutBool(m_hLayerImage, LC_PROP_LAYER_LOCKED, TRUE);
	HANDLE hLayer = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER, STR_LAYER_0);
	lcPropPutHandle(g_hLcDrw, LC_PROP_DRW_LAYER, hLayer);  // set active layer
	int cnt = lcDrwCountObjects(g_hLcDrw, LC_OBJ_LAYER);
	
	lcPropPutInt(hLayer, LC_PROP_TABLE_PRIORITY, cnt);
	lcPropPutInt(m_hLayerImage, LC_PROP_TABLE_PRIORITY, 1);

	lcDrwSortObjects(g_hLcDrw, LC_OBJ_LAYER);
	lcBlockSortEnts(hBlock, true, 0);
	lcEntUpdate(m_hRefImg);
	//lcDrwRegenViews(g_hLcDrw, 0);
	//pDoc->m_work.CalcHairpinInfo();
	pDoc->m_work.NewCalcHairpinInfo();
	lcWndRedraw(g_hLcWnd);
	lcEventsEnable(TRUE);
	return 0;
}

int CIWSView::ShowRefImage(CString strImagePath)
{
	CString strImagFileName;
	int nIndex = strImagePath.ReverseFind(_T('\\'));

	strImagFileName = strImagePath.Mid(nIndex + 1);

	CIWSDoc* pDoc = GetDocument();
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	CString strRefImg = strImagePath;//pDoc->m_strImgPath + STR_REFIMG_FILE;
	if (!m_hLayerImage) {
		m_hLayerImage = lcDrwAddLayer(g_hLcDrw, STR_LAYER_IMAGE_BK, L"100,100,100", 0, LC_LWEIGHT_DEFAULT);
	}
	lcPropPutHandle(g_hLcDrw, LC_PROP_DRW_LAYER, m_hLayerImage);  // set active layer
	lcPropPutBool(m_hLayerImage, LC_PROP_LAYER_LOCKED, FALSE);
	HANDLE hOldImg = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_IMAGE, strImagFileName);
	if (hOldImg)
	{
		BOOL bret = lcDrwDeleteObject(g_hLcDrw, hOldImg);
		lcBlockPurge(hBlock);

	}
	BOOL bRet = lcLayerClear(m_hLayerImage, NULL);
	HANDLE hImg = lcDrwAddImage(g_hLcDrw, strImagFileName, strRefImg);
	if (!hImg) return -1;

	if (m_hRefImg) {
		lcDrwDeleteObject(g_hLcDrw, m_hRefImg);
		lcBlockPurge(hBlock);
		m_hRefImg = NULL;
	}

	double sx, sy;
	double imgx, imgy, imgcx, imgcy;
	pDoc->m_config.GetRefImgCnt(&imgcx, &imgcy);
	//pDoc->m_config.GetRefImgRng(&imgx, &imgy);

	pDoc->GetRefImgSize(&imgx, &imgy);
	POINTF sc = pDoc->GetScanCenter();

	//double scaleX, scaleY;
	//pDoc->m_config.GetRefImgScale(&scaleX, &scaleY);
	//imgcx *= scaleX;
	//imgcy *= scaleY;

	sx = -imgx / 2.0 + imgcx + double(sc.x);
	sy = -imgy / 2.0 + imgcy + double(sc.y);

	//	sx = -pDoc->m_work.m_fRefImgSizeW / 2.0 + pDoc->m_config.m_WorkCenter.x;
	//	sy = -pDoc->m_work.m_fRefImgSizeH / 2.0 + pDoc->m_config.m_WorkCenter.y;
	m_hRefImg = lcBlockAddImageRef(hBlock, hImg, sx, sy, imgx, imgy, TRUE);

	//	sx = -197.5 / 2.0 + pDoc->m_config.m_WorkCenter.x-0.10;
	//	sy = - 202.5 / 2.0 + pDoc->m_config.m_WorkCenter.y+0.15;
	//	m_hRefImg = lcBlockAddImageRef(hBlock, hImg,sx,sy, 197.5,202.5, TRUE);
	lcEntToAbove(m_hRefImg, g_hDrwWorkArea);
	lcPropPutBool(m_hLayerImage, LC_PROP_LAYER_LOCKED, TRUE);
	HANDLE hLayer = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER, STR_LAYER_0);
	lcPropPutHandle(g_hLcDrw, LC_PROP_DRW_LAYER, hLayer);  // set active layer
	int cnt = lcDrwCountObjects(g_hLcDrw, LC_OBJ_LAYER);

	lcPropPutInt(hLayer, LC_PROP_TABLE_PRIORITY, cnt);
	lcPropPutInt(m_hLayerImage, LC_PROP_TABLE_PRIORITY, 1);

	lcDrwSortObjects(g_hLcDrw, LC_OBJ_LAYER);
	lcBlockSortEnts(hBlock, true, 0);
	lcEntUpdate(m_hRefImg);
	//lcDrwRegenViews(g_hLcDrw, 0);
	//pDoc->m_work.CalcHairpinInfo();
	pDoc->m_work.NewCalcHairpinInfo();
	lcWndRedraw(g_hLcWnd);
	lcEventsEnable(TRUE);
	return 0;
}

BOOL CIWSView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CIWSView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CIWSView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CIWSView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CIWSView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CIWSView 진단

#ifdef _DEBUG
void CIWSView::AssertValid() const
{
	CView::AssertValid();
}

void CIWSView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CIWSDoc* CIWSView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIWSDoc)));
	return (CIWSDoc*)m_pDocument;
}


#endif //_DEBUG


// CIWSView 메시지 처리기


int CIWSView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	GetDocument()->m_pDrawView = this;
	
	lcEventSetProc(LC_EVENT_ADDENTITY,	eventAddEntity,		0, 0);
	lcEventSetProc(LC_EVENT_ENTERASE,	eventEraseEntity,	0, 0);
	lcEventSetProc(LC_EVENT_SELECT,		eventSelectEntity,	0, 0);
	lcEventSetProc(LC_EVENT_ENTPROP,	eventEntPropChanged,0, 0);
	lcEventSetProc(LC_EVENT_EXTENTS,	eventHatchChange, 0, 0);
	lcEventSetProc(LC_EVENT_ENTMOVE,	eventHatchChange, 0, 0);
	//lcEventSetProc(LC_EVENT_KEYDOWN, eventKeyDown, 0, 0);
	lcEventSetProc(LC_EVENT_MOUSEMOVE, eventMouseMove, 0, 0);
	//lcEventSetProc(LC_EVENT_PAINT, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_RBDOWN, eventOthers, 0, (HANDLE)this);	
	lcEventSetProc(LC_EVENT_WNDVIEW, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_VIEWBLOCK, eventOthers, 0, (HANDLE)this);
	//lcEventSetProc(LC_EVENT_EXTENTS, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_CMDINWND, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_GRID, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_OSNAP, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_PTRACK, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_ORTHO, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_FILE, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_WNDPROP, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_DRWPROP, eventOthers, 0, (HANDLE)this);
	//lcEventSetProc(LC_EVENT_ENTMOVE, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_ENTSCALE, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_ENTROTATE, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_ENTMIRROR, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_DRAWIMAGE, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_PICKENT, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_ADDSTR, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_ADDCMD, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_CCMD, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_SELENT1, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_SELENTS, eventOthers, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_CMD1, eventOthers, 0, (HANDLE)this);

	CString str = lcPropGetStr(NULL, LC_PROP_G_DIRCFG);
	CString str16 = str + L"\\iws16.ico";
	CString str32 = str + L"\\iws32.ico";
	lcPropPutStr(NULL, LC_PROP_G_ICON16, str16);
	lcPropPutStr(NULL, LC_PROP_G_ICON32, str32);
	lcPropPutStr(0, LC_PROP_G_MSGTITLE, L"IWS");
	lcInitialize();  // loads "Litecad.cfg"
	lcPropPutStr(0, LC_PROP_G_REGCODE, L"c4b8-467d-91b9-9e63");

	int Style;
	Style = /*LC_WS_HSCROLL | LC_WS_VSCROLL |*/ LC_WS_BORDER | LC_WS_DEFAULT | LC_WS_RULERS;
	Style &= ~(LC_WS_HSCROLL | LC_WS_VSCROLL);

	g_hLcWnd = lcCreateWindow(this->GetSafeHwnd(), Style);
	g_hLcDrw = lcCreateDrawing();
	g_bEventEnable = TRUE;
	return 0;

}


void CIWSView::OnDestroy()
{
	CView::OnDestroy();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CIWSView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	if (g_hLcWnd) {
		lcWndResize(g_hLcWnd, 0, 0, cx, cy);

// 		HANDLE hView = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_VIEW);
		double Lef = lcPropGetFloat(g_hLcDrw, LC_PROP_DRW_LIM_MAXLEF);
		double Bot = lcPropGetFloat(g_hLcDrw, LC_PROP_DRW_LIM_MAXBOT);
		double Rig = lcPropGetFloat(g_hLcDrw, LC_PROP_DRW_LIM_MAXRIG);
		double Top = lcPropGetFloat(g_hLcDrw, LC_PROP_DRW_LIM_MAXTOP);
		//lcViewSetRect2(hView, Lef-10, Bot-10, Rig+10, Top+10);
		
	}
		
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CIWSView::OnButtonCadLine()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_LINE, LC_LINE_SEPARATE);
}


void CIWSView::OnButtonCadPolyline()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_POLYLINE, 0);
}


void CIWSView::OnButtonCadCircle()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_CIRCLE, 0);

}


void CIWSView::OnButtonCadArc()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_ARC, 0);
}


void CIWSView::OnButtonCadExtend()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_EXTEND, 0);
}


void CIWSView::OnButtonCadTrim()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_TRIM, 0);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	lcBlockPurge(hBlock);
}


void CIWSView::OnButtonCadOffset()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_OFFSET, 0);
}

// #include "DlgCadRoundInput.h"
void CIWSView::OnButtonCadRound()
{
//	CDlgCadRoundInput dlg;
// 	if (dlg.DoModal() != IDOK) return;
	lcWndExeCommand(g_hLcWnd, LC_CMD_FILLET, 5);
	return;
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	int nSelEnts = lcPropGetInt(hBlock, LC_PROP_BLOCK_NSELENTS);
	if (nSelEnts != 2)
	{
		AfxMessageBox(L"two specified entities");
		return;
	}
	CString szValue;
	double Radius = 5.0;   // initial value

	szValue.Format(L"%.3f", Radius);
	lcPropPutStr(0, LC_PROP_G_DLGVAL, szValue);
	if (lcDgGetValue(g_hLcWnd, -1, -1, L"Fillet", L"Enter Radius:")) {
		szValue = lcPropGetStr(0, LC_PROP_G_DLGVAL);
		if (szValue[0] != 0) {
			Radius = wcstod(szValue, NULL);
		}
	}

	HANDLE ent1 = lcBlockGetFirstSel(hBlock);
	HANDLE ent2 = lcBlockGetNextSel(hBlock);

	HANDLE newEnt = lcBlockAddFillet(hBlock, ent1, ent2, Radius);
	if (newEnt) {
		GetDocument()->DrawAddEntity(newEnt);
	}
	//
}


void CIWSView::OnButtonZoomExtents()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_ZOOM_EXT, 0);
}


void CIWSView::OnButtonCadUndo()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_UNDO, 0);
}


void CIWSView::OnButtonCadRedo()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_REDO, 0);
}


void CIWSView::OnButtonZoomBox()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_ZOOM_WIN, 0);
}


void CIWSView::OnButtonZoomPrev()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_ZOOM_PREV, 0 );
}


void CIWSView::OnButtonCadPoint()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_POINT, 0);
}

void CIWSView::OnButtonLaserPens()
{
	//CDlgLaserPen dlg;
	//CDlgLaserPenCW dlg;
	
	switch (GetDocument()->m_config.m_iLaser)
	{
	case TRUMPF_TRUDISK:
	{
		CIWSDoc* pDoc = GetDocument();
		if (pDoc->m_User.GetUser() == USER_ADMINISTRATOR)
		{
			CDlgLaserPenCW dlg;
			dlg.m_pPens = &(GetDocument()->m_pens);
			if (dlg.DoModal() == IDOK) GetDocument()->SaveLaserPen(GetDocument()->m_strFileLaserPen);
		}
		else
			AfxMessageBox(L"Please log in administrator", MB_ICONWARNING);
	}
	break;		
	case IPG_CW_YLR2000SM:
	{
		CDlgLaserPenCW dlg;
		dlg.m_pPens = &(GetDocument()->m_pens);
		if (dlg.DoModal() == IDOK) GetDocument()->SaveLaserPen(GetDocument()->m_strFileLaserPen);

	}
	break;
	case IPG_CW_YLR6000QCW:
	{
		CDlgLaserPenCW dlg;
		dlg.m_pPens = &(GetDocument()->m_pens);
		if (dlg.DoModal() == IDOK) GetDocument()->SaveLaserPen(GetDocument()->m_strFileLaserPen);
	}
	break;
	case IPG_PULSE_YLR6000QCW:
	{
		CDlgLaserPenPulseShape dlg;
		dlg.m_pPens = &(GetDocument()->m_pens);
		if (dlg.DoModal() == IDOK) GetDocument()->SaveLaserPen(GetDocument()->m_strFileLaserPen);
	}
	break;
	}
}

void CIWSView::SetWorkArea()
{
	CIWSDoc* pDoc = GetDocument();
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	lcEventsEnable(FALSE);
	HANDLE hLayerWorkAreaBk = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER, STR_LAYER_WORKAREA_BK);
	if (!hLayerWorkAreaBk) {
		hLayerWorkAreaBk = lcDrwAddLayer(g_hLcDrw, STR_LAYER_WORKAREA_BK, L"28,28,28", 0, 100);
	}
	lcPropPutHandle(g_hLcDrw, LC_PROP_DRW_LAYER, hLayerWorkAreaBk);  // set active layer
	lcPropPutBool(hLayerWorkAreaBk, LC_PROP_LAYER_LOCKED, FALSE);;
	
	double x, y, cx, cy;
	pDoc->m_config.GetWorkRng(&x, &y);
	pDoc->m_config.GetRefImgCnt(&cx, &cy);
	lcBlockAddRect(hBlock,cx,cy,x,y, 0, FALSE);
	g_hDrwWorkArea = lcBlockAddRect(hBlock, cx,cy,x,y, 0, TRUE);

	// 	lcDrwSetLimits(g_hLcDrw, -m_pDoc->m_config.m_dWorkAreaX / 2, -m_pDoc->m_config.m_dWorkAreaY / 2,
	// 		m_pDoc->m_config.m_dWorkAreaX / 2, m_pDoc->m_config.m_dWorkAreaY / 2);
	lcPropPutBool(g_hLcDrw, LC_PROP_DRW_LIM_MAXON, TRUE);
	lcPropPutStr(g_hDrwWorkArea, LC_PROP_ENT_COLOR, L"255,255,255");
	lcPropPutBool(hLayerWorkAreaBk, LC_PROP_LAYER_LOCKED, TRUE);

	if (pDoc->m_config.m_bUseHairpin) {
		UpdateHapinTxt(hBlock);
	}

	HANDLE hLayer = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER,STR_LAYER_0);
	lcPropPutHandle(g_hLcDrw, LC_PROP_DRW_LAYER, hLayer);  // set active layer
	lcPropPutStr(g_hLcDrw, LC_PROP_ENT_COLOR, L"107,142,35");
	lcPropPutInt(hLayer, LC_PROP_LAYER_COLOR, RGB(102, 204, 0));  //green
	lcPropPutInt(hLayer, LC_PROP_DRW_COLOR, RGB(102, 204, 0));  //green
// 	lcPropPutBool(hLayer, LC_PROP_LAYER_JUMPLINES, TRUE);
// 	lcPropPutBool(g_hLcDrw, LC_PROP_DRW_JL_LAYER, TRUE);
	lcPropPutBool(g_hLcDrw, LC_PROP_DRW_JL_BASE, FALSE);
	lcEventsEnable(TRUE);
}

void CIWSView::SetDrwNew()
{
	lcDrwNew(g_hLcDrw, NULL, g_hLcWnd);
	//m_wndDrawProp.InitPane();
	// Set background color for Model space
	lcPropPutInt(g_hLcDrw, LC_PROP_DRW_COLORBACKM, COLOR_GREY100);
	// Set foreground color for Model space
	lcPropPutInt(g_hLcDrw, LC_PROP_DRW_COLORFOREM, COLOR_BLACK);
	// Set cursor color for Model space
	lcPropPutInt(g_hLcDrw, LC_PROP_DRW_COLORCURSORM, RGB(50, 130, 100));
	lcPropPutInt(g_hLcDrw, LC_PROP_DRW_LWMODE, LC_LW_REAL);
	//lcPropPutInt(g_hLcDrw, LC_PROP_DRW_LWIDTH, LC_LWEIGHT_DEFAULT);
	lcPropPutInt(g_hLcDrw, LC_PROP_DRW_LWDEFAULT, 100);  // 0.25 mm
	//lcPropPutInt(g_hLcDrw, LC_PROP_DRW_LWIDTH, 1000);
	lcPropPutFloat(g_hLcDrw, LC_PROP_DRW_LWSCALE, 1.0);  // 0.25 mm
	//cPropPutBool(g_hLcDrw, LC_PROP_DRW_LWDISPLAY, TRUE);
	lcPropPutBool(g_hLcDrw, LC_PROP_DRW_DIRTY, false);
	//lcPropPutInt(g_hLcDrw, LC_PROP_DRW_AUNITS, LC_AUNIT_DEGREE);
	lcPropPutInt(g_hLcDrw, LC_PROP_G_GETDELENT, FALSE);
	lcPropPutBool(g_hLcDrw, LC_PROP_DRW_EXPLODEARC, true);
	lcPropPutBool(g_hLcDrw, LC_PROP_DRW_EXPPLINELA, false);
	lcPropPutBool(g_hLcDrw, LC_PROP_DRW_EXPSIMPLINE, true);
	lcPropPutBool(g_hLcDrw, LC_PROP_DRW_EXPTEXTSPLINE, false);

	SetWorkArea();

	m_hRefImg = NULL;
	m_hWorkArea = NULL;
	m_hLayerImage = NULL;

}

void CIWSView::SortLayerOrder()
{
	HANDLE hLayer0 = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER, STR_LAYER_0);
	HANDLE hLayerWorkAreaBk = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER, STR_LAYER_WORKAREA_BK);
	HANDLE hLayerImg = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER, STR_LAYER_IMAGE_BK);
	HANDLE hLegend = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER, STR_LAYER_LEGEND);

	int cnt = lcDrwCountObjects(g_hLcDrw, LC_OBJ_LAYER);
	lcPropPutInt(hLayer0, LC_PROP_TABLE_PRIORITY, cnt);
	if (hLayerWorkAreaBk) {
		lcPropPutInt(hLayerWorkAreaBk, LC_PROP_TABLE_PRIORITY, 1);
		lcPropPutInt(hLayerImg, LC_PROP_TABLE_PRIORITY, 2);
		if(hLegend)
			lcPropPutInt(hLegend, LC_PROP_TABLE_PRIORITY, 3);
	}
	else {

		lcPropPutInt(hLayerImg, LC_PROP_TABLE_PRIORITY, 1);
		if (hLegend)
			lcPropPutInt(hLegend, LC_PROP_TABLE_PRIORITY, 2);

	}

	lcDrwSortObjects(g_hLcDrw, LC_OBJ_LAYER);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	lcBlockSortEnts(hBlock, true, 0);
	lcWndRedraw(g_hLcDrw);
}

void CIWSView::UpdateHapinTxt(HANDLE hBlock)
{
	CIWSDoc* pDoc = GetDocument();
	if (!pDoc->m_config.m_bUseLegend)
		return;

	double dTxtStartAng = pDoc->m_config.m_dHpLegendStartAng;// pDoc->m_work.m_hp.startAng;
	double dTxtPCD = pDoc->m_config.m_dHpLegendDia;// pDoc->m_work.m_hp.LayerDia[0] * 0.9;
	double dTxtDir = pDoc->m_config.m_dHpLegendDir;
	int		nSlot = pDoc->m_work.m_hp.nSlot;

	double step = LC_PI / (double(nSlot) / 2.0f);

	double tx, ty;
	CString str;
	dTxtStartAng = dTxtStartAng * LC_DEG_TO_RAD;
	double dTxtR = dTxtPCD / 2.0;
	double theta;
	HANDLE hTxt;
	HANDLE hLegend = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER, STR_LAYER_LEGEND);
	if (!hLegend) {
		hLegend = lcDrwAddLayer(g_hLcDrw, STR_LAYER_LEGEND, L"100,100,100", 0, 100);
	}

	lcPropPutHandle(g_hLcDrw, LC_PROP_DRW_LAYER, hLegend);  // set active layer
	lcPropPutBool(hLegend, LC_PROP_LAYER_LOCKED, FALSE);;
	lcLayerClear(hLegend, NULL);

	for (int i = 0; i < nSlot; i++) {
		theta = step * i*dTxtDir + dTxtStartAng;
		tx = dTxtR * cos(theta);
		ty = dTxtR * sin(theta);
		str.Format(L"%d", i + 1);
		hTxt = lcBlockAddText2(hBlock, str, tx, ty, LC_TA_CENTER, 1.5, 1, 0, false);
		lcPropPutStr(hTxt, LC_PROP_ENT_COLOR, L"100,100,100");
	}
	lcPropPutBool(hLegend, LC_PROP_LAYER_LOCKED, TRUE);

}

void CIWSView::OnButtonLoadScoutimg()
{ 
	ShowRefImage();
}


void CIWSView::OnButtonCadSpiral()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_SPIRAL, 0);
}


void CIWSView::OnButtonCadRect()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_RECT, 0);
}


void CIWSView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CRibbonBarIWS *pRbn = pFrm->pGetRibbonBar();
	CMFCRibbonButton *pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_LEFT));
	if (pBtn)
		pBtn->SetImageIndex(0, FALSE);
}


void CIWSView::OnButtonZoomFittowork()
{
	CIWSDoc* pDoc = GetDocument();
	double sx, sy;
	double x, y;
	pDoc->m_config.GetWorkRng(&x, &y);
	sx = x / 2.0 + 10;
	sy = y / 2.0 + 10;
lcWndZoomRect(g_hLcWnd, -sx, -sy, sx, sy);
}


void CIWSView::OnButtonEllipse()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_ELLIPSE, 0);
}

BOOL CIWSView::PreTranslateMessage(MSG* pMsg)
{

	if (theApp.m_iwsMode == CIWSApp::IWSMODE_HAIRPIN)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if (pMsg->wParam == VK_DELETE) // ENTER키 눌릴 시
				return TRUE;
			/*else if (pMsg->wParam == VK_RETURN) {
				GetDocument()->m_work.GotoSelectedScannerPos();
				return TRUE;
			}*/
			else if (pMsg->wParam == 'Z') {
				GetDocument()->m_work.SetPrevJob();
				GetDocument()->m_work.GotoSelectedScannerPos();
				return TRUE;
			}
			else if (pMsg->wParam == 'X') {
				GetDocument()->m_work.GotoSelectedScannerPos();
				GetDocument()->m_work.AlignHairpin();
				return TRUE;
			}
			else if (pMsg->wParam == 'C') {
				GetDocument()->m_work.SetNextJob();
				GetDocument()->m_work.GotoSelectedScannerPos();
				return TRUE;
			}
			else if (pMsg->wParam == VK_SPACE) {
				GetDocument()->m_work.AlignHairpin();
				GetDocument()->m_work.GotoSelectedScannerPos();
				return TRUE;
			}
			else if ((pMsg->wParam == 'q' || pMsg->wParam == 'Q') && GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				GetDocument()->m_pMainFrm->LaserpenHiddern();
				return TRUE;
			}
			else if ((pMsg->wParam == 'n' || pMsg->wParam == 'N') && GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				GetDocument()->ChangePrgNo();
				return TRUE;
			}

			/*else if (pMsg->wParam == VK_TAB && GetAsyncKeyState(VK_SHIFT) & 0x8000) {
				GetDocument()->m_work.SetPrevJob();
				return TRUE;
			}
			else if (pMsg->wParam == VK_TAB) {
				GetDocument()->m_work.SetNextJob();
				return TRUE;
			}
			else if (pMsg->wParam == 'C') {
				lcWndExeCommand(g_hLcWnd, LC_CMD_COPY, LC_COPY_MOVE);
				return TRUE;
			}*/
		}

	}
	else
	{
		if (pMsg->wParam == 'X') {
		GetDocument()->m_work.GotoSelectedScannerPos();
		GetDocument()->m_work.AlignHairpin();
		return TRUE;
			}
	}

	/*if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_SPACE:
			GetDocument()->m_work.GotoSelectedScannerPos();
			return TRUE;
		case 'c':
		case 'C':
			lcWndExeCommand(g_hLcWnd, LC_CMD_COPY, LC_COPY_MOVE);
			return TRUE;
		}
	}
	else if(pMsg->message == WM_RBUTTONDOWN){
		return TRUE;
	}*/
	return CView::PreTranslateMessage(pMsg);
}

	
void CIWSView::OnButtonCadHatch()
{
	HANDLE hBlock = lcPropGetHandle(g_hLcWnd, LC_PROP_WND_BLOCK);

	HANDLE hSel = lcBlockGetFirstSel(hBlock);
	if (!hSel)
	{
		AfxMessageBox(L"No selected entity");
		return;
	}
	CDlgHatchPattern dlg;
	if (dlg.DoModal() != IDOK) return;
	
	//GetDocument()->GenerationHatch(dlg.m_dDistance, dlg.m_dAngle);
	GetDocument()->AddHatch(dlg.m_dDistance, dlg.m_dAngle);
}


void CIWSView::OnButtonWeldpath()
{
	CDlgWeldPathView dlg;
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	CIWSDoc* pDoc = GetDocument();
	m_bDisableCadEvent = TRUE;
		
	pDoc->m_work.m_bWeldPath = TRUE;

	int nSelEnts = lcPropGetInt(hBlock, LC_PROP_BLOCK_NSELENTS);
	if (nSelEnts) {
		IWS_VECTOR_JOB sel;
		HANDLE hEnt = lcBlockGetFirstSel(hBlock);
		while (hEnt != 0) {
			if (!pDoc->m_work.GetJob(hEnt)) {
				hEnt = lcBlockGetNextSel(hBlock);
				continue;
			}
			sel.push_back(pDoc->m_work.GetJob(hEnt));
			hEnt = lcBlockGetNextSel(hBlock);
		}
		double x, y;
		pDoc->m_config.GetWorkRng(&x, &y);
		dlg.SetWndSize(x, y);
		dlg.SetJobs(&sel);
		dlg.DoModal();

	}
	else //if (pJobs->size()) 
	{
		pDoc->m_work.DoJobProcessing();
		IWS_VECTOR_JOB sel;
		/*if (pDoc->m_config.m_bUseHairpin)
		{

		}
		else
		{
			for (auto i : pDoc->m_work.m_WeldIdx)
				sel.push_back(pDoc->m_work.GetJob(i));
		}*/

		for (auto i : pDoc->m_work.m_WeldIdx)
		{
			//if (pDoc->m_config.m_bUseHairpin)
			//{
			//	if (pDoc->m_config.m_hp.nSwitchWelding)
			//	{
			//		sel.push_back(job);

			//		continue;
			//	}
			//}

			for (auto job : pDoc->m_work.m_JobGroup[i]->m_jobs)
			{
				sel.push_back(job);
			}
		}
		

		double x, y;
		pDoc->m_config.GetWorkRng(&x, &y);
		dlg.SetWndSize(x, y);
		dlg.SetJobs(&sel);
		//dlg.SetJobs(pJobs);
		dlg.DoModal();

		//C-Ring(Division blower Weldpath 사용 시에만 사용)
		pDoc->m_work.m_bWeldPath = FALSE;
	}

	m_bDisableCadEvent = FALSE;
}

void CIWSView::OnHairpinOffset()
{
	CIWSDoc* pDoc = GetDocument();
	
	// pDoc->m_work.m_nModel == 0 ? _T("Bottom") : _T("C_RING")
	CDlgHairpinOffset dlg;
	dlg.m_nSlot = pDoc->m_work.m_hp.nDrawSlot;
	dlg.m_nLayer = pDoc->m_work.m_hp.nLayer / 2;


	memcpy(dlg.m_hpOffset, pDoc->m_work.m_hp.hpOffset, sizeof(IWS_HAIRPIN_OFFSET) * 512);

	//gseom_240705_2DInsp 주석 추가-->
	/* // 2023.07.30 dhpark 현장 요청 - PLC 검사 상태에 따라 Offset 화면 조절
	dlg.m_b3DUse = TRUE;
	if ((pDoc->m_plc.GetWorkJig() == 1 && pDoc->m_plc.GetDo2DInspectJigA() == 1)
		|| (pDoc->m_plc.GetWorkJig() == 2 && pDoc->m_plc.GetDo2DInspectJigB() == 1))
	{
		dlg.m_b3DUse = FALSE;
	}
	//dlg.m_b3DUse = pDoc->m_work.m_hp.b3DUse;
	memcpy(dlg.m_hpOffset2D, pDoc->m_work.m_hp.hpOffset2D, sizeof(IWS_HAIRPIN_OFFSET) * 512);*/
	//gseom_240705_2DInsp 주석 추가<--

	dlg.m_dBaseR	= pDoc->m_dBaseR	;
	dlg.m_dBaseH	= pDoc->m_dBaseH	;
	dlg.m_dHRRatio	= pDoc->m_dHRRatio	;
	dlg.m_dCenOffX	= pDoc->m_dCenOffX	;
	dlg.m_dCenOffY	= pDoc->m_dCenOffY	;
	if (dlg.DoModal() != IDOK) 
		return;
	//gseom_240705_2DInsp 주석 추가-->
	//pDoc->m_work.m_hp.b3DUse = dlg.m_b3DUse;
	//if (dlg.m_b3DUse)
	//{
		for (int i = 0; i < dlg.m_nSlot * dlg.m_nLayer; i++)
		{
			pDoc->m_work.m_hp.hpOffset[i].ax = dlg.m_grid.GetDouble(0, i);
			pDoc->m_work.m_hp.hpOffset[i].ay = dlg.m_grid.GetDouble(1, i);
			pDoc->m_work.m_hp.hpOffset[i].bx = dlg.m_grid.GetDouble(2, i);
			pDoc->m_work.m_hp.hpOffset[i].by = dlg.m_grid.GetDouble(3, i);
		}
	//}
	/*else
	{
		for (int i = 0; i < dlg.m_nSlot * dlg.m_nLayer; i++)
		{
			pDoc->m_work.m_hp.hpOffset2D[i].ax = dlg.m_grid.GetDouble(0, i);
			pDoc->m_work.m_hp.hpOffset2D[i].ay = dlg.m_grid.GetDouble(1, i);
			pDoc->m_work.m_hp.hpOffset2D[i].bx = dlg.m_grid.GetDouble(2, i);
			pDoc->m_work.m_hp.hpOffset2D[i].by = dlg.m_grid.GetDouble(3, i);
		}
	}*/
	//gseom_240705_2DInsp 주석 추가<--

	pDoc->m_dBaseR		= dlg.m_dBaseR	;
	pDoc->m_dBaseH		= dlg.m_dBaseH	;
	pDoc->m_dHRRatio	= dlg.m_dHRRatio;
	pDoc->m_dCenOffX	= dlg.m_dCenOffX;
	pDoc->m_dCenOffY	= dlg.m_dCenOffY;

	//memcpy(pDoc->m_work.m_hp.hpOffset, dlg.m_hpOffset, sizeof(IWS_HAIRPIN_OFFSET) * 1024);
	//pDoc->m_work.DoVScan();
}

void CIWSView::OnWeld()
{
	CIWSDoc* pDoc = GetDocument();

	// 선택 된 객체 가져오기
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	int nSelEnts = lcPropGetInt(hBlock, LC_PROP_BLOCK_NSELENTS);
	IWS_VECTOR_JOB sel;
	if (nSelEnts != 0) 
	{
		// 다중 선택
		HANDLE hEnt = lcBlockGetFirstSel(hBlock);
		while (hEnt != 0) {
			if (!pDoc->m_work.GetJob(hEnt)) {
				hEnt = lcBlockGetNextSel(hBlock);
				continue;
			}
			sel.push_back(pDoc->m_work.GetJob(hEnt));
			hEnt = lcBlockGetNextSel(hBlock);
		}
	}

	BOOL bSelect = sel.size() ? TRUE : FALSE;
	CDlgManualWork dlg;
	dlg.m_iSelection = bSelect;
	dlg.m_dPowerRatio = pDoc->m_pScanner->GetPowerRatio()*100.0;
	dlg.m_dSpeedRatio = pDoc->m_pScanner->GetSpeedRatio()*100.0;
	if (dlg.DoModal() != IDOK) return;

	if (dlg.m_iSelection)
	{
		// 선택 용접
		for (auto group : pDoc->m_work.m_JobGroup)
			for (auto p : group->m_jobs)
				p->m_bSelected = FALSE;

		CJobWeld* jobWeld;
		for (auto job : sel)
		{
			jobWeld = (CJobWeld*)job;
			jobWeld->m_bSelected = TRUE;			
		}
	}
	else
	{
		// 전체 용접
	}
	for (auto group : pDoc->m_work.m_JobGroup)
	{
		for (auto p : group->m_jobs)
		{
			if (p->m_bSelected)
			{
				CString msg;
				msg.Format(L"Index: %d, Slot: %d, Layer: %d", group->m_nIndex, group->m_nSlot, group->m_nLayer);
				_tracer(TRACE_ERROR, 1, msg);
			}
		}
	}
		


	BOOL bSimulation = pDoc->m_work.m_bSimulation;
	//pDoc->StartWork(bSimulation, dlg.m_dPowerRatio / 100.f, dlg.m_dSpeedRatio / 100.f);
}

//void CIWSView::OnButtonCadMerge()
//{
//	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
//	lcBlockSelJoin(hBlock, 0.001);
//}


void CIWSView::OnButtonCadJoin()
{
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = lcBlockSelJoin(hBlock, 0.001);
	if (hEnt) {
		GetDocument()->DrawAddEntity(hEnt);
	}
}


void CIWSView::OnButtonCadCopy()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_COPY, LC_COPY_MOVE);
}


void CIWSView::OnButtonCadSnap()
{

	bool bret = lcPropGetBool(g_hLcWnd, LC_PROP_WND_OSNAP);
	if (bret) {
		lcPropPutBool(g_hLcWnd, LC_PROP_WND_OSNAP, false);
		return;
	}
	int OSnap;
	OSnap = LC_OSNAP_ENDPOINT | LC_OSNAP_MIDPOINT | LC_OSNAP_CENTER;
	lcPropPutInt(g_hLcWnd, LC_PROP_WND_OSNAP, OSnap);
	
	lcPropPutBool(g_hLcWnd, LC_PROP_WND_OSNAP, true);
}


void CIWSView::OnButtonCopymenu()
{
	OnButtonCadCopy();
}


void CIWSView::OnUpdateButtonCopymenu(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);
}


void CIWSView::OnButtonCadCopyCircle()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_COPY, LC_COPY_ARRCIRC);

}


void CIWSView::OnButtonCadCopyRect()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_COPY, LC_COPY_ARRRECT);
}


void CIWSView::OnButtonCadAlignBottom()
{
	m_alignLast = LC_ALIGN_BOTTOM;
	lcWndExeCommand(g_hLcWnd, LC_CMD_ALIGN, m_alignLast);

// 	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
// 	CRibbonBarIWS *pRbn = pFrm->pGetRibbonBar();
// 	CMFCRibbonButton *pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_BOTTOM ));
// 	if (pBtn)		pBtn->SetImageIndex(0, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_RIGHT)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_TOP)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_LEFT)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_CENTER)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);

}


void CIWSView::OnButtonCadAlignCenter()
{
	m_alignLast = LC_ALIGN_CENTER;
	lcWndExeCommand(g_hLcWnd, LC_CMD_ALIGN, m_alignLast);

// 	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
// 	CRibbonBarIWS *pRbn = pFrm->pGetRibbonBar();
// 	CMFCRibbonButton *pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_CENTER ));
// 	if (pBtn)		pBtn->SetImageIndex(0, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_RIGHT)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_TOP)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_BOTTOM)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_LEFT)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);

}


void CIWSView::OnButtonCadAlignLeft()
{
	m_alignLast = LC_ALIGN_LEFT;
	lcWndExeCommand(g_hLcWnd, LC_CMD_ALIGN, m_alignLast);
// 	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
// 	CRibbonBarIWS *pRbn = pFrm->pGetRibbonBar();
// 	CMFCRibbonButton *pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton,pRbn->FindByID(ID_BUTTON_CAD_ALIGN_LEFT));
// 	if(pBtn)		pBtn->SetImageIndex(0, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_RIGHT)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_TOP)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_BOTTOM)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_CENTER)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
}


void CIWSView::OnButtonCadAlignRight()
{
	m_alignLast = LC_ALIGN_RIGHT;
	lcWndExeCommand(g_hLcWnd, LC_CMD_ALIGN, m_alignLast);

// 	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
// 	CRibbonBarIWS *pRbn = pFrm->pGetRibbonBar();
// 	CMFCRibbonButton *pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_RIGHT));
// 	if (pBtn)		pBtn->SetImageIndex(0, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_LEFT )); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_TOP)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_BOTTOM)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_CENTER)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);

}


void CIWSView::OnButtonCadAlignTop()
{
	m_alignLast = LC_ALIGN_TOP;
	lcWndExeCommand(g_hLcWnd, LC_CMD_ALIGN, m_alignLast);

// 	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
// 	CRibbonBarIWS *pRbn = pFrm->pGetRibbonBar();
// 	CMFCRibbonButton *pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_TOP));
// 	if (pBtn)		pBtn->SetImageIndex(0, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_RIGHT)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_LEFT)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_BOTTOM)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);
// 	pBtn = DYNAMIC_DOWNCAST(CMFCRibbonButton, pRbn->FindByID(ID_BUTTON_CAD_ALIGN_CENTER)); if (pBtn)		pBtn->SetImageIndex(-1, FALSE);

}


void CIWSView::OnButtonCadKbmove()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_KBMOVE, 0);


}


void CIWSView::OnButtonCadAlignment()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_ALIGN, m_alignLast);
}

afx_msg LRESULT CIWSView::OnUwmCiwsview(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case CIWSVIEWMSG_ENT_UPDATE:
	{
		int seq = 0;
		MSG msg;
		while (::PeekMessage(&msg, this->GetSafeHwnd(), UWM_CIWSVIEW, UWM_CIWSVIEW, PM_REMOVE))
		{
			seq++;
		}
		TRACE("OnUwmCiwsview %d msg removed \n", seq);
		CIWSDoc* pDoc = GetDocument();
		HANDLE hEnt = (HANDLE)lParam;
		pDoc->DrawChangedEntity(hEnt);
	}
		break;
	}
	
	return 0;
}

void CIWSView::OnButtonHairpin()
{
	// hslee
	CDlgGenHaipin dlg;
	CIWSDoc* pDoc = GetDocument();
	//CConfigIWS *pCfg = &(GetDocument()->m_wor);
	dlg.m_nSlot = pDoc->m_work.m_hp.nSlot;
	dlg.m_nDrawSlot = pDoc->m_work.m_hp.nDrawSlot;
	dlg.m_nLayer = pDoc->m_work.m_hp.nLayer;
	dlg.m_dStartAng = pDoc->m_work.m_hp.startAng;
	dlg.m_dHairpinSize = pDoc->m_work.m_hp.HpSize;
	dlg.m_dHairpinSizeW = pDoc->m_work.m_hp.HpSizeW;
	dlg.m_d2ndWeldSizeW = pDoc->m_work.m_hp.HpSize2nd;
	for (int i = 0; i < MAX_HAIRPIN_LAYER; i++)
		dlg.m_dLayerDia[i] = pDoc->m_work.m_hp.LayerDia[i];

	dlg.m_nWeldRepeatP1 = pDoc->m_work.m_hp.nCntP1;
	dlg.m_nWeldPenP1 = pDoc->m_work.m_hp.nPenP1;
	dlg.m_nWeldShapeP1 = pDoc->m_work.m_hp.nWeldShapeP1;
	
	dlg.m_nWeldZigZagFreqP1 = pDoc->m_work.m_hp.nWeldZigZagFreqP1;

	dlg.m_nWeldRepeatP2 = pDoc->m_work.m_hp.nCntP2;
	dlg.m_nWeldPenP2 = pDoc->m_work.m_hp.nPenP2;
	dlg.m_nWeldShapeP2 = pDoc->m_work.m_hp.nWeldShapeP2;
	dlg.m_nWeldZigZagFreqP2 = pDoc->m_work.m_hp.nWeldZigZagFreqP2;
	dlg.m_dOverLen = pDoc->m_work.m_hp.HpOverLen;

	dlg.m_nModel = pDoc->m_work.m_nModel;

	// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가
	dlg.m_bUseReversal = pDoc->m_work.m_hp.bReversal;

	// sjyi 2023.04.14 헤어핀 패턴 회전 각도 추가
	dlg.m_dPtnRotationAng = pDoc->m_work.m_hp.dPtnRotationAngle;

	// sjyi 2024.05.25 Spiral 관련 내용 추가
	dlg.m_d2ndWeldSizeW2 = pDoc->m_work.m_hp.HpSize2nd2;
	dlg.m_bEnableOutToInP1 = pDoc->m_work.m_hp.bUseOutToInP1;
	dlg.m_bCloseSpiralP1 = pDoc->m_work.m_hp.bCloseSpiralP1;
	dlg.m_bEnableOutToInP2 = pDoc->m_work.m_hp.bUseOutToInP2;
	dlg.m_bCloseSpiralP2 = pDoc->m_work.m_hp.bCloseSpiralP2;

	// sjyi 2024.05.25 Sortie Repeat 관련 내용 추가
	dlg.m_bUseSortieRpt = pDoc->m_work.m_hp.bEnableSortiRpt;
	dlg.m_nSortieDir = pDoc->m_work.m_hp.nSortieDir;
	dlg.m_nSortieCnt = pDoc->m_work.m_hp.nSortieCnt;

	dlg.m_bUseSpiralPtn = pDoc->m_config.m_bUseSpiralPtn;
	dlg.m_bUseSandGlassPtn = pDoc->m_config.m_bUseSandGlassPtn;
	dlg.m_bUseSortieRepeat = pDoc->m_config.m_bUseSortieRepeat;

	if (dlg.DoModal() != IDOK) return;

	theApp.m_iwsMode = CIWSApp::IWSMODE_HAIRPIN;
	pDoc->m_work.m_nDefLayer = dlg.m_nLayer;

	pDoc->m_work.m_hp.nSlot = dlg.m_nSlot;
	pDoc->m_work.m_hp.nDrawSlot = dlg.m_nDrawSlot;
	pDoc->m_work.m_hp.nLayer = dlg.m_nLayer;
	pDoc->m_work.m_hp.startAng = dlg.m_dStartAng;
	pDoc->m_work.m_hp.HpSize = dlg.m_dHairpinSize;
	pDoc->m_work.m_hp.HpSizeW = dlg.m_dHairpinSizeW;
	pDoc->m_work.m_hp.HpSize2nd = dlg.m_d2ndWeldSizeW;

	for (int i = 0; i < MAX_HAIRPIN_LAYER; i++)
		pDoc->m_work.m_hp.LayerDia[i] = dlg.m_dLayerDia[i];

	pDoc->m_work.m_hp.nCntP1 = dlg.m_nWeldRepeatP1;
	pDoc->m_work.m_hp.nPenP1 = dlg.m_nWeldPenP1;
	pDoc->m_work.m_hp.nWeldShapeP1 = dlg.m_nWeldShapeP1;
	pDoc->m_work.m_hp.nWeldZigZagFreqP1 = dlg.m_nWeldZigZagFreqP1;

	pDoc->m_work.m_hp.nCntP2 = dlg.m_nWeldRepeatP2;
	pDoc->m_work.m_hp.nPenP2 = dlg.m_nWeldPenP2;
	pDoc->m_work.m_hp.nWeldShapeP2 = dlg.m_nWeldShapeP2;
	pDoc->m_work.m_hp.nWeldZigZagFreqP2 = dlg.m_nWeldZigZagFreqP2;
	pDoc->m_work.m_hp.HpOverLen = dlg.m_dOverLen;

	// sjyi 2023.04.14 헤어핀 패턴 회전 각도 추가
	pDoc->m_work.m_hp.dPtnRotationAngle = dlg.m_dPtnRotationAng;
	pDoc->m_work.m_hp.dPtnRotationAngle2 = dlg.m_dPtnRotationAng;

	// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가
	pDoc->m_work.m_hp.bReversal = dlg.m_bUseReversal;

	// sjyi 2024.05.25 Spiral 관련 내용 추가
	pDoc->m_work.m_hp.HpSize2nd2 = dlg.m_d2ndWeldSizeW2;
	pDoc->m_work.m_hp.bUseOutToInP1	 = dlg.m_bEnableOutToInP1;
	pDoc->m_work.m_hp.bCloseSpiralP1 = dlg.m_bCloseSpiralP1;
	pDoc->m_work.m_hp.bUseOutToInP2  = dlg.m_bEnableOutToInP2;
	pDoc->m_work.m_hp.bCloseSpiralP2 = dlg.m_bCloseSpiralP2;

	// sjyi 2024.05.25 Sortie Repeat 관련 내용 추가
	pDoc->m_work.m_hp.bEnableSortiRpt	= dlg.m_bUseSortieRpt;
	pDoc->m_work.m_hp.nSortieDir		= dlg.m_nSortieDir;
	pDoc->m_work.m_hp.nSortieCnt		= dlg.m_nSortieCnt;
	
	// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가
	//pDoc->m_work.GenHairPin(dlg.m_nSlot, dlg.m_nLayer,
	//dlg.m_dStartAng, dlg.m_dHairpinSize, dlg.m_dHairpinSizeW, dlg.m_d2ndWeldSizeW,
	//dlg.m_dLayerDia,
	//dlg.m_nWeldRepeatP1, dlg.m_nWeldRepeatP2, dlg.m_nWeldPenP1, dlg.m_nWeldPenP2, dlg.m_nModel, dlg.m_nDrawSlot, dlg.m_dOverLen, 
	//	pDoc->m_work.m_hp.dPtnRotationAngle); // sjyi 2023.04.14 헤어핀 패턴 회전 각도 추가

	pDoc->m_work.GenHairPin(dlg.m_nSlot, dlg.m_nLayer,
		dlg.m_dStartAng, dlg.m_dHairpinSize, dlg.m_dHairpinSizeW, dlg.m_d2ndWeldSizeW,
		dlg.m_dLayerDia,
		dlg.m_nWeldRepeatP1, dlg.m_nWeldRepeatP2, dlg.m_nWeldPenP1, dlg.m_nWeldPenP2, dlg.m_nModel, dlg.m_nDrawSlot, dlg.m_dOverLen,
		pDoc->m_work.m_hp.dPtnRotationAngle, pDoc->m_work.m_hp.bReversal,
		pDoc->m_work.m_hp.HpSize2nd2, pDoc->m_work.m_hp.nGenDir); // sjyi 2024.02..28 헤어핀 외경 반전 변수 추가

	pDoc->m_work.AlignHairpin();

	// Division 초기화(Division In->Out)
	pDoc->m_work.m_hp.nDivStartSlot = 1;
	pDoc->m_work.m_hp.nDivJumpOffset = dlg.m_nSlot/3;
	pDoc->m_work.m_hp.nWeldType = IWS_JOB_DIV_INOUT;
	
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	pFrm->m_wndJobListView.UpdateJobList(pDoc->m_work.GetJobs());
	pFrm->m_wndPropertiesView.ShowJobProperty((CJob*)NULL);
	pFrm->m_wndWorkView.UpdateWork();
}

void CIWSView::OnFileHpConfig()
{
	CIWSDoc *pDoc = GetDocument();
	pDoc->OnFileHpConfig();
}

void CIWSView::OnButtonPattern()
{
	if (theApp.m_iwsMode != CIWSApp::IWSMODE_HAIRPIN)
	{
		AfxMessageBox(L"Only in hairpin mode.");
		return;
	}

	CIWSDoc *pDoc = GetDocument();
	CDlgHairpinPattern dlg;

	dlg.m_dHpSize = pDoc->m_work.m_hp.HpSize;
	dlg.m_dHpSizeW = pDoc->m_work.m_hp.HpSizeW;
	dlg.m_dHpSize2ndW = pDoc->m_work.m_hp.HpSize2nd;
	dlg.m_iRptP1 = pDoc->m_work.m_hp.nCntP1;
	dlg.m_iRptP2 = pDoc->m_work.m_hp.nCntP2;
	dlg.m_dHpOverLen = pDoc->m_work.m_hp.HpOverLen;

	dlg.m_nWeldPenP1 = pDoc->m_work.m_hp.nPenP1;
	dlg.m_nWeldPenP2 = pDoc->m_work.m_hp.nPenP2;

	dlg.m_nWeldShapeP1 = pDoc->m_work.m_hp.nWeldShapeP1;
	dlg.m_nWeldShapeP2 = pDoc->m_work.m_hp.nWeldShapeP2;

	dlg.m_nWeldZigZagFreqP1 = pDoc->m_work.m_hp.nWeldZigZagFreqP1;
	dlg.m_nWeldZigZagFreqP2 = pDoc->m_work.m_hp.nWeldZigZagFreqP2;
	memcpy(dlg.m_dPower1st, pDoc->m_work.m_hp.dRptPowerProfile_1st, sizeof(double)*MAX_NUM_REPEAT_WELD);
	memcpy(dlg.m_dPower2nd, pDoc->m_work.m_hp.dRptPowerProfile_2nd, sizeof(double)*MAX_NUM_REPEAT_WELD);
	dlg.m_bUseRptPowerProfile1st = pDoc->m_work.m_hp.bUseRptPowerProfile_1st;
	dlg.m_bUseRptPowerProfile2nd = pDoc->m_work.m_hp.bUseRptPowerProfile_2nd;

	// sjyi 2023.04.14 헤어핀 회전 추가
	dlg.m_dPtnRotation = pDoc->m_work.m_hp.dPtnRotationAngle; 
	dlg.m_dPtnRotation2 = pDoc->m_work.m_hp.dPtnRotationAngle2; 

	dlg.m_bUseReversal = pDoc->m_work.m_hp.bReversal; // sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가

	dlg.m_dHpSize2ndW2 = pDoc->m_work.m_hp.HpSize2nd2;
	dlg.m_bCloseSpiralP1 = pDoc->m_work.m_hp.bCloseSpiralP1;
	dlg.m_bCloseSpiralP2 = pDoc->m_work.m_hp.bCloseSpiralP2;
	dlg.m_bEnableOutToInP1 = pDoc->m_work.m_hp.bUseOutToInP1;
	dlg.m_bEnableOutToInP2 = pDoc->m_work.m_hp.bUseOutToInP2;

	dlg.m_bUseSpiralPtn = pDoc->m_config.m_bUseSpiralPtn;
	dlg.m_bUseSandGlassPtn = pDoc->m_config.m_bUseSandGlassPtn;

	if (dlg.DoModal() != IDOK)
		return;

	pDoc->m_work.m_hp.HpSize = dlg.m_dHpSize;
	pDoc->m_work.m_hp.HpSizeW = dlg.m_dHpSizeW;
	pDoc->m_work.m_hp.HpSize2nd = dlg.m_dHpSize2ndW;
	pDoc->m_work.m_hp.nCntP1 = dlg.m_iRptP1;
	pDoc->m_work.m_hp.nCntP2 = dlg.m_iRptP2;
	pDoc->m_work.m_hp.HpOverLen = dlg.m_dHpOverLen;

	pDoc->m_work.m_hp.nPenP1 = dlg.m_nWeldPenP1;
	pDoc->m_work.m_hp.nPenP2 = dlg.m_nWeldPenP2;

	pDoc->m_work.m_hp.nWeldShapeP1 = dlg.m_nWeldShapeP1;
	pDoc->m_work.m_hp.nWeldZigZagFreqP1 = dlg.m_nWeldZigZagFreqP1;

	pDoc->m_work.m_hp.nWeldShapeP2 = dlg.m_nWeldShapeP2;
	pDoc->m_work.m_hp.nWeldZigZagFreqP2 = dlg.m_nWeldZigZagFreqP2;

	//pDoc->m_work.SetLayerPattern(dlg.m_n1ST, dlg.m_n2ND);

	// mspark 2022.04.22
	memcpy(pDoc->m_work.m_hp.dRptPowerProfile_1st, dlg.m_dPower1st, sizeof(double)*MAX_NUM_REPEAT_WELD);
	memcpy(pDoc->m_work.m_hp.dRptPowerProfile_2nd, dlg.m_dPower2nd, sizeof(double)*MAX_NUM_REPEAT_WELD);
	pDoc->m_work.m_hp.bUseRptPowerProfile_1st = dlg.m_bUseRptPowerProfile1st;
	pDoc->m_work.m_hp.bUseRptPowerProfile_2nd = dlg.m_bUseRptPowerProfile2nd;

	pDoc->m_work.m_hp.dPtnRotationAngle = dlg.m_dPtnRotation; // sjyi 2023.04.14 헤어핀 회전 추가
	pDoc->m_work.m_hp.dPtnRotationAngle2 = dlg.m_dPtnRotation2; // sjyi 2023.04.14 헤어핀 회전 추가

	pDoc->m_work.m_hp.bReversal = dlg.m_bUseReversal; // sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가

	pDoc->m_work.m_hp.HpSize2nd2 = dlg.m_dHpSize2ndW2;
	pDoc->m_work.m_hp.bCloseSpiralP1 = dlg.m_bCloseSpiralP1;
	pDoc->m_work.m_hp.bCloseSpiralP2 = dlg.m_bCloseSpiralP2;
	pDoc->m_work.m_hp.bUseOutToInP1 = dlg.m_bEnableOutToInP1;
	pDoc->m_work.m_hp.bUseOutToInP2 = dlg.m_bEnableOutToInP2;

	int n1st, n2nd;
	if (dlg.m_bUseSlotMode){
		n1st = dlg.m_n1STSlot;
		n2nd = dlg.m_n2NDSlot;
	}
	else{
		n1st = dlg.m_n1ST;
		n2nd = dlg.m_n2ND;
	}

	//pDoc->m_work.ModifyHairpin(dlg.m_n1ST, dlg.m_n2ND, TRUE, dlg.m_dPtnRotation, dlg.m_dPtnRotation2);
	//pDoc->m_work.ModifyHairpin(dlg.m_n1ST, dlg.m_n2ND, TRUE, dlg.m_dPtnRotation, dlg.m_dPtnRotation2, FALSE, dlg.m_bUseReversal);
	pDoc->m_work.ModifyHairpin(n1st, n2nd, TRUE, dlg.m_dPtnRotation, dlg.m_dPtnRotation2, FALSE, dlg.m_bUseReversal, dlg.m_bUseSlotMode);

	pDoc->m_work.ModifyHairpinWeldRepeat(pDoc->m_work.m_hp.nCntP1, pDoc->m_work.m_hp.nCntP2, dlg.m_n1ST, dlg.m_n2ND);	

	pDoc->m_work.AlignHairpin();

	pDoc->m_pMainFrm->m_wndWorkView.UpdateWork();
}
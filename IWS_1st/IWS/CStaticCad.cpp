#include "stdafx.h"
#include "CStaticCad.h"
#define STR_LAYER_WORKAREA_BK _T("WorkAreaBk")
#define STR_LAYER_IMAGE _T("Image")
#define STR_REFIMG_FILE _T("refImage.bmp")
#define CIRPOINT_SIZE 0.5
DECLARE_USER_MESSAGE(UWM_STATICCAD)

void CALLBACK EventProc(HANDLE hEvent)
{
	int EventType;
	CStaticCad* pApp = (CStaticCad*)lcPropGetHandle(hEvent, LC_PROP_EVENT_APPPRM2);
	if (pApp) {
		EventType = lcPropGetInt(hEvent, LC_PROP_EVENT_TYPE);
		switch (EventType) {
		case LC_EVENT_MOUSEMOVE: 
			pApp->OnMouseMove(hEvent);
			break;
		case LC_EVENT_SELECT:
			pApp->OnSelect(hEvent);
			break;
		case LC_EVENT_ENTMOVE:
			break;
		case LC_EVENT_ENTPROP:
			pApp->OnPropChange(hEvent);
			break;
		case LC_EVENT_LBDOWN:
			pApp->OnLBDown(hEvent);
			break;
		case LC_EVENT_KEYDOWN:  
			pApp->OnKeydown(hEvent);
			break;
		case LC_EVENT_ADDCMD:
			break;
		}
	}
}



CStaticCad::CStaticCad()
{
	m_lcWnd = NULL;
	m_lcDrw = NULL;
	m_lcWnd = NULL;
	m_hRefImg = NULL;
	m_hLayerImage = NULL;
	m_hRcvWnd = NULL;
	m_hCurrPos = NULL;
	m_imgSizeX = 100;
	m_imgSizeY = 100;

	m_dTxtStartAng = 90;
	m_dTxtPCD = 120;
	m_dTxtDir = -1;
	m_nSlot = 48;

	m_bDisplayhapinTxt = FALSE;
}


CStaticCad::~CStaticCad()
{
// 	lcUninitialize(true);
	if (m_lcDrw)
		lcDeleteDrawing(m_lcDrw);
	if (m_lcWnd)
		lcDeleteWindow(m_lcWnd);
}


BEGIN_MESSAGE_MAP(CStaticCad, CStatic)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CStaticCad::EnableGrip(BOOL bFlag)
{
	lcPropPutBool(m_lcDrw,LC_PROP_DRW_ENABLEGRIPS, bFlag);
}

void CStaticCad::clearOverlay()
{
	m_overlay.clear();
}

void CStaticCad::AddOverlay(int itype, double x0, double y0, double r)
{
	cad_overlay circle;
	circle.x0 = x0;
	circle.y0 = y0;
	circle.r = r;
	circle.iType = 1;
	m_overlay.push_back(circle);
}

void CStaticCad::AddOverlay(int itype, double x0, double y0, double x1, double y1)
{
	cad_overlay line;
	line.iType = 0;
	line.x0 = x0;
	line.y0 = y0;
	line.x1 = x1;
	line.y1 = y1;
	m_overlay.push_back(line);
}

void CStaticCad::UpdateOverlay()
{
	if (!m_hLayerImage) {
		m_hLayerImage = lcDrwAddLayer(m_lcDrw, STR_LAYER_IMAGE, L"100,100,100", 0, LC_LWEIGHT_DEFAULT);
	}
	lcPropPutHandle(m_lcDrw, LC_PROP_DRW_LAYER, m_hLayerImage);  // set active layer
	lcPropPutBool(m_hLayerImage, LC_PROP_LAYER_LOCKED, FALSE);

	for (auto p : m_overlay) {
		switch (p.iType)
		{
		case 0:
			AddLine(p.x0, p.y0, p.x1, p.y1);
			break;
		case 1:
			AddCircle(p.x0, p.y0, p.r);
			break;
		}
	}


	lcPropPutBool(m_hLayerImage, LC_PROP_LAYER_LOCKED, TRUE);
	HANDLE hLayer = lcDrwGetObjectByName(m_lcDrw, LC_OBJ_LAYER, L"0");
	lcPropPutHandle(m_lcDrw, LC_PROP_DRW_LAYER, hLayer);  // set active layer
	lcWndRedraw(m_lcWnd);
}

void CStaticCad::RedrawCad()
{
	lcWndRedraw(m_lcWnd);
}

HANDLE CStaticCad::GetBlockHandle()
{
	return lcDrwGetFirstObject(m_lcDrw, LC_OBJ_BLOCK);
}

void CStaticCad::ClearImage()
{
	HANDLE hBlock = lcDrwGetFirstObject(m_lcDrw, LC_OBJ_BLOCK);
	if (!m_hLayerImage) return;
	lcPropPutHandle(m_lcDrw, LC_PROP_DRW_LAYER, m_hLayerImage);  // set active layer
	lcPropPutBool(m_hLayerImage, LC_PROP_LAYER_LOCKED, FALSE);
	HANDLE hOldImg = lcDrwGetObjectByName(m_lcDrw, LC_OBJ_IMAGE, STR_REFIMG_FILE);
	if (hOldImg)
	{
		BOOL bret = lcDrwDeleteObject(m_lcDrw, hOldImg);
		lcBlockPurge(hBlock);
	}
	if (m_hRefImg) {
		lcDrwDeleteObject(m_lcDrw, m_hRefImg);
		m_hRefImg = NULL;
	}
	lcPropPutBool(m_hLayerImage, LC_PROP_LAYER_LOCKED, TRUE);
	HANDLE hLayer = lcDrwGetObjectByName(m_lcDrw, LC_OBJ_LAYER, L"0");
	lcPropPutHandle(m_lcDrw, LC_PROP_DRW_LAYER, hLayer);  // set active layer
	lcWndRedraw(m_lcDrw);

}

void CStaticCad::AddImage(CString strPath, double sizex, double sizey, double scx , double scy)
{
	if (sizex <= 0) m_imgSizeX = m_wsX;
	else m_imgSizeX = sizex;
	if (sizey <= 0) m_imgSizeY = m_wsY;
	else m_imgSizeY = sizey;

	HANDLE hBlock = lcDrwGetFirstObject(m_lcDrw, LC_OBJ_BLOCK);
	CString strRefImg = strPath;
	if (!m_hLayerImage) {
		m_hLayerImage = lcDrwAddLayer(m_lcDrw, STR_LAYER_IMAGE, L"100,100,100", 0, LC_LWEIGHT_DEFAULT);
	}
	lcPropPutHandle(m_lcDrw, LC_PROP_DRW_LAYER, m_hLayerImage);  // set active layer
	lcPropPutBool(m_hLayerImage, LC_PROP_LAYER_LOCKED, FALSE);
	HANDLE hOldImg = lcDrwGetObjectByName(m_lcDrw, LC_OBJ_IMAGE, STR_REFIMG_FILE);
	if (hOldImg)
	{
		BOOL bret = lcDrwDeleteObject(m_lcDrw, hOldImg);
		lcBlockPurge(hBlock);

	}
	HANDLE hImg = lcDrwAddImage(m_lcDrw, STR_REFIMG_FILE, strRefImg);
	if (!hImg) return ;

	if (m_hRefImg) {
		lcDrwDeleteObject(m_lcDrw, m_hRefImg);
		m_hRefImg = NULL;
	}
	double sx, sy;
	sx = -m_imgSizeX / 2.0 + m_wsXc + scx;
	sy = -m_imgSizeY / 2.0 + m_wsYc + scy;
	m_hRefImg = lcBlockAddImageRef(hBlock, hImg, sx, sy, m_imgSizeX, m_imgSizeY, TRUE);
	lcEntToBottom(m_hRefImg);
	lcPropPutBool(m_hLayerImage, LC_PROP_LAYER_LOCKED, TRUE);
	HANDLE hLayer = lcDrwGetObjectByName(m_lcDrw, LC_OBJ_LAYER, L"0");
	lcPropPutHandle(m_lcDrw, LC_PROP_DRW_LAYER, hLayer);  // set active layer
	lcEntUpdate(m_hRefImg);
	//lcDrwRegenViews(g_hLcDrw, 0);
	lcWndRedraw(m_lcDrw);
	lcEventsEnable(TRUE);
	lcWndExeCommand(m_lcWnd, LC_CMD_ZOOM_EXT, 0);
}

void CStaticCad::Clear()
{
	if (m_lcDrw)
		lcDeleteDrawing(m_lcDrw);
	m_lcDrw = lcCreateDrawing();
	lcPropPutInt(m_lcDrw, LC_PROP_DRW_LWMODE, LC_LW_REAL); //LC_LW_REAL, LC_LW_THIN
// 	lcPropPutInt(m_lcDrw, LC_LWIDTH_BYLAYER, LC_LW_PIXEL); //LC_LW_REAL, LC_LW_THIN
// 
// 	
// 	lcPropPutInt(m_lcDrw, LC_PROP_DRW_LWDEFAULT, 30);  // 0.25 mm
	lcPropPutInt(m_lcDrw, LC_PROP_DRW_COLOR, RGB(102, 204, 0));  //green
	lcPropPutInt(m_lcDrw, LC_PROP_G_GETDELENT, FALSE);
	lcPropPutBool(m_lcWnd, LC_PROP_WND_KBMOVE_ENABLE, TRUE);
	lcDrwNew(m_lcDrw, L"", m_lcWnd);
	m_hDrwWS = NULL; // work space handle
	m_hLayerImage = NULL;
	m_hRefImg = NULL;
	m_hCurrPos = NULL;
	HANDLE hBlock = lcDrwGetFirstObject(m_lcDrw, LC_OBJ_BLOCK);
	EnableGrip(FALSE);
	lcEventsEnable(FALSE);
	HANDLE hLayerWorkAreaBk = lcDrwGetObjectByName(m_lcDrw, LC_OBJ_LAYER, STR_LAYER_WORKAREA_BK);
	if (!hLayerWorkAreaBk) {
		hLayerWorkAreaBk = lcDrwAddLayer(m_lcDrw, STR_LAYER_WORKAREA_BK, L"100,100,100", 0, 100);
	}
	lcPropPutHandle(m_lcDrw, LC_PROP_DRW_LAYER, hLayerWorkAreaBk);  // set active layer
	lcPropPutBool(hLayerWorkAreaBk, LC_PROP_LAYER_LOCKED, FALSE);

	m_hDrwWS = lcBlockAddRect(hBlock, m_wsXc, m_wsYc, m_wsX, m_wsY, 0, FALSE);
	
	if(m_bDisplayhapinTxt)
		UpdateHapinTxt(hBlock);

	//lcBlockAddRect(hBlock, m_wsXc, m_wsYc, m_wsX, m_wsY, 0, TRUE);
	// 	lcDrwSetLimits(g_hLcDrw, -m_pDoc->m_config.m_dWorkAreaX / 2, -m_pDoc->m_config.m_dWorkAreaY / 2,
	// 		m_pDoc->m_config.m_dWorkAreaX / 2, m_pDoc->m_config.m_dWorkAreaY / 2);
	lcPropPutBool(m_lcDrw, LC_PROP_DRW_LIM_MAXON, TRUE);
	lcPropPutStr(m_hDrwWS, LC_PROP_ENT_COLOR, L"100,100,100"); //0x6B, 0x8E, 0x23
	lcPropPutBool(hLayerWorkAreaBk, LC_PROP_LAYER_LOCKED, TRUE);
	HANDLE hLayer = lcDrwGetObjectByName(m_lcDrw, LC_OBJ_LAYER, L"0");
	lcPropPutHandle(m_lcDrw, LC_PROP_DRW_LAYER, hLayer);  // set active layer
	lcPropPutInt(hLayer, LC_PROP_LAYER_COLOR, RGB(102, 204, 0));  //green
	lcPropPutInt(m_lcDrw, LC_PROP_DRW_COLOR, RGB(102, 204, 0));  //green
	lcPropPutInt(m_lcDrw, LC_PROP_DRW_LWDEFAULT, 30);  // 0.25 mm
	lcEventsEnable(TRUE);
	lcWndZoomRect(m_lcWnd, 0, 0, 0, 0);
	/*lcWndExeCommand(m_lcWnd, LC_CMD_ZOOM_EXT, 0);*/	m_hEntity.clear();
}

void CStaticCad::ClearSelect()
{
	lcWndExeCommand(m_lcWnd, LC_CMD_RESET, 0);
}

void CStaticCad::AddCirclePoint(double cx, double cy)
{
	HANDLE hBlock = lcDrwGetFirstObject(m_lcDrw, LC_OBJ_BLOCK);

	HANDLE handle = lcBlockAddCircle(hBlock, cx, cy, CIRPOINT_SIZE, FALSE);
	lcPropPutStr(handle, LC_PROP_ENT_COLOR, L"255,0,0");
	lcPropPutInt(handle, LC_PROP_ENT_LWIDTH, 200);
	m_hEntity.push_back(handle);
}

void CStaticCad::AddCircle(double cx, double cy, double radius)
{

	HANDLE hBlock = lcDrwGetFirstObject(m_lcDrw, LC_OBJ_BLOCK);
	HANDLE handle = lcBlockAddCircle(hBlock, cx, cy, radius, FALSE);
	lcPropPutStr(handle, LC_PROP_ENT_COLOR, L"230,230,230");
	lcPropPutInt(handle, LC_PROP_ENT_LWIDTH, 20);

}

void CStaticCad::AddLine(double x0, double y0, double x1, double y1)
{
	HANDLE hBlock = lcDrwGetFirstObject(m_lcDrw, LC_OBJ_BLOCK);

	HANDLE handle = lcBlockAddLine(hBlock, x0,y0,x1, y1);
	lcPropPutStr(handle, LC_PROP_ENT_COLOR, L"230,230,230");
	lcPropPutInt(handle, LC_PROP_ENT_LWIDTH, 20);	
}

void CStaticCad::ChangeEntity(int idx, HANDLE handle)
{
	HANDLE hBlock = lcDrwGetFirstObject(m_lcDrw, LC_OBJ_BLOCK);
	if (lcEntErase(m_hEntity[idx], TRUE)) {
		lcBlockPurge(hBlock);
		m_hEntity[idx] = handle;
		lcBlockSelectEnt(hBlock, handle, TRUE);
	}
}

void CStaticCad::ModifyPosition(int idx, double x, double y)
{
	
	HANDLE hBlock = lcDrwGetFirstObject(m_lcDrw, LC_OBJ_BLOCK);
	HANDLE handle = m_hEntity[idx];
	lcEventsEnable(FALSE);
	lcPropPutFloat(handle, LC_PROP_CIRCLE_X, x);
	lcPropPutFloat(handle, LC_PROP_CIRCLE_Y, y);
	lcPropPutFloat(handle, LC_PROP_CIRCLE_RAD, CIRPOINT_SIZE);
	lcBlockSelectEnt(hBlock, handle, TRUE);
	lcEntUpdate(handle);
	lcBlockUpdate(hBlock, TRUE, handle);
	lcEventsEnable(TRUE);
	//lcBlockUpdate(hBlock, TRUE, NULL);
	lcWndRedraw(m_lcWnd);
}
	
	

void CStaticCad::UpdateHapinTxt(HANDLE hBlock)
{
	double step = LC_PI / (double(m_nSlot) / 2.0f);

	double tx, ty;
	CString str;
	double dTxtStartAng = m_dTxtStartAng * LC_DEG_TO_RAD;
	double dTxtR = m_dTxtPCD / 2.0;
	double theta;
	for (int i = 0; i < m_nSlot; i++) {
		theta = step * i*m_dTxtDir + dTxtStartAng;
		tx = dTxtR * cos(theta);
		ty = dTxtR * sin(theta);
		str.Format(L"%d", i + 1);
		lcBlockAddText2(hBlock, str, tx, ty, LC_TA_CENTER, 2, 1, 0, false);
	}
}

void CStaticCad::AdjustDraw(double x0, double y0, double x1, double y1)
{
	lcWndZoomRect(m_lcWnd, x0,y0,x1,y1);
}

void CStaticCad::SetSelectEntity(int idx)
{
	if (idx >= m_hEntity.size()) return;
	HANDLE handle = m_hEntity[idx];
	HANDLE hBlock = lcDrwGetFirstObject(m_lcDrw, LC_OBJ_BLOCK);
	lcEventsEnable(FALSE);
	lcBlockUnselect(hBlock);
	TRACE("SetSelectEntity ===============================\n ");
	for (auto p : m_idxSelect) {
		handle = m_hEntity[p];
		lcBlockSelectEnt(hBlock, handle, TRUE);
		TRACE("SetSelectEntity ,%d\n", p);
	}
	TRACE("\n");
	lcWndRedraw(m_lcWnd);
	lcEventsEnable(TRUE);
}

void CStaticCad::BackUpDrwLimit()
{
	m_backupXmin = lcPropGetFloat(m_lcWnd, LC_PROP_WND_XMIN);
	m_backupYmin = lcPropGetFloat(m_lcWnd, LC_PROP_WND_YMIN);
	m_backupXmax = lcPropGetFloat(m_lcWnd, LC_PROP_WND_XMAX);
	m_backupYmax = lcPropGetFloat(m_lcWnd, LC_PROP_WND_YMAX);
}

void CStaticCad::RestoreDrwLimt()
{
	lcWndZoomRect(m_lcWnd, m_backupXmin, m_backupYmin, m_backupXmax, m_backupYmax);
	lcWndRedraw(m_lcWnd);
}

void CStaticCad::OnMouseMove(HANDLE hEvent)
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
		::SendMessage(m_hRcvWnd, UWM_STATICCAD, CAD_EVENT_MOUSEMOVE,0);
}

void CStaticCad::OnPropChange(HANDLE hEvent)
{
	HANDLE hEntity = lcPropGetHandle(hEvent, LC_PROP_EVENT_ENTITY);
	int nsize = (int)m_hEntity.size();
	int idx = 0;
	
	for (int i = 0; i < nsize; i++) {
		if (m_hEntity[i] == hEntity) {
			m_posx = lcPropGetFloat(hEntity, LC_PROP_CIRCLE_X);
			m_posy = lcPropGetFloat(hEntity, LC_PROP_CIRCLE_Y);
			TRACE("OnPropChange : %d\n", i);
			if (m_hRcvWnd)
				::SendMessage(m_hRcvWnd, UWM_STATICCAD, CAD_EVENT_ENTCHANGE, i);
			break;
		}
	}
	
}

void CStaticCad::OnLBDown(HANDLE hEvent)
{
	HANDLE hLcWnd = lcPropGetHandle(hEvent, LC_PROP_EVENT_WND);
	BOOL bCtrl = lcPropGetInt(hEvent, LC_PROP_EVENT_INT4);
	if (!bCtrl) return;
	m_posx = lcPropGetFloat(hLcWnd, LC_PROP_WND_CURX);
	m_posy = lcPropGetFloat(hLcWnd, LC_PROP_WND_CURY);
	if (m_hCurrPos) {
		lcPropPutFloat(m_hCurrPos, LC_PROP_POINT_X, m_posx);
		lcPropPutFloat(m_hCurrPos, LC_PROP_POINT_Y, m_posy);
	}
	else {
		HANDLE hBlock = lcDrwGetFirstObject(m_lcDrw, LC_OBJ_BLOCK);
		m_hCurrPos = lcBlockAddPoint(hBlock, m_posx, m_posy);
	}
	if (m_hRcvWnd)
		::PostMessage(m_hRcvWnd, UWM_STATICCAD, CAD_EVENT_LBDOWN, 0);
}

void CStaticCad::OnKeydown(HANDLE hEvent)
{
	int iky0 = lcPropGetInt(hEvent, LC_PROP_EVENT_INT1);
	int iky1 = lcPropGetInt(hEvent, LC_PROP_EVENT_INT2);
	int iky2 = lcPropGetInt(hEvent, LC_PROP_EVENT_INT3);
	int iky3 = lcPropGetInt(hEvent, LC_PROP_EVENT_INT4);
	TRACE("iky0=%d, iky0=%d, iky0=%d, iky0=%d, keydn=%d\n", iky0, iky1, iky2, iky3, VK_RIGHT);
	HANDLE hBlock = lcDrwGetFirstObject(m_lcDrw, LC_OBJ_BLOCK);
	HANDLE handle = lcBlockGetFirstSel(hBlock);
	int nSelEnts = lcPropGetInt(hBlock, LC_PROP_BLOCK_NSELENTS);

	if (!handle) return;
	double cx, cy;
	cx = lcPropGetFloat(handle, LC_PROP_CIRCLE_X);
	cy = lcPropGetFloat(handle, LC_PROP_CIRCLE_Y);
	switch (iky0)
	{
	case 65:// left, a
		while (handle)
		{
			lcEntMove(handle, -0.1, 0);
			handle = lcBlockGetNextSel(hBlock);
		}
		
		break;
	case 68:// right, d
		while (handle)
		{
			lcEntMove(handle, 0.1, 0);
			handle = lcBlockGetNextSel(hBlock);
		}
		break;
	case 87:// up, w
		while (handle)
		{
			lcEntMove(handle, 0,0.1);
			handle = lcBlockGetNextSel(hBlock);
		}
		break;
	case 83:// down, s	  
		while (handle)
		{
			lcEntMove(handle, 0,-0.1);
			handle = lcBlockGetNextSel(hBlock);
		}
		break;
	}

	lcEntUpdate(handle);
	lcBlockUpdate(hBlock,TRUE, handle);
	lcWndRedraw(m_lcWnd);
}

void CStaticCad::OnSelect(HANDLE hEvent)
{
	HANDLE hEntity = lcPropGetHandle(hEvent, LC_PROP_EVENT_ENTITY);
	BOOL bSelect = lcPropGetInt(hEvent, LC_PROP_EVENT_INT1);
	BOOL nSel = lcPropGetInt(hEvent, LC_PROP_EVENT_INT2);
	BOOL nTotal = lcPropGetInt(hEvent, LC_PROP_EVENT_INT3);
	m_idxSelect.clear();
	if (nTotal == 0 && nSel != 0) {
		if (m_hRcvWnd)
			::PostMessage(m_hRcvWnd, UWM_STATICCAD, CAD_EVENT_SELECTCLEAR, 0);
	}
	TRACE("OnSelectStart Event : %ld, bSel(%d), nSel(%d), nTotal(%d)\n", hEntity, bSelect, nSel, nTotal);;
	int nsize = (int)m_hEntity.size();
	int idx = 0;
	HANDLE hBlock  = lcPropGetHandle(hEvent, LC_PROP_EVENT_BLOCK);
	hEntity = lcBlockGetFirstSel(hBlock);
	while (hEntity) {
		for (int i = 0; i < nsize; i++) {
			if (m_hEntity[i] == hEntity) {

				TRACE("OnSelect = %d, bSelect=%d\n", i, bSelect);
				m_idxSelect.push_back(i);
				break;
			}
		}
		hEntity = lcBlockGetNextSel(hBlock);
	}
	if (m_idxSelect.size()) {
		if (m_hRcvWnd)
			::PostMessage(m_hRcvWnd, UWM_STATICCAD, CAD_EVENT_ENTSENECT, MAKELPARAM(m_idxSelect[0], bSelect));
	}
	lcEventReturnCode(1);

}

void CStaticCad::InitStaticCad()
{

#ifdef _STATIC_CAD_CALLBACK_USE
	lcEventSetProc(LC_EVENT_MOUSEMOVE, EventProc, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_SELECT, EventProc, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_ENTMOVE, EventProc, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_ENTPROP, EventProc, 0, (HANDLE)this);
	lcEventSetProc(LC_EVENT_KEYDOWN, EventProc, 0, (HANDLE)this);	
	lcEventSetProc(LC_EVENT_LBDOWN, EventProc, 0, (HANDLE)this);
#endif // _STATIC_CAD_CALLBACK_USE
	CString str = lcPropGetStr(NULL, LC_PROP_G_DIRCFG);
	CString str16 = str + L"\\iws16.ico";
	CString str32 = str + L"\\iws32.ico";
	lcPropPutStr(0, LC_PROP_G_ICON16, str16);
	lcPropPutStr(0, LC_PROP_G_ICON32, str32);
	lcPropPutStr(0, LC_PROP_G_MSGTITLE, L"IWS");

	lcPropPutStr(0, LC_PROP_G_REGCODE, L"c4b8-467d-91b9-9e63");
// 	lcInitialize();  // loads "Litecad.cfg"

	int Style;
	Style = /*LC_WS_HSCROLL | LC_WS_VSCROLL |*/ LC_WS_BORDER | LC_WS_DEFAULT | LC_WS_RULERS;
	Style &= ~(LC_WS_HSCROLL | LC_WS_VSCROLL);
	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	/*GetDlgItem(IDC_STATIC_WORKDISPLAY)->GetSafeHwnd()*/
	m_lcWnd = lcCreateWindow(this->GetSafeHwnd(), Style);
	
	Clear();
}

void CStaticCad::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	if (m_lcWnd) {
		lcWndResize(m_lcWnd, 0, 0, cx, cy);
	}
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


BOOL CStaticCad::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_RBUTTONDOWN) {
		return TRUE;
	}
	return CStatic::PreTranslateMessage(pMsg);
}

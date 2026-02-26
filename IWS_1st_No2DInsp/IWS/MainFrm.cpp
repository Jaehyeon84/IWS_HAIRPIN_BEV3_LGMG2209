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

// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "IWS.h"

#include "MainFrm.h"
#include "IWSView.h"
#include "HomeView.h"
#include "EntitySpiral.h"
#include "VRSaux/GrabImgDlg.h"
#include "VRSaux/DispImgDlg.h"
#include "CDlgVScanPositionSet.h"
#include "CDlgLaserPenHiddern.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
DECLARE_USER_MESSAGE(UWM_ACTIVE_RIBBON_TAB_CHANGED)
DECLARE_USER_MESSAGE(UWM_CIWSWORK)
DECLARE_USER_MESSAGE(UWM_INTERFACE_THREAD)

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(UWM_ACTIVE_RIBBON_TAB_CHANGED, &CMainFrame::OnUwmActiveRibbonTabChanged)
	ON_REGISTERED_MESSAGE(UWM_CIWSWORK, &CMainFrame::OnUwmCIWSwork)
	ON_REGISTERED_MESSAGE(UWM_INTERFACE_THREAD, &CMainFrame::OnInterfaceThread)
	ON_WM_CLOSE()
	ON_COMMAND(ID_ACC_VIEW_HOME, &CMainFrame::OnAccViewHome)
	ON_COMMAND(ID_ACC_VSCAN, &CMainFrame::OnAccVscan)
	ON_COMMAND(ID_ACC_VIEW_DRW, &CMainFrame::OnAccViewDrw)
	ON_COMMAND(ID_ACC_LASERPEN, &CMainFrame::OnAccLaserpen)
	ON_COMMAND(ID_ACC_REFIMG_SETTING, &CMainFrame::OnAccRefimgSetting)
	ON_COMMAND(ID_ACC_WELDONCE, &CMainFrame::OnAccWeldonce)
	ON_COMMAND(ID_FILE_SET_VSCAN, &CMainFrame::OnFileSetVscan)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CMainFrame::OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SHOW_INSPECT, &CMainFrame::OnUpdateFileShowInspect)
	ON_UPDATE_COMMAND_UI(ID_FILE_DXF, &CMainFrame::OnUpdateFileDxf)
	ON_UPDATE_COMMAND_UI(ID_FILE_HP_CONFIG, &CMainFrame::OnUpdateFileHpConfig)
	ON_UPDATE_COMMAND_UI(ID_FILE_SET_VSCAN, &CMainFrame::OnUpdateFileSetVscan)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOCATION_SAVE, &CMainFrame::OnUpdateFileLocationSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOCATION_LOAD, &CMainFrame::OnUpdateFileLoadAs)
	ON_UPDATE_COMMAND_UI(ID_MATRIX, &CMainFrame::OnUpdateMatrix)
	ON_UPDATE_COMMAND_UI(ID_MATRIX_PCC, &CMainFrame::OnUpdateMatrixPcc)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_HAIRPIN, &CMainFrame::OnUpdateButtonHairpin)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_HAIRPIN_CONFIG, &CMainFrame::OnUpdateButtonHairpinConfig)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_HAIRPIN_OFFSET, &CMainFrame::OnUpdateButtonHairpinOffset)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PATTERN, &CMainFrame::OnUpdateButtonPattern)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_LASER_PENS, &CMainFrame::OnUpdateButtonLaserPens)
END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	m_pDoc = NULL;
	m_bInitCadFrame = FALSE;
	m_pLiveCCDDlg = NULL;
	m_hAccelTable = NULL;
}

CMainFrame::~CMainFrame()
{
	if (m_pLiveCCDDlg != NULL) { delete m_pLiveCCDDlg; }
	delete g_pGrabImgDlg;
}

void CMainFrame::FitWorkAreaToFrame()
{
	lcWndExeCommand(g_hLcWnd, LC_CMD_ZOOM_EXT, 0);
	double x, y;
	m_pDoc->m_config.GetWorkRng(&x, &y);
	double Xmin = -x/2.0;
	double Ymin = -y/2.0;
	double Xmax = x/2.0;
	double Ymax = y/2.0;


	lcPropPutFloat(g_hLcWnd, LC_PROP_WND_XMIN, Xmin - 10);
	lcPropPutFloat(g_hLcWnd, LC_PROP_WND_YMIN, Ymin - 10);
	lcPropPutFloat(g_hLcWnd, LC_PROP_WND_XMAX, Xmax + 10);
	lcPropPutFloat(g_hLcWnd, LC_PROP_WND_YMAX, Ymax + 10);

	lcWndZoomRect(g_hLcWnd, Xmin-10, Ymin-10, Xmax+10, Ymax+10);

//	HANDLE hView = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_VIEW);
// 	lcPropPutBool(hView, LC_PROP_VIEW_LIMITS, TRUE);
// 	double Lef = lcPropGetFloat(hView, LC_PROP_VIEW_LEF);
// 	double Bot = lcPropGetFloat(hView, LC_PROP_VIEW_BOT);
// 	double Rig = lcPropGetFloat(hView, LC_PROP_VIEW_RIG);
// 	double Top = lcPropGetFloat(hView, LC_PROP_VIEW_TOP);
// 	lcViewSetLimits(hView, Lef - 10, Bot - 10, Rig + 10, Top + 10);
// 	lcViewSetRect2(hView, Lef - 10, Bot - 10, Rig + 10, Top + 10);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	////리본바메인버튼 제거
	//m_wndRibbonBar.DelAppButton();
	//
	////리본바QA메뉴들 제거
	m_wndRibbonBar.DelQAT();

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);

	// 빌드 날짜 추가_빌드일 확인하기 위함_230118_hajh
	strTitlePane2.Format (_T ("IWS Build Date. %s"), _T (__DATE__));

	//m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBarPaneXY = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE);	
	m_wndStatusBar.AddElement(m_wndStatusBarPaneXY, strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 메뉴 항목 이미지를 로드합니다(표준 도구 모음에 없음).
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 도킹 창을 만듭니다.
	if (!CreateDockingWindows())
	{
		TRACE0("도킹 창을 만들지 못했습니다.\n");
		return -1;
	}

	//m_wndDrawProp.EnableDocking(CBRS_ALIGN_ANY);
	m_wndPropertiesView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndWorkView.EnableDocking(CBRS_ALIGN_LEFT);
	CDockablePane* pTabbedBar = nullptr;
	//DockPane(&m_wndDrawProp);

	//m_wndPropertiesView.AttachToTabWnd(&m_wndDrawProp, DM_SHOW, TRUE, &pTabbedBar);
	DockPane(&m_wndPropertiesView);
	DockPane(&m_wndWorkView);
	m_wndDrawCmdView.EnableDocking(CBRS_BOTTOM);
	DockPane(&m_wndDrawCmdView);

	m_wndJobListView.EnableDocking(CBRS_ALIGN_LEFT);
	DockPane(&m_wndJobListView);
	// 모든 사용자 인터페이스 요소를 그리는 데 사용하는 비주얼 관리자를 설정합니다.
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);

	m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR_IWS));

	// hslee
	//m_wndRibbonBar.ShowCategory(2, false);
	//m_wndRibbonBar.ShowCategory(3, false);

	// 2024.02.15 jh.kim 주석 제거
	if (m_pDoc->m_config.m_bUseHairpin)
		m_wndRibbonBar.ShowCategory(2, false);
	else
		m_wndRibbonBar.ShowCategory(3, false);
	
	return 0;
}

void CMainFrame::InitViews()
{
	m_nCurView = 0;        // Save index of the currently active view class
	CView* pActiveView = this->GetActiveView();

	m_pViews[DRAW_VIEW] = pActiveView;
	m_pViews[HOME_VIEW] = (CView *) new CHomeView;

	CStringArray strClassName;
	strClassName.Add(_T(STRINGIFY(CIWSView)));
	strClassName.Add(_T(STRINGIFY(CHomeView)));

	CDocument* pCurrentDoc = GetActiveDocument();
	CCreateContext newContext;
	newContext.m_pNewViewClass = NULL;
	newContext.m_pNewDocTemplate = NULL;
	newContext.m_pLastView = NULL;
	newContext.m_pCurrentFrame = NULL;
	newContext.m_pCurrentDoc = pCurrentDoc;
	CRect rect(0, 0, 0, 0); // gets resized later

	for (int nView = 1; nView < VIEW_END; nView++)
	{
		m_pViews[nView]->Create(NULL, strClassName[nView],
			(AFX_WS_DEFAULT_VIEW & ~WS_VISIBLE),
			// views are created with the style of AFX_WS_DEFAULT_VIEW
			// In MFC 4.0, this is (WS_BORDER | WS_VISIBLE | WS_CHILD)
			rect, this,
			AFX_IDW_PANE_FIRST + nView, &newContext);

		// When a document template creates a view, the WM_INITIALUPDATE
		// message is sent automatically. However, this code must
		// explicitly send the message, as follows.
		m_pViews[nView]->SendMessage(WM_INITIALUPDATE, 0, 0);

	}

	m_pLiveCCDDlg = new CDispImgDlg(m_pDoc);
	m_pLiveCCDDlg->Create(IDD_CAMERA_VIEW_DLG);
	g_pGrabImgDlg = new CGrabImgDlg(m_pDoc);
	g_pGrabImgDlg->Create(IDD_GRAB_DLG);
	
	if (g_pGrabImgDlg->m_pCamera != nullptr)
		((CHomeView*)m_pViews[HOME_VIEW])->m_statusCCD.On();

	// set pointer for access
	g_pGrabImgDlg->SetLiveCCDDlgPtr(m_pLiveCCDDlg);
	m_wndDrawCmdView.InitPane();
}

void CMainFrame::SwitchView(UINT id)
{
	const int nIndex = id;
	BOOL bHide;

	ASSERT(nIndex >= 0 && nIndex < VIEW_END);
	if (nIndex < 0 || nIndex >= VIEW_END)
		return;
	
	CView* pNewView = m_pViews[nIndex];
	if (!pNewView)
		return;
	CView* pActiveView = GetActiveView();
	if (!pActiveView)    // No currently active view
		return;
	if (!pActiveView)    // No currently active view
		return;

	if (pNewView == pActiveView)    // Already there
		return;
	//CView* pOldView = m_pViews[m_nCurView];
	m_nCurView = nIndex;    // Store the new current view's index
	// exchange view window ID's so RecalcLayout() works

	UINT temp = ::GetWindowLong(pActiveView->m_hWnd, GWL_ID);
	::SetWindowLong(pActiveView->m_hWnd, GWL_ID, ::GetWindowLong(pNewView->m_hWnd, GWL_ID));
	::SetWindowLong(pNewView->m_hWnd, GWL_ID, temp);
	//m_pDoc->AddView(pNewView);
	//m_pDoc->RemoveView(pOldView);
	// Display and update the new current view - hide the old one    
	bHide = m_wndRibbonBar.GetHideFlags() == AFX_RIBBONBAR_HIDE_ELEMENTS;
	TRACE("RibboneBar show = %d\n", bHide);
	
	pActiveView->ShowWindow(SW_HIDE);
	pNewView->ShowWindow(SW_SHOW);
	SetActiveView(pNewView);
	bHide = m_wndRibbonBar.GetHideFlags() == AFX_RIBBONBAR_HIDE_ELEMENTS;
	TRACE("RibboneBar show = %d\n", bHide);
	switch (nIndex)
	{
	case DRAW_VIEW:
		m_wndDrawCmdView.ShowPane(TRUE, FALSE, TRUE);		
		m_wndPropertiesView.ShowPane(TRUE, FALSE, TRUE);
		m_wndWorkView.ShowPane(FALSE, FALSE, FALSE);

		if (m_pDoc->m_config.m_bUseHairpin)
			m_wndJobListView.ShowPane(FALSE, FALSE, TRUE);
		else
			m_wndJobListView.ShowPane(TRUE, FALSE, TRUE);

		FitWorkAreaToFrame();
		/*if (bHide) {
			m_wndRibbonBar.ToggleMimimizeState();
		}*/
		break;
	case HOME_VIEW:
		m_wndDrawCmdView.ShowPane(FALSE, FALSE, FALSE);
		m_wndJobListView.ShowPane(FALSE, FALSE, FALSE);
		m_wndPropertiesView.ShowPane(FALSE, FALSE, FALSE);
		m_wndWorkView.ShowPane(TRUE, FALSE, TRUE);
		/*if (!bHide) {
			m_wndRibbonBar.ToggleMimimizeState();
		}*/
		m_wndWorkView.UpdateWork();
		break;
	}
	RecalcLayout();
	pNewView->Invalidate();
}

void CMainFrame::InitializeIWS()
{
	//m_pLiveCCDDlg->StartDisplay();
	m_pDoc->InitializeIWS();
	SwitchView(HOME_VIEW);
	m_pLiveCCDDlg->StartDisplay();

	if (::GetSystemMetrics(SM_CMONITORS) > 1) 
		((CHomeView*)m_pViews[HOME_VIEW])->OnButtonCcdliive();
	else
		m_pLiveCCDDlg->ShowWindow(FALSE);
}

void CMainFrame::TerminateIWS()
{
	m_pDoc->TerminateIWS();
}

void CMainFrame::ResetInterfaceFlag()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{

	CString strDrawProp = L"Property";

	CString strWorkView = L"WorkView";
	if (!m_wndWorkView.Create(strWorkView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_WORK, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("LiveCCD 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	m_wndWorkView.InitPane(m_pDoc);
	CString strDrawCmd= L"DrawCommand";
	if (!m_wndDrawCmdView.Create(strDrawCmd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_DRAWCOMMANDWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("DrawCommand 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	CString strJobList = L"JobList";
	if (!m_wndJobListView.Create(strJobList, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_JOBLISTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("JobList 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	m_wndJobListView.SetDocument(m_pDoc);

	CString strProperties = L"Property";
	if (!m_wndPropertiesView.Create(strProperties, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT| CBRS_FLOAT_MULTI))
	{
		TRACE0("Property 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
	m_wndPropertiesView.SetDocument(m_pDoc);

	SetDockingWindowIcons(theApp.m_bHiColorIcons);

	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndDrawCmdView.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndPropertiesView.SetIcon(hPropertiesBarIcon, FALSE);
}

// CMainFrame 진단
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

// CMainFrame 메시지 처리기
afx_msg LRESULT CMainFrame::OnUwmActiveRibbonTabChanged(WPARAM wParam, LPARAM lParam)
{
	int iActivated = (int)wParam;
	//if (iActivated == 1) SwitchView(MODEL_VIEW);

	switch (iActivated)
	{
 	case 1: SwitchView(HOME_VIEW); break;
 	case 2: SwitchView(DRAW_VIEW); break;
	case 3: SwitchView(DRAW_VIEW); break;	// hslee
	}
// #ifdef _DEBUG
// 	DebugTrace(_T("ActiveRibbonTabChange = %d\n"), iActivated);
// #endif  //_DEBUG
	return 0;
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_pDoc = (CIWSDoc*)pContext->m_pCurrentDoc;
	return CFrameWndEx::OnCreateClient(lpcs, pContext);
}

void CMainFrame::LoadVSCanImg()
{
	double x, y, cx, cy;
	double imgx, imgy;
	m_pDoc->m_config.GetWorkRng(&x, &y);
	m_pDoc->m_config.GetRefImgCnt(&cx, &cy);
	m_pDoc->GetRefImgSize(&imgx, &imgy);
	//m_pDoc->m_config.GetRefImgRng(&imgx, &imgy);
	//m_pDoc->Update2DPosition();

	////DHChoi 
	//double scaleX, scaleY;
	//m_pDoc->m_config.GetRefImgScale(&scaleX, &scaleY);
	//cx *= scaleX;
	//cy *= scaleY;

	m_wndWorkView.UpdateWorkImage(m_pDoc->m_strImgPath + STR_REFIMG_FILE, imgx, imgy); //m_pDoc->m_work.m_fRefImgSizeW, m_pDoc->m_work.m_fRefImgSizeH);
	m_wndWorkView.SetWorkspace(x, y, cx, cy);

	m_pDoc->m_pDrawView->OnButtonLoadScoutimg();

	m_pDoc->m_work.AlignHairpin();
	m_wndWorkView.UpdateWork();
}

afx_msg LRESULT CMainFrame::OnUwmCIWSwork(WPARAM wParam, LPARAM lParam)
{
	BOOL bRes = FALSE;
	/*if(g_bMode)
		{
			m_pDoc->m_nLastRcpIdx = 1;

			CString strFilename = m_pDoc->GetExePath() + L"\\Work\\";
			strFilename = strFilename + m_pDoc->m_config.m_arWorkList[1];
			m_pDoc->OpenWork(strFilename);
		}*/

	switch (wParam) 
	{
	case EVENT_WORK_CURRENT:
		m_wndWorkView.SetCurrent((int)lParam);
		break;
	case EVENT_WORK_VSCAN:
		_tracer(TRACE_NORMAL, 9999, L"EVENT_WORK_VSCAN");
		LoadVSCanImg();

		//gseom_240704_2DInsp 주석 추가-->
		// Set Inspect Response ON to PLC
		/*if (m_pDoc->GetDo2DInspection() && m_pDoc->m_plc.GetReqInspect())
		{
			m_pDoc->m_plc.SetInspectReq(TRUE);
		}*/
		//gseom_240704_2DInsp 주석 추가<--
		//else if (m_pDoc->GetAllwaysScan() && m_pDoc->m_plc.GetReqInspect()) gseom_240704_2DInsp 주석 추가로 코드수정
		if (m_pDoc->GetAllwaysScan() && m_pDoc->m_plc.GetReqInspect())
		{
			m_pDoc->m_plc.SetInspectReq(TRUE);
		}

		// Set WeldDone ON to PLC
		if (m_pDoc->GetAllwaysScanAftWelding() && m_pDoc->GetUseInterface() && m_pDoc->m_plc.GetWeldRun())
		{
			// Set Weld Done ON to PLC
			if (m_pDoc->GetUseInterface())
				m_pDoc->m_plc.SetWeldComplete(TRUE);

			// Set Weld Run OFF to PLC
			m_pDoc->m_plc.SetWeldRun(FALSE);
		}

		break;
	case EVENT_WORK_INSPECT:
		LoadVSCanImg();
//		m_pDoc->m_plc.SetInspectComplete(TRUE);
		bRes = m_pDoc->m_work.GetInspectjudgement();
		if (!m_pDoc->m_config.m_bUseInspect)
			bRes = TRUE;

		//m_pDoc->m_plc.SetInspectOK(bRes);	
		//gseom_240704_2DInsp 주석 추가-->
		//if (m_pDoc->m_config.m_bUse2DProc)
		//	m_pDoc->Update2DPosition();
		//gseom_240704_2DInsp 주석 추가<--

		((CHomeView*)m_pViews[HOME_VIEW])->UpdateCycleTime();
		break;
	case EVENT_WORK_WELDDONE:
		/*m_pDoc->m_pScanner->LaserEnable(TRUE, 0);*/
		//m_pDoc->m_plc.SetWeldReq(TRUE);
		//m_pDoc->m_plc.SetLaserPower(m_pDoc->m_work.GetAverageLaserPower());

		// Set WeldDone ON to PLC
		if (m_pDoc->GetAllwaysScanAftWelding() && m_pDoc->GetUseInterface() && m_pDoc->m_plc.GetWeldRun())
		{
			// VisionScan
			m_pDoc->m_work.DoVScan();

			//// VScan 완료 확인
			//if (m_pDoc->m_work.m_bRunVScan)
			//{
			//	while (1)
			//	{
			//		if (g_pGrabImgDlg->IsDoneStitchImage())
			//		{
			//			break;
			//		}
			//	}
			//}
		}
		else
		{
			// Set Weld Done ON to PLC
			if (m_pDoc->GetUseInterface())
				m_pDoc->m_plc.SetWeldComplete(TRUE);

			// Set Weld Run OFF to PLC
			m_pDoc->m_plc.SetWeldRun(FALSE);
		}
		
		// 용접 완료 시간 Check
		((CHomeView*)m_pViews[HOME_VIEW])->UpdateCycleTime();
		break;
	}
	return 0;
}


void CMainFrame::OnAccHomeview()
{
	SwitchView(HOME_VIEW);
}

void CMainFrame::OnAccDrawview()
{
	SwitchView(DRAW_VIEW);
}

afx_msg LRESULT CMainFrame::OnInterfaceThread(WPARAM wParam, LPARAM lParam)
{
	if (wParam == INTERFACE_WELD_START) 
	{
		if (theApp.m_bWorkOn)
			return 0;

		m_pDoc->m_plc.SetWeldReq(TRUE);

		theApp.m_bWorkOn = TRUE;
		m_pDoc->m_work.m_bSimulation = FALSE;
		
		double dPower = 1.0;
		double dSpeed = 1.0;
		BOOL bRepair = m_pDoc->m_plc.GetReWeld(); // 2 = 재용접, 그 외 = 본용접
		if (bRepair)
		{
			dPower = m_pDoc->m_work.m_hp.dblPowerRatio / 100.f;
			dSpeed = m_pDoc->m_work.m_hp.dblSpeedRatio / 100.f;

			_tracer(TRACE_NORMAL, 1, L"Start Repair Weld");
		}
		else
		{
			_tracer(TRACE_NORMAL, 1, L"Start Weld");
		}
		
		if (!m_pDoc->StartWork(FALSE, dPower, dSpeed))
			_tracer(TRACE_ERROR, 1, L"Start weld fail");
		else
			m_pDoc->m_plc.SetAlarm(TRUE);

		SetEvent(m_pDoc->m_hEvtOpenRecipe);
		//int iRcp = (int)lParam;
		//
		//CString strFilename = m_pDoc->GetExePath() + L"\\Work\\";
		//if (iRcp < m_pDoc->m_config.m_arWorkList.GetSize())
		//	strFilename = strFilename + m_pDoc->m_config.m_arWorkList[iRcp];
		//if (m_pDoc->OpenWork(strFilename)) { // 
		//	theApp.m_bWorkOn = TRUE;
		//	m_pDoc->m_work.m_bSimulation = FALSE;
		//	SetEvent(m_pDoc->m_hEvtOpenRecipe);
	//		if(!m_pDoc->StartWork(FALSE))
		//		_tracer(TRACE_ERROR, 1, L"start weld fail");
		//	m_pDoc->m_plc.SetWeldReq(TRUE);
		//}else
		//	theApp.m_bWorkOn = FALSE;
	}
	else if (wParam == INTERFACE_INSPECT_START)
	{
		if (theApp.m_bWorkOn)
			return 0;
		
		_tracer(TRACE_NORMAL, 1, L"Receiveed inspection Request From PLC");

		int iRcp = (int)lParam;
		CString strFilename = m_pDoc->GetExePath() + L"\\Work\\";
		if (iRcp < m_pDoc->m_config.m_arWorkList.GetSize())
			strFilename = strFilename + m_pDoc->m_config.m_arWorkList[iRcp];

		if (m_pDoc->OpenWork(strFilename)) 
		{
			theApp.m_bWorkOn = TRUE;

			if (!m_pDoc->Update3DPosition())
			{
				m_pDoc->m_plc.SetAlarm(TRUE);
				_tracer(TRACE_ERROR, 1, L"Vision Data Read Fail");
				theApp.m_bWorkOn = FALSE;
				return FALSE;
			}

			// sjyi 2023.09.12 2D 검사를 위한 내용 추가 -->
			if (m_pDoc->m_plc.GetReWeld() == TRUE)
			{
				if (!m_pDoc->Update3DPosition())
				{
					m_pDoc->m_plc.SetAlarm(TRUE);
					_tracer(TRACE_ERROR, 1, L"Vision Data Read Fail");
					theApp.m_bWorkOn = FALSE;
					return FALSE;
				}
			}

			//_tracer(TRACE_NORMAL, 999, L"[OnInterfaceThread()] Check Vision m_bUseAlwaysScan : %d, GetReWeld : %d", m_pDoc->m_config.m_bUseAlwaysScan, m_pDoc->m_plc.GetReWeld());
			//_tracer(TRACE_NORMAL, 999, L"[OnInterfaceThread()] Check Vision m_bUseAlwaysScan : %d, GetReWeld : %d", m_pDoc->GetAllwaysScan(), m_pDoc->m_plc.GetReWeld());
			_tracer(TRACE_NORMAL, 999, L"[OnInterfaceThread()] Check Vision m_bUseAlwaysScan(%d) : %d, GetReWeld : %d", m_pDoc->m_plc.GetWorkJig() , m_pDoc->GetAllwaysScan(), m_pDoc->m_plc.GetReWeld());
			// sjyi 2023.06.12 Vision Scan 항상 사용 옵션 추가
			// Scan 추가
			// 항상 스캔하는 모드이고, 재용접이 아닐 경우에만 스캔 실행
			//if (m_pDoc->m_config.m_bUseAlwaysScan )
			// sjyi 2023.07.18 m_bUseAlwasScan 사용 안함(사유, PLC에서 2D모드-스캔모드 활성화)
			//if ((m_pDoc->m_config.m_bUseAlwaysScan == TRUE) && (m_pDoc->m_plc.GetReWeld() == FALSE))

			// 2D 검사모드이면 스캔 해야 함.
			//if ((m_pDoc->GetDo2DInspection() == TRUE) && (m_pDoc->m_plc.GetReWeld() == FALSE))
			//{				
			//	// VisionScan
			//	m_pDoc->m_work.DoVScan();

			//	// VScan 완료 확인
			//	if (m_pDoc->m_work.m_bRunVScan)
			//	{
			//		while (1)
			//		{
			//			if (g_pGrabImgDlg->IsDoneStitchImage())
			//			{
			//				break;
			//			}
			//		}
			//	}
			//}
			//else if ((m_pDoc->m_config.m_bUseAlwaysScan == TRUE) && (m_pDoc->m_plc.GetReWeld() == FALSE)) // sjyi 2023.10.13 Add for VSCAN
			//else if (m_pDoc->GetAllwaysScan()) // sjyi 2024.05.28 재용접포함하여 스캔하는 모드인지 확인하도록 수정
			if (m_pDoc->GetAllwaysScan()) // sjyi 2024.05.28 재용접포함하여 스캔하는 모드인지 확인하도록 수정
			{
				// VisionScan
				m_pDoc->m_work.DoVScan();

				// VScan 완료 확인
				if (m_pDoc->m_work.m_bRunVScan)
				{
					while (1)
					{
						if (g_pGrabImgDlg->IsDoneStitchImage())
						{
							break;
						}
					}
				}
			}
			else // 스캔을 하지 않으면 InspectResponse 신호 ON
			{
				m_pDoc->m_plc.SetInspectReq(TRUE);
			}
			// sjyi 2023.09.12 2D 검사를 위한 내용 추가 <--

			//_tracer(TRACE_NORMAL, 1, L"Update3DPosition Complete");
			//m_pDoc->m_plc.SetInspectReq(TRUE);
			SetEvent(m_pDoc->m_hEvtOpenRecipe);
		}
			
		theApp.m_bWorkOn = FALSE;
	}

	return 0;
}


void CMainFrame::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_pLiveCCDDlg != NULL) 
	{ 
		m_pLiveCCDDlg->DestroyWindow(); //DHChoi 20200702
		delete m_pLiveCCDDlg; 
		m_pLiveCCDDlg = NULL;
	}
	m_pDoc->TerminateIWS();
	CFrameWndEx::OnClose();
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (m_hAccelTable) 
		if (::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg)) 
			return(TRUE);

	return CFrameWndEx::PreTranslateMessage(pMsg);
}

void CMainFrame::OnAccViewHome()
{
	SwitchView(HOME_VIEW);
}

void CMainFrame::OnAccVscan()
{
	m_pDoc->m_work.DoVScan();
}

void CMainFrame::OnAccViewDrw()
{
	SwitchView(DRAW_VIEW);
}

void CMainFrame::OnAccLaserpen()
{
	m_pDoc->m_pDrawView->OnButtonLaserPens();
}

void CMainFrame::LaserpenHiddern()
{
	CDlgLaserPenHiddern dlg;
	for (int i = 0; i < 10; i++)
	{
		dlg.m_dPenPower[i] = m_pDoc->m_work.m_hp.m_dPenPower[i];
		dlg.m_dPenSpeed[i] = m_pDoc->m_work.m_hp.m_dPenSpeed[i];
	}
		

	if (dlg.DoModal() != IDOK)
		return;

	for (int i = 0; i < 10; i++)
	{
		m_pDoc->m_work.m_hp.m_dPenPower[i] = dlg.m_dPenPower[i];
		m_pDoc->m_work.m_hp.m_dPenSpeed[i] = dlg.m_dPenSpeed[i];
	}
		

	m_pDoc->m_config.WriteConfig();
}

void CMainFrame::OnAccRefimgSetting()
{
	CDlgVScanPositionSet dlg;
	m_pDoc->m_config.GetScanRng(&(dlg.m_sx), &(dlg.m_sy), &(dlg.m_ex), &(dlg.m_ey));
	dlg.m_nLightPower = m_pDoc->m_config.m_nLightPower;
	// sjyi 2022-06-08 시리얼 다시 살림(남경1차는 시리얼 통신) --> 
	//dlg.m_pCtrl = &m_pDoc->m_CeminiSerial;
	// sjyi 2022-06-08 시리얼 다시 살림(남경1차는 시리얼 통신) <--

	// mspark 2020.06.17
	// CorrParam param = m_pDoc->m_config.GetCorrParam(m_pDoc->m_config.m_iCurrLens);
	// 포인터로 처리해야 나중에 다이얼로그에서 변수 값을 변경하고 
	// save config해면 저장이 되고 갱신이 됨.
	CorrParam *param = m_pDoc->m_config.pGetCorrParam(m_pDoc->m_config.m_iCurrLens);
	dlg.m_dblCenImgX = param->dblImgCenX;
	dlg.m_dblCenImgY = param->dblImgCenY;

	dlg.m_dModel1_X = m_pDoc->m_config.m_cp.dModel1_X;
	dlg.m_dModel1_Y = m_pDoc->m_config.m_cp.dModel1_Y;
	dlg.m_dModel2_X = m_pDoc->m_config.m_cp.dModel2_X;
	dlg.m_dModel2_Y = m_pDoc->m_config.m_cp.dModel2_Y;
	dlg.m_dModel3_X = m_pDoc->m_config.m_cp.dModel3_X;
	dlg.m_dModel3_Y = m_pDoc->m_config.m_cp.dModel3_Y;
	dlg.m_dModel4_X = m_pDoc->m_config.m_cp.dModel4_X;
	dlg.m_dModel4_Y = m_pDoc->m_config.m_cp.dModel4_Y;

	// sjyi 2022-06-08 시리얼 다시 살림(남경1차는 시리얼 통신) --> 
	if (dlg.DoModal() != IDOK)
	{
		//m_pDoc->m_CeminiSerial.SetLight(m_pDoc->m_config.m_nLightPower);
		return;
	}
	// sjyi 2022-06-08 시리얼 다시 살림(남경1차는 시리얼 통신) <--

	//DHChoi 20200629
	//dblImgCenX는 Calibration설정 후 스켄범위 설정에 따라 내부적으로 결정됨
	//아래쪽 m_pDoc->m_config.SetRefImgCnt에서 저장됨.
	//param->dblImgCenX = dlg.m_dblCenImgX;
	//param->dblImgCenY = dlg.m_dblCenImgY;
	m_pDoc->m_pScanner->SetCorrParam(*param);

	m_pDoc->m_config.SetScanRng(dlg.m_sx, dlg.m_sy, dlg.m_ex, dlg.m_ey);
	m_pDoc->m_config.SetCenterPos(dlg.m_dModel1_X, dlg.m_dModel1_Y, dlg.m_dModel2_X, dlg.m_dModel2_Y, dlg.m_dModel3_X, dlg.m_dModel3_Y, dlg.m_dModel4_X, dlg.m_dModel4_Y);
	m_pDoc->m_config.m_nLightPower = dlg.m_nLightPower;
	m_pDoc->m_config.WriteConfig();

	// sjyi 2022-06-08 시리얼 다시 살림(남경1차는 시리얼 통신) --> 
	//m_pDoc->m_CeminiSerial.SetLight(m_pDoc->m_config.m_nLightPower);
	// sjyi 2022-06-08 시리얼 다시 살림(남경1차는 시리얼 통신) <--

	float resolX, resolY;
	m_pDoc->GetCcdResolution(&resolX, &resolY);
	g_pGrabImgDlg->ChangeScanRange(resolX, resolY, dlg.m_sx, dlg.m_sy, dlg.m_ex, dlg.m_ey);


	//DHChoi 20200629
	if (param->bUseCalib)
	{
		double cx, cy;
		g_pGrabImgDlg->GetRefImgCnt(&cx, &cy);
		m_pDoc->m_config.SetRefImgCnt(cx, cy);
		//m_pDoc->m_config.SetRefImgCnt(dlg.m_dblCenImgX, dlg.m_dblCenImgY);
		m_pDoc->m_config.WriteConfig();
	}
}


void CMainFrame::OnAccWeldonce()
{
//	CString strMsg;
//	strMsg = m_pDoc->m_config.m_bWeldAllAtOnce ?
//		L"Weld one by one ": L"Weld all at once";
//	
//	if (AfxMessageBox(strMsg, MB_OKCANCEL) == IDOK) 
//	{
//		m_pDoc->m_config.m_bWeldAllAtOnce = !m_pDoc->m_config.m_bWeldAllAtOnce;
////		m_pDoc->m_work.SetWeldOnce(m_pDoc->m_config.m_bWeldAllAtOnce);
//		m_pDoc->m_config.WriteConfig();
//	}
}

void CMainFrame::OnFileSetVscan()
{
	OnAccRefimgSetting();
}

void CMainFrame::CadMousePos(double x, double y)
{
	CString str;
	str.Format(L"X : %.2f, Y : %.2f", x, y);
	m_wndStatusBarPaneXY->SetText(str);
	m_wndStatusBarPaneXY->Redraw();
}

void CMainFrame::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CMainFrame::OnUpdateFileShowInspect(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CMainFrame::OnUpdateFileDxf(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}


void CMainFrame::OnUpdateFileHpConfig(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CMainFrame::OnUpdateFileSetVscan(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CMainFrame::OnUpdateFileLocationSave(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CMainFrame::OnUpdateFileLoadAs(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CMainFrame::OnUpdateMatrix(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CMainFrame::OnUpdateMatrixPcc(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CMainFrame::OnUpdateButtonHairpin(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CMainFrame::OnUpdateButtonHairpinConfig(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CMainFrame::OnUpdateButtonHairpinOffset(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CMainFrame::OnUpdateButtonPattern(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CMainFrame::OnUpdateButtonLaserPens(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CMainFrame::UpdateButtonEnable(CCmdUI *pCmdUI)
{
	BOOL bLogin = FALSE;
	if (m_pDoc != NULL) bLogin = (m_pDoc->m_User.GetUser() != USER_OPERATOR);
	pCmdUI->Enable(bLogin && !m_pDoc->GetUseInterface());
}

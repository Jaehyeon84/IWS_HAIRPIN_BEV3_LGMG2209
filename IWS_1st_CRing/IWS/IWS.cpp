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

// IWS.cpp: 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "IWS.h"
#include "MainFrm.h"

#include "IWSDoc.h"
#include "IWSView.h"

#include "./HubiLicense/HubiLicenseCheck.h"
#include "./util/SplashWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CString GetCompileDate()
{
	const char *str_pre_macro_date = __DATE__;
	const char *str_pre_macro_time = __TIME__;

	CString sCompDate;
	const int comfile_date_len = 12;

	// error check
	if (NULL == str_pre_macro_date || comfile_date_len - 1 != strlen(str_pre_macro_date))
		return 0;

	const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

	char s_month[5] = { 0, };
	int iyear = 0, iday = 0;

	sscanf_s(str_pre_macro_date, "%s %d %d", s_month, (unsigned int) sizeof(s_month), &iday, &iyear);
	int imonth = (int)(strstr(month_names, s_month) - month_names) / 3 + 1;

	CStringA saTime; saTime.Format("%s", str_pre_macro_time);


	sCompDate.Format(_T("[Comfile: %04d%02d%02d %s]"), iyear, imonth, iday, (CString)saTime);

	return sCompDate;
}
// CIWSApp

BEGIN_MESSAGE_MAP(CIWSApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CIWSApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	//ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CIWSApp 생성

CIWSApp::CIWSApp() noexcept
{
	m_bHiColorIcons = TRUE;

	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 응용 프로그램을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
	//     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
	//     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("IWS.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CIWSApp 개체입니다.

CIWSApp theApp;
HANDLE g_hLcWnd;
HANDLE g_hLcCmd;
HANDLE g_hLcProp;
HANDLE g_hLcDrw;
HANDLE g_hDrwWorkArea;
BOOL g_bEventEnable;
//BOOL g_bMode;
CGrabImgDlg* g_pGrabImgDlg;

// CIWSApp 초기화
// LiteCAD event procedure
// it is called by all LiteCAD events
//-----------------------------------------------
void CALLBACK LcadEventProc(HANDLE hEvent)
{
	int EventType = lcPropGetInt(hEvent, LC_PROP_EVENT_TYPE);
	switch (EventType) 
	{
	case LC_EVENT_PAINT:     TRACE("OnPaint(hEvent)\n");  break;
	case LC_EVENT_MOUSEMOVE: TRACE("OnMouseMove(hEvent)\n");  break;
	case LC_EVENT_WNDVIEW:  TRACE(" OnWndView(hEvent)\n");  break;
		//    case LC_EVENT_EXTENTS:   OnExtents( hEvent );  break;
	case LC_EVENT_RBDOWN:    TRACE("OnRBDown(hEvent)\n");  break;
	case LC_EVENT_KEYDOWN:   TRACE("OnKeyDown(hEvent)\n");  break;
	}
}
BOOL CIWSApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CString strAppName;
	strAppName.LoadString(IDS_APPNAME);
	m_hMutex = ::CreateMutex(FALSE, 0, strAppName);

	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(L"The program is running.");
		return FALSE;
	}

	CWinAppEx::InitInstance();


	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면 AfxInitRichEdit2()가 있어야 합니다.
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(strAppName);
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.

	TCHAR pszPathName[_MAX_PATH];
	::GetModuleFileName(::AfxGetInstanceHandle(), pszPathName, _MAX_PATH);
	CString sPath = pszPathName;
	int iPos = sPath.ReverseFind(_T('\\'));
	CString sLogFile;
	BOOL bTRACE = FALSE;

	if (iPos >= 0)
	{
		sPath = sPath.Mid(0, iPos);
		sPath = sPath + L"\\Log\\";
		CFileFind file;
		CString strFile = L"*.*";
		BOOL bResult = file.FindFile(sPath + strFile);
		if (!bResult)
			bResult = CreateDirectory(sPath, NULL);

		//sLogFile.Format(_T("%sIWS.txt"), sPath);
		sLogFile.Format(_T("%sIWS_"), sPath);

		bTRACE = TRUE;
	}

	// GDI init
	Gdiplus::GdiplusStartupInput gdiSI;
	Gdiplus::GdiplusStartupOutput gdiSO;
	ULONG_PTR gdiToken;
	ULONG_PTR gdiHookToken;
	gdiSI.SuppressBackgroundThread = TRUE;
	Gdiplus::GdiplusStartup(&gdiToken, &gdiSI, &gdiSO);
	gdiSO.NotificationHook(&gdiHookToken);
	//
	CSplashWnd splash;
	TCHAR Path[MAX_PATH];
	GetModuleFileName(NULL, Path, MAX_PATH);
	CString m_strExePath = Path;
	m_strExePath = m_strExePath.Left(m_strExePath.ReverseFind('\\'));
	CString strIntroFile = m_strExePath + _T("\\intro.gif");
	CFileFind finder;
	BOOL bShowSplash = finder.FindFile(strIntroFile);
	if (bShowSplash) 
	{
		splash.SetImage(strIntroFile);
		splash.SetWindowName(L"Wait while application load its data...");
		splash.Show();
	}
	
	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();

	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다.  문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CIWSDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CIWSView));

	if (!pDocTemplate)
		return FALSE;

	AddDocTemplate(pDocTemplate);

	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	// DDE Execute 열기를 활성화합니다.
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_pMainWnd->ShowWindow(SW_HIDE);
	if (bTRACE)
		vTRACE_INIT(NULL, sLogFile, 0, _T(""), 1);

	CString sCompInfo = GetCompileDate();
	CString sStartMsg;
	CString sIWSVer = IWS_VERSION;
	
//	_tracer(TRACE_NORMAL, 1, strAppName+L" started=======================================");
	sStartMsg.Format(_T("%s %s %s Started======================================="), strAppName, sIWSVer, sCompInfo);
	_tracer(TRACE_NORMAL, 1, sStartMsg);

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	((CMainFrame*)m_pMainWnd)->InitViews();
	
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.
	// 끌어서 놓기에 대한 열기를 활성화합니다.
	m_pMainWnd->DragAcceptFiles();
	((CMainFrame*)m_pMainWnd)->InitializeIWS();

	if (bShowSplash) 
		splash.Close();

	gdiSO.NotificationUnhook(gdiHookToken);
	Gdiplus::GdiplusShutdown(gdiToken);	

	return TRUE;
}

int CIWSApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	::ReleaseMutex(m_hMutex);
	AfxOleTerm(FALSE);
	_tracer(TRACE_NORMAL, 1, _T("IWS exit!======================================================"));
	vTRACE_Close();
	return CWinAppEx::ExitInstance();
}

// CIWSApp 메시지 처리기


////////////////////////////////////////////////////////////////////////////////////////////////////
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;
	virtual BOOL OnInitDialog ();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)	
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog ()
{
	CDialogEx::OnInitDialog ();

	CStatic* pProject = (CStatic*)GetDlgItem (IDC_STATIC_PROJECT);
	//pProject->SetWindowText (_T("IWS [LGMG2209]_1st_Top"));
	pProject->SetWindowText (IWS_VERSION);
	CStatic* pBuild = (CStatic*)GetDlgItem (IDC_STATIC_BUILD);
	CString str;
	str.Format (_T ("Build Date. %s"), _T (__DATE__));
	pBuild->SetWindowText (str);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CIWSApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CIWSApp 사용자 지정 로드/저장 방법

void CIWSApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CIWSApp::LoadCustomState()
{
}

void CIWSApp::SaveCustomState()
{
}

// CIWSApp 메시지 처리기

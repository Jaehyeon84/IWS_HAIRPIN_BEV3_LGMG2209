#include "stdafx.h"
#include "IWS.h"

#include "IWSDoc.h"
#include "HomeView.h"
#include "CDlgScanner.h"
#include "MainFrm.h"
#include "VRSaux/DispImgDlg.h"
#include "DlgManualWork.h"
#include "CDlgRecipeList.h"
#include "CLaserIO.h"
#include "DlgPassword.h"
#include "CDlgReworkJob.h"
/* //gseom_240704_2DInsp 주석 추가-->
#include "CDlg2DSetting.h" // dhpark 2023.06.26			
#include "../Hairpin2DInsp/Hairpin2D.h" // ehjang 2D Insp. 추가 
*/	//gseom_240704_2DInsp 주석 추가<--
#include "VRSaux/GrabImgDlg.h" // ehjang 2D Insp. 추가	
#include "VRSaux/ScanImgsProc.h" // ehjang 2D Insp. 추가	

#define TIMER_LASER_STATUS 100

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CHomeView, CBaseFormView)

BEGIN_MESSAGE_MAP(CHomeView, CBaseFormView)
	ON_COMMAND(ID_BUTTON_PARAM_SCANNER, &CHomeView::OnButtonParamScanner)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_BUTTON_MANUAL_SCANNER, &CHomeView::OnButtonManualScanner)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_MANUAL_START, &CHomeView::OnBnClickedButtonManualStart)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_WORK, &CHomeView::OnBnClickedButtonOpenWork)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_LASER_SIMULATION, &CHomeView::OnBnClickedButtonLaserSimulation)
	ON_BN_CLICKED(IDC_BUTTON_LASER_GUIDE, &CHomeView::OnBnClickedButtonLaserGuide)
	ON_BN_CLICKED(IDC_BUTTON_LASER_LASER, &CHomeView::OnBnClickedButtonLaserLaser)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH_WORKLIST2, &CHomeView::OnBnClickedButtonRefreshWorklist2)
	ON_COMMAND(ID_BUTTON_CCDLIIVE, &CHomeView::OnButtonCcdliive)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_BN_CLICKED(IDC_BUTTON_DETECT_STATOR, &CHomeView::OnBnClickedButtonDetectStator)
	ON_BN_CLICKED(IDC_BUTTON_SCAN_STATOR, &CHomeView::OnBnClickedButtonScanStator)
	ON_BN_CLICKED(IDC_BUTTON_RESET_DETECTION, &CHomeView::OnBnClickedButtonResetDetection)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_LOG, &CHomeView::OnBnClickedButtonClearLog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_ONLINE, &CHomeView::OnBnClickedButtonOnline)
	ON_BN_CLICKED(IDC_BUTTON_OFFLINE, &CHomeView::OnBnClickedButtonOffline)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT, &CHomeView::OnBnClickedButtonLight)
	ON_COMMAND(ID_BUTTON_SCAN_RNG, &CHomeView::OnButtonScanRng)
	ON_BN_CLICKED(IDC_BUTTON_SCAN_INSPECTION, &CHomeView::OnBnClickedButtonScanInspection)
	
	ON_UPDATE_COMMAND_UI(ID_BUTTON_LOGIN, &CHomeView::OnUpdateButtonLogInOut)
	ON_COMMAND(ID_BUTTON_LOGIN, &CHomeView::OnButtonUserLogInOut)
	ON_MESSAGE(WM_DISPLAYCHANGE, &CHomeView::OnDisplayChange)
	//	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CHomeView::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_SNAP, &CHomeView::OnBnClickedButtonSnap)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PARAM_SCANNER, &CHomeView::OnUpdateButtonParamScanner)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_MANUAL_SCANNER, &CHomeView::OnUpdateButtonManualScanner)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_CCDLIIVE, &CHomeView::OnUpdateButtonCcdliive)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SCAN_RNG, &CHomeView::OnUpdateButtonScanRng)
	ON_BN_CLICKED(IDC_RCP_CHECK, &CHomeView::OnBnClickedRcpCheck)
	ON_BN_CLICKED(IDC_REWORK_SELECT, &CHomeView::OnBnClickedReworkSelect)
	ON_BN_CLICKED(IDC_BTN_REINIT_PLC, &CHomeView::OnBnClickedBtnReinitPlc)
	ON_BN_CLICKED(IDC_BTN_HCOMP_TEST, &CHomeView::OnBnClickedBtnHcompTest)
	//ON_BN_CLICKED(IDC_BTN_2D_SET, &CHomeView::OnBnClickedBtn2dSet) // dhpark 2023.06.26 //gseom_240704_2DInsp 주석 추가
	//ON_COMMAND(ID_BUTTON_2D_RESULT, &CHomeView::OnButton2dResult)	gseom_240704_2DInsp 주석 추가
END_MESSAGE_MAP()

CHomeView::CHomeView() noexcept
	:CBaseFormView(IDD_FORMVIEW_HOME)
{
	m_pDlgManualScan = NULL;	
}


CHomeView::~CHomeView()
{
	
}

void CHomeView::DoDataExchange(CDataExchange* pDX)
{
	CBaseFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
	//  DDX_Control(pDX, IDC_STC_HEAD_STATE, m_lbStateScanner);
	DDX_Control(pDX, IDC_STC_STATE_LASER, m_lbStateLaser);
	DDX_Control(pDX, IDC_STC_STATE_GUIDE, m_lbStateGuide);
	DDX_Control(pDX, IDC_STC_SIMULATION, m_lbStateSimulation);
	DDX_Control(pDX, IDC_STC_DOWORK, m_lbStateDoWork);
	DDX_Control(pDX, IDC_STC_STATE_LIGHT, m_lbStateLight);
	DDX_Control(pDX, IDC_STC_LABEL_DIO, m_lbDIO);
	DDX_Control(pDX, IDC_STC_LABEL_WORKLIST, m_lbWorklist);
	//DDX_Control(pDX, IDC_STC_LABEL_CONTROL, m_lbControl);
	DDX_Control(pDX, IDC_COMBO_WORKLIST, m_cbWorkList);
	DDX_Control(pDX, IDC_STC_LABEL_SYSTEM, m_lbSystemStatus);
	DDX_Control(pDX, IDC_STC_LABEL_PLC, m_lbPLC);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID_COMMPARAM, m_gridEntityParam);
	DDX_Control(pDX, IDC_STATUS_WORKING, m_statusWorking);
	DDX_Control(pDX, IDC_STATUS_PLC, m_statusPLC);
	DDX_Control(pDX, IDC_STATUS_LASER, m_statusLaser);
	DDX_Control(pDX, IDC_STATUS_SCANNER, m_statusScanner);
	DDX_Control(pDX, IDC_STATUS_CCD, m_statusCCD);
	DDX_Control(pDX, IDC_STATUS_INTERFACE, m_statusInterface);
	DDX_Control(pDX, IDC_STATIC_RCP_NUMBER, m_stcRcpNumber);
	DDX_Control(pDX, IDC_STATIC_RCP_MODEL, m_stcRcpModel);
	DDX_Control(pDX, IDC_STATIC_RCP_LAST, m_stcLastJobNo);
	DDX_Control(pDX, IDC_STATIC_PLC_JIG, m_stcPlcJig);
	DDX_Control(pDX, IDC_STATIC_PLC_WorkWeld, m_stcPlcWorkWeld);
	DDX_Control(pDX, IDC_STATIC_PLC_WorkMode, m_stcPlcWorkMode);
	DDX_Control(pDX, IDC_EDIT_DETECT_RANGE, m_editDetectRange);
	DDX_Control(pDX, IDC_STC_LABEL_TACTTIME, m_stcTactTime);
}


void CHomeView::RefreshWorklist()
{
	CString strExePath = GetDocument()->GetExePath();
	CString strPath = strExePath + L"\\Work\\*.wrk";
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strPath);
	m_arrWorkList.RemoveAll();

	while (bWorking) 
	{
		bWorking = finder.FindNextFileW();

		if(finder.IsDots())
			continue;
		if(finder.IsDirectory())
			continue;

		CString strFilename = finder.GetFileName();
		m_arrWorkList.Add(strFilename);
	}
}

void CHomeView::AdjustLayoutUI()
{
	if (!GetDlgItem(IDC_EDIT_LOG)->GetSafeHwnd()) 
		return;

	CRect rect;
	GetClientRect(rect);
	static int yi = 0;
	static int xi = 0;

	if (!yi) 
	{
		CRect cr;
		GetDlgItem(IDC_EDIT_LOG)->GetClientRect(cr);
		GetDlgItem(IDC_EDIT_LOG)->ClientToScreen(cr);
		this->ScreenToClient(cr);
		yi = cr.top + 15; // sjyi 2022-05-19 Log Edit 위치 수정
		xi = cr.left;
	}
// 	if (!joblist_yi) {
// 		CRect cr;
// 		GetDlgItem(IDC_STC_GRID_JOBLIST)->GetClientRect(cr);
// 		GetDlgItem(IDC_STC_GRID_JOBLIST)->ClientToScreen(cr);
// 		this->ScreenToClient(cr);
// 		joblist_yi = cr.top;
// 		joblist_xi = cr.left;
// 		joblist_cx = cr.Width();
// 	}
	int x, y, cx, cy;
	x = 0;
	y = yi;
	cx = rect.Width();
	cy = rect.Height() - y;
	GetDlgItem(IDC_EDIT_LOG)->MoveWindow(x, y, cx, cy);
	
}

BOOL CHomeView::GetSubMonitorPos(RECT &rect)
{
	int iMonCnt = ::GetSystemMetrics(SM_CMONITORS);
	if (iMonCnt < 2) 
		return FALSE;
	DISPLAY_DEVICE dd;
	DEVMODE dm;

	for (int i = 0; i < iMonCnt; i++) 
	{
		memset(&dd, 0, sizeof(DISPLAY_DEVICE));
		dd.cb = sizeof(DISPLAY_DEVICE);
		::EnumDisplayDevices(NULL, i, &dd, 0);
		memset(&dm, 0, sizeof(DEVMODE));

		if(EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm))
		{
			
			TRACE("%d,%d,%d,%d, start%d\n",
				dm.dmPosition.x, dm.dmPosition.y,
				dm.dmPosition.x + dm.dmPelsWidth,
				dm.dmPosition.y + dm.dmPelsHeight, dd.StateFlags&DISPLAY_DEVICE_PRIMARY_DEVICE);

			if (!(dd.StateFlags&DISPLAY_DEVICE_PRIMARY_DEVICE)) 
			{
				rect.left = dm.dmPosition.x;
				rect.top = dm.dmPosition.y;
				rect.right = dm.dmPosition.x + dm.dmPelsWidth;
				rect.bottom = dm.dmPosition.y + dm.dmPelsHeight;
			}
		}
	}

	return TRUE;
// 	if (::GetSystemMetrics(SM_CMONITORS) < 2) // ����� ���� ���ϱ�
// 	{
// 		return false;
// 	}
// 
// 
// 
// 	// ���� ����� ����� ���̾�α� ���
// 	HMONITOR    hMonitorThis, hMonitorTarget;
// 	static MONITORINFO oMonitorThis, oMonitorTarget;
// 	POINT oPoint;
// 	// ���� �������� ����� ��ġ ���ϱ�
// 	hMonitorThis = ::MonitorFromWindow(this->GetSafeHwnd(), MONITOR_DEFAULTTONEAREST);
// 	oMonitorThis.cbSize = sizeof(MONITORINFO);
// 	::GetMonitorInfo(hMonitorThis, &oMonitorThis);
// 	// Ÿ�� �������� ����� ��ġ ���ϱ�
// 	if (oMonitorThis.rcMonitor.left != 0)
// 	{
// 		// ���� ������� ���� ����Ϳ� ��ġ�� �ִ�.
// 		// ���� Ÿ�� ������� �� �����(0, 0)�� �����Ѵ�.
// 		oPoint.x = 0; oPoint.y = 0;
// 		hMonitorTarget = ::MonitorFromPoint(oPoint, MONITOR_DEFAULTTONEAREST);
// 		oMonitorTarget.cbSize = sizeof(MONITORINFO);
// 		::GetMonitorInfo(hMonitorTarget, &oMonitorTarget);
// 	}
// 	else
// 	{
// 		// ���� ������� �� ����Ϳ� ��ġ�� �ִ�.
// 		// ���� ����Ͱ� ���ʿ� �ִ��� �����ʿ� �ִ��� Ȯ���ؾ� �Ѵ�.
// 		oPoint.x = 0; oPoint.y = 0;
// 		hMonitorTarget = ::MonitorFromPoint(oPoint, MONITOR_DEFAULTTONULL);
// 		if (hMonitorTarget <= 0)
// 		{
// 			oPoint.y = oMonitorThis.rcMonitor.bottom + 1; 
// 			oPoint.x = 0;
// 			hMonitorTarget = ::MonitorFromPoint(oPoint, MONITOR_DEFAULTTONULL);
// 			oMonitorTarget.cbSize = sizeof(MONITORINFO);
// 			::GetMonitorInfo(hMonitorTarget, &oMonitorTarget);
// 			goto EndOfGetSubMonitorPos;
// 		}
// 		
// 		oMonitorTarget.cbSize = sizeof(MONITORINFO);
// 		::GetMonitorInfo(hMonitorTarget, &oMonitorTarget);
// 	}
// 	EndOfGetSubMonitorPos:
// 	rect = oMonitorTarget.rcMonitor;
// 	return true;
}

void CHomeView::SetEnableUI(BOOL bEnable)
{
	EnableWindow(bEnable);
}

BOOL CHomeView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CBaseFormView::PreCreateWindow(cs);
}

void CHomeView::OnInitialUpdate()
{
	CBaseFormView::OnInitialUpdate();
// 	GetParentFrame()->RecalcLayout();
// 	ResizeParentToFit();

	if (GetDocument()) 
	{
		CIWSDoc *pDoc = GetDocument();
		int nsize = (int)pDoc->m_config.m_arWorkList.GetCount();
		if (m_cbWorkList.GetSafeHwnd()) 
		{
			m_cbWorkList.ResetContent();
			for (int i = 0; i < nsize; i++) 
			{
				CString str;
				str.Format(L"%d : ",i);
				m_cbWorkList.AddString(str + pDoc->m_config.m_arWorkList[i]);
			}
		}

		for (int i = 0; i < pDoc->m_pScanner->m_pLaserIO->m_strDINameArr.GetSize(); i++)
			m_gridDIO.m_strArrInputname.Add(pDoc->m_pScanner->m_pLaserIO->m_strDINameArr[i]);

		for (int i = 0; i < pDoc->m_pScanner->m_pLaserIO->m_strDONameArr.GetSize(); i++)
			m_gridDIO.m_strArrOutputname.Add(pDoc->m_pScanner->m_pLaserIO->m_strDONameArr[i]);

		m_gridDIO.SetErrorDIO(pDoc->m_pScanner->m_pLaserIO->GetLaserErrorIndex(), -1);
	}

	if (m_bInitUpdated) 
		return;

	vTRACE_ShowHandle((CWnd*)&m_editLog);
	if (!m_pDlgManualScan)
	{
		CIWSDoc *pDoc = GetDocument();

		m_pDlgManualScan = new CDlgManualScan();
		m_pDlgManualScan->SetDocument(pDoc);
		m_pDlgManualScan->Create(IDD_DIALOG_MANUAL_SCAN, this);
	}

	m_gridDIO.SetNumDIO(MAX_SCANNER_DI, MAX_SCANNER_DO);
	m_gridPLC.SetNumDIO(8, 8);
	m_gridPLC.SetNumCmdIO(4);

	m_lbStateLaser.On();
	m_lbStateLaser.Off();
	m_lbStateGuide.On();
	m_lbStateGuide.Off();
	m_lbStateSimulation.On();
	m_lbStateSimulation.Off();
	m_lbStateDoWork.On();
	m_lbStateDoWork.Off();
	m_statusInterface.On();
	m_statusInterface.Off();

	m_statusWorking.SetType(ALARM_IO);
	m_statusWorking.On();
	m_statusWorking.Off();

	CIWSDoc *pDoc = GetDocument();
	if((pDoc)->m_config.m_bUsePLC)		
	m_statusPLC.SetType(ALARM_IO);
	m_statusPLC.On();
	m_statusPLC.Off();
	m_statusLaser.SetType(ALARM_IO);
	m_statusLaser.On();
	m_statusLaser.Off();
	m_statusScanner.SetType(ALARM_IO);
	m_statusScanner.On();
	m_statusScanner.Off();
	m_statusCCD.SetType(ALARM_IO);
	m_statusCCD.On();
	m_statusCCD.Off();
	m_gridDIO.AttachGrid(this, IDC_STC_GRID_DIO);
	m_gridPLC.AttachGrid(this, IDC_STC_GRID_PLC);
// 	m_btnSimulation.EnableWindowsTheming(FALSE);
// 	m_btnSimulation.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
// 	m_btnSimulation.m_bTransparent = TRUE;
// 	m_btnSimulation.SetFaceColor(COLOR_GREY33, true);
// 	m_btnSimulation.SetTextColor(COLOR_WHITESMOKE);
	m_lbDIO.SetBkColor(COLOR_GREY44);
	m_lbDIO.SetTextColor(COLOR_WHITESMOKE);
	m_lbDIO.SetFontBold(TRUE);
	m_lbDIO.SetText(L"Scanner Input / Output");
	m_stcTactTime.SetBkColor(COLOR_GREY32);
	m_stcTactTime.SetTextColor(COLOR_OLIVEDRAB1);
	m_stcTactTime.SetFontBold(TRUE);
	m_stcTactTime.SetText(L"00 s");
	m_lbWorklist.SetBkColor(COLOR_GREY44);
	m_lbWorklist.SetTextColor(COLOR_WHITESMOKE);
	m_lbWorklist.SetFontBold(TRUE);
	m_lbWorklist.SetText(L"Work List");
	
	int iLaser = GetDocument()->m_pScanner->GetLaser();
	CString strLaser;
	strLaser.Format(L"Laser : %s", g_dscLaserName[iLaser]);
	//m_lbControl.SetText(strLaser);

	//m_lbControl.SetText(L"Manual Control");

	m_lbSystemStatus.SetBkColor(COLOR_GREY44);
	m_lbSystemStatus.SetTextColor(COLOR_WHITESMOKE);
	m_lbSystemStatus.SetFontBold(TRUE);
	m_lbSystemStatus.SetText(L"System Status");

	m_lbPLC.SetBkColor(COLOR_GREY44);
	m_lbPLC.SetTextColor(COLOR_WHITESMOKE);
	m_lbPLC.SetFontBold(TRUE);
	m_lbPLC.SetText(L"PLC");

	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	pFrm->m_wndWorkView.SetPropertyGrid(&m_gridEntityParam);
	m_gridEntityParam.SetVSDotNetLook();
	
	CRect rect;
	m_gridEntityParam.GetClientRect(rect);
	HDITEM hdItem;
	hdItem.mask = HDI_WIDTH; // indicating cxy is width
	hdItem.cxy = rect.Width()/2; // whatever you want the property name column width to be
	m_gridEntityParam.GetHeaderCtrl().SetItem(0, &hdItem);
	m_bInitUpdated = TRUE;
	m_editDetectRange.SetValue(GetDocument()->m_config.m_dDetectRange, L"%.1f");

	// sjyi 2023.04.12 Manual Save 버튼 보이기
	if (pDoc->m_config.m_bViewHCompManual)
	{
		GetDlgItem(IDC_BTN_HCOMP_TEST)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_BTN_HCOMP_TEST)->ShowWindow(SW_HIDE);
	}
}

#ifdef _DEBUG
void CHomeView::AssertValid() const
{
	CBaseFormView::AssertValid();
}

void CHomeView::Dump(CDumpContext& dc) const
{
	CBaseFormView::Dump(dc);
}

CIWSDoc* CHomeView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIWSDoc)));
	return (CIWSDoc*)m_pDocument;
}

#endif //_DEBUG

void CHomeView::OnButtonParamScanner()
{
	CIWSDoc *pDoc = GetDocument();
	CDlgScanner dlg;
	dlg.SetDocument(GetDocument());
	dlg.SetCorrParam(&pDoc->m_config.m_arCorrParam[pDoc->m_config.m_iCurrLens]);
	dlg.DoModal();

	if (dlg.IsChanged()) 
	{
		CorrParam Param = pDoc->m_config.GetCorrParam(pDoc->m_config.m_iCurrLens);
		pDoc->m_pScanner->SetCorrParam(Param);
		if (!pDoc->m_pScanner->ApplyCorrParam())
			AfxMessageBox(L"Scanner Update Fail ");
		
		//pDoc->m_config.m_arCorrParam[0] = *(pDoc->m_pScanner->GetCorrParam());
		pDoc->m_config.WriteConfig();
	}

}


void CHomeView::OnTimer(UINT_PTR nIDEvent)
{
	CScanner *pScan = GetDocument()->m_pScanner;
	CIWSDoc *pDoc = GetDocument();

	UINT iIn = pDoc->m_status.scanDI;
	int iOut = pDoc->m_status.scanDO;
	
	m_gridDIO.SetInOut(iIn, iOut);
	
	int iPlcRead = (int)(GetDocument()->m_plc.GetReadData(0));
	int iPlcWrite = (int)(GetDocument()->m_plc.GetWriteData(0));
	int iPlcRead1 = (int)(GetDocument()->m_plc.GetReadData(1));
	int iPlcWrite1 = (int)(GetDocument()->m_plc.GetWriteData(1));

	m_gridPLC.SetInOut(iPlcRead, iPlcWrite, iPlcRead1, iPlcWrite1);
	if (pScan->GetDOEmission())
		m_lbStateLaser.On();
	else  
		m_lbStateLaser.Off();

	if (!pScan->GetDOEmission())
		m_lbStateGuide.On();
	else  
		m_lbStateGuide.Off();

	CIWSWork *pWork = &(pDoc->m_work);
	if (pWork->m_bSimulation) 
		m_lbStateSimulation.On();
	else 
		m_lbStateSimulation.Off();

	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	if (pWork->IsWorkOn()) 
	{
		m_lbStateDoWork.On();
		pFrm->m_wndWorkView.m_pWorkView->SetEnableEvent(FALSE);
		SetEnableUI(FALSE);
	}
	else 
	{
		m_lbStateDoWork.Off();
		pFrm->m_wndWorkView.m_pWorkView->SetEnableEvent(TRUE);
		SetEnableUI(TRUE);
	}

	if (theApp.m_bWorkOn)
		m_statusWorking.On();
	else
		m_statusWorking.Off();

	if (pDoc->m_status.bOk[IWS_PLC]) 
		m_statusPLC.On();
	else 
		m_statusPLC.Off();

	if (pDoc->m_status.bOk[IWS_LASER]) 
		m_statusLaser.On();
	else 
		m_statusLaser.Off();

	if (pDoc->m_status.bOk[IWS_SCANNER]) 
		m_statusScanner.On();
	else 
		m_statusScanner.Off();

	if (pDoc->m_status.bOk[IWS_VSCAN]) 
		m_statusCCD.On(); 

	if (pDoc->GetUseInterface()) 
	{
		m_statusInterface.On();
		GetDlgItem(IDC_BUTTON_LASER_SIMULATION)->EnableWindow(FALSE);
		pDoc->m_work.m_bSimulation = FALSE;
	}
	else 
	{
		m_statusInterface.Off();
		GetDlgItem(IDC_BUTTON_LASER_SIMULATION)->EnableWindow(TRUE);
	}

	CString strTmp;
	int iRcp = pDoc->m_plc.GetRecipe();
	strTmp.Format(L"%d",iRcp);
	m_stcRcpNumber.SetWindowTextW(strTmp);

	strTmp.Format(L"%d", pDoc->m_nLastRcpIdx);
	
	// sjyi 2023.07.16 검사모드 화면 표시
	//strTmp.Format(L"%d", pDoc->m_nLastRcpIdx);
	strTmp.Format(_T("3D"));
	//gseom_240705_2DInsp 주석 추가-->
	/*if (pDoc->m_plc.GetWorkJig() == 1 && pDoc->m_plc.GetDo2DInspectJigA() == 1)
	{
		strTmp.Format(_T("2D"));
	}
	else if (pDoc->m_plc.GetWorkJig() == 2 && pDoc->m_plc.GetDo2DInspectJigB() == 1)
	{
		strTmp.Format(_T("2D"));
	}*/
	//gseom_240705_2DInsp 주석 추가<--
	m_stcLastJobNo.SetWindowTextW(strTmp);

	strTmp.Format(L"%s", pDoc->m_work.m_nModel == 0 ? _T("Bottom") : _T("C_RING"));
	m_stcRcpModel.SetWindowTextW(strTmp);

	strTmp.Format(L"%s", pDoc->m_plc.GetWorkJig() == 1 ? _T("JigA") : _T("JigB"));
	m_stcPlcJig.SetWindowTextW(strTmp);

	strTmp.Format(L"%s", pDoc->m_plc.GetReWeld() == TRUE ? _T("Rework") : _T("Weld"));
	m_stcPlcWorkWeld.SetWindowTextW(strTmp);

	strTmp.Format(L"%s", pDoc->m_plc.GetSimulation() == TRUE ? _T("Simulation") : _T("Weld"));
	m_stcPlcWorkMode.SetWindowTextW(strTmp);

	//pFrm->m_wndWorkView.m_pWorkView->m_grid.RedrawAll();

	// sjyi 2022-06-08 시리얼 다시 살림(남경1차는 시리얼 버전) -->
	//if (pDoc->m_CeminiSerial.m_bOnOff)
	//	m_lbStateLight.On();
	//else
	//	m_lbStateLight.Off();

	// sjyi 2022-05-24 라이트 관련 추가 -->
	if (pDoc->m_pScanner->IsLightOn())
	{
		m_lbStateLight.On();
	}
	else
	{
		m_lbStateLight.Off();
	}
	// sjyi 2022-05-24 라이트 관련 추가 <--
	// sjyi 2022-06-08 시리얼 다시 살림(남경1차는 시리얼 통신) <--


	//===========================================================
	// 버튼 상태
	//===========================================================
	BOOL bLogin = (pDoc->m_User.GetUser() != USER_OPERATOR);
	BOOL bUseInterface = pDoc->GetUseInterface();
	GetDlgItem(IDC_BUTTON_REFRESH_WORKLIST2)->EnableWindow(bLogin & !bUseInterface);
	GetDlgItem(IDC_REWORK_SELECT)->EnableWindow(bLogin & !bUseInterface);

	GetDlgItem(IDC_BUTTON_OPEN_WORK)->EnableWindow(!bUseInterface);
	//GetDlgItem(IDC_BTN_2D_SET)->EnableWindow(bLogin & !bUseInterface);
	GetDlgItem(IDC_BUTTON_LASER_SIMULATION)->EnableWindow(!bUseInterface);
	GetDlgItem(IDC_BUTTON_LASER_GUIDE)->EnableWindow(!bUseInterface);
	GetDlgItem(IDC_BUTTON_LIGHT)->EnableWindow(!bUseInterface);
	GetDlgItem(IDC_BUTTON_LASER_LASER)->EnableWindow(!bUseInterface);
	GetDlgItem(IDC_BUTTON_SCAN_STATOR)->EnableWindow(!bUseInterface);
	GetDlgItem(IDC_BUTTON_SCAN_INSPECTION)->EnableWindow(!bUseInterface);
	GetDlgItem(IDC_BUTTON_MANUAL_START)->EnableWindow(!bUseInterface);

	CBaseFormView::OnTimer(nIDEvent);
}

void CHomeView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CBaseFormView::OnShowWindow(bShow, nStatus);

	if (bShow) 
	{
		SetTimer(TIMER_LASER_STATUS, 500, NULL);
	}
	else 
	{
		KillTimer(TIMER_LASER_STATUS);
	}
}

void CHomeView::OnButtonManualScanner()
{
	CIWSDoc *pDoc = GetDocument();
	if (pDoc->GetUseInterface() == FALSE)
	{
		// Welding Ready check
		if (!pDoc->m_plc.GetReadyWelding())
		{
			AfxMessageBox(L"[PLC]Not Ready For Welding.", MB_OK + MB_ICONERROR);
			return;
		}
	}
	m_pDlgManualScan->ShowWindow(TRUE);
// 	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
// 	pFrm->m_pDlgWorkViewer->ShowWindow(TRUE);
}

void CHomeView::OnDestroy()
{
	CBaseFormView::OnDestroy();
	vTRACE_ShowHandle(NULL);
}

void CHomeView::OnBnClickedButtonManualStart()
{
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	vector_int* pselect = pFrm->m_wndWorkView.pGetSelectionList();
	BOOL bSelect = pselect->empty() ? FALSE : TRUE;
	CIWSDoc *pDoc = GetDocument();
	if (pDoc->GetUseInterface())
	{
		AfxMessageBox(L"Change To Offline");
		return;
	}
	else
	{
		// Welding Ready check
		if (!pDoc->m_plc.GetReadyWelding())
		{
			AfxMessageBox(L"[PLC]Not Ready For Welding.", MB_OK + MB_ICONERROR);
			return;
		}
	}

	CDlgManualWork dlg;
	dlg.m_iSelection = bSelect;

	dlg.m_dPowerRatio = pDoc->m_pScanner->GetPowerRatio()*100.0;
	dlg.m_dSpeedRatio = pDoc->m_pScanner->GetSpeedRatio()*100.0;

	if (dlg.DoModal() != IDOK) 
		return;
	
	IWS_VECTOR_JOB_GROUP *pJobGroup = pDoc->m_work.GetJobs();
	if (dlg.m_iSelection) 
	{
		for (auto group : (*pJobGroup))
		{
			for (auto p : group->m_jobs)
			{
				p->m_bSelected = FALSE;
			}
		}			

		for (auto p : (*pselect))
		{
			pDoc->m_work.GetJob(p)->m_bSelected = TRUE;
		}
	}

	//pFrm->m_wndWorkView.UpdateWork();
	BOOL bSimulation =  pDoc->m_work.m_bSimulation;

	pDoc->StartWork(bSimulation, dlg.m_dPowerRatio / 100.f, dlg.m_dSpeedRatio / 100.f);
}

void CHomeView::OnBnClickedButtonOpenWork()
{
	int isel = m_cbWorkList.GetCurSel();
	if (isel < 0) return;
	if (GetDocument()->GetUseInterface()) 
	{
		AfxMessageBox(L"Change To Offline");
		return;
	}

	CIWSDoc *pDoc = GetDocument();
	pDoc->m_nLastRcpIdx = isel;

	CString strFilename = pDoc->GetExePath() + L"\\Work\\";
	strFilename = strFilename + pDoc->m_config.m_arWorkList[isel];	
	pDoc->OpenWork(strFilename);

	//g_pGrabImgDlg->LoadRefImage(isel);
	/*CIWSWork *pWork = &(pDoc->m_work);
	int nsize = (int)pWork->GetJobs()->size();*/

	/* change v-scan parameter (size of image (nblock, fov ...) : yjko*/
	// pDoc->ChangeScanRange(); need to confirm
}


void CHomeView::OnSize(UINT nType, int cx, int cy)
{
	CBaseFormView::OnSize(nType, cx, cy);
	AdjustLayoutUI();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void CHomeView::OnBnClickedButtonLaserSimulation()
{
	CIWSDoc *pDoc = GetDocument();
	if (pDoc->GetUseInterface()) 
	{
		AfxMessageBox(L"Change To Offline");
		return;
	}

	CScanner *pScan = pDoc->m_pScanner;
#if (LASER_SOURCE == _TRUMF_LASER)
	pScan->LaserEnable(FALSE, 0);
	pDoc->m_work.m_bSimulation = TRUE;
#elif (LASER_SOURCE == _SPI_LASER)
	pScan->INT_DO_GuideOn(pScan->INT_BE_GuideOn());
	pScan->LaserEnable(FALSE, 0);
	pScan->LaserOn(FALSE);
	pDoc->m_work.m_bSimulation = !pDoc->m_work.m_bSimulation;
#elif(LASER_SOURCE == _IPG_SINGLE)
	pDoc->m_work.m_bSimulation = !pDoc->m_work.m_bSimulation;
	pScan->INT_DO_GuideOn(pDoc->m_work.m_bSimulation);
	pScan->LaserEnable(FALSE, 0);
	pScan->LaserOn(FALSE);
#else
	pScan->INT_DO_GuideOn(pScan->INT_BE_GuideOn());
	pScan->LaserEnable(FALSE, 0);
	pDoc->m_work.m_bSimulation = !pDoc->m_work.m_bSimulation;
#endif

}

void CHomeView::OnBnClickedButtonLaserGuide()
{
	CIWSDoc *pDoc = GetDocument();
	if (pDoc->GetUseInterface()) 
	{
		AfxMessageBox(L"Change To Offline");
		return;
	}

	CScanner *pScan = pDoc->m_pScanner;
#if (LASER_SOURCE == _TRUMF_LASER)
	pScan->LaserEnable(FALSE, 0);
	pDoc->m_work.m_bSimulation = FALSE;
	pScan->SetGuideOn(TRUE);
#elif (LASER_SOURCE == _SPI_LASER)
	pScan->INT_DO_GuideOn(!pScan->INT_BE_GuideOn());
	pScan->LaserEnable(FALSE, 0);
	pScan->LaserOn(FALSE);
	pDoc->m_work.m_bSimulation = FALSE;
#elif(LASER_SOURCE == _IPG_SINGLE)
	pScan->INT_DO_GuideOn(!pScan->INT_BE_GuideOn());
	pScan->LaserEnable(FALSE, 0);
	pScan->LaserOn(FALSE);
	pDoc->m_work.m_bSimulation = FALSE;
#else
	pScan->INT_DO_GuideOn(!pScan->INT_BE_GuideOn());
	pScan->LaserEnable(FALSE, 0);
	pDoc->m_work.m_bSimulation = FALSE;
#endif
}


void CHomeView::OnBnClickedButtonLaserLaser()
{
	CIWSDoc *pDoc = GetDocument();
	if (pDoc->GetUseInterface()) 
	{
		AfxMessageBox(L"Change To Offline");
		return;
	}

	CScanner *pScan = pDoc->m_pScanner;
#if (LASER_SOURCE == _TRUMF_LASER)
	pScan->LaserPowerON(TRUE);
	pScan->LaserEnable(!pScan->GetDOEmission(), 0);
	pDoc->m_work.m_bSimulation = FALSE;
	pScan->SetGuideOn(FALSE);
#elif (LASER_SOURCE == _SPI_LASER)
	pScan->INT_DO_GuideOn(FALSE);
	pScan->LaserEnable(!pScan->INT_BE_EmissionEnable(), 0);
	pScan->LaserOn(TRUE);
	pDoc->m_work.m_bSimulation = FALSE;
#elif(LASER_SOURCE == _IPG_SINGLE)
	pScan->INT_DO_GuideOn(FALSE);
	pScan->LaserEnable(!pScan->INT_BE_EmissionEnable(), 0);
	pScan->LaserOn(TRUE);
	pDoc->m_work.m_bSimulation = FALSE;
#else
	pScan->INT_DO_GuideOn(FALSE);
	pScan->LaserEnable(!pScan->INT_BE_EmissionEnable(), 0);

	pDoc->m_work.m_bSimulation = FALSE;
#endif
}

void CHomeView::OnBnClickedButtonRefreshWorklist2()
{
	CDlgRecipeList dlg;
	CIWSDoc *pDoc = GetDocument();
	if (GetDocument()->GetUseInterface()) 
	{
		AfxMessageBox(L"Change To Offline");
		return;
	}

	RefreshWorklist();
	for (int i = 0; i < pDoc->m_config.m_arWorkList.GetSize(); i++) 
		dlg.m_arrSelectedList.Add(pDoc->m_config.m_arWorkList[i]);

	for (int i = 0; i < m_arrWorkList.GetSize(); i++) 
		dlg.m_arrWorkFiles.Add(m_arrWorkList[i]);
	
	if (dlg.DoModal() != IDOK) 
		return;

	pDoc->m_config.m_arWorkList.RemoveAll();

	for (int i = 0; i < dlg.m_arrSelectedList.GetSize(); i++)
		pDoc->m_config.m_arWorkList.Add(dlg.m_arrSelectedList[i]);

	int nsize = (int)pDoc->m_config.m_arWorkList.GetCount();
	if (m_cbWorkList.GetSafeHwnd()) 
	{
		m_cbWorkList.ResetContent();
		for (int i = 0; i < nsize; i++) 
		{
			CString str;
			str.Format(L"%d : ", i);
			m_cbWorkList.AddString(str + pDoc->m_config.m_arWorkList[i]);
		}
	}

	pDoc->m_config.WriteConfig();
}

void CHomeView::OnButtonCcdliive()
{
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	RECT sub_monitor_pos;
	if (GetSubMonitorPos(sub_monitor_pos))
		pFrm->m_pLiveCCDDlg->MoveWindow((LPRECT)&sub_monitor_pos);
	
	pFrm->m_pLiveCCDDlg->ShowWindow(SW_SHOW);
	return;
}

LRESULT CHomeView::OnPropertyChanged(WPARAM wparam, LPARAM lparam) 
{
#pragma warning(disable:4800)
	CMFCPropertyGridProperty * pProp = (CMFCPropertyGridProperty *)lparam;
	if (!pProp) 
		return 0;

	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	pFrm->m_wndWorkView.UpdateCurrentSelect(pProp);

#pragma warning(default:4800)

	return TRUE;
}


void CHomeView::OnBnClickedButtonDetectStator()
{
	CIWSDoc *pDoc = GetDocument();
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();

	//pDoc->m_work.CalcHairpinInfo();
	pDoc->m_work.NewCalcHairpinInfo();
	pDoc->m_config.m_dDetectRange = m_editDetectRange.GetValue();
	pDoc->m_work.DoInspection();
	//pDoc->InspectStator();
	//pDoc->SetWorkOffset();
	//
 	//int widthPxl, heightPxl;
 	//float widthLength, heightLength;
 	//pDoc->ReqStitchImage(FALSE, &widthPxl, &heightPxl, &widthLength, &heightLength);
 	//
 	//CString strRefImg = pDoc->m_strImgPath + STR_REFIMG_FILE;
 	//
	//pFrm->m_wndWorkView.UpdateWorkImage(strRefImg,widthLength, heightLength);
	//pFrm->m_wndWorkView.UpdateWork();
}

#include "./HubiLicense/HubiLicenseCheck.h"
void CHomeView::OnBnClickedButtonScanStator()
{
	//g_bMode = FALSE;
	CIWSDoc *pDoc = GetDocument();

	if (pDoc->GetUseInterface())
	{
		AfxMessageBox(L"Change To Offline");
		return;
	}

	pDoc->m_work.DoVScan();


//	int widthPxl, heightPxl;
//	float widthLength, heightLength;
//
//	pDoc->ReqStitchImage(TRUE, &widthPxl, &heightPxl, &widthLength, &heightLength);
//
//	CString strRefImg = pDoc->m_strImgPath + STR_REFIMG_FILE;
//	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
//	pFrm->m_wndWorkView.UpdateWorkImage(strRefImg,  200, 200);

}

// sjyi 2022-05-19 comment 현재 사용안함.(현재, 리소스에서 숨김)
void CHomeView::OnBnClickedButtonResetDetection()
{
	CIWSDoc *pDoc = GetDocument();
	pDoc->ClearWorkOffset();
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	pFrm->m_wndWorkView.UpdateWork();
}


void CHomeView::OnBnClickedButtonClearLog()
{
	m_editLog.SetWindowTextW(L"");

	//imsi
	//CIWSDoc *pDoc = GetDocument();
	//pDoc->SaveImageAndPos();
}

void CHomeView::SetCurrentWorkFile(CString strWrk)
{
	CIWSDoc *pDoc = GetDocument();
	int nsize = (int)pDoc->m_config.m_arWorkList.GetCount();
	int iSelect = -1;
	if (m_cbWorkList.GetSafeHwnd()) 
	{
		m_cbWorkList.ResetContent();
		for (int i = 0; i < nsize; i++) 
		{
			CString str;
			str.Format(L"%d : ", i);
			m_cbWorkList.AddString(str + pDoc->m_config.m_arWorkList[i]);
			if (pDoc->m_config.m_arWorkList[i] == strWrk) 
			{
				iSelect = i;
				pDoc->m_iCurrentWork = i;
			}
		}
	}
	m_cbWorkList.SetCurSel(iSelect);
	pDoc->m_nLastRcpIdx = iSelect;
}

void CHomeView::UpdateCycleTime()
{
	CString strTact;
	strTact.Format(L"%5.2f s", GetDocument()->m_work.GetTactTime() / 1000.);
	m_stcTactTime.SetBkColor(COLOR_GREY32);
	m_stcTactTime.SetTextColor(COLOR_OLIVEDRAB1);
	m_stcTactTime.SetText(strTact);
}

int CHomeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBaseFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	GetDocument()->m_pHomeView = this;

	return 0;
}

void CHomeView::OnBnClickedButtonOnline()
{
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	_tracer(TRACE_NORMAL, 0, L"Online Mode");
	// 프로그램 번호 체크 
	if (check_bit(pDoc->m_status.scanDO, 8))
	{
		AfxMessageBox(L"Program number is not 0 or 1.");
		return;
	}

	pDoc->m_User.LogOut();

	if (pDoc->m_config.m_bUseHairpin)
		pFrm->m_wndRibbonBar.ShowCategory(3, false);
	else
		pFrm->m_wndRibbonBar.ShowCategory(2, false);

	pFrm->m_wndRibbonBar.RecalcLayout();

	pDoc->SetUseInterface(TRUE);

	//CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
}


void CHomeView::OnBnClickedButtonOffline()
{
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	_tracer(TRACE_NORMAL, 0, L"Offline Mode");
	if (pDoc->m_config.m_bUseHairpin)
		pFrm->m_wndRibbonBar.ShowCategory(3, true);
	else
		pFrm->m_wndRibbonBar.ShowCategory(2, true);

	pFrm->m_wndRibbonBar.RecalcLayout();

	GetDocument()->SetUseInterface(FALSE);
}

void CHomeView::OnBnClickedButtonLight()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (GetDocument()->GetUseInterface()) 
	{
		AfxMessageBox(L"Change To Offline");
		return;
	}

	// sjyi 2022-06-08 라이트 재 수정(시리얼로 통신) --->
	// GetDocument()->m_CeminiSerial.SetOnOff(!GetDocument()->m_CeminiSerial.m_bOnOff);


	// sjyi 2022-05-24 라이트 관련 추가 -->
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	pDoc->m_pScanner->LightOn(!pDoc->m_pScanner->IsLightOn());
	// sjyi 2022-05-24 라이트 관련 추가 <--

	// sjyi 2022-06-08 라이트 재 수정(시리얼로 통신) --->
}


void CHomeView::OnButtonScanRng()
{
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	pFrm->OnAccRefimgSetting();
}


void CHomeView::OnBnClickedButtonScanInspection()
{
	//g_bMode = TRUE;
	CIWSDoc *pDoc = GetDocument();
	BOOL bLogin = (pDoc->m_User.GetUser() != USER_OPERATOR);
	BOOL bUseInterface = pDoc->GetUseInterface();
	pDoc->Update3DPosition();
	//if(IsSHIFTPressed() & IsCTRLPressed()) // ehjang 2D Insp. 추가
	//gseom_240704_2DInsp 주석 추가-->
	/*if (IsSHIFTPressed() & IsCTRLPressed() & bLogin & !bUseInterface)
	{
		CorrParam p = pDoc->m_config.GetCorrParam(pDoc->m_config.m_iCurrLens);

		int imageWidth(g_pGrabImgDlg->m_pScanImgsProc->m_stitchImg.downImgWd);
		int imageHeight(g_pGrabImgDlg->m_pScanImgsProc->m_stitchImg.downImgHt);
		auto downMosaicImg = g_pGrabImgDlg->m_pScanImgsProc->m_stitchImg.pDownMosaicImg;

		BYTE* pbyImage = (BYTE*)imaqImageToArray(downMosaicImg, IMAQ_NO_RECT, &imageWidth, &imageHeight);

		float resolX = g_pGrabImgDlg->m_pScanImgsProc->m_fResolX * 2.f;
		float resolY = g_pGrabImgDlg->m_pScanImgsProc->m_fResolY * 2.f;

		auto slotCount = pDoc->m_work.m_hp.nSlot;
		auto layerCount = pDoc->m_work.m_hp.nLayer;
		auto layerPair = layerCount / 2;

		int nJig = pDoc->m_plc.GetWorkJig();
		int nJigAddress(0);
		CString strJig = _T("");
		if (nJig == 1)
		{
			strJig = _T("JigA");
			nJigAddress = PLC_READ_INSPECT_A;
		}
		else if (nJig == 2)
		{
			strJig = _T("JigB");
			nJigAddress = PLC_READ_INSPECT_B;
		}
		else
		{
			strJig = _T("No Jig");
		}

		auto wordCnt = (pDoc->m_work.m_hp.nDrawSlot * 16) * (pDoc->m_work.m_hp.nLayer / 2);
		
		short* pnIWIResult = nullptr;
		float* pfIWIH1 = nullptr;
		float* pfIWIH2 = nullptr;
		float* pfIWIH3 = nullptr;
		float* pfIWIX_afterMatrix = nullptr;
		float* pfIWIY_afterMatrix = nullptr;

		if (strJig != _T("No Jig") && pDoc->m_plcData != nullptr)
		{
			if (pDoc->m_bImportPLCData == FALSE)
			{
				if (!pDoc->m_plcData->UpdateInput_InspData(nJigAddress, wordCnt))
				{
					pnIWIResult = nullptr;
					pfIWIH1 = nullptr;
					pfIWIH2 = nullptr;
					pfIWIH3 = nullptr;
					pfIWIX_afterMatrix = nullptr;
					pfIWIY_afterMatrix = nullptr;
				}
				else
				{
					pnIWIResult = pDoc->m_pnIWIResult;
					pfIWIH1 = pDoc->m_pfIWIH1;
					pfIWIH2 = pDoc->m_pfIWIH2;
					pfIWIH3 = pDoc->m_pfIWIH3;
					pfIWIX_afterMatrix = pDoc->m_pfIWIX_afterMatrix;
					pfIWIY_afterMatrix = pDoc->m_pfIWIY_afterMatrix;
				}
			}

			// IWI 의 결과 가져오기
			long nIdx(0);
			long slot(0), layer(0);
			int nMaxLayer = pDoc->m_work.m_nDefLayer / 2;
			for (int i = 0; i < wordCnt; i += 16)
			{
				long sum1 = slot * layerPair + (layer / 2);

				pnIWIResult[sum1] = floor(pDoc->m_plcData->m_rDataInsp[i]);
				pfIWIH1[sum1] = floor(pDoc->m_plcData->m_rDataInsp[i + 1]) / 100.f;
				pfIWIH2[sum1] = floor(pDoc->m_plcData->m_rDataInsp[i + 2]) / 100.f;
				pfIWIH3[sum1] = floor(pDoc->m_plcData->m_rDataInsp[i + 3]) / 100.f;

				layer += 2;

				if (layer == layerCount)
				{
					layer = 0;
					slot++;
				}

				nIdx++;
			}
		}
		//gseom_240704_2DInsp 주석 추가-->
		pDoc->m_pHairpin2D->InitParameters(pbyImage, imageWidth, imageHeight, resolX, resolY, p.dblImgCenX, p.dblImgCenY, pnIWIResult, pfIWIH1, pfIWIH2, pfIWIH3, pfIWIX_afterMatrix, pfIWIY_afterMatrix, pDoc->m_work.m_str2DRecipePath, strJig);
		pDoc->m_pHairpin2D->DoModal();
		//gseom_240704_2DInsp 주석 추가<--
		imaqDispose(pbyImage);
	}
	else
	{
		if (pDoc->GetDo2DInspection() == TRUE)
		{
			if (pDoc->m_work.m_b2DInspection == TRUE) // ehjang 2D Insp. 추가
			{
				pDoc->Run2DInspection();

				CString strResult(_T(""));
				long okCount(0);
				long ngCount(0);
				pDoc->m_pHairpin2D->GetTotalResult(strResult, okCount, ngCount);

				BOOL b2DInspectOK = TRUE;
				if (ngCount > 0)
				{
					b2DInspectOK = FALSE;
				}

				// sjyi 2023.07.14 2D검사 결과 반영을 위해 추가
				if (b2DInspectOK)
				{
					//pDoc->m_plc.Set2DInspectResult(1); // OK
					pDoc->Set2DInspectionResult(1);
				}
				else
				{
					//pDoc->m_plc.Set2DInspectResult(2); // NG
					pDoc->Set2DInspectionResult(2);
				}
			}
		}
		else
		{
			pDoc->Update3DPosition();
		}
	}*/ 
	//gseom_240704_2DInsp 주석 추가<--
}

//void CHomeView::OnBnClickedButtonRefresh()
//{
//	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	((CMainFrame*)AfxGetMainWnd())->m_wndWorkView.UpdateWork();	
//}


void CHomeView::OnButtonUserLogInOut()
{
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	//if (pDoc->m_User.GetUser() != USER_OPERATOR) 
	//{
	//	pDoc->m_User.LogOut();
	//	//pFrm->SetUserName(L"Operator");
	//	/*if (pDoc->m_config.m_bUseHairpin)
	//		pFrm->m_wndRibbonBar.ShowCategory(3, false);
	//	else
	//		pFrm->m_wndRibbonBar.ShowCategory(2, false);
	//	pFrm->m_wndRibbonBar.RecalcLayout();
	//	//pFrm->RibbonShowCategory(CATEGORY_DRAW, FALSE);*/
	//	_tracer(TRACE_NORMAL, 0, L"Log Out : Operator");
	//	return;
	//}

	CDlgPassword dlg;
	dlg.m_nUser = pDoc->m_User.GetUser();
	dlg.m_strOriPassword = pDoc->m_config.m_strPassword;

	if (dlg.DoModal() == IDOK) 
	{
		if (pDoc->m_User.LogOn(dlg.m_strPassword)) 
		{
			switch (pDoc->m_User.GetUser()) 
			{
			case USER_OPERATOR:
				//pFrm->SetUserName(L"OPERATOR");
				_tracer(TRACE_NORMAL, 0, L"Log On : Operator");
				break;
			case USER_ADMINISTRATOR:
				//pFrm->SetUserName(L"Engineer");
				_tracer(TRACE_NORMAL, 0, L"Log On : Administrator");
				break;
			case USER_MASTER:
				//pFrm->SetUserName(L"Master");
				_tracer(TRACE_NORMAL, 0, L"Log On : Master");
				break;
			}
			
			/*
			// Ribbon Menu
			if (pDoc->m_User.GetUser() != USER_OPERATOR)
			{
				if (pDoc->m_config.m_bUseHairpin)
					pFrm->m_wndRibbonBar.ShowCategory(3, true);
				else
					pFrm->m_wndRibbonBar.ShowCategory(2, true);

				pFrm->m_wndRibbonBar.RecalcLayout();
			}*/
		}
	}

	if (dlg.m_bChgPassword)
	{
		pDoc->m_User.ChangePassword(dlg.m_strNewPassword);
		pDoc->m_config.m_strPassword = dlg.m_strNewPassword;
		pDoc->m_config.WriteConfig();
	}
}

LRESULT CHomeView::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	OnButtonCcdliive();
	return 0;
}

void CHomeView::OnBnClickedButtonSnap()
{
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	pDoc->SnapSave();
}


void CHomeView::OnUpdateButtonParamScanner(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}


void CHomeView::OnUpdateButtonManualScanner(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}


void CHomeView::OnUpdateButtonCcdliive(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}


void CHomeView::OnUpdateButtonScanRng(CCmdUI *pCmdUI)
{
	UpdateButtonEnable(pCmdUI);
}

void CHomeView::OnUpdateButtonLogInOut(CCmdUI *pCmdUI)
{
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	//if (pDoc->m_User.GetUser() != USER_OPERATOR)
	//	pCmdUI->SetText(L"LOG OUT");
	//else
	//	pCmdUI->SetText(L"LOG IN");

	//UpdateButtonEnable(pCmdUI);
	pCmdUI->Enable(!pDoc->GetUseInterface());
}

void CHomeView::UpdateButtonEnable(CCmdUI *pCmdUI)
{
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	BOOL bLogin = (pDoc->m_User.GetUser() != USER_OPERATOR);
	pCmdUI->Enable(bLogin && !pDoc->GetUseInterface());
}



void CHomeView::OnBnClickedRcpCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	int nRcp = pDoc->m_nLastRcpIdx;
	if (nRcp > 0 && nRcp < 7)
	{
		CString msg;
		msg.Format(L"RCP No: %d, Scale: %.4f, Angle: %.4f, OffsetX: %.4f, OffsetY: %.4f",
			nRcp,
			pDoc->m_config.m_dRcpScale[nRcp - 1],
			pDoc->m_config.m_dRcpAngle[nRcp - 1],
			pDoc->m_config.m_dRcpOffsetX[nRcp - 1],
			pDoc->m_config.m_dRcpOffsetY[nRcp - 1]
		);

		_tracer(TRACE_NORMAL, 1, msg);
	}
	else
	{
		_tracer(TRACE_NORMAL, 1, L"The recipe should be number 1-6.");
	}
}

void CHomeView::OnBnClickedReworkSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (GetDocument()->GetUseInterface())
	{
		AfxMessageBox(L"Change To Offline");
		return;
	}
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	CDlgReworkJob cDlgRework(pDoc->m_work.m_hp);
	if (cDlgRework.DoModal() == IDOK)
	{
		// Get Rework Job List
		IWS_VECTOR_REWORK_JOB vecReworkJob;
		cDlgRework.GetReworkJob(vecReworkJob);
		CJobWeld* pJob = NULL;
		// Rework Job(Enable/Disable) Update
		BOOL bEnable = FALSE;
		for (auto group : pDoc->m_work.m_JobGroup)
		{
			for (int i = 0; i < group->m_jobs.size(); i++)
			{
				if (i < 2)
				{
					bEnable = cDlgRework.IsExistReworkJob(group->m_nSlot + 1, group->m_nLayer, 1);
				}
				else
				{
					bEnable = cDlgRework.IsExistReworkJob(group->m_nSlot + 1, group->m_nLayer, 2);
				}
				// Get Job Point
				pJob = (CJobWeld*)group->m_jobs[i];
				DWORD dwData = 0;
				COleVariant value;
				// Set Job Enable
				pJob->SetDoJob(bEnable);
				// Set Pen
				pJob->GetEntity()->SetPen(&pJob->m_iPenID);
				// Entity Update
				pJob->GetEntity()->UpdateEntity(dwData, value);
			}
		}
		// Cad View Update
		pFrm->m_wndWorkView.UpdateWork();
	}
}
void CHomeView::OnBnClickedBtnReinitPlc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	pDoc->ReInitPLC();
}

void CHomeView::OnBnClickedBtnHcompTest()
{
	// TODO: ?¬ê¸°??ì»¨íŠ¸ë¡??Œë¦¼ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê??©ë‹ˆ??

	HCompTestSave(1.0, _T("HCompResut_1mm.csv"), 1, 1);
	HCompTestSave(2.0, _T("HCompResut_2mm.csv"), 1, 1);
	HCompTestSave(-1.0, _T("HCompResut_N1mm.csv"), 1, 1);
	HCompTestSave(-2.0, _T("HCompResut_N2mm.csv"), 1, 1);

	//int nIndex = m_cbWorkList.GetCurSel();

	//g_pGrabImgDlg->LoadRefImage(nIndex);

	//// sjyi 2023.07.16 스캔 이미지 검사 방법 변경(하나의 Ref 데이터에서 Recipe별 Ref 데이터로 변경)
	//g_pGrabImgDlg->CheckResultImage(nIndex);

}


// Model No : 1 = 77 Model, 2 = 76 Model
// Jig No : 1 = JIGA, 2 = JIGB
void CHomeView::HCompTestSave(double dHeight, TCHAR* szFileName, int nModelNo, int nJigNo)
{
	int nModelNoTemp = 0;
	int nJigNoTemp = 0;
	if (nModelNo < 0 || nModelNo > 2)
	{
		nModelNoTemp = 1;
		_tracer(TRACE_ERROR, 999, L"Model No Error(%d)", nModelNo);
	}
	else
	{
		nModelNoTemp = nModelNo;
	}

	if (nJigNo < 0 || nJigNo > 2)
	{
		nJigNoTemp = 1;
		_tracer(TRACE_ERROR, 999, L"Jig No Error(%d)", nJigNo);
	}
	else
	{
		nJigNoTemp = nModelNo;
	}


	// Get Pointer of CIWSDoc
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = GetDocument();

	// Get File Path
	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof path);
	CString strStartUpPath = path;
	strStartUpPath = strStartUpPath.Left(strStartUpPath.ReverseFind('\\'));

	// Get Job Group
	IWS_VECTOR_JOB_GROUP *pJobGroup = pDoc->m_work.GetJobs();

	// Set FileName
	strStartUpPath = strStartUpPath + _T("\\") + szFileName;

	// Open File
	CFile cfile;
	if (!cfile.Open(strStartUpPath, CFile::modeCreate | CFile::modeReadWrite))
	{
		return;
	}

	double dRefHeight = pDoc->m_config.m_dHCompRefHeight[nModelNoTemp][nJigNoTemp];
	// Write Header
	CString strData;
	strData.Format(_T("-- Height Compensation Data --\n"));
	cfile.Write(strData, strData.GetLength() * sizeof(TCHAR));
	strData.Format(_T("Telecentric Angle(Max),%.3f\n"), pDoc->m_config.m_dHCompTCMaxAngle);
	cfile.Write(strData, strData.GetLength() * sizeof(TCHAR));
	strData.Format(_T("Working Radius,%.3f\n"), pDoc->m_config.m_dHCompWorkRadius);
	cfile.Write(strData, strData.GetLength() * sizeof(TCHAR));
//	strData.Format(_T("Ref Height,%.3f\n"), pDoc->m_config.m_dHCompRefHeight);
	strData.Format(_T("Ref Height,%.3f\n"), dRefHeight);
	cfile.Write(strData, strData.GetLength() * sizeof(TCHAR));
	strData.Format(_T("Slot, Layer, DataX, DataY, DataXNew, DataYNew, dX, dY, dZ\n"));
	cfile.Write(strData, strData.GetLength() * sizeof(TCHAR));

	int nLayer = 1, nSlot = 1;
	for (auto JobGrp : (*pJobGroup))	// For All JobGroup
	{
		int nJobSize = JobGrp->m_jobs.size();

		double daX, daY, dbX, dbY, daZ, dbZ;
		double daXNew, daYNew, dbXNew, dbYNew;
		double dRadius;
		double dX, dY, dZ;

		// Get First Job
		daX = ((CJobWeld*)JobGrp->m_jobs[0])->GetEntity()->m_cen.x;
		daY = ((CJobWeld*)JobGrp->m_jobs[0])->GetEntity()->m_cen.y;
		daZ = ((CJobWeld*)JobGrp->m_jobs[0])->GetEntity()->m_cen.z;
		if (dHeight != 0.0)
		{
			daZ = dRefHeight + dHeight;
		}

		dRadius = sqrt(daX * daX + daY * daY);

		// Calc HComp
		pDoc->CalcHeightCompensation2(daX, daY, daZ, &daXNew, &daYNew, dRefHeight);

		dX = daXNew - daX;
		dY = daYNew - daY;
		dZ = daZ - dRefHeight;

		// Save CSV
		strData.Format(_T("%d, %d, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\n"), nSlot, nLayer, daX, daY, daXNew, daYNew, dX, dY, dZ);
		cfile.Write(strData, strData.GetLength() * sizeof(TCHAR));
		nLayer++;

		// Get Sencond Job
		dbX = ((CJobWeld*)JobGrp->m_jobs[1])->GetEntity()->m_cen.x;
		dbY = ((CJobWeld*)JobGrp->m_jobs[1])->GetEntity()->m_cen.y;
		dbZ = ((CJobWeld*)JobGrp->m_jobs[1])->GetEntity()->m_cen.z;
		if (dHeight != 0.0)
		{
			dbZ = dRefHeight + dHeight;
		}

		dRadius = sqrt(dbX * dbX + dbY * dbY);

		// Calc HComp
		pDoc->CalcHeightCompensation2(dbX, dbY, dbZ, &dbXNew, &dbYNew, dRefHeight);

		dX = dbXNew - dbX;
		dY = dbYNew - dbY;
		dZ = dbZ - dRefHeight;

		// Save CSV
		strData.Format(_T("%d, %d, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\n"), nSlot, nLayer, dbX, dbY, dbXNew, dbYNew, dX, dY, dZ);
		cfile.Write(strData, strData.GetLength() * sizeof(TCHAR));
		nLayer++;

		if (nLayer > pDoc->m_work.m_hp.nLayer)
		{
			nLayer = 1;

			nSlot++;
		}
	}

	cfile.Close();
}

/* //gseom_240704_2DInsp 주석 추가-->
// dhpark 2023.06.27
void CHomeView::OnBnClickedBtn2dSet()
{
	CIWSDoc *pDoc = GetDocument();
	
	CDlg2DSetting dlg;
	
	dlg.m_str2DPathBefore = pDoc->m_work.m_str2DRecipePath;
	dlg.m_b2DUseBefore = pDoc->m_work.m_b2DInspection;


	if (dlg.DoModal() != IDOK)
		return;

	pDoc->m_work.m_n2DPathLength = dlg.m_str2DPath.GetLength() + 1;
	pDoc->m_work.m_str2DRecipePath = dlg.m_str2DPath;
	pDoc->m_work.m_b2DInspection = dlg.m_b2DUse;

	pDoc->m_pMainFrm->m_wndWorkView.UpdateWork();
	
}

void CHomeView::OnButton2dResult()
{
	CIWSDoc *pDoc = GetDocument();
	//pDoc->m_pHairpin2D->ResultViewerDoModal();
}*/
//gseom_240704_2DInsp 주석 추가<--
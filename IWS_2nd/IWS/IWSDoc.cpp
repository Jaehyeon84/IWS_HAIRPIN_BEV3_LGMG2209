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

// IWSDoc.cpp: CIWSDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "IWS.h"
#endif

#include "IWSDoc.h"
#include "MainFrm.h"
#include "IWSView.h"
#include "HomeView.h"

#include "EntyArc.h"
#include "EntyCircle.h"
#include "EntyEllipse.h"
#include "EntyLine.h"
#include "EntyPoint.h"
#include "EntyPolyline.h"
#include "EntitySpiral.h"
#include "EntityRect.h"
#include "EntityHatch.h"
#include "ScannerRTC4.h"
#include "ScannerRTC5.h"
#include "VRSaux/GrabImgRunner.h"
//#include "VRSaux/GrabImgDlg.h"
#include "CDlgHairpinSet.h"
#include "CDlgLicense.h"
#include "IWSWorkHairpin.h"
#include "./HubiLicense/HubiLicenseCheck.h"
#include "VRSaux/ScanImgsProc.h"
#include <propkey.h>
#include "CDlgMatrix.h"

#include "CDlgLaserPrgNo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CIWSDoc
DECLARE_USER_MESSAGE(UWM_INTERFACE_THREAD)

IMPLEMENT_DYNCREATE(CIWSDoc, CDocument)

BEGIN_MESSAGE_MAP(CIWSDoc, CDocument)
	ON_COMMAND(ID_FILE_SAVE, &CIWSDoc::OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, &CIWSDoc::OnFileOpen)
	//ON_COMMAND(ID_FILE_CURR_TO_WORK, &CIWSDoc::OnFileCurrToWork)
	ON_COMMAND(ID_FILE_HP_CONFIG, &CIWSDoc::OnFileHpConfig)
	ON_COMMAND(ID_FILE_DXF, &CIWSDoc::OnFileImportDxf)
	ON_COMMAND(ID_FILE_LOCATION_SAVE, &CIWSDoc::OnFileLocationInfoSave)
	ON_COMMAND(ID_FILE_SHOW_INSPECT, &CIWSDoc::OnFileShowInspect)	
	ON_COMMAND(ID_MATRIX, &CIWSDoc::OnMatrix)
END_MESSAGE_MAP()


// CIWSDoc 생성/소멸

CIWSDoc::CIWSDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
#ifndef _DEBUG
	CString strLicense = theApp.GetProfileStringW(L"License", L"Key", L"");
	CStringA strLicenseA = CStringA(strLicense);

	if (!strLicense.GetLength()) {
		CDlgLicense dlg;
		if (dlg.DoModal() != IDOK) exit(-1);
		strLicenseA = dlg.m_strLicense;
		strLicense = CString(strLicenseA);
		theApp.WriteProfileStringW(L"License", L"Key", strLicense);
	}
	CHubiLicenseCheck checker;
	if (checker.CheckLicense(strLicenseA) <= 0) {
		theApp.WriteProfileStringW(L"License", L"Key", L"");
		exit(-1);
	}
#endif // !DEBUG
		

	m_pDrawView = NULL;
	m_pMainFrm = NULL;
	m_bRunInterfaceThread = FALSE;
	TCHAR Path[MAX_PATH];
	GetModuleFileName(NULL, Path, MAX_PATH);
	m_strExePath = Path;
	m_strExePath = m_strExePath.Left(m_strExePath.ReverseFind('\\'));

	m_strLastFilePath = L"";
	m_strLastFileName = L"";
	m_nLastRcpIdx = 0;

	// load pen file
	CFileFind finder;
	CString strPenFile = m_strExePath + _T("\\");
	strPenFile = strPenFile + STR_PENFILE;
	m_strFileLaserPen = strPenFile;
	BOOL bFlag = finder.FindFile(strPenFile);
	if (bFlag) {
		OpenLaserPen(strPenFile);
	}
	else {
		for (int i = 0; i < MAX_PENS; i++) {
			CLaserPen pen;
			pen.m_iPenID = i;
			m_pens.push_back(pen);
		}
		SaveLaserPen(strPenFile);
	}

	// Image folder
	m_strImgPath = CreateFolder(m_strExePath, L"Image");
	CString strWorklistPath = CreateFolder(m_strExePath, L"Work");
	m_strWorkPath = strWorklistPath;
	// Scanner folder
	CString strScannerPath = CreateFolder(m_strExePath, L"Scanner");

	// Config Path
	CString strConfigPath = CreateFolder(m_strExePath, L"Config");
	m_strConfigPath = strConfigPath;

	//  Vision Path
	CString strVisionPath = CreateFolder(m_strExePath, L"Vision");
	CString strVisionLogPath = CreateFolder(m_strExePath, L"Vision\\Log");
	CString strSimulImagePath = CreateFolder(m_strExePath, L"Vision\\Simulation");

	m_config.SetPath(strConfigPath);
	m_config.ReadConfig();
	// TODO: 보드 선택에 따라 Scanner 생성
	m_pScanner = new CScannerRTC5(strScannerPath, strConfigPath);
	m_pScanner->SetLaser(m_config.m_iLaser);
	//m_pScanner = new CScannerRTC4();
	int iLens = m_config.GetCurrentLens();
	m_pScanner->SetCorrParam(m_config.GetCorrParam(iLens));
	m_pScanner->DigitalIoRegisterData(0);
	m_work.SetScanner(m_pScanner);
	m_work.SetMovePen(&m_pens[14]);
	m_work.SetDocument(this);
	//m_work.SetNumSlotLayer(m_config.m_hp.nSlot, m_config.m_hp.nLayer);
	//m_work.SetWeldOnce(m_config.m_bWeldAllAtOnce);
	//m_bUseInterface = m_config.m_bUsePLC;
	m_hEvtOpenRecipe = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_User.ChangePassword(m_config.m_strPassword);

	m_bUseInterface = FALSE;

	m_plcData = NULL;
	m_bImportPLCData = FALSE;

	m_dCenOffX = 0.0;
	m_dCenOffY = 0.0;

	m_dBaseR	= 89.6;// 90.4;
	m_dBaseH	= 2.0;
	m_dHRRatio = 0.0;// 0.25;//0.279112506;

}

CIWSDoc::~CIWSDoc()
{
	// sjyi 2022-05-19 TerminateIWS() 다시 살림.
	// X로 종료 시에는 MainFrame의 OnClose()함수에서 TerminateIWS()함수 호출하나,
	// 리본 메뉴의 Close 버튼 누를 때에는 OnClose()함수 따라가지 않음.
	TerminateIWS();
	CloseHandle(m_hEvtOpenRecipe);
	m_pScanner->DigitalIoRegisterData();
	if (m_pScanner) delete m_pScanner;
	m_pScanner = NULL;

	if (m_config.m_bUsePLC)
	{
		if (m_plcData)
		{
			m_plcData->PLC_DisConnect();
			delete m_plcData;
		}
	}
}

CString CIWSDoc::CreateFolder(CString strBasePath, CString strPath)
{
	CFileFind finder;
	CString strTmpFolder = strBasePath + _T("\\");
	strTmpFolder = strTmpFolder + strPath;

	bool bFlag = finder.FindFile(strTmpFolder);
	if (!bFlag) 
	{
		CreateDirectory(strTmpFolder, NULL);
	}
	strTmpFolder = strTmpFolder + _T("\\");

	return strTmpFolder;
}

BOOL CIWSDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.
	//lcWndSelectView(g_hLcWnd, 0);

	if (m_config.m_bUseHairpin)
		theApp.m_iwsMode = CIWSApp::IWSMODE_HAIRPIN;
	else
		theApp.m_iwsMode = CIWSApp::IWSMODE_NORMAL;

	if (!m_pMainFrm)
		m_pMainFrm = (CMainFrame*)AfxGetMainWnd();

	m_pDrawView->SetDrwNew();
	m_work.clear();
	CEntity::m_iEntCount = 0;
	m_pMainFrm->m_wndJobListView.UpdateJobList(NULL);
	m_pMainFrm->m_wndPropertiesView.ShowJobProperty((CJob*)NULL);
	return TRUE;
}




// CIWSDoc serialization

void CIWSDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

void CIWSDoc::DrawAddEntity(HANDLE handle)
{
	if (!m_pMainFrm)
		m_pMainFrm =(CMainFrame*) AfxGetMainWnd();

	int EntType = lcPropGetInt(handle, LC_PROP_ENT_TYPE);
	if (EntType == LC_ENT_POLYLINE) 
	{
		int nSize = lcPropGetInt(handle, LC_PROP_ENT_XDATASIZE);
		if (nSize == sizeof(ENTSPIRAL)) EntType = LC_ENT_SPIRAL;
	}

	lcEventsEnable(FALSE);

	CJobWeld *pJob = new CJobWeld;
	switch (EntType)
	{
	case LC_ENT_LINE://		101  // Line
	{
		CEntyLine *entity = new CEntyLine;
		entity->SetDrawEntity(handle);
		pJob->m_entity = entity;
		break;
	}
	case LC_ENT_POLYLINE://		102  // Polyline
	{
		TRACE("Polyline ");
		CEntyPolyline *entity = new CEntyPolyline;
		entity->SetDrawEntity(handle);
		pJob->m_entity = entity;
		break;
	}
	case LC_ENT_CIRCLE://		103  // Circle
	{
		TRACE("Circle ");
		CEntyCircle *entity = new CEntyCircle;
		entity->SetDrawEntity(handle);
		pJob->m_entity = entity;
		break;
	}
	case LC_ENT_ARC://         104  // Arc
	{
		TRACE("Arc ");
		CEntyArc *entity = new CEntyArc;
		entity->SetDrawEntity(handle);
		pJob->m_entity = entity;
		break;
	}
	case LC_ENT_POINT://       107  // Point
	{
		TRACE("Point ");
		if (m_work.GetJob(handle))
			return;
		CEntyPoint *entity = new CEntyPoint;
		//CEntitySpiral *entity = new CEntitySpiral;
		entity->SetDrawEntity(handle);
		pJob->m_entity = entity;
		break;
	}
	case LC_ENT_ELLIPSE://     109  // Ellipse or elliptical arc
	{
		TRACE("ELLIPSE ");
		CEntyEllipse *entity = new CEntyEllipse;
		entity->SetDrawEntity(handle);
		pJob->m_entity = entity;
		break;
	}
	case LC_ENT_RECT://        118  // Rectangle
	{
		TRACE("RECT ");
		CEntityRect *entity = new CEntityRect;
		entity->SetDrawEntity(handle);
		pJob->m_entity = entity;
		break;
	}
	case LC_ENT_HATCH://       124  // Hatch
	{
		TRACE("HATCH ");
		{
			CEntityHatch *entity = new CEntityHatch;
			entity->SetDrawEntity(handle);
			pJob->m_entity = entity;
			break;
		}
		break;
	}
	case LC_ENT_SPIRAL:
	{
		TRACE("Block");
		CEntitySpiral *entity = new CEntitySpiral;
		entity->SetDrawEntity(handle);
		pJob->m_entity = entity;
		break;
	}
	case LC_ENT_IMAGEREF:
		delete pJob;
		return;


	default:
	{
		AfxMessageBox(L" Unknown Shape");
		pJob->m_entity = NULL;
		lcEntErase(handle, TRUE);
		HANDLE hBlock = lcPropGetHandle(handle, LC_PROP_ENT_BLOCK);
		lcBlockPurge(hBlock);
		lcBlockUpdate(hBlock, TRUE, 0);
		lcEventsEnable(TRUE);
		delete pJob;
	}
	return;
	}
	TRACE("entity was added(Pen=%d)\r\n", pJob->m_entity->GetPen());
	int iPen = pJob->m_entity->GetPen();

	pJob->m_pScan = m_pScanner;
	pJob->m_pPens = &m_pens;
	if (iPen > 0)
		pJob->m_iPenID = iPen;

	m_work.AddJob(pJob);
	m_pMainFrm->m_wndJobListView.UpdateJobList(m_work.GetJobs());
	m_pMainFrm->m_wndPropertiesView.ShowJobProperty(m_work.GetLastJob());
	//lcWndExeCommand(g_hLcWnd, LC_CMD_RESET, 0);
	lcEventsEnable(TRUE);
}

void CIWSDoc::DrawDeleteEntity(HANDLE entity)
{
	if (!m_pMainFrm)
		m_pMainFrm = (CMainFrame*)AfxGetMainWnd();

	int iType = lcPropGetInt(entity, LC_PROP_ENT_TYPE);
	switch (iType)
	{
	case LC_ENT_POINT:
	case LC_ENT_LINE:
	case LC_ENT_POLYLINE:
	case LC_ENT_RECT:
	case LC_ENT_CIRCLE:
	case LC_ENT_ELLIPSE:
	case LC_ENT_ARC:
	case LC_ENT_HATCH:
	case LC_ENT_SPIRAL:
		break;
	case LC_ENT_BLOCKREF:
		{
			HANDLE hRef = lcPropGetHandle(entity, LC_PROP_BLKREF_BLOCK);
			lcDrwDeleteObject(g_hLcDrw, hRef);
		}
		break;
	default:
		return;
	}

	//m_pMainFrm->m_wndJobListView.DeleteJob(m_work.GetJobIdx(entity));
	m_work.DeleteJob(entity);

	m_pMainFrm->m_wndJobListView.UpdateJobList(m_work.GetJobs());
}

void CIWSDoc::DrawSelectedEntity(HANDLE entity)
{
	if (!entity) 
	{
		m_pMainFrm->m_wndJobListView.UpdateSelection(m_work.GetJobs());
		m_pMainFrm->m_wndPropertiesView.ShowJobProperty((CJob*) NULL);
	}

	if (!m_work.GetJob(entity)) 
		return;

	int iType = lcPropGetInt(entity, LC_PROP_ENT_TYPE);
	switch (iType)
	{
	case LC_ENT_POINT:
	case LC_ENT_LINE:
	case LC_ENT_POLYLINE:
	case LC_ENT_RECT:
	case LC_ENT_CIRCLE:
	case LC_ENT_ELLIPSE:
	case LC_ENT_ARC:
	case LC_ENT_HATCH:
	case LC_ENT_BLOCKREF:
	case LC_ENT_SPIRAL:
		break;
	default:
		return;
	}

	m_pMainFrm->m_wndJobListView.UpdateSelection(m_work.GetJobs());
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	int nSelEnts = lcPropGetInt(hBlock, LC_PROP_BLOCK_NSELENTS);
	if (nSelEnts != 1)
	{
		IWS_VECTOR_JOB sel;
		HANDLE hEnt = lcBlockGetFirstSel(hBlock);
		while (hEnt != 0) 
		{
			if (!m_work.GetJob(hEnt)) 
			{
				hEnt = lcBlockGetNextSel(hBlock);
				continue;
			}
			sel.push_back(m_work.GetJob(hEnt));
			hEnt = lcBlockGetNextSel(hBlock);
		}
		m_pMainFrm->m_wndPropertiesView.ShowJobProperty(&sel);
	}
	else
	{
		m_pMainFrm->m_wndPropertiesView.ShowJobProperty(m_work.GetJob(entity));
	}
}

void CIWSDoc::DrawChangedEntity(HANDLE entity)
{
	int iType = lcPropGetInt(entity, LC_PROP_ENT_TYPE);
	switch (iType)
	{
	case LC_ENT_POINT:
	case LC_ENT_LINE:
	case LC_ENT_POLYLINE:
	case LC_ENT_RECT:
	case LC_ENT_CIRCLE:
	case LC_ENT_ELLIPSE:
	case LC_ENT_ARC:
	case LC_ENT_BLOCKREF:
	case LC_ENT_SPIRAL:
	case LC_ENT_HATCH:
		break;

	default:
		return;
	}
	
	// Draw 오브젝트 핸들 획득
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);

	// Draw 오브젝트 핸들의 Entity 개수 확인
	int nSelEnts = lcPropGetInt(hBlock, LC_PROP_BLOCK_NSELENTS);

	// Draw 오브젝트 핸들의 Entity 개수가 1개가 아니면, Group 임(Multi 선택)
	if (nSelEnts != 1) // Group 선택
	{
		int idx = 0;
		HANDLE hEnt = lcBlockGetFirstSel(hBlock);	// 첫번째
//		HANDLE hLast = NULL;
		while (hEnt != 0) 
		{
			//idx++;
			//if (idx == nSelEnts)
			//{
			//	hLast = hEnt;
			//}

			IWS_VECTOR_JOB sel; // Job Vector

			// Job Pointer 획득
			if (!m_work.GetJob(hEnt)) // Job Pointer가 Null이면
			{
				// 그냥 다음 Entity Handle 획득
				hEnt = lcBlockGetNextSel(hBlock);
				continue;
			}

			// Cad Entity에 대해, Job 내용 반영
			m_work.ChangeJob(hEnt);

			// Job Vector에 추가
			sel.push_back(m_work.GetJob(hEnt));

			// Index 증가
			idx++;
			if (idx == nSelEnts)	// Index가 Group의 Entity 개수와 같으면 마지막 Entity
			{
				// Property 내용 반영
				m_pMainFrm->m_wndPropertiesView.ShowJobProperty(&sel);

				// Job List 반영
				m_pMainFrm->m_wndJobListView.UpdateJobList(m_work.GetJobs());

				// 디버깅 로그
				TRACE("eventEntPropChanged - n-selecte = %d\n", nSelEnts);
			}

			// 다음 Entity 핸들 획득
			hEnt = lcBlockGetNextSel(hBlock);
		}

		// 다음에 들어오는 이벤트 Entity가 그룹의 마지막 Entity이면
		//if (hLast == entity) 
		//{
		//	HANDLE hEnt = lcBlockGetFirstSel(hBlock);
		//	while (hEnt != 0) 
		//	{
		//		if (!m_work.GetJob(hEnt))
		//		{
		//			hEnt = lcBlockGetNextSel(hBlock);
		//			continue;
		//		}
		//		m_work.ChangeJob(hEnt);

		//		hEnt = lcBlockGetNextSel(hBlock);
		//	}
		//	m_pMainFrm->m_wndPropertiesView.ShowJobProperty(m_work.GetJob(entity));
		//	m_pMainFrm->m_wndJobListView.UpdateJobList(m_work.GetJobs());
		//	TRACE("eventEntPropChanged - n-selecte = %d\n", nSelEnts);
		//}
	}
	else 
	{
		if (m_work.GetJob(entity)) 
		{
			if (m_work.ChangeJob(entity)) 
			{
				TRACE("eventEntPropChanged - n-selecte = %d\n", nSelEnts);
				if (nSelEnts == 1)
					m_pMainFrm->m_wndPropertiesView.ShowJobProperty(m_work.GetJob(entity));

				m_pMainFrm->m_wndJobListView.UpdateJobList(m_work.GetJobs());
			}
		}

	}
}

void CIWSDoc::DrawChangedEntityHatch(HANDLE entity)
{

}

void CIWSDoc::DrawUpdateJob(CJob *pJob)
{
	m_pMainFrm->m_wndJobListView.UpdateJobList(m_work.GetJobs());
}

void CIWSDoc::OpenLaserPen(CString strName)
{
	CFile file;
	if (!file.Open(strName, CFile::modeRead)) 
	{
		AfxMessageBox(L"Laser Pen file open fail");
		return;
	}

	CArchive ar(&file, CArchive::load);
	int nPen = 0;
	ar >> nPen;
	m_pens.clear();

	for (int i = 0; i < nPen; i++) 
	{
		CLaserPen pen;
		pen.Serialize(ar);
		switch (m_config.m_iLaser)
		{
			case IPG_CW_YLR2000SM:
			case TRUMPF_TRUDISK:
			case COHERENT_ROFIN:
				pen.m_bUsePulseShape = FALSE;
			break;
			case IPG_CW_YLR6000QCW:
				pen.m_bUsePulseShape = FALSE;
			break;
			case IPG_PULSE_YLR6000QCW:
				pen.m_bUsePowerShape = FALSE;
			break;
		}

		m_pens.push_back(pen);
	}

	ar.Close();
	file.Close();
	m_strFileLaserPen = strName;
}

void CIWSDoc::SaveLaserPen(CString strName)
{
	if (m_pens.empty()) return;
	CFile file;
	if (!file.Open(strName, CFile::modeCreate | CFile::modeWrite)) 
	{
		AfxMessageBox(L"Laser Pen file open fail");
		return;
	}

	CArchive ar(&file, CArchive::store);
	int nPen = (int)m_pens.size();
	ar << nPen;

	for (int i = 0; i < nPen;i++) 
	{
		m_pens[i].Serialize(ar);
	}

	ar.Close();
	file.Close();
}

void CIWSDoc::ImportDxf()
{
	CString fileName;

	CString strFilter;
	strFilter = _T("Dxf File(*.dxf)|*.dxf||");
	CString strTitle = this->GetTitle();
	int idot = strTitle.ReverseFind('.');
	CString strDefaultFilename = strTitle.Mid(0, idot);
	CFileDialog dlgFile(
		TRUE,														//BOOL bOpenFileDialog,
		_T("*.dxf"),												//LPCTSTR lpszDefExt = NULL,
		strDefaultFilename,											//LPCTSTR lpszFileName = NULL,												
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_LONGNAMES,		//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter													//LPCTSTR lpszFilter = NULL,
																	//CWnd* pParentWnd = NULL,
																	//DWORD dwSize = 0,
																	//BOOL bVistaStyle = TRUE
	);

	if (dlgFile.DoModal() != IDOK) return;
	fileName = dlgFile.GetPathName();

	OnNewDocument();
	BOOL   bRet;
	bRet = lcDrwLoad(g_hLcDrw, fileName, g_hLcWnd);
	if (bRet)
		ParseDxf();
}

void CIWSDoc::AddHatch(double dist, double angle)
{
	HANDLE hBlock = lcPropGetHandle(g_hLcWnd, LC_PROP_WND_BLOCK);
	HANDLE hSel = lcBlockGetFirstSel(hBlock);
	//HANDLE hFill = lcDrwAddFilling(g_hLcDrw, L"Hatch1");

	if (!hSel)
		return;

	HANDLE hHandle = lcBlockAddHatch(hBlock, L"", L"0, 0,0, 0,1", dist, angle*LC_DEG_TO_RAD);
	CJob* p = m_work.GetJob(hSel);	
	if (p) 
		p->GetEntity()->m_hHatch = hHandle;

	lcBlockUnselect( hBlock );
	lcBlockUpdate( hBlock, false, 0 );
	lcPropPutBool(hHandle, LC_PROP_HATCH_ASSOC, FALSE);
	DrawAddEntity(hHandle);
	lcWndRedraw(g_hLcWnd);
}

BOOL CIWSDoc::StartWork(BOOL bSimulation, double dRatioPwr, double dRatioSpd)
{
	if (GetUseInterface()) // Online
	{
		/*if (!m_plc.GetReadyWelding() && !bSimulation ||
			!m_plc.GetReadyWelding() && m_pScanner->INT_BE_EmissionEnable()) {
			AfxMessageBox(L"설비 상태를 확인 하세요");
			return FALSE;
		}*/
		if (!m_plc.GetReadyWelding()) 
		{
			_tracer(TRACE_ERROR, 1, L"Not Ready For Welding.");
			return FALSE;
		}

		// 프로그램 번호 체크 
		if (check_bit(m_status.scanDO, 8))
		{
			_tracer(TRACE_ERROR, 1, L"Program number is not 0 or 1.");
			return FALSE;
		}

		//// 레시피 번호 체크
		//int nJig = m_plc.GetWorkJig();
		//int nRcp = m_plc.GetRecipe();
		//CString strMsg;

		//if (nJig == 1)
		//{
		//	if(!(nRcp == 1 || nRcp == 3))
		//	{
		//		strMsg.Format(L"The recipe number is wrong. Jig: %d, Rcp: %d", nJig, nRcp);
		//		_tracer(TRACE_NORMAL, 1, strMsg);
		//		return FALSE;
		//	}
		//}
		//else if (nJig == 2)
		//{
		//	if (!(nRcp == 2 || nRcp == 4))
		//	{
		//		strMsg.Format(L"The recipe number is wrong. Jig: %d, Rcp: %d", nJig, nRcp);
		//		_tracer(TRACE_NORMAL, 1, strMsg);
		//		return FALSE;
		//	}
		//}
		//else
		//{
		//	strMsg.Format(L"Jig number is not correct.", nJig, nRcp);
		//	_tracer(TRACE_NORMAL, 1, strMsg);
		//	return FALSE;
		//}
	}

	m_work.m_bSimulation = bSimulation;
	m_pScanner->SetRatio(dRatioPwr, dRatioSpd);
	m_work.SetReceiveHandle(m_pMainFrm->GetSafeHwnd());
	m_work.DoWork();
	return TRUE;
}

void CIWSDoc::InitializeIWS()
{
	_tracer(TRACE_NORMAL, 1, _T("Initializing Inteli Weld System : start"));
	// sjyi 2022-06-08 시리얼 다시 살림(남경1차는 시리얼 통신) -->
	// 20.03.05 hslee
	//if (m_CeminiSerial.ConnectEMini(m_config.m_nLightPort))
	//{
	//	Sleep(100);
	//	m_CeminiSerial.SetLight(m_config.m_nLightPower);
	//	m_CeminiSerial.SetOnOff(true);
	//}
	// sjyi 2022-06-08 시리얼 다시 살림(남경1차는 시리얼 통신) <--

	if (!m_pScanner->Open(0)) 
	{
		CString str;
		str = _T("Failed to open the scanner controller");
		AfxMessageBox(str, MB_OK | MB_TOPMOST);
		_tracer(TRACE_ERROR, 1, str);
		_tracer(TRACE_NORMAL, 1, _T("Initializing Inteli Weld System : end"));
	}
	else 
	{
		switch (m_config.m_iLaser)
		{
		case TRUMPF_TRUDISK:
			m_pScanner->LaserEnable(FALSE);
			break;
		case COHERENT_ROFIN:
			m_pScanner->SetPower(0);
			m_pScanner->LaserOn(FALSE);
			break;
		}

		m_pScanner->SetLaserPen(&m_pens[0]);
		m_pScanner->SetLaserPenParam();
		m_pScanner->Move(0, 0);
		m_pScanner->WaitMoving();
		m_pScanner->LaserIOInit();
	//	m_pScanner->INT_DO_ErrorReset(TRUE);
	//	Sleep(100);
	//#if (LASER_SOURCE==_TRUMF_LASER)
	//	// Laser Power Power ON
	//	m_pScanner->LaserPowerON(TRUE);
	//	Sleep(5000);
	//	m_pScanner->INT_DO_REQUEST(TRUE);
	//#endif // LASER_SOURCE==_TRUMF_LASE
	//	m_pScanner->INT_DO_ErrorReset(FALSE);


		/*m_pScanner->SetLaserPowerSupplyON(FALSE);
		Sleep(500);
		m_pScanner->SetLaserPowerSupplyON(TRUE);*/
	}

	if (m_config.m_bUseLight)
	{
		m_pScanner->LightOn(TRUE);
	}

	m_work.SetReceiveHandle(m_pMainFrm->GetSafeHwnd());
	Sleep(100);

	if (m_config.m_bUsePLC)
	{
		m_plc.Open(m_config.m_strPLC_IP, m_config.m_iPLC_Port, m_config.m_nJigNo, m_config.m_nPLC_Read, m_config.m_nPLC_Write);

		m_plcData = new CEthernetPLC(NULL, m_config.m_strPLC_IP, m_config.m_iPLC_Port + 2);
		if (m_plcData->PLC_Connect() == FALSE) {
			_tracer(TRACE_NORMAL, 9999, L"PLC Data Connection Error(IP : %s, port : %d)", m_config.m_strPLC_IP, m_config.m_iPLC_Port+2);
		}
		else {
			_tracer(TRACE_NORMAL, 9999, L"PLC Data Connected(IP : %s, port : %d)", m_config.m_strPLC_IP, m_config.m_iPLC_Port+2);
		}
	}

	ChangeScanRange(); // mspark 2020.06.17

	AfxBeginThread(InterfaceThread, this);

	// open deafult recipe
	CString strLastFile = theApp.GetProfileStringW(L"IWS", L"LAST_WORK", L"default.wrk");
	CFileFind finder;
	BOOL bFlag = finder.FindFile(strLastFile);
	if (!bFlag) 
		AfxMessageBox(L"Failed to open default recipe. Create a recipe.");

	OpenWork(strLastFile);
	//m_plc.SetInit(TRUE);
	_tracer(TRACE_NORMAL, 1, _T("Initializing Inteli Weld System : end"));
}

void CIWSDoc::TerminateIWS()
{
	g_bGrabThreadAlive = FALSE;
	m_bRunInterfaceThread = FALSE;
	Sleep(500);

	if (m_config.m_bUseLight)
	{
		m_pScanner->LightOn(FALSE);
	}

//	// Statical을 먼저 죽이고
//	m_pScanner->LaserEnable(FALSE);
//	m_pScanner->INT_DO_REQUEST(FALSE);
//	//m_pScanner->SetLaserPowerSupplyON(FALSE);
//
//#if (LASER_SOURCE == _TRUMF_LASER)
//	// TRUMPF Laser Power OFF
//	m_pScanner->LaserPowerON(FALSE);
	m_pScanner->LaserIOTerminate();
	Sleep(500);
	
//#endif // (LASER_SOURCE == _TRUMF_LASER)
	m_pScanner->Close();
}


#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CIWSDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CIWSDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CIWSDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CIWSDoc 진단

#ifdef _DEBUG
void CIWSDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIWSDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}


#endif //_DEBUG


// CIWSDoc 명령


void CIWSDoc::OnFileSave()
{
	CString fileName;

	CString strFilter;
	// sjyi 2025.09.29 Jason, wrk 모두 대응 가능하도록 수정 -->
	//strFilter = _T("Work File(*.wrk)|*.wrk||");
	strFilter = _T("Work Files (*.wrk)|*.wrk|JSON Files (*.json)|*.json|All Files (*.*)|*.*||");
	// sjyi 2025.09.29 Jason, wrk 모두 대응 가능하도록 수정 <--

	CString strTitle = this->GetTitle();
	int idot = strTitle.ReverseFind('.');
	CString strDefaultFilename = strTitle.Mid(0, idot);
	CFileDialog dlgFile(
		FALSE,														//BOOL bOpenFileDialog,
		_T("*.wrk"),													//LPCTSTR lpszDefExt = NULL,
		strDefaultFilename,											//LPCTSTR lpszFileName = NULL,												
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_LONGNAMES,		//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter													//LPCTSTR lpszFilter = NULL,
																	//CWnd* pParentWnd = NULL,
																	//DWORD dwSize = 0,
																	//BOOL bVistaStyle = TRUE
	);

	if (dlgFile.DoModal() != IDOK) return;

	fileName = dlgFile.GetPathName();
	SaveFile(fileName);
}

void CIWSDoc::SaveFile(CString fileName, BOOL isCurrent, BOOL bBackUP)
{
	if (bBackUP)
		BackUpWork(fileName); // sjyi 2024.06.14 추가, 안전 및 수정사항 확인을 위해 추가 

	CFile file;
	if (!file.Open(fileName, CFile::modeCreate | CFile::modeReadWrite)) 
		return;

	if (isCurrent)
		m_work.OffsetToCurrent();

	// sjyi 2025.09.29 Jason 사용을 위한 함수 추가 --->
	//m_work.SaveWork(&file, this);
	//file.Close();

	BOOL bJson = CheckJson(fileName);
	if (bJson) {
		m_work.SaveWorkJson(&file, this);
	}
	else {
		m_work.SaveWork(&file, this);
	}

	file.Close();
	// sjyi 2025.09.29 Jason 사용을 위한 함수 추가 <---
}


void CIWSDoc::OnFileOpen()
{
	CString fileName;

	CString strFilter;
	
	// sjyi 2025.09.29 Jason, wrk 모두 대응 가능하도록 수정 -->
//	strFilter = _T("Work File(*.wrk)|*.wrk||");
	strFilter = _T("Work Files (*.wrk)|*.wrk|JSON Files (*.json)|*.json|All Files (*.*)|*.*||");
	// sjyi 2025.09.29 Jason, wrk 모두 대응 가능하도록 수정 <--

	CString strTitle = this->GetTitle();
	int idot = strTitle.ReverseFind('.');
	CString strDefaultFilename = strTitle.Mid(0, idot);
	CFileDialog dlgFile(
		TRUE,														//BOOL bOpenFileDialog,
		_T("*.wrk"),													//LPCTSTR lpszDefExt = NULL,
		strDefaultFilename,											//LPCTSTR lpszFileName = NULL,												
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_LONGNAMES,		//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter													//LPCTSTR lpszFilter = NULL,
																	//CWnd* pParentWnd = NULL,
																	//DWORD dwSize = 0,
																	//BOOL bVistaStyle = TRUE
	);

	if (dlgFile.DoModal() != IDOK) return;
	fileName = dlgFile.GetPathName();

	OpenWork(fileName);
}

// sjyi 2022-06-03 comment 워크파일 로드 -->
BOOL  CIWSDoc::OpenWork(CString strFilename)
{
	CFile file;
	if (!file.Open(strFilename, CFile::modeReadWrite)) 
		return FALSE;

	_tracer(TRACE_NORMAL, 1, L"Open Work File : %s", strFilename);
	OnNewDocument();

	// sjyi 2025.09.29 Jason, wrk 모두 대응 가능하도록 수정 --> 
	//BOOL bRet = m_work.LoadWork(&file, this);
	//file.Close();
	BOOL bRet = FALSE;
	BOOL bChkJson = CheckJson(strFilename);

	if (bChkJson) {
		file.Close();
		bRet = m_work.OpenWorkJson(strFilename, this);
	}
	else {
		bRet = m_work.LoadWork(&file, this);
		file.Close();
	}
	// sjyi 2025.09.29 Jason, wrk 모두 대응 가능하도록 수정 <--

	//lcWndRedraw(g_hLcWnd);
	lcWndExeCommand(g_hLcWnd, LC_CMD_ZOOM_EXT, 0);

	m_pMainFrm->m_wndJobListView.UpdateJobList(m_work.GetJobs());
	m_pMainFrm->m_wndPropertiesView.ShowJobProperty((CJob*)NULL);
	m_pMainFrm->m_wndWorkView.ClearWorkImage();

	m_pMainFrm->m_wndWorkView.UpdateWork();

	m_pDrawView->SetWorkArea();
	m_pDrawView->SortLayerOrder();
	this->SetTitle(strFilename);
	theApp.WriteProfileStringW(L"IWS", L"LAST_WORK", strFilename);
	_tracer(TRACE_NORMAL, 1, L"Open Done Work File : %s", strFilename);
	CString name = strFilename.Right(strFilename.GetLength() - strFilename.ReverseFind('\\') - 1);

	m_pHomeView->SetCurrentWorkFile(name);
	m_plc.SetLaserPower(m_work.GetAverageLaserPower());

	m_strLastFilePath = strFilename;
	m_strLastFileName = name;

	// 2022.09.06 jh.kim 추가 - 용접전 센터로 이동 추가
	// -->
	CJob *pJob;
	if (m_work.m_JobGroup[0]->m_jobs.size() == 4)
		pJob = (CJob *)m_work.GetJob(3);
	else
		pJob = (CJob *)m_work.GetJob(2);

	CEntity *pEntity = pJob->GetEntity();
	Point3Dbl cen    = pEntity->m_cen;
	Point3Dbl offset = pEntity->m_offset;
	m_pMainFrm->m_wndWorkView.m_pWorkView->GotoScaner(cen.x+offset.x, cen.y+offset.y);
	// <--

// 	if(bRet) 
// 		m_plc.SetRecipe(1);
// 	else 
// 		m_plc.SetRecipe(0);
	return TRUE;
}

BOOL CIWSDoc::Update2DPosition()
{
	if (!m_config.m_bUse2DProc)
		return FALSE;

	if (!m_config.m_bUseHairpin)
		return FALSE;

	if (!m_work.m_JobGroup.size())
		return FALSE;

	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof path);

	CString strStartUpPath = path;
	int i = strStartUpPath.ReverseFind('\\'); //실행 파일 이름을 지우기 위해서 왼쪽에 있는 '/'를 찾는다.
	strStartUpPath = strStartUpPath.Left(i); //뒤에 있는 현재 실행 파일 이름을 지운다.

	CString strSavePath;
	strSavePath.Format(_T("%s\\Image\\PreWeldInfo.txt"), strStartUpPath);

	CString strLoadPath;
	strLoadPath.Format(_T("%s\\InspectionResult\\PWI_Inspection_Results.csv"), strStartUpPath);

	// 파일삭제
	DeleteFile(strSavePath);
	DeleteFile(strLoadPath);

	// 좌표 남기기 
	CFile file;
	if (!file.Open(strSavePath, CFile::modeCreate | CFile::modeReadWrite))
	{
		_tracer(TRACE_ERROR, 1, _T("File Save Fail IwsPosition.dst"));
		AfxMessageBox(L"File Save Fail IwsPosition.dst ");
		return FALSE;
	}

	// 좌표 파일 남기기
	CString msg;
	int nCount = 0;
	for (auto group : m_work.m_JobGroup)
		nCount++;

	msg.Format(L"%d\n", nCount * 2);	// Count
	file.Write(msg, msg.GetLength() * sizeof(TCHAR));

	int nIdx = 0;
	double x, y;
	CEntity *pEnt;
	float resol_x, resol_y;

	CorrParam p = m_config.GetCorrParam(m_config.m_iCurrLens);
	GetCcdResolution(&resol_x, &resol_y);

	for (auto group : m_work.m_JobGroup)
	{
		for (int i = 0; i < 2; i++)
		{
			pEnt = group->m_jobs[i]->GetEntity();
			pEnt->m_offset.x = 0;
			pEnt->m_offset.y = 0;
			// 좌표변환
			x = (pEnt->m_cen.x - p.dblImgCenX) / resol_x;
			y = (pEnt->m_cen.y - p.dblImgCenY) / resol_y;

			msg.Format(L"%d,%.2f,%.2f\n", nIdx, x, y);
			file.Write(msg, msg.GetLength() * sizeof(TCHAR));
			nIdx++;
		}
	}
	file.Close();

	// 프로그램 실행 후 대기
	int nRcp = 0;
	if (m_nLastRcpIdx == 0)
		nRcp = m_plc.GetRecipe();
	else
		nRcp = m_nLastRcpIdx;

	CString strProcess;
	strProcess.Format(_T("%s\\PWI\\PWI.exe %d %d"), strStartUpPath, m_work.m_nModel, 
		nRcp < 5 ? 1 : 2); // 1~4(1): 8 Layer, 5~8(2): 6 Layer

	WinExec((CStringA)strProcess, SW_HIDE);

	CFileFind find;
	nIdx = 0;
	int nSleep = 100;
	while (TRUE)
	{
		if (find.FindFile(strLoadPath))
		{
			break;
		}

		if (nIdx == 5000)
		{
			_tracer(TRACE_ERROR, 1, _T("2D Inspection TimeOut"));
			AfxMessageBox(L"2D Inspection TimeOut ");
			return FALSE;
		}

		nIdx += nSleep;
		Sleep(nSleep);
	}

	std::vector<CString> readList;
	/*CStringA strTmp;
	char tmp[1024];
	FILE  *pf;
	fopen_s(&pf, CStringA(strLoadPath).GetBuffer(), "r");
	fread_s(tmp, 1024, 1, 1024, pf);
	for (int i = 0; i < 48; i++) {

		fread_s(tmp, 1024, 1, 1024, pf);
	}
	fclose(pf);*/

	// ==============================================================
	CStdioFile rFile;
	CString strTmp;
	if (!rFile.Open(strLoadPath, CFile::modeRead))
	{
		CString strMsg;
		strMsg.Format(L"File Open Fail %s", strLoadPath);
		AfxMessageBox(strMsg);
		return FALSE;
	}

	while (rFile.ReadString(strTmp))
		readList.push_back(strTmp);

	rFile.Close();

	// ==============================================================
	/*CFile openFile;
	if (!openFile.Open(strLoadPath, CFile::modeRead))
	{
		CString strMsg;
		strMsg.Format(L"File Open Fail %s", strLoadPath);
		AfxMessageBox(strMsg);
		return FALSE;
	}

	CArchive ar(&openFile, CArchive::load);
	CString strTmpU;
	while (ar.ReadString(strTmpU))
	{
		if (!strTmpU.GetLength())
			break;

		readList.push_back(strTmpU);
	}
	openFile.Close();*/

	// 읽은 좌표 데이터 처리
	//if (readList.size() != ((m_work.m_JobGroup.size() * 2) + 1))
	//{
	//	_tracer(TRACE_ERROR, 1, _T("(PositionList & JobGroup) Different sizes"));
	//	AfxMessageBox(L"(PositionList & JobGroup) Different sizes ");
	//	return FALSE;
	//}

	nIdx = 0;
	for (int i = 1; i < readList.size(); i += 2)
	{
		if (!Process2D(readList[i], nIdx, 0))
			return FALSE;

		if (!Process2D(readList[i + 1], nIdx, 1))
			return FALSE;

		nIdx++;
	}

	m_work.AlignHairpin();

	return TRUE;
}

BOOL CIWSDoc::Process2D(CString strTokken, int nIdx, int nJob)
{
	std::vector<CString> tokken = SplitString(strTokken);
	if (!tokken.size())
	{
		_tracer(TRACE_ERROR, 1, _T("Split Count in NULL"));
		AfxMessageBox(L"Split Count in NULL ");
		return FALSE;
	}

	if (tokken[2] == "-99999.9" || tokken[2] == "-99999.9")
		return TRUE;

	CorrParam p = m_config.GetCorrParam(m_config.m_iCurrLens);
	float resol_x, resol_y;
	GetCcdResolution(&resol_x, &resol_y);
	float sx, sy;
	g_pGrabImgRunner->GetCurFullFov(&sx, &sy);

	// 좌표변환
	double x, y;
	x = ((_wtof(tokken[2]) * resol_x*2)/1000.0 - sx/2.0);
	y = ((_wtof(tokken[3]) * resol_y*2)/1000.0 - sy/2.0) ;

	((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[nJob])->m_dX = x + p.dblImgCenX;
	((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[nJob])->m_dY = -y + p.dblImgCenY;
	SetOffsetEntity(((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[nJob]));

	return TRUE;
}

BOOL CIWSDoc::Update3DPosition()
{
	if (!m_config.m_bUseHairpin)
		return FALSE;

	if (!m_work.m_JobGroup.size())
		return FALSE;

	CString strMsg;
	int nJig = m_plc.GetWorkJig();
	strMsg.Format(L"Update3DPosition Read Work Jig: %d", nJig);
	_tracer(TRACE_NORMAL, 1, strMsg);

	if (nJig == 1)
	{
		nJig = PLC_READ_INSPECT_A;
	}
	else if (nJig == 2)
	{
		nJig = PLC_READ_INSPECT_B;
	}
	else
	{
		strMsg.Format(L"Update3DPosition None Work Jig: %d", nJig);
		_tracer(TRACE_ERROR, 1, strMsg);
		return FALSE;
	}

	// Bottom 77: 6144
	// Bottom 76: 6144
	// CRING 77: ??
	// CRING 76: ??
	// 8Layer 기준으로 Read 하고 없는 Layer 건너뛰기
	int nWordCnt = (m_work.m_hp.nDrawSlot * 16) * (m_work.m_hp.nLayer / 2);
	strMsg.Format(L"Update3DPosition Read Word Count: %d", nWordCnt);
	_tracer(TRACE_NORMAL, 1, strMsg);

	if (m_bImportPLCData == FALSE)
	{
		if (!m_plcData->UpdateInput_InspData(nJig, nWordCnt))
		{
			_tracer(TRACE_ERROR, 1, _T("Vision Posiion Read Fail"));
			return FALSE;
		}
	}

	/*int nRcp = m_plc.GetRecipe();*/
	int nRcp = m_nLastRcpIdx;
	strMsg.Format(L"Update3DPosition RCP No: %d", nRcp);
	_tracer(TRACE_NORMAL, 1, strMsg);

	double dblScale = 1.0;
	double dblAngle = 0.0;
	double dblOffsetX = 0.0;
	double dblOffsetY = 0.0;
	
	if (nRcp > 0 && nRcp < 7)
	{
		dblScale = m_config.m_dRcpScale[nRcp - 1];
		dblAngle = m_config.m_dRcpAngle[nRcp - 1] * D2R;
		dblOffsetX = m_config.m_dRcpOffsetX[nRcp - 1];
		dblOffsetY = m_config.m_dRcpOffsetY[nRcp - 1];

		if (dblScale == 0)
			dblScale = 1;
	}

	// 총 15개 (예비 데이터 포함)
	// Result	1H	2H	3H	1A	2A	aX	aY	bX	bY	C	Jump	θ	reserved1	reserved2  reserved3
	//   0      1   2   3   4   5   6   7   8   9   10   11     12     13          14          15
	int nIdx = 0;
	double ax, ay, az, bx, by, bz;
	int nMaxLayer = m_work.m_nDefLayer / 2;
	int nLayer = 0;

	// 2022-07-07 Matrix 변수 
	double a11, a12, a13, a21, a22, a23;
	double Xa, Xb, Ya, Yb;

	int nPinNo = 0, nMatrixNo = -1, nNo = 0;
	int nMcount = (int)((m_work.m_hp.nDrawSlot * m_work.m_hp.nLayer / 2) / MATRIX_DIVISION);

	for (int i = 0; i < nWordCnt; i += 16)
	{
		nPinNo = (int)(i / 16);
		if(nMcount > 0)
			nNo = (int)(nPinNo / nMcount);
		else
			nNo = 1;

		if (nMatrixNo != nNo) {
			nMatrixNo = nNo;
			// Jig No에 따른 Maxtrix 결과 사용
			switch (nJig)
			{
			case PLC_READ_INSPECT_A:	// Jig A
				a11 = m_config.m_MatrixA[nMatrixNo].a11;
				a12 = m_config.m_MatrixA[nMatrixNo].a12;
				a13 = m_config.m_MatrixA[nMatrixNo].a13;
				a21 = m_config.m_MatrixA[nMatrixNo].a21;
				a22 = m_config.m_MatrixA[nMatrixNo].a22;
				a23 = m_config.m_MatrixA[nMatrixNo].a23;
				break;
			case PLC_READ_INSPECT_B:	// Jig B
				a11 = m_config.m_MatrixB[nMatrixNo].a11;
				a12 = m_config.m_MatrixB[nMatrixNo].a12;
				a13 = m_config.m_MatrixB[nMatrixNo].a13;
				a21 = m_config.m_MatrixB[nMatrixNo].a21;
				a22 = m_config.m_MatrixB[nMatrixNo].a22;
				a23 = m_config.m_MatrixB[nMatrixNo].a23;
				break;
			default: // Unknown
				a11 = m_config.m_MatrixA[nMatrixNo].a11;
				a12 = m_config.m_MatrixA[nMatrixNo].a12;
				a13 = m_config.m_MatrixA[nMatrixNo].a13;
				a21 = m_config.m_MatrixA[nMatrixNo].a21;
				a22 = m_config.m_MatrixA[nMatrixNo].a22;
				a23 = m_config.m_MatrixA[nMatrixNo].a23;
				break;
			}
			strMsg.Format(L"[%.6f][%.6f][%.6f][%.6f][%.6f][%.6f]", a11, a12, a13, a21, a22, a23);
			_tracer(TRACE_NORMAL, 1, L"Calibration Metrix no %02d - [a11 ~ a13, a21 ~ a23]= %s", nMatrixNo, strMsg);
		}

		// 해당 Layer가 아닌 경우 건너뛰기
		nLayer = m_work.m_JobGroup[nIdx]->m_nIndex % (m_work.m_nDefLayer / 2) + 1;
		if (nLayer > nMaxLayer)
			continue;

		m_work.m_JobGroup[nIdx]->m_d3H   = (m_plcData->m_rDataInsp[i + 3] / 100.0);
		//m_work.m_JobGroup[nIdx]->m_d4H   = (m_plcData->m_rDataInsp[i + 4] / 100.0);
		m_work.m_JobGroup[nIdx]->m_dC    = (m_plcData->m_rDataInsp[i + 10] / 100.0);
		m_work.m_JobGroup[nIdx]->m_dJump = (m_plcData->m_rDataInsp[i + 11] / 100.0);

		//0 = NG
		//1 = OK
		//2 = Spect Out(Spect Out도 추 후에 수동 용접 가능성이 있으므로 좌표 업데이트는 하되 비활성화 되는걸로) - 21.07.22 PJM
		//if (m_plcData->m_rDataInsp[i]) // OK or Spec Out
		if ((m_plcData->m_rDataInsp[i] == 1) || m_plcData->m_rDataInsp[i] == 2)
		{
			//RCP 별 Layer로 분할 용접이므로 RCP 번호에 맞게 해당 레이어만 활성화 21.07.29
			//A Jig(Uiwang Front & Rear)		//B Jig(Atop Front & Rear)
			//RCP 1 - 일괄 용접						//RCP 6 - 일괄 용접
			//RCP 2 - 1-2 Layer 분할 용접				//RCP 7 - 1-2 Layer 분할 용접
			//RCP 3 - 3-4 Layer 분할 용접				//RCP 8 - 3-4 Layer 분할 용접
			//RCP 4 - 5-6 Layer 분할 용접				//RCP 9 - 5-6 Layer 분할 용접
			//RCP 5 - 7-8 Layer 분할 용접				//RCP 10 - 7-8 Layer 분할 용접			

			//  2023.01.18 jh.kim 수정 - 2차 용접기 Offset으로 적용
			/*
			//일괄용접, 분할용접(RCP 별로 Layer 용접) 처리 - 21.07.29 PJM
			if (nRcp == 1 || nRcp == 6)
			{
				for (int j = 0; j < m_work.m_JobGroup[nIdx]->m_jobs.size(); j++)
				{
					//전 검사 결과가 OK면 활성화
					if (m_plcData->m_rDataInsp[i] == 1)
						((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(TRUE);
					else
						//NG면 좌표 업데이트는 하되 비 활성화
						((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(FALSE);
				}
			}
			//1-2 Layer 활성
			else if (nRcp == 2 || nRcp == 7)
			{
				if (nLayer == 1)
				{
					for (int j = 0; j < m_work.m_JobGroup[nIdx]->m_jobs.size(); j++)
					{
						//전 검사 결과가 OK면 활성화
						if (m_plcData->m_rDataInsp[i] == 1)
							((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(TRUE);
						else
							//NG면 좌표 업데이트는 하되 비 활성화
							((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(FALSE);
					}
				}
				else
				{
					//RCP 별 해당 레이어 외에 다른 레이어 비활성화 
					for (int j = 0; j < m_work.m_JobGroup[nIdx]->m_jobs.size(); j++)
					{
						((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(FALSE);
					}
				}
			}
			//3-4 Layer 활성
			else if (nRcp == 3 || nRcp == 8)
			{
				if (nLayer == 2)
				{
					for (int j = 0; j < m_work.m_JobGroup[nIdx]->m_jobs.size(); j++)
					{
						//전 검사 결과가 OK면 활성화
						if (m_plcData->m_rDataInsp[i] == 1)
							((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(TRUE);
						else
							//NG면 좌표 업데이트는 하되 비 활성화
							((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(FALSE);
					}
				}
				else
				{
					//RCP 별 해당 레이어 외에 다른 레이어 비활성화 
					for (int j = 0; j < m_work.m_JobGroup[nIdx]->m_jobs.size(); j++)
					{
						((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(FALSE);
					}
				}
			}
			//5-6 Layer 활성
			else if (nRcp == 4 || nRcp == 9)
			{
				if (nLayer == 3)
				{
					for (int j = 0; j < m_work.m_JobGroup[nIdx]->m_jobs.size(); j++)
					{
						//전 검사 결과가 OK면 활성화
						if (m_plcData->m_rDataInsp[i] == 1)
							((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(TRUE);
						else
							//NG면 좌표 업데이트는 하되 비 활성화
							((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(FALSE);
					}
				}
				else
				{
					//RCP 별 해당 레이어 외에 다른 레이어 비활성화 
					for (int j = 0; j < m_work.m_JobGroup[nIdx]->m_jobs.size(); j++)
					{
						((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(FALSE);
					}
				}
			}
			//7-8 Layer 활성
			else if (nRcp == 5 || nRcp == 10)
			{
				if (nLayer == 4)
				{
					for (int j = 0; j < m_work.m_JobGroup[nIdx]->m_jobs.size(); j++)
					{
						//전 검사 결과가 OK면 활성화
						if (m_plcData->m_rDataInsp[i] == 1)
							((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(TRUE);
						else
							//NG면 좌표 업데이트는 하되 비 활성화
							((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(FALSE);
					}
				}
				else
				{
					//RCP 별 해당 레이어 외에 다른 레이어 비활성화 
					for (int j = 0; j < m_work.m_JobGroup[nIdx]->m_jobs.size(); j++)
					{
						((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(FALSE);
					}
				}
			}
			*/

			az = floor(m_plcData->m_rDataInsp[i +  1]) / 100;
			bz = floor(m_plcData->m_rDataInsp[i +  2]) / 100;

			ax = floor(m_plcData->m_rDataInsp[i +  6]) / 100;
			ay = floor(m_plcData->m_rDataInsp[i +  7]) / 100;
			bx = floor(m_plcData->m_rDataInsp[i +  8]) / 100;
			by = floor(m_plcData->m_rDataInsp[i +  9]) / 100;


			//===============================================================
			// 높이에따른 보정 시작
			//===============================================================
			/*
			// 1차 내경 보정
			double dRadius = sqrt((ax*ax) + (ay*ay));
			double dCos = ax / dRadius;
			double dSin = ay / dRadius;

			double dr = dRadius - m_dBaseR;
			double drOffset = m_dHRRatio * (az - m_dBaseH);;
			double drCorr = (dr > 0.0) ? (dr - drOffset) : (dr + drOffset);
			if (drOffset > 0.1)
			{
				TRACE(_T("\n(%d, %d, a) H=%f, offset =%f \t\t"), (int)(nIdx / 4), nIdx % 4, az, drOffset);
			}
			double dNewR = m_dBaseR + drCorr;
			ax = dNewR * dCos;
			ay = dNewR * dSin;


			// 1차 외경 보정
			dRadius = sqrt((bx*bx) + (by*by));
			dCos = bx / dRadius;
			dSin = by / dRadius;

			dr = dRadius - m_dBaseR;
			drOffset = m_dHRRatio * (bz - m_dBaseH);;
			drCorr = (dr > 0.0) ? (dr - drOffset) : (dr + drOffset);
			if (drOffset > 0.1)
			{
				TRACE(_T("\n(%d, %d, b) H=%f, offset =%f \t\t"), (int)(nIdx / 4), nIdx % 4, bz, drOffset);
			}
			dNewR = m_dBaseR + drCorr;
			bx = dNewR * dCos;
			by = dNewR * dSin;


			ax +=m_dCenOffX;
			ay +=m_dCenOffY;
			bx += m_dCenOffX;
			by += m_dCenOffY;
			*/
			//===============================================================
			// 높이에따른 보정 끝
			//===============================================================

			//if ((ax == 0 && ay == 0) || (bx == 0 && by == 0) || az == 0 || bz == 0)
			//if (ax == 0 || ay == 0 || bx == 0 || by == 0)
			//if (fabs(ax) == 0.0001 || fabs(ay) == 0.00001 || fabs(bx) == 0.0001 || fabs(by) == 0.0001)
			//  2023.01.18 jh.kim 수정 - 2차 용접기 Offset으로 적용
			/*if ((ax == 0 && ay == 0) || (bx == 0 && by == 0))
			{
				for (int j = 0; j < m_work.m_JobGroup[nIdx]->m_jobs.size(); j++)
				{
					((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(FALSE);
					((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->GetEntity()->SetOffset(0, 0);
				}
			}
			else
			{*/
				// sjyi 2022-07-08 MOB와 각도 계산 관련 내용 다름 확인 필요. -->
				// 일단 MOB와 같은 코드로 변경, 주석 처리
				// Angle				
				// ax = (ax * cos(dblAngle)) - (ay * sin(dblAngle));
				// ay = (ax * sin(dblAngle)) + (ay * cos(dblAngle));
				// bx = (bx * cos(dblAngle)) - (by * sin(dblAngle));
				// by = (bx * sin(dblAngle)) + (by * cos(dblAngle));
				// sjyi 2022-07-08 MOB와 각도 계산 관련 내용 다름 확인 필요. <--

				// 2023.01.18 jh.kim 수정 - 2차 용접기 Offset으로 적용
				/*
				//좌표 매칭을 위한 Metrix 계산 - 21.11.25 PJM
				Xa = (a11 * ax) + (a12 * ay) + a13;
				Ya = (a21 * ax) + (a22 * ay) + a23;
				Xb = (a11 * bx) + (a12 * by) + a13;
				Yb = (a21 * bx) + (a22 * by) + a23;

				////Degree Offset(pair)
				//if (nLayer == 1)
				//	dblAngle = m_work.m_hp.dDegree12 * D2R;
				//else if (nLayer == 2)
				//	dblAngle = m_work.m_hp.dDegree34 * D2R;
				//else if (nLayer == 3)
				//	dblAngle = m_work.m_hp.dDegree56 * D2R;
				//else if (nLayer == 4)
				//	dblAngle = m_work.m_hp.dDegree78 * D2R;
				//else if (nLayer == 5)
				//	dblAngle = m_work.m_hp.dDegree90 * D2R;

				// Scale, Offset
				//ax = ax * dblScale + dblOffsetX + m_work.m_hp.hpOffset[nIdx].ax;
				//ay = ay * dblScale + dblOffsetY + m_work.m_hp.hpOffset[nIdx].ay;
				//bx = bx * dblScale + dblOffsetX + m_work.m_hp.hpOffset[nIdx].bx;
				//by = by * dblScale + dblOffsetY + m_work.m_hp.hpOffset[nIdx].by;

				//각 슬롯 별 각도 옵셋
				ax = (Xa * cos(dblAngle)) - (Ya * sin(dblAngle));
				ay = (Xa * sin(dblAngle)) + (Ya * cos(dblAngle));
				bx = (Xb * cos(dblAngle)) - (Yb * sin(dblAngle));
				by = (Xb * sin(dblAngle)) + (Yb * cos(dblAngle));

				// 개별 Slot, Layer Offset
				ax = ax * dblScale + dblOffsetX + m_work.m_hp.hpOffset[nIdx].ax;
				ay = ay * dblScale + dblOffsetY + m_work.m_hp.hpOffset[nIdx].ay;
				bx = bx * dblScale + dblOffsetX + m_work.m_hp.hpOffset[nIdx].bx;
				by = by * dblScale + dblOffsetY + m_work.m_hp.hpOffset[nIdx].by;

				// 1차 패턴 내경
				((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[0])->m_dZ = az;
				((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[0])->m_dX = ax;
				((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[0])->m_dY = ay;
				SetOffsetEntity(((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[0]));

				// 1차 패턴 외경
				((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[1])->m_dZ = bz;
				((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[1])->m_dX = bx;
				((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[1])->m_dY = by;
				SetOffsetEntity(((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[1]));

				// sjyi 2022-06-09 Zigzag2일 경우, 2차 패턴에 대한 Update 필요 -->
				if (m_work.m_JobGroup[nIdx]->m_jobs.size() == 4)
				{
					// 2차 패턴 내경
					((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[2])->m_dZ = az;
					((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[2])->m_dX = ax;
					((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[2])->m_dY = ay;
					SetOffsetEntity(((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[2]));

					// 2차 패턴 외경
					((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[3])->m_dZ = bz;
					((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[3])->m_dX = bx;
					((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[3])->m_dY = by;
					SetOffsetEntity(((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[3]));
				}
				// sjyi 2022-06-09 Zigzag2일 경우, 2차 패턴에 대한 Update 필요 <--
			}
			*/

			// 2023.01.18 jh.kim 수정 - 2차 용접기 Offset으로 적용
			// 1차 패턴 내경
			//((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[0])->m_dZ = az;
			((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[0])->m_dX = ax;
			((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[0])->m_dY = ay;
			SetOffsetEntity(((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[0]));

			_tracer(TRACE_NORMAL, 999, _T("1st Offset X: %.3f, Y : %.3f"), ax, ay);
			double dOrgX = ((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[0])->GetEntity()->m_cen.x;
			double dOrgY = ((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[0])->GetEntity()->m_cen.y;
			_tracer(TRACE_NORMAL, 999, _T("1st Move (%.3f, %.3f) -> (%.3f, %.3f)"), dOrgX, dOrgY, dOrgX + ax, dOrgY + ay);

			// 1차 패턴 외경
			//((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[1])->m_dZ = bz;
			((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[1])->m_dX = bx;
			((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[1])->m_dY = by;
			SetOffsetEntity(((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[1]));

			_tracer(TRACE_NORMAL, 999, _T("2nd Offset X: %.3f, Y : %.3f"), bx, by);
			dOrgX = ((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[1])->GetEntity()->m_cen.x;
			dOrgY = ((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[1])->GetEntity()->m_cen.y;
			_tracer(TRACE_NORMAL, 999, _T("2nd Move (%.3f, %.3f) -> (%.3f, %.3f)"), dOrgX, dOrgY, dOrgX + bx, dOrgY + by);

			// sjyi 2022-06-09 Zigzag2일 경우, 2차 패턴에 대한 Update 필요 -->
			if (m_work.m_JobGroup[nIdx]->m_jobs.size() == 4)
			{
				// 2차 패턴 내경
				//((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[2])->m_dZ = az;
				((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[2])->m_dX = ax;
				((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[2])->m_dY = ay;
				SetOffsetEntity(((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[2]));

				// 2차 패턴 외경
				//((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[3])->m_dZ = bz;
				((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[3])->m_dX = bx;
				((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[3])->m_dY = by;
				SetOffsetEntity(((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[3]));
			}
		}
		else // NG
		{
			_tracer(TRACE_NORMAL, 999, _T("Inspection NG : %d"), nIdx);

			for (int j = 0; j < m_work.m_JobGroup[nIdx]->m_jobs.size(); j++)
			{
				((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->SetDoJob(FALSE);
				((CJobWeld*)m_work.m_JobGroup[nIdx]->m_jobs[j])->GetEntity()->SetOffset(0, 0);
			}
		}

		// 8Layer가 아닐 경우 Address Jump
		if (nLayer == nMaxLayer)
			i += (((m_work.m_hp.nLayer/2) - nMaxLayer) * 16);

		nIdx++;
	}

	// 리워크 데이터 Read
	if (m_plc.GetReWeld())
	{
		int jig = m_plc.GetWorkJig();
		if (jig == 1)
		{
			jig = PLC_READ_REPAIR_A;
		}
		else if (jig == 2)
		{
			jig = PLC_READ_REPAIR_B;
		}
		else
		{
			_tracer(TRACE_ERROR, 1, _T("Rework None Work Jig: " + jig));
			return FALSE;
		}

		_tracer(TRACE_NORMAL, 1, _T("Rework Read Work Jig: " + jig));

		nWordCnt = m_work.m_hp.nDrawSlot * (m_work.m_hp.nLayer / 2);
		if (!m_plcData->UpdateInput_RepairData(jig, nWordCnt))
		{
			_tracer(TRACE_ERROR, 1, _T("Repair Point Read Fail"));
			return FALSE;
		}

		for (int i = 0; i < nWordCnt; i++)
		{
			if (m_plcData->m_rDataRepair[i] != 2)	// Rework가 아닐 경우 비활성화
			{
				// sjyi 2022-06-13 Job 그룹 개수 3개 고정에서 4개 가변으로 변경 -->
				//for (int j = 0; j < 3; j++)
				for (int j = 0; j < m_work.m_JobGroup[i]->m_jobs.size(); j++)
				{
					((CJobWeld*)m_work.m_JobGroup[i]->m_jobs[j])->SetDoJob(FALSE);
					if (((CJobWeld*)m_work.m_JobGroup[i]->m_jobs[j])->GetEntity() != NULL)	// sjyi 2022-06-13 예외 처리 추가
					{
						((CJobWeld*)m_work.m_JobGroup[i]->m_jobs[j])->GetEntity()->SetOffset(0, 0);
					}
				}
				// sjyi 2022-06-13 Job 그룹 개수 3개 고정에서 4개 가변으로 변경 <--
			}
		}
	}

	m_work.AlignHairpin();
	m_pMainFrm->m_wndWorkView.UpdateWork();

	//m_pMainFrm->m_wndWorkView.m_pWorkView->m_cadDisplay.Update();

	/*m_pMainFrm->RecalcLayout();
	m_pMainFrm->m_pViews[0]->Invalidate();*/


	// 2024.01.29 jh.kim 추가 - 용접전 센터로 이동 추가
	// -->
	if ( m_work.m_JobGroup.size() > 0 )
	{
		_tracer(TRACE_NORMAL, 1, L"RCP OPEN - Scanner center position moving start");
		CJob *pJob0, *pJob1;
		if (m_work.m_JobGroup[0]->m_jobs.size() == 4)
			pJob0 = (CJob *)m_work.GetJob(3);
		else
			pJob0 = (CJob *)m_work.GetJob(2);

		pJob1 = (CJob *)m_work.GetJob(0);

		CEntity *pEntity0 = pJob0->GetEntity();
		Point3Dbl cen = pEntity0->m_cen;
		CEntity *pEntity1 = pJob1->GetEntity();
		Point3Dbl offset = pEntity1->m_offset;
		//m_pMainFrm->m_wndWorkView.m_pWorkView->GotoScaner(cen.x + offset.x, cen.y + offset.y);
		m_pMainFrm->m_wndWorkView.m_pWorkView->GotoScaner(cen.x + 0, cen.y + offset.y);

		_tracer(TRACE_NORMAL, 1, L"RCP OPEN - Scanner center position moving end");
	}
	// <--

	// sjyi 2024.06.14

	return TRUE;
}

void CIWSDoc::ChangePrgNo()
{
	CDlgLaserPrgNo dlg;
	if (dlg.DoModal() != IDOK)
		return;

	switch (m_config.m_iLaser)
	{
	case TRUMPF_TRUDISK:
		m_pScanner->LaserEnable(FALSE);
		m_pScanner->ChangePrgNo(dlg.m_nValue);
		break;
	case COHERENT_ROFIN:
		break;
	}
}

std::vector<CString> CIWSDoc::SplitString(CString strData)
{
	vector<CString> dataList;
	CString field;

	int index = 0;
	// last argument is the delimitter
	while (AfxExtractSubString(field, strData, index, _T(',')))
	{
		dataList.push_back(field);
		++index;
	}

	return dataList;
}

void CIWSDoc::SetOffsetEntity(CJobWeld* jobWled)
{
	double dx, dy;
	//double dxo, dyo;
	CEntity *pEnt = jobWled->GetEntity();

	if (_isnan(pEnt->m_cen.x))
		pEnt->m_cen.x = 0.0;
	if (_isnan(pEnt->m_cen.y))
		pEnt->m_cen.y = 0.0;

	// 2023.01.19
	dx = jobWled->m_dX;
	dy = jobWled->m_dY;

	pEnt->SetOffset(dx, dy);
	pEnt->ChangedEntity(pEnt->m_hCadEntity);
	TRACE("[sjyi TEST] Set Offset (%.3f, %.3f)\n", dx, dy);
	TRACE("[sjyi TEST] Entity(%.3f, %.3f), Entity Offset(%.3f, %.3f)\n", pEnt->m_cen.x, pEnt->m_cen.y, pEnt->m_offset.x, pEnt->m_offset.y);


	pEnt->SetOffset(0, 0);
	pEnt->ChangedEntity(pEnt->m_hCadEntity);
	TRACE("[sjyi TEST] Set Offset (%.3f, %.3f)\n", dx, dy);
	TRACE("[sjyi TEST] Entity(%.3f, %.3f), Entity Offset(%.3f, %.3f)\n", pEnt->m_cen.x, pEnt->m_cen.y, pEnt->m_offset.x, pEnt->m_offset.y);

	/*pEnt->m_cen.x = jobWled->m_dX;
	pEnt->m_cen.y = jobWled->m_dY;

	pEnt->SetOffset(0, 0);
	pEnt->ChangedEntity(pEnt->m_hCadEntity);*/
}

UINT CIWSDoc::InterfaceThread(LPVOID lpParam)
{
	CIWSDoc *pMe = (CIWSDoc*)lpParam;
	pMe->InterfaceThreadBody();
	return 0;
}

UINT CIWSDoc::InterfaceThreadBody()
{
#define THREAD_CYCLE_TIME 100
#define TIME_1000MS 1000
#define TIME_500MS   500
#define TIME_200MS   200
#define RESPONSE_TIMEOUT 3000

	m_bRunInterfaceThread = TRUE;
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, L"InterfaceEvent");
	DWORD dwRes;
	DWORD dwTime_1000ms = timeGetTime();
	DWORD dwTime_500ms = dwTime_1000ms;
	DWORD dwTime_200ms = dwTime_1000ms;
	DWORD dwTimeLink = dwTime_1000ms;
	DWORD dwCurr = 0;
	int ierr = 0;
	int iBefErr = 0;
	int iSeq = 0;
	BOOL bLinkFlag=FALSE;
	//BOOL bScannerStatus;
	//BOOL bScannerLaserOn;
	//BOOL bScannerLaserReady;
	BOOL bScannerLaserEmmision;
	//BOOL bScannerLaserActive;

	int iCntTimeOutInspectComplet=0;
	int iCntTimeOutWeldComplet=0;

	BOOL bInitWorkFlag = FALSE;
	BOOL bTimeWorkFlag = FALSE;

	BOOL bPLCAlarmOld = FALSE;

	while (m_bRunInterfaceThread)
	{
		dwRes = WaitForSingleObject(hEvent, THREAD_CYCLE_TIME);
		theApp.m_bWorkOn = m_work.IsWorkOn();
		dwCurr = timeGetTime();
		switch (dwRes)
		{
		case WAIT_ABANDONED:
		case WAIT_FAILED:
			break;
		case WAIT_TIMEOUT:
			m_plc.UpdateInput();
			m_status.plcRead = m_plc.GetReadData(PLC_WORD_STATUS);
			m_status.plcCmd = m_plc.GetWriteData(PLC_WORD_COMMAND);

			if (dwCurr - dwTimeLink < TIME_1000MS)
				m_status.bOk[IWS_PLC] = TRUE;
			else 
				m_status.bOk[IWS_PLC] = FALSE;

			if (m_plc.GetLink() != bLinkFlag) 
			{
				bLinkFlag = m_plc.GetLink();
				dwTimeLink = dwCurr;
			}

			// status check
			if (dwCurr - dwTime_1000ms > TIME_1000MS) 
			{
				m_plc.SetToggleLink();
				m_status.bOk[IWS_SCANNER] = m_pScanner->HeadStatusCheck();
				dwTime_1000ms = dwCurr;
			}

			m_status.scanDI = m_pScanner->ReadInput();
			m_status.scanDO = m_pScanner->GetDO_Value();
			//bScannerStatus = m_pScanner->LaserIOStatus();
			//bScannerLaserOn = m_pScanner->GetDILaserOn();
			//bScannerLaserReady = TRUE;//m_pScanner->INT_DI_Ready();
			//bScannerLaserActive = m_pScanner->GetDILaserAssigned();
			bScannerLaserEmmision = m_pScanner->GetDIEmission();
			if (dwCurr - dwTime_200ms > TIME_200MS) 
			{
				m_plc.SetToggleLink();
				//m_status.bOk[IWS_SCANNER] = m_pScanner->HeadStatusCheck();
				dwTime_200ms = dwCurr;
			}

			if ((m_status.plcCmd&PLC_BIT_WELDDONE) == PLC_BIT_WELDDONE)
				iCntTimeOutWeldComplet++;
			else
				iCntTimeOutWeldComplet = 0;

			//if ((m_status.plcCmd&PLC_BIT_INSPECTDONE) == PLC_BIT_INSPECTDONE)
			//	iCntTimeOutInspectComplet++;
			//else
			//	iCntTimeOutInspectComplet = 0;

			if (iCntTimeOutWeldComplet*THREAD_CYCLE_TIME > RESPONSE_TIMEOUT) 
			{
				//m_plc.SetWeldComplete(FALSE);
				iCntTimeOutWeldComplet = 0;
				_tracer(TRACE_WARNING, 1, L"TimeOutWeldComplet");
			}

			if (iCntTimeOutInspectComplet*THREAD_CYCLE_TIME > RESPONSE_TIMEOUT) 
			{
				//m_plc.SetInspectComplete(FALSE);
				m_plc.SetAlarm(FALSE);
				//m_plc.SetInspectOK(FALSE);
				iCntTimeOutInspectComplet = 0;
				_tracer(TRACE_WARNING, 1, L"TimeOutInspectComplet");
			}

#if(LASER_SOURCE == _TRUMF_LASER)
			m_status.bOk[IWS_LASER] = !(m_pScanner->GetDIError()) && m_pScanner->GetDILaserAssigned();
			//m_status.bOk[IWS_LASER] = m_pScanner->GetDILaserAssigned(); // FaultLaser도 같이 봐야 함
			//m_status.bOk[IWS_LASER] = !(m_pScanner->INT_DI_Alarm());
#elif(LASER_SOURCE == _IPG_LASER)
			m_plc.SetLaserState(check_bit(m_status.scanDI, 4), // ready
				check_bit(m_status.scanDI, 5), // power
				check_bit(m_status.scanDI, 6), // active
				check_bit(m_status.scanDI, 7)); // Emission
			m_status.bOk[IWS_LASER] = check_bit(m_status.scanDI, 6) 
				&& check_bit(m_status.scanDI, 4);
#elif(LASER_SOURCE == _IPG_SINGLE)
			m_status.bOk[IWS_LASER] = (m_pScanner->INT_DI_Ready());
#elif ( LASER_SOURCE==_COHERENT_ROFIN)
			m_status.bOk[IWS_LASER] = !(m_pScanner->GetDIError()) && m_pScanner->GetDILaserOn();
#else
			m_status.bOk[IWS_LASER] = (m_pScanner->DI_SystemFault());
#endif // LASER_SOURCE == _TRUMF_LASER

			m_status.bOk[IWS_VSCAN] = g_pGrabImgRunner->GetCameraState();

			ierr = 0;
			m_plc.SetAlarmCode(0);
			for (int i = IWS_LASER; i < IWS_SCANNER/*IWS_RECIPE*/; i++) 
			{
				if (!m_status.bOk[i]) 
				{
					ierr |= 1 << i;
					m_plc.SetAlarmCode(i);
				}
			}

			/*if (m_config.m_bUsePLC)
			{
				if (m_plc.GetRecipe() >= m_config.m_arWorkList.GetSize()) 
				{
					m_plc.SetAlarmCode(IWS_RECIPE);
					ierr |= PLC_ALARM_RECIPE;
					if (ierr != iBefErr)
					{
						_tracer(TRACE_ERROR, 1, L"Recipe number miss match");
						iBefErr = ierr;
					}
				}
			}*/

			//// PLC Alarm Check(for Log)
			//if (bPLCAlarmOld != m_plc.GetAlarm())
			//{
			//	bPLCAlarmOld = m_plc.GetAlarm();
			//	if (bPLCAlarmOld) {
			//		_tracer(TRACE_ERROR, 1, _T("PLC->PC : !!! PLC Fault Signal On !!!, Set Laser Fault On !!!"));
			//	}
			//	else {
			//		_tracer(TRACE_ERROR, 1, _T("PLC->PC : !!! PLC Fault Signal Off !!!, Please Restart Program !!!"));
			//	}
			//}

			//// PLC Alarm Check(for Laser Fault)
			//if (m_plc.GetAlarm()) {
			//	m_pScanner->SetLaserFault(TRUE);
			//	goto INTERFACE_CONTINUE;
			//}

			// sjyi 2024.08.09 Laser 안전을 위한 코드 추가(Ready 신호 Off일 때, Laser Fault ON)
			// PLC Alarm Check(for Log)
			if (bPLCAlarmOld != m_plc.GetReadyWelding())
			{
				bPLCAlarmOld = m_plc.GetReadyWelding();
				if (bPLCAlarmOld) {
					_tracer(TRACE_ERROR, 1, _T("PLC->PC : !!! Ready Welding Signal On !!!, Please Restart Program !!!"));
				}
				else {
					_tracer(TRACE_ERROR, 1, _T("PLC->PC : !!! Ready Welding Signal Off !!!, Set Laser Fault On !!!"));
				}
			}

			// PLC Alarm Check(for Laser Fault)
			if (!m_plc.GetReadyWelding()) {
				m_pScanner->SetLaserFault(TRUE);
				goto INTERFACE_CONTINUE;
			}

			// status check end
			//if (!theApp.m_bAuto) goto INTERFACE_CONTINUE;
			if (theApp.m_bWorkOn) 
			{
				m_plc.SetWeldReady(FALSE);
				goto INTERFACE_CONTINUE;
			}

			if (m_plc.GetReqWeldingComplete())
				m_plc.SetWeldComplete(FALSE);

			// RUN COMMAND
			if (ierr)
			{				
				m_plc.SetWeldReady(FALSE);
				
				switch (m_config.m_iLaser)
				{
				case TRUMPF_TRUDISK:
					if (m_pScanner->GetDOEmission()) m_pScanner->LaserEnable(FALSE);
					break;
				case COHERENT_ROFIN:
					if (m_pScanner->GetLaserOn()) {
						m_pScanner->SetPower(0);
						m_pScanner->LaserOn(FALSE);
					}
					break;
				}

				goto INTERFACE_CONTINUE;
			}

			//if (GetUseInterface())
			if (GetUseInterface() & bScannerLaserEmmision)
			{
				// Online 상태일 때 
				m_plc.SetAuto(TRUE);
				m_plc.SetManual(FALSE);

				m_plc.SetWeldReady(TRUE);
			}
			else
			{
				// Offline 상태일 때 
				m_plc.SetAuto(FALSE);
				m_plc.SetManual(TRUE);

				m_plc.SetWeldReady(FALSE);
				goto INTERFACE_CONTINUE;
			}

			// 초기화 요청
			if (m_plc.GetReqInit() && !bInitWorkFlag)
			{
				_tracer(TRACE_NORMAL, 1, L"Initialize Request");
				bInitWorkFlag = TRUE;
				theApp.m_bWorkOn = FALSE;

				m_plc.SetAlarm(0);
				m_plc.SetWeldReq(FALSE);
				m_plc.SetWeldComplete(FALSE);

				m_plc.SetInspectReq(FALSE);

				m_plc.SetInit(TRUE);
			}
			else if (bInitWorkFlag)
			{
				_tracer(TRACE_NORMAL, 1, L"Initialize Response");
				bInitWorkFlag = FALSE;
				m_plc.SetInit(FALSE);
			}

			// 시간동기화 요청
			if (m_plc.GetReqTimeSync() && !bTimeWorkFlag)
			{
				_tracer(TRACE_NORMAL, 1, L"Time Sync Request");
				bTimeWorkFlag = TRUE;
				m_plc.UpdateDateTime();
				m_plc.SetTimeSync(TRUE);
			}
			else if (bTimeWorkFlag)
			{
				_tracer(TRACE_NORMAL, 1, L"Time Sync Response");
				bTimeWorkFlag = FALSE;
				m_plc.SetTimeSync(FALSE);
			}

			// Laser On = DI_Ready(0), DI_LaserAssigned(1) 
			// Guide On = DI_Ready(1), DI_LaserAssigned(1) 

			//bLaserOn = bScannerLaserEmmision; //m_plc.GetLaserOn();
			//if (bLaserOn) 
			//	m_plc.SetWeldReady(!m_pScanner->GetDILaserOn() && m_pScanner->GetDIEmission());
			//else 
			//	m_plc.SetWeldReady(m_pScanner->GetDILaserOn() && m_pScanner->GetDIEmission());
			m_plc.SetWeldReady(m_pScanner->GetDIEmission());

			if(bInitWorkFlag || bTimeWorkFlag)
				goto INTERFACE_CONTINUE;

			// TODO: Online, Offline 삭제하면서 주석처리 		
			/*if (m_plc.GetLaserOn() != m_pScanner->INT_BE_EmissionEnable()) {
				m_pScanner->LaserEnable(m_plc.GetLaserOn());
				m_pScanner->INT_DO_GuideOn(!m_plc.GetLaserOn());
			}
			if(!bLaserOn && !m_pScanner->INT_BE_GuideOn())
				m_pScanner->INT_DO_GuideOn(TRUE);*/
//			if (m_plc.GetLaserOn() != m_pScanner->INT_BE_EmissionEnable()) {
//				m_pScanner->LaserEnable(m_plc.GetLaserOn());
#if(LASER_SOURCE != _TRUMF_LASER) && (LASER_SOURCE != _COHERENT_ROFIN)
				m_pScanner->INT_DO_GuideOn(!m_plc.GetLaserOn());
#endif
//			}
#if(LASER_SOURCE != _TRUMF_LASER) && (LASER_SOURCE != _COHERENT_ROFIN)
			if (!bLaserOn && !m_pScanner->INT_BE_GuideOn())
				m_pScanner->INT_DO_GuideOn(TRUE);
#endif

			// 좌표 읽기 요청
			if (m_plc.GetReqInspect() && !m_plc.GetResInspect()) 
			{
				if (!m_pMainFrm) 
					goto INTERFACE_CONTINUE;

				ResetEvent(m_hEvtOpenRecipe);
				int iRcp = m_plc.GetRecipe();
				::PostMessage(m_pMainFrm->GetSafeHwnd(), UWM_INTERFACE_THREAD, INTERFACE_INSPECT_START, iRcp);

				DWORD dwResult = WaitForSingleObject(m_hEvtOpenRecipe, 5000);
				if (dwResult == WAIT_TIMEOUT) 
				{
					m_plc.SetAlarmCode(IWS_RECIPE);
					_tracer(TRACE_ERROR, 1, L"Time out error : recipe open");
				}
				else
				{
					goto INTERFACE_CONTINUE;
				}
			}
			else if(!m_plc.GetReqInspect() && m_plc.GetResInspect())
			{
				m_plc.SetInspectReq(FALSE);
			}

			//if (!m_plc.GetReadyWelding()) goto INTERFACE_CONTINUE;
			// 용접 시작 요청
			if (m_plc.GetReqWelding() && !m_plc.GetResWelding())
			{
				if (!m_pMainFrm)
					goto INTERFACE_CONTINUE;

				int iRcp = m_plc.GetRecipe();
				ResetEvent(m_hEvtOpenRecipe);
				::PostMessage(m_pMainFrm->GetSafeHwnd(), UWM_INTERFACE_THREAD, INTERFACE_WELD_START, iRcp);

				DWORD dwResult = WaitForSingleObject(m_hEvtOpenRecipe, 3000);
				if (dwResult == WAIT_TIMEOUT) 
				{
					m_plc.SetAlarmCode(IWS_RECIPE);
					_tracer(TRACE_ERROR, 1, L"Time out error : recipe open");
				}
			}
			else if (!m_plc.GetReqWelding() && m_plc.GetResWelding())
			{
				m_plc.SetWeldReq(FALSE);
			}

INTERFACE_CONTINUE:
			m_plc.UpdateOutput();
			m_status.plcWrite = m_plc.GetWriteData(PLC_WORD_STATUS);
			break;
		default:
			break;
		}
	}
	CloseHandle(hEvent);
	return 0l;
}

//BOOL CIWSDoc::InspectStator()
//{
//	CLaserPen *pOldPen = m_pScanner->GetLaserPen();
//	m_pScanner->SetLaserPen(&m_pens[14]);
//	m_pScanner->SetLaserPenParam();
//	m_pScanner->WaitMoving(TRUE);
//
//	g_pGrabImgRunner->SetFlagInspectStator(FALSE);
//	g_pGrabImgRunner->InspectionStart();
//
//	/* ---------------------------- */
//	BOOL bPeekAndPump = TRUE;
//	if (bPeekAndPump) {
//		while (!g_pGrabImgRunner->IsDoneInspectStator()) {
//			PeekAndPump();
//		}
//	}
//	else {
//		while (!g_pGrabImgRunner->IsDoneInspectStator()) {
//			Sleep(1);
//		}
//	}
//	m_pScanner->WaitMoving(TRUE);
//	m_pScanner->SetLaserPen(pOldPen);
//	m_pScanner->SetLaserPenParam();
//	return TRUE;
//}

void CIWSDoc::GetRefImgSize(double *wid, double *height)
{
	float fovX, fovY;
	g_pGrabImgRunner->GetCurFullFov(&fovX, &fovY);
	*wid = fovX;
	*height = fovY;

	double scaleX, scaleY;
	m_config.GetRefImgScale(&scaleX, &scaleY);

	*wid *= scaleX;
	*height *= scaleY;
}

void CIWSDoc::GetInspectResults(POINTF *pRes, int *nData)
{
	//POINTF imgCenter;
	//g_pGrabImgRunner->ParseInspectionRes(pRes, &imgCenter);
}

void CIWSDoc::ReqStitchImage(BOOL bScan, int* pWidthPxl, int* pHeightPxl,
	float* pWidthLength, float* pHeightLength)
{
	g_pGrabImgRunner->SetFlagStitchImageDone(FALSE);

	CString strStitchImg = m_strImgPath + STR_REFIMG_FILE;
	g_pGrabImgRunner->ScanAndSave(strStitchImg, bScan, pWidthPxl, pHeightPxl,
		pWidthLength, pHeightLength);

	// check wait done
	BOOL bPeekAndPump = TRUE;
	if (bPeekAndPump) 
	{
		while (!g_pGrabImgRunner->IsDoneStitchImage())
		{
			PeekAndPump();
		}
	}
	else 
	{
		while (!g_pGrabImgRunner->IsDoneStitchImage())
		{
			Sleep(1);
		}
	}
}

BOOL CIWSDoc::PeekAndPump()
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
	{
		if (GetMessage(&msg, NULL, NULL, NULL)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{  
			return FALSE;
		}
	}
	return TRUE;
}

// sjyi 2022-05-19 Comment 현재 사용 안함.
void CIWSDoc::SetWorkOffset()
{	
	int nSlot = m_work.m_hp.nSlot;
	int nLayer = m_work.m_hp.nLayer;
	IWS_VECTOR_JOB_GROUP *pJobGroup = m_work.GetJobs();
	int nJob = m_work.GetNumberOfJobs();

	POINTF *points = new POINTF[nJob];
	int idx = 0;
	for (auto group : (*pJobGroup))
	{
		for (auto p : group->m_jobs)
		{
			if (p->GetJobType() != JobType::JOB_TYPE_WELD) continue;
			points[idx].x = (FLOAT)p->GetEntity()->m_cen.x;
			points[idx].y = (FLOAT)p->GetEntity()->m_cen.y;
			idx++;
		}
	}

	ASSERT(idx == nJob);
	int nRes = nSlot * nLayer;
	POINTF *pInspRes = new POINTF[nRes];
	idx = 1;
	pInspRes[0] = points[0];
	for (int i = 1; i < nJob;i++) 
	{
		if (!((i+1) % MAX_NUM_GRP)) continue;
		pInspRes[idx] = points[i];
		idx++;
	}

	GetInspectResults(pInspRes, &nRes);

	idx = 1;
	points[0] = pInspRes[0];
	for (int i = 1; i < nRes; i++) 
	{
		points[idx] = pInspRes[i];
		idx++;

		if (!((i+1) % 2)) 
		{
			points[idx].x = 0;
			points[idx].y = 0;
			idx++;
		}
	}
	delete[] pInspRes;


	float tmpx, tmpy;
	float radius;
	float theta;
	float r1, r2, cx, cy;

	// check offset
	double dx, dy;		//
	double dxo, dyo;
	idx = 0;
	double MaxOffset = 50;
	double dOffset;
	for (auto group : (*pJobGroup))
	{
		for (auto p : group->m_jobs)
		{
			if (p->GetJobType() != JobType::JOB_TYPE_WELD) 
				continue;

			dxo = p->GetEntity()->m_cen.x;
			dyo = p->GetEntity()->m_cen.y;
			dx = points[idx].x - dxo;
			dy = points[idx].y - dyo;
			TRACE("Offset : %f, %f\n", dx, dy);
			if (!((idx + 1) % MAX_NUM_GRP)) {
				CEntity *Ent = p->GetEntity();
				if (Ent->GetEntityType() == LC_ENT_ELLIPSE) {
					CEntyEllipse *e = (CEntyEllipse*)Ent;
					tmpx = float((points[idx - 2].x + points[idx - 1].x) * 0.5);
					tmpy = float((points[idx - 2].y + points[idx - 1].y) * 0.5);
					radius = sqrtf(tmpx*tmpx + tmpy * tmpy);
					cx = tmpx;// radius * cos(theta);
					cy = tmpy;// radius * sin(theta);

					tmpx = (points[idx - 1].x - points[idx - 2].x);
					tmpy = (points[idx - 1].y - points[idx - 2].y);
					theta = atan2f(tmpy, tmpx);
					r1 = (float)sqrtf(tmpx*tmpx + tmpy * tmpy);
					r2 = (float)m_work.m_hp.HpSize;
					//cx = tmpx;// radius * cos(theta);
					//cy = tmpy;// radius * sin(theta);
					e->Rmajor = r1 * 0.5;
					e->Rminor = r2;
					e->RotAngle = theta * LC_RAD_TO_DEG;
					e->center.x = cx;
					e->center.y = cy;
					e->m_cen.x = cx;
					e->m_cen.y = cy;
					e->m_offset.x = 0;
					e->m_offset.y = 0;
					e->UpdateEntity(NULL, COleVariant::COleVariant(0.0));
				}
			}
			else 
			{
				dOffset = sqrt((dx*dx + dy * dy));
				if (dOffset < MaxOffset) 
				{
					p->GetEntity()->SetOffset(dx, dy);
				}
				else
				{
					points[idx].x = (FLOAT)dxo;
					points[idx].y = (FLOAT)dyo;
				}
			}

			idx++;
		}
	}

	delete[] points;
}

void CIWSDoc::ClearWorkOffset()
{
	// recalc ellipse
	float tmpx, tmpy;
	float radius;
	float theta;
	float r1, r2, cx, cy;
	int idx = 0;
	IWS_VECTOR_JOB_GROUP *pJobGroup = m_work.GetJobs();
	int nJob = m_work.GetNumberOfJobs();

	POINTF *points = new POINTF[nJob];

	for (auto group : (*pJobGroup))
	{
		for (auto p : group->m_jobs) 
		{
			if (p->GetJobType() != JobType::JOB_TYPE_WELD) continue;
			points[idx].x = (FLOAT)(p->GetEntity()->m_cen.x);
			points[idx].y = (FLOAT)(p->GetEntity()->m_cen.y);
			idx++;
		}
	}

	idx = 0;
	for (auto group : (*pJobGroup))
	{
		for (auto p : group->m_jobs)
		{
			if (p->GetJobType() != JobType::JOB_TYPE_WELD) continue;
			p->GetEntity()->SetOffset(0, 0);
			idx++;
		}
	}

	idx = 0;
	for (auto group : (*pJobGroup))
	{
		for (auto p : group->m_jobs) 
		{
			if (p->GetJobType() != JobType::JOB_TYPE_WELD) continue;
			if (!((idx + 1) % MAX_NUM_GRP)) 
			{
				CEntity *Ent = p->GetEntity();
				if (Ent->GetEntityType() == LC_ENT_ELLIPSE) 
				{
					CEntyEllipse *e = (CEntyEllipse*)Ent;
					tmpx = (float)((points[idx - 2].x + points[idx - 1].x) * 0.5);
					tmpy = (float)((points[idx - 2].y + points[idx - 1].y) * 0.5);
					radius = sqrtf(tmpx*tmpx + tmpy * tmpy);
					cx = tmpx;// radius * cos(theta);
					cy = tmpy;// radius * sin(theta);

					tmpx = (points[idx - 1].x - points[idx - 2].x);
					tmpy = (points[idx - 1].y - points[idx - 2].y);
					theta = atan2f(tmpy, tmpx);
					r1 = (float)sqrtf(tmpx*tmpx + tmpy * tmpy);
					r2 = (float)m_work.m_hp.HpSize;
					//cx = tmpx;// radius * cos(theta);
					//cy = tmpy;// radius * sin(theta);
					e->Rmajor = r1 * 0.5;
					e->Rminor = r2;
					e->RotAngle = theta * LC_RAD_TO_DEG;
					e->center.x = cx;
					e->center.y = cy;
					e->m_cen.x = cx;
					e->m_cen.y = cy;
					e->m_offset.x = 0;
					e->m_offset.y = 0;
					e->UpdateEntity(NULL, COleVariant::COleVariant(0.0));
				}
			}
			idx++;
		}
	}

	delete[] points;
}

void CIWSDoc::ChangeScanRange()
{
	//DHChoi 20200626
	int iCurLensIdx = m_config.m_iCurrLens;
	BOOL bUseCalib = m_config.m_arCorrParam[iCurLensIdx].bUseCalib;
	int  iOverlay = m_config.m_arCorrParam[iCurLensIdx].iVSOverlap;
	g_pGrabImgRunner->m_pScanImgsProc->UseCalibration(FALSE);
	if (bUseCalib)
	{
		CStringA strFilename = CStringA(m_config.m_arCorrParam[iCurLensIdx].szCalibDir);
		if (strFilename.GetLength() > 1)
		{
			g_pGrabImgRunner->m_pScanImgsProc->UseCalibration(TRUE);
			CString sCalibPath = CString(strFilename);
			int nBlockX = m_config.m_arCorrParam[iCurLensIdx].iCalibX;
			int nBlockY = m_config.m_arCorrParam[iCurLensIdx].iCalibY;
			if (g_pGrabImgRunner->m_pScanImgsProc->SetCalibData(sCalibPath, nBlockX, nBlockY) < 0)
				g_pGrabImgRunner->m_pScanImgsProc->UseCalibration(FALSE);
		}
	}

	// 20.03.04 hslee
	double sx, sy, ex, ey;
	m_config.GetScanRng(&(sx), &(sy), &(ex), &(ey));

	// m_config.SetScanRng(sx, sy, ex, ey);	yjko
	// m_config.WriteConfig();				yjko

	/* update v-scan parameter: yjko */ 
	float resolX, resolY;
	GetCcdResolution(&resolX, &resolY);
	g_pGrabImgRunner->ChangeScanRange(resolX, resolX, sx, sy, ex, ey);
}

POINTF CIWSDoc::GetScanCenter()
{
	return g_pGrabImgRunner->GetScanCenter();
}

void CIWSDoc::OnFileCurrToWork()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString fileName;

	CString strFilter;
	
	// sjyi 2025.09.29 Jason, wrk 모두 대응 가능하도록 수정 -->
	//	strFilter = _T("Work File(*.wrk)|*.wrk||");
	strFilter = _T("Work Files (*.wrk)|*.wrk|JSON Files (*.json)|*.json|All Files (*.*)|*.*||");
	// sjyi 2025.09.29 Jason, wrk 모두 대응 가능하도록 수정 <--

	CString strTitle = this->GetTitle();
	int idot = strTitle.ReverseFind('.');
	CString strDefaultFilename = strTitle.Mid(0, idot);
	CFileDialog dlgFile
	(
		FALSE,														//BOOL bOpenFileDialog,
		_T("*.wrk"),													//LPCTSTR lpszDefExt = NULL,
		strDefaultFilename,											//LPCTSTR lpszFileName = NULL,												
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_LONGNAMES,		//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter													//LPCTSTR lpszFilter = NULL,
																	//CWnd* pParentWnd = NULL,
																	//DWORD dwSize = 0,
																	//BOOL bVistaStyle = TRUE
	);

	if (dlgFile.DoModal() != IDOK) 
		return;

	fileName = dlgFile.GetPathName();
	SaveFile(fileName, TRUE);
}


void CIWSDoc::OnFileHpConfig()
{
	if (theApp.m_iwsMode != CIWSApp::IWSMODE_HAIRPIN) 
	{
		AfxMessageBox(L"Only in hairpin mode.");
		return;
	}

 	CDlgHairpinSet dlg;
	dlg.m_nSwitchWelding = m_work.m_hp.nSwitchWelding;
	dlg.m_nWeldType = m_work.m_hp.nWeldType;

	dlg.m_bZCompUse = m_work.m_zComp.bUse;
	dlg.m_dblZThreshold = m_work.m_zComp.dblThreshold;
	dlg.m_nZCtn = m_work.m_zComp.nCnt;
	dlg.m_dblZPwr = m_work.m_zComp.dblPower;// mspark 2022.04.20

	for (int i = 0; i < 10; i++)
	{
		dlg.m_zComp[i].dblMin = m_work.m_zComp.zHpArr[i].dblMin;
		dlg.m_zComp[i].dblMax = m_work.m_zComp.zHpArr[i].dblMax;
		dlg.m_zComp[i].nCnt = m_work.m_zComp.zHpArr[i].nCnt;
	}

	for (int i = 0; i < 6; i++)
	{
		dlg.m_dRcpScale[i] = m_config.m_dRcpScale[i];
		dlg.m_dRcpAngle[i] = m_config.m_dRcpAngle[i];
		dlg.m_dRcpOffsetX[i] = m_config.m_dRcpOffsetX[i];
		dlg.m_dRcpOffsetY[i] = m_config.m_dRcpOffsetY[i];
	}

	dlg.m_dPowerRatio = m_work.m_hp.dblPowerRatio;
	dlg.m_dSpeedRatio = m_work.m_hp.dblSpeedRatio;

	dlg.m_dLegendAng = m_config.m_dHpLegendStartAng;
	dlg.m_dLegendDia = m_config.m_dHpLegendDia;
	dlg.m_dLegendDir = m_config.m_dHpLegendDir;

	/*dlg.m_dDegree12 = m_work.m_hp.dDegree12;
	dlg.m_dDegree34 = m_work.m_hp.dDegree34;
	dlg.m_dDegree56 = m_work.m_hp.dDegree56;
	dlg.m_dDegree78 = m_work.m_hp.dDegree78;*/

	dlg.m_bSaveImage = m_config.m_bSaveImage;
	dlg.m_nSaveDelay = m_config.m_nSaveDelay;

	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	lcBlockUnselect(hBlock);
	lcBlockUpdate(hBlock, false, 0);
	lcWndRedraw(g_hLcWnd);

 	if (dlg.DoModal() != IDOK) 
		return;

	m_work.m_hp.nSwitchWelding = dlg.m_nSwitchWelding;
	m_work.m_hp.nWeldType = dlg.m_nWeldType;

	m_work.m_zComp.bUse = dlg.m_bZCompUse;
	m_work.m_zComp.dblThreshold = dlg.m_dblZThreshold;
	m_work.m_zComp.nCnt = dlg.m_nZCtn;
	m_work.m_zComp.dblPower = dlg.m_dblZPwr;// mspark 2022.04.20

	for (int i = 0; i < 10; i++)
	{
		m_work.m_zComp.zHpArr[i].dblMin = dlg.m_zComp[i].dblMin;
		m_work.m_zComp.zHpArr[i].dblMax = dlg.m_zComp[i].dblMax;
		m_work.m_zComp.zHpArr[i].nCnt = dlg.m_zComp[i].nCnt;
	}

	for (int i = 0; i < 6; i++)
	{
		m_config.m_dRcpScale[i] = dlg.m_dRcpScale[i];
		m_config.m_dRcpAngle[i] = dlg.m_dRcpAngle[i];
		m_config.m_dRcpOffsetX[i] = dlg.m_dRcpOffsetX[i];
		m_config.m_dRcpOffsetY[i] = dlg.m_dRcpOffsetY[i];
	}

	m_work.m_hp.dblPowerRatio = dlg.m_dPowerRatio;
	m_work.m_hp.dblSpeedRatio = dlg.m_dSpeedRatio;

	m_config.m_dHpLegendStartAng = dlg.m_dLegendAng;
	m_config.m_dHpLegendDia = dlg.m_dLegendDia;
	m_config.m_dHpLegendDir = dlg.m_dLegendDir;

	/*m_work.m_hp.dDegree12 = dlg.m_dDegree12;
	m_work.m_hp.dDegree34 = dlg.m_dDegree34;
	m_work.m_hp.dDegree56 = dlg.m_dDegree56;
	m_work.m_hp.dDegree78 = dlg.m_dDegree78;*/

	m_config.m_bSaveImage = dlg.m_bSaveImage;
	m_config.m_nSaveDelay = dlg.m_nSaveDelay;

	m_config.WriteConfig();

 	m_pMainFrm->m_wndWorkView.UpdateWork();
}

void CIWSDoc::OnFileImportDxf()
{
	ImportDxf();
}

void CIWSDoc::ParseDxf()
{
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = lcBlockGetFirstEnt(hBlock);
	HANDLE hLayer0 = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER, STR_LAYER_0);
	HANDLE hLayerWorkAreaBk = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER, STR_LAYER_WORKAREA_BK);
	HANDLE hLayerImg = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER, STR_LAYER_IMAGE_BK);

	int EntType;
	while (hEnt) 
	{
		EntType = lcPropGetInt(hEnt, LC_PROP_ENT_TYPE);
		CString str = lcPropGetStr(hEnt, LC_PROP_ENT_LAYER);
		if (str != "0") 
		{
			lcPropPutHandle(hEnt, LC_PROP_ENT_LAYER, hLayer0);
		}

		TRACE("ParseDxf : %d\n", EntType);
		switch (EntType)
		{
		case LC_ENT_LINE://		101  // Line
		case LC_ENT_POLYLINE://		102  // Polyline
		case LC_ENT_CIRCLE://		103  // Circle
		case LC_ENT_ARC://         104  // Arc
		case LC_ENT_POINT://       107  // Point
		case LC_ENT_ELLIPSE://     109  // Ellipse or elliptical arc
		case LC_ENT_RECT://        118  // Rectangle
		case LC_ENT_HATCH://       124  // Hatch
		case LC_ENT_SPIRAL:
		case LC_ENT_IMAGEREF:
			lcPropPutBool(hEnt, LC_PROP_ENT_FILLED, FALSE);
			DrawAddEntity(hEnt);
			break;
		}
		hEnt = lcBlockGetNextEnt(hBlock, hEnt);
	}

	std::vector<HANDLE> hDel;
	HANDLE hLayer = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_LAYER);
	while (hLayer)
	{
		CString str = lcPropGetStr(hLayer, LC_PROP_LAYER_NAME);
		if (str == STR_LAYER_0 || str == STR_LAYER_WORKAREA_BK || str == STR_LAYER_IMAGE_BK) 
		{
			hLayer = lcDrwGetNextObject(g_hLcDrw, hLayer);

			continue;
		}
		hDel.push_back(hLayer);
		hLayer = lcDrwGetNextObject(g_hLcDrw, hLayer);
	}

	for (auto p : hDel) 
	{
		lcDrwDeleteObject(g_hLcDrw, p);
	}
	lcBlockPurge(hBlock);

	if (!hLayerWorkAreaBk) 
	{
		m_pDrawView->SetWorkArea();
		m_pDrawView->SortLayerOrder();
	}

}
// Include Lens Power
void CIWSDoc::GetCcdResolution(float* resolX, float* resolY)	// YJKO
{
	int iLens = m_config.m_iCurrLens;
	CorrParam *p = &(m_config.m_arCorrParam[iLens]);

	float resol_x = m_config.m_fCcdResol_X;
	float resol_y = m_config.m_fCcdResol_Y;

	// power corrected resolution
	*resolX = resol_x / (float)p->dblLensPowerX;
	*resolY = resol_y / (float)p->dblLensPowerY;
}

// implement for just convienence
void CIWSDoc::GetWorkRange(float* fMaxScanRangeX, float* fMaxScanRangeY)	// YJKO
{
	int iLens = m_config.m_iCurrLens;
	CorrParam *p = &(m_config.m_arCorrParam[iLens]);
	*fMaxScanRangeX = (float)p->dblWorkX;	// total scan range, X
	*fMaxScanRangeY = (float)p->dblWorkY;	//  ""               Y
}

// implement for just convienence
void CIWSDoc::GetCcdInfo(UINT* nGrabWd, UINT* nGrabHt, UINT* nGrabCh, CString* pStrSNCCD, int* pFR2Want) //yjko
{
	*nGrabHt = m_config.m_nCcdHeight;
	*nGrabWd = m_config.m_nCcdWidth;
	*nGrabCh = FRAME_IMG_CH;

	if (pStrSNCCD != NULL)
		for(int j = 0; j < NUM_MAX_CAMERA; j++)
			pStrSNCCD[j] = m_config.m_strSNCCD[j];

	if (pFR2Want != NULL)
		(*pFR2Want) = m_config.m_nFR2Want;
}

void CIWSDoc::GetCcd2ImgInfo(UINT* nWidth, UINT* nHeight)
{
	*nWidth = m_config.m_nCcd2ImgWidth;// / DOWN_SAMPLE_FACTOR;
	*nHeight = m_config.m_nCcd2ImgHeight;// / DOWN_SAMPLE_FACTOR;
}

#include "CDlgInspecResluts.h"
void CIWSDoc::OnFileShowInspect()
{
	CDlgInspecResluts dlg;
	dlg.m_pPLC = m_plcData;
	dlg.m_nLayer = m_work.m_hp.nLayer;
	dlg.m_nSlot = m_work.m_hp.nSlot;
	dlg.m_pDoc = this;
	dlg.DoModal();
}

void CIWSDoc::OnFileLocationInfoSave()
{
	if (!m_config.m_bUseHairpin)
		return;

	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof path);

	CString strStartUpPath = path;
	int i = strStartUpPath.ReverseFind('\\'); //실행 파일 이름을 지우기 위해서 왼쪽에 있는 '/'를 찾는다.
	strStartUpPath = strStartUpPath.Left(i); //뒤에 있는 현재 실행 파일 이름을 지운다.
	strStartUpPath.Format(_T("%s\\LocationInfo.csv"), strStartUpPath);

	CFile file;
	if (!file.Open(strStartUpPath, CFile::modeCreate | CFile::modeReadWrite))
	{
		_tracer(TRACE_ERROR, 1, _T("File Save Fail IWeldResult"));
		return;
	}

	CString strHeader;
	strHeader.Format(L"Slot, Layer, aX, aY, bX, bY\n");
	file.Write(strHeader, strHeader.GetLength() * sizeof(TCHAR));

	for (auto group : m_work.m_JobGroup)
	{
		if (group->m_jobs.size() < 2)
			continue;

		POINTF points;

		strHeader.Format(L"%d, %d,", group->m_nSlot + 1, group->m_nLayer);
		file.Write(strHeader, strHeader.GetLength() * sizeof(TCHAR));

		CString strData;
		//1차 패턴에 대한 좌표값 확인차
		for (int i = 0; i < 2; i++)
		{
			CEntity *pEnt = group->m_jobs[i]->GetEntity();

			points.x = FLOAT(pEnt->m_cen.x + pEnt->m_offset.x);
			points.y = FLOAT(pEnt->m_cen.y + pEnt->m_offset.y);

			TRACE("Slot = [%d], Layer = [%d] = X : [%f], y : [%F]\n", group->m_nSlot + 1, group->m_nLayer, points.x, points.y);

			CString strTmp;
			strTmp.Format(L"%.4f, %.4f,", points.x, points.y);
			strData += strTmp;
		}

		strData += (L"\n");
		file.Write(strData, strData.GetLength() * sizeof(TCHAR));
	}

	file.Close();
}

void CIWSDoc::SnapSave()
{
	// 프로그램 실행 경로
	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof path);

	CString strDefPath = path;
	int i = strDefPath.ReverseFind('\\');//실행 파일 이름을 지우기 위해서 왼쪽에 있는 '/'를 찾는다.
	strDefPath = strDefPath.Left(i);//뒤에 있는 현재 실행 파일 이름을 지운다.
	strDefPath.Format(L"%s\\SnapImage\\", strDefPath);

	SYSTEMTIME cTime;
	GetLocalTime(&cTime);

	// 년월일 폴더 생성
	CString strTimeDir;
	strTimeDir.Format(L"%4d%02d%02d", cTime.wYear, cTime.wMonth, cTime.wDay);
	strDefPath = strDefPath + strTimeDir;

	CFileFind file;
	BOOL bResult = file.FindFile(strDefPath);
	if (!bResult)
		CreateDirectory(strDefPath, NULL);

	strDefPath.Format(L"%s\\", strDefPath);

	// 시분초 폴더 생성	
	strTimeDir.Format(L"%02d%02d%02d", cTime.wHour, cTime.wMinute, cTime.wSecond);
	strDefPath = strDefPath + strTimeDir;

	bResult = file.FindFile(strDefPath);
	if (!bResult)
		CreateDirectory(strDefPath, NULL);

	strDefPath.Format(L"%s\\", strDefPath);

	// Snap	
	POINTF points;

	for (auto group : m_work.m_JobGroup)
	{
		if (group->m_jobs.size() < 2)
			continue;

		CEntity *pEnt = group->m_jobs[2]->GetEntity();
		points.x = FLOAT(pEnt->m_cen.x + pEnt->m_offset.x);
		points.y = FLOAT(pEnt->m_cen.y + pEnt->m_offset.y);

		m_pScanner->Move((double)points.x, (double)points.y);
		m_pScanner->WaitMoving(FALSE);
		Sleep(100);

		CString strFileName;
		strFileName.Format(L"%s\\%d Slot %d Layer.jpg", group->m_nSlot, group->m_nLayer);
		int iRet = g_pGrabImgRunner->SnapAndInspect(strFileName);
	}
}

// 2022-07-07 Matrix 관련 코드 추가(MOB 참고)--->
void CIWSDoc::OnMatrix()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (!m_config.m_bUseHairpin)
		return;

	CDlgMatrix dlg;
	dlg.m_pDoc = this;
	dlg.DoModal();
}
// 2022-07-07 Matrix 관련 코드 추가(MOB 참고)<---

void CIWSDoc::SaveImageAndPos()
{
	// Check Config
	if (m_config.m_bSaveImage == FALSE) return;

	// Move 2nd Position
	MoveCenterPos();

	Pause(m_config.m_nSaveDelay);

	SYSTEMTIME stTime;
	GetLocalTime(&stTime);

	CString strStartUpPath = m_strImgPath;
	strStartUpPath = strStartUpPath.Left(strStartUpPath.ReverseFind('\\'));

	CString strSaveDir;
	strSaveDir.Format(_T("%s\\%04d\\%02d\\%02d\\"), strStartUpPath, (int)stTime.wYear, (int)stTime.wMonth, (int)stTime.wDay);
	if (_CreateDirectory(strSaveDir) == FALSE)
	{
		_tracer(TRACE_ERROR, 9999, _T("Cannot create directory(%s)"), strSaveDir);
	}
	else
	{
		// Single Grab & Save Image
		if (g_pGrabImgRunner)
		{
			// Barcode
			CString strBarcode = m_plc.GetLOTID();
			if (strBarcode.GetLength() < 10) strBarcode = _T("NO BARCODE");

			// Set File Name
			CString strFileName;
			//imsistrFileName.Format(_T("%s%02d%02d%02d_%s_%s.jpg"), strSaveDir, stTime.wHour, stTime.wMinute, stTime.wSecond,
			//strFileName.Format(_T("%s_%02d%02d%02d.bmp"), strBarcode, stTime.wHour, stTime.wMinute, stTime.wSecond);
			strFileName.Format(_T("%s_%02d%02d%02d.jpg"), strBarcode, stTime.wHour, stTime.wMinute, stTime.wSecond);

			g_pGrabImgRunner->SnapAndSaveImg(strSaveDir, strFileName);
		}
	}
}

BOOL CIWSDoc::_CreateDirectory(CString sDirPath)
{
	int iLng = sDirPath.GetLength();
	int iPos1 = sDirPath.ReverseFind(_T('\\')) + 1;
	int iPos2 = sDirPath.ReverseFind(_T('/'));
	if (iLng != iPos1 && iLng != iPos2)
	{
		sDirPath += _T("\\");
	}

	LPCTSTR lpszPath = (LPCTSTR)sDirPath;

	TCHAR szPathBuffer[MAX_PATH];

	size_t len = _tcslen(lpszPath);

	for (size_t i = 0; i < len; i++)
	{
		szPathBuffer[i] = *(lpszPath + i);
		if (szPathBuffer[i] == _T('\\') || szPathBuffer[i] == _T('/'))
		{
			szPathBuffer[i + 1] = NULL;
			if (!PathFileExists(szPathBuffer))
			{
				if (!::CreateDirectory(szPathBuffer, NULL))
				{
					if (GetLastError() != ERROR_ALREADY_EXISTS)
						return FALSE;
				}
			}
		}
	}
	return TRUE;
}

// sjyi 2024.06.14 추가, 안전 및 수정사항 확인을 위해 추가
void CIWSDoc::BackUpWork(CString strWorkFile)
{
	CString strWorkFileName;
	int nFindDir = strWorkFile.ReverseFind('\\') + 1;
	strWorkFileName = strWorkFile.Mid(nFindDir, strWorkFile.GetLength() - nFindDir - 4);

	CString strBackUPPath = GetExePath() + L"\\Work\\BackUp\\";

	// Make Directory
	_CreateDirectory(strBackUPPath);

	// Make Full Path
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	strBackUPPath.Format(_T("%s%s_%04d%02d%02d_%02d%02d%02d.wrk"),
		strBackUPPath,
		strWorkFileName,
		stTime.wYear, stTime.wMonth, stTime.wDay,
		stTime.wHour, stTime.wMinute, stTime.wSecond);

	// Copy File
	if (CopyFile(strWorkFile, strBackUPPath, FALSE))
	{
		_tracer(TRACE_NORMAL, 1, _T("Back Up Work File : %s"), strBackUPPath);
	}
}

void CIWSDoc::MoveCenterPos() // sjyi 2024.06.15 2차패턴 중심으로 이동
{
	// 2022.09.06 jh.kim 추가 - 용접전 센터로 이동 추가
	// -->
	CJob *pJob;
	if (m_work.m_JobGroup[0]->m_jobs.size() == 4)
		pJob = (CJob *)m_work.GetJob(3);
	else
		pJob = (CJob *)m_work.GetJob(2);

	CEntity *pEntity = pJob->GetEntity();
	Point3Dbl cen = pEntity->m_cen;
	Point3Dbl offset = pEntity->m_offset;
	m_pMainFrm->m_wndWorkView.m_pWorkView->GotoScaner(cen.x + offset.x, cen.y + offset.y);
	// <--
}


// 2025.09.29 Jason 사용을 위한 함수 추가 --->
BOOL CIWSDoc::CheckJson(CString strPath)
{
	CString tmp = strPath.Left(strPath.ReverseFind('.'));
	CString path = tmp.Left(tmp.ReverseFind('\\') + 1);
	CString fileName = tmp.Right(tmp.GetLength() - (tmp.ReverseFind('\\') + 1));
	CString extension = strPath.Right(strPath.GetLength() - strPath.ReverseFind('.'));

	if (extension.Compare(_T(".json")) == 0) return TRUE;
	else return FALSE;
}
// CDlgVScanCalib.cpp: 구현 파일
//
#include "./VRSaux/VRS.h"
#include "stdafx.h"
#include "IWS.h"
#include "CDlgVScanCalib.h"
#include "afxdialogex.h"
#include "IWSDoc.h"
#include "VRSaux/GrabImgRunner.h"
//#include "./VRSaux/GrabImgDlg.h"
#include "./VRSaux/ScanImgsProc.h"
#include "CDlgVScanCalib_Scan.h"
#include "CDlgVScanCalib_Learn.h"
#include "CDlgVScanCalib_Stiching.h"
// CDlgVScanCalib 대화 상자

IMPLEMENT_DYNAMIC(CDlgVScanCalib, CDialogEx)

CDlgVScanCalib::CDlgVScanCalib(CIWSDoc* pDoc, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_VSCANING_CALIB, pParent)
	, m_bInitGridParam(FALSE)
	, m_pScanImage_org(NULL)
	, m_nBlockX(0)
	, m_nBlockY(0)
	, m_pGrabPos(NULL)
	, m_pDarkImg(NULL)
	, m_bLoadedLearnImg(FALSE)
	, m_pDlgScan	(NULL)
	, m_pDlgLearning(NULL)
	, m_pDlgStiching(NULL)
	, m_rdoCalibStep(0)
{
	m_pDoc = pDoc;

	
	m_iGridUnitSize_px = 373;
	
	
	m_fLensPower = 1.0f;
	m_fLearnStrengthMin = 10.0;
	m_fLearnStrengthMax = 25.0;

	m_pDarkImg = imaqCreateImage(IMAQ_IMAGE_U8, 7);
	imaqSetImageSize(m_pDarkImg, 1400, 1200);
	PixelValue fillValue;
	vImaqMakePixelValue(&fillValue, IMAQ_IMAGE_U8, 0, 0, 0, 0);
	imaqFillImage(m_pDarkImg, fillValue, NULL);
	m_sCalibPath = m_pDoc->m_strExePath;

	m_pMergeCtrl = NULL;
	m_bSetCenterPos = FALSE;
	m_bFindDarkObj = FALSE;
}

CDlgVScanCalib::~CDlgVScanCalib()
{

	if (m_pDarkImg != NULL) {
		imaqDispose(m_pDarkImg);  m_pDarkImg = NULL;
	}
	if (m_pGrabPos != NULL)
	{
		for (int i = 0; i < m_nBlockX; i++)
		{
			delete[] m_pGrabPos[i];
		}
		delete[] m_pGrabPos;
		m_pGrabPos = NULL;
	}
	if (m_pScanImage_org != NULL)
	{
		for (int x = 0; x < m_nBlockX; x++)
		{
			for (int y = 0; y < m_nBlockY; y++)
			{
				if (m_pScanImage_org[x][y] != NULL)
				{
					imaqDispose(m_pScanImage_org[x][y]);
					m_pScanImage_org[x][y] = NULL;
				}
			}
			delete[] m_pScanImage_org[x];
		}
		delete[] m_pScanImage_org;
		m_pScanImage_org = NULL;
	}


	if (m_pDlgScan != NULL)
	{
		m_pDlgScan->DestroyWindow();
		delete m_pDlgScan;
		m_pDlgScan = NULL;
	}
	if(m_pDlgLearning !=NULL)
	{
		m_pDlgLearning->DestroyWindow();
		delete m_pDlgLearning;
		m_pDlgLearning = NULL;
	}
	if(m_pDlgStiching !=NULL)
	{
		m_pDlgStiching->DestroyWindow();
		delete m_pDlgStiching;
		m_pDlgStiching = NULL;
	}
}

void CDlgVScanCalib::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_INIT_N_SCAN, m_rdoCalibStep);


	DDX_Control(pDX, IDC_CHECK_SEL_MODE, m_chkSelBlockMode);

}


BEGIN_MESSAGE_MAP(CDlgVScanCalib, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_INIT_N_SCAN, &CDlgVScanCalib::OnBnClickedRadioInitNScan)
	ON_BN_CLICKED(IDC_RADIO_LEARNING, &CDlgVScanCalib::OnBnClickedRadioLearning)
	ON_BN_CLICKED(IDC_RADIO_Stiching, &CDlgVScanCalib::OnBnClickedRadioStiching)
	ON_BN_CLICKED(IDC_RADIO_SAVE, &CDlgVScanCalib::OnBnClickedRadioSave)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CDlgVScanCalib::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgVScanCalib::OnBnClickedCancel)

	




	

	ON_BN_CLICKED(IDC_BUTTON_DO_SAVE_LEARN, &CDlgVScanCalib::OnBnClickedButtonDoSaveLearn)
	ON_BN_CLICKED(IDC_BUTTON_DO_LOAD_LEARN, &CDlgVScanCalib::OnBnClickedButtonDoLoadLearn)


	ON_BN_CLICKED(IDC_BUTTON_SAVE_DATA, &CDlgVScanCalib::OnBnClickedButtonSaveData)
	
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_CENTER_POS, &CDlgVScanCalib::OnBnClickedButtonChangeCenterPos)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_SCALING, &CDlgVScanCalib::OnBnClickedButtonMoveScaling)
	ON_BN_CLICKED(IDC_CHECK_SEL_MODE, &CDlgVScanCalib::OnBnClickedSelBlockMode)
END_MESSAGE_MAP()


// CDlgVScanCalib 메시지 처리기




BOOL CDlgVScanCalib::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pMergeCtrl = &(g_pGrabImgRunner->m_pScanImgsProc->m_pMergeCtrl);

	int iCurLensIdx = m_pDoc->m_config.m_iCurrLens;
	m_dOldLensPowerX = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].dblLensPowerX;
	m_dOldLensPowerY = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].dblLensPowerY;
	m_pDoc->m_config.GetScanRng(&m_dOldScanSx, &m_dOldScanSy, &m_dOldScanEx, &m_dOldScanEy);

	m_iOldBlockX = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibX;
	m_iOldBlockY = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibY;
	m_iOldOverlap = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iVSOverlap;



	m_fLensPower = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].dblLensPowerX;
	m_iOverlap = m_iOldOverlap;
	m_dScanXmin = m_dOldScanSx;
	m_dScanYmin = m_dOldScanSy;
	m_dScanXmax = m_dOldScanEx;
	m_dScanYmax = m_dOldScanEy;

	float resolX_umPcnt, resolY_umPcnt;
	m_pDoc->GetCcdResolution(&resolX_umPcnt, &resolY_umPcnt);
	float fXmm = (100.0f - m_iOverlap) * 0.01f* g_pGrabImgRunner->m_nGrabWd*resolX_umPcnt*0.001f;
	float fYmm = (100.0f - m_iOverlap) * 0.01f* g_pGrabImgRunner->m_nGrabHt*resolX_umPcnt*0.001f;

	m_iImgW = g_pGrabImgRunner->m_nGrabWd;
	m_iImgH = g_pGrabImgRunner->m_nGrabHt;

	CStringA strFilename = CStringA(m_pDoc->m_config.m_arCorrParam[iCurLensIdx].szCalibDir);
	m_sCalibPath = CString(strFilename);

	m_iGridUnitSize_px = 365;//test.
	m_fEdgeMinThreshold = 20.0f;
	CString sVal;

	

	m_chkSelBlockMode.SetCheck(FALSE);

	CStatic m_stImg;




	m_nBlockX = m_iOldBlockX;
	m_nBlockY = m_iOldBlockY;



	


	m_pDlgScan		= new CDlgVScanCalib_Scan(this);
	m_pDlgScan->m_pDoc			= m_pDoc;
	m_pDlgScan->m_pMergeCtrl	= m_pMergeCtrl;
	m_pDlgScan->m_pDlgCalib		= this;

	m_pDlgLearning	= new CDlgVScanCalib_Learn(this);
	m_pDlgLearning->m_pDoc			= m_pDoc;
	m_pDlgLearning->m_pMergeCtrl	= m_pMergeCtrl;
	m_pDlgLearning->m_pDlgCalib		= this;

	m_pDlgStiching	= new CDlgVScanCalib_Stiching(this);
	m_pDlgStiching->m_pDoc			= m_pDoc;
	m_pDlgStiching->m_pMergeCtrl	= m_pMergeCtrl;
	m_pDlgStiching->m_pDlgCalib		= this;


	m_pDlgScan->Create(IDD_DIALOG_VSCANING_CALIB_SCAN, this);
	m_pDlgLearning->Create(IDD_DIALOG_VSCANING_CALIB_LEARN, this);
	m_pDlgStiching->Create(IDD_DIALOG_VSCANING_CALIB_STICHING, this);

	CRect r;
	GetDlgItem(IDC_STATIC_DLG_AREA)->GetWindowRect(r);
	ScreenToClient(r);
	m_pDlgScan->MoveWindow(r);
	m_pDlgLearning->MoveWindow(r);
	m_pDlgStiching->MoveWindow(r);
	m_rdoCalibStep = 0;
	UpdateData(FALSE);
	m_pDlgLearning->ShowWindow(SW_HIDE);
	m_pDlgStiching->ShowWindow(SW_HIDE);
	m_pDlgScan->ShowWindow(SW_SHOW);




	GetDlgItem(IDC_RADIO_LEARNING)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_Stiching)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_SAVE)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgVScanCalib::OnBnClickedRadioInitNScan()
{
	UpdateData(TRUE);

	m_pDlgLearning->ShowWindow(SW_HIDE);
	m_pDlgStiching->ShowWindow(SW_HIDE);
	m_pDlgScan->ShowWindow(SW_SHOW);
}


void CDlgVScanCalib::OnBnClickedRadioLearning()
{
	UpdateData(TRUE);

	m_pDlgStiching->ShowWindow(SW_HIDE);
	m_pDlgScan->ShowWindow(SW_HIDE);
	m_pDlgLearning->ShowWindow(SW_SHOW);
}


void CDlgVScanCalib::OnBnClickedRadioStiching()
{
	UpdateData(TRUE);

	m_pDlgScan->ShowWindow(SW_HIDE);
	m_pDlgLearning->ShowWindow(SW_HIDE);
	m_pDlgStiching->ShowWindow(SW_SHOW);
}

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg) {
		// 폴더선택 다이얼로그의 초기화가 끝난 경우
	case BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}
	return 0;
}


void CDlgVScanCalib::OnBnClickedRadioSave()
{
	if (m_pMergeCtrl->m_pBlock == NULL)
	{
		AfxMessageBox(_T("Please try after completing Learning."));
		return;
	}
	if (m_bSetCenterPos == FALSE)
	{
		AfxMessageBox(_T("Try after setting the center block."));
		return;
	}

	BROWSEINFO BrInfo;
	TCHAR szBuffer[512];                                      // 경로저장 버퍼 

	::ZeroMemory(&BrInfo, sizeof(BROWSEINFO));
	::ZeroMemory(szBuffer, 512);

	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.lpszTitle = _T("Select Data-Directory");
	BrInfo.pidlRoot = NULL;
	BrInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_BROWSEINCLUDEURLS | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
	BrInfo.lpfn = BrowseCallbackProc;

	BrInfo.lParam = (LPARAM)m_sCalibPath.GetBuffer();
	LPITEMIDLIST pItemIdList = ::SHBrowseForFolder(&BrInfo);
	::SHGetPathFromIDList(pItemIdList, szBuffer);               // 파일경로 읽어오기
	CString sPath = CString(szBuffer);
	if (sPath.GetLength() < 1)
	{
		return;
	}
	m_sCalibPath = sPath;


	m_pMergeCtrl->SetInfoDirPath(m_sCalibPath);
	if (m_pMergeCtrl->SaveMerginInfo() < 0)
	{
		AfxMessageBox(_T("Error!"));
		return;
	}

	PointFloat pS = m_pMergeCtrl->GetBlock(0, 0)->pLearn.pInfo.pStartPos_mm;
	PointFloat pE = m_pMergeCtrl->GetBlock(m_pMergeCtrl->m_nBlockX - 1, m_pMergeCtrl->m_nBlockY - 1)->pLearn.pInfo.pEndPos_mm;



	int iCurLensIdx = m_pDoc->m_config.m_iCurrLens;

	m_pDoc->m_config.m_arCorrParam[iCurLensIdx].dblLensPowerX = (double)m_fLensPower;
	m_pDoc->m_config.m_arCorrParam[iCurLensIdx].dblLensPowerY = (double)m_fLensPower;
	m_pDoc->m_config.m_arCorrParam[iCurLensIdx].bUseCalib = TRUE;
	m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibX = m_nBlockX;
	m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibY = m_nBlockY;
	m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iVSOverlap = m_iOverlap;
	m_pDoc->m_config.m_arCorrParam[iCurLensIdx].dblWorkX = fabsf(pS.x) < fabsf(pE.x) ? fabsf(pS.x)*2.0f : fabsf(pE.x)*2.0f;
	m_pDoc->m_config.m_arCorrParam[iCurLensIdx].dblWorkY = fabsf(pS.y) < fabsf(pE.y) ? fabsf(pS.y)*2.0f : fabsf(pE.y)*2.0f;


	wmemcpy_s(m_pDoc->m_config.m_arCorrParam[iCurLensIdx].szCalibDir, MAX_PATH, m_sCalibPath.GetBuffer(), m_sCalibPath.GetLength() + 1);
	m_pDoc->m_config.SetScanRng(m_dOldScanSx, m_dOldScanSy, m_dOldScanEx, m_dOldScanEy);


	double dWorkWSStepSize = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].dblWorkX / 4.0;
	if (dWorkWSStepSize != m_pDoc->m_config.m_dWPCalibStep)
	{
		m_pDoc->m_config.m_dWPCalibStep = dWorkWSStepSize;
		AfxMessageBox(_T("Be sure to try Welding spot correction again"));
	}

	m_pDoc->m_config.WriteConfig();

	float resolX, resolY;
	m_pDoc->GetCcdResolution(&resolX, &resolY);

	g_pGrabImgRunner->m_pScanImgsProc->UseCalibration(TRUE);
	//g_pGrabImgRunner->m_pScanImgsProc->SetCalibData(m_sCalibPath, m_nBlockX, m_nBlockY);
	g_pGrabImgRunner->ChangeScanRange(resolX, resolY, m_dOldScanSx, m_dOldScanSy, m_dOldScanEx, m_dOldScanEy);

	AfxMessageBox(_T("Finished Save..."));
}


void CDlgVScanCalib::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (m_pDlgScan != NULL)
	{
		m_pDlgScan->DestroyWindow();
		delete m_pDlgScan;
		m_pDlgScan = NULL;
	}
	if (m_pDlgLearning != NULL)
	{
		m_pDlgLearning->DestroyWindow();
		delete m_pDlgLearning;
		m_pDlgLearning = NULL;
	}
	if (m_pDlgStiching != NULL)
	{
		m_pDlgStiching->DestroyWindow();
		delete m_pDlgStiching;
		m_pDlgStiching = NULL;
	}
}





void CDlgVScanCalib::OnBnClickedSelBlockMode()
{
	imaqSetCurrentTool(IMAQ_POINT_TOOL);
}


void CDlgVScanCalib::OnBnClickedButtonDoSaveLearn()
{

}

void CDlgVScanCalib::OnBnClickedButtonDoLoadLearn()
{
	BROWSEINFO BrInfo;
	TCHAR szBuffer[512];                                      // 경로저장 버퍼 

	::ZeroMemory(&BrInfo, sizeof(BROWSEINFO));
	::ZeroMemory(szBuffer, 512);

	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.lpszTitle = _T("Select Data-Directory");
	BrInfo.pidlRoot = NULL;
	BrInfo.ulFlags = BIF_BROWSEINCLUDEURLS | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
	BrInfo.lpfn = BrowseCallbackProc;

	BrInfo.lParam = (LPARAM)m_sCalibPath.GetBuffer();
	LPITEMIDLIST pItemIdList = ::SHBrowseForFolder(&BrInfo);
	::SHGetPathFromIDList(pItemIdList, szBuffer);               // 파일경로 읽어오기
	CString sPath = CString(szBuffer);
	if (sPath.GetLength() < 1)
	{
		return;
	}
	m_sCalibPath = sPath;


	int iCurLensIdx = m_pDoc->m_config.m_iCurrLens;
	int nBlockX = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibX;
	int nBlockY = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibY;
	if (nBlockX < 1 || nBlockY < 1)
	{
		return;
	}

	m_pMergeCtrl->SetInfoDirPath(m_sCalibPath);
	if (m_pMergeCtrl->LoadMerginInfo(nBlockX, nBlockY) < 0)
	{
		AfxMessageBox(_T("Error!"));
		return;
	}

	m_nBlockX = nBlockX;
	m_nBlockY = nBlockY;

	//m_spinBlockX.SetRange(0, m_nBlockX - 1);
	//m_spinBlockY.SetRange(0, m_nBlockY - 1);
	//CString sBCnt;
	//sBCnt.Format(_T("/ %d"), m_nBlockX - 1); m_stBlockCntX.SetWindowText(sBCnt);
	//sBCnt.Format(_T("/ %d"), m_nBlockY - 1); m_stBlockCntY.SetWindowText(sBCnt);

	m_bSetCenterPos = TRUE;

	ImgBlock* pBlockMin = m_pMergeCtrl->GetBlock(0, 0);
	ImgBlock* pBlockMax = m_pMergeCtrl->GetBlock(m_nBlockX - 1, m_nBlockY - 1);
	float xMin = pBlockMin->pLearn.pInfo.pStartPos_mm.x;
	float xMax = pBlockMax->pLearn.pInfo.pEndPos_mm.x;

	float yMax = pBlockMin->pLearn.pInfo.pStartPos_mm.y;
	float yMin = pBlockMax->pLearn.pInfo.pEndPos_mm.y;

	CString sVal;
	//sVal.Format(_T("%.2f"), xMin);  m_eLearnAreaSX.SetWindowText(sVal);
	//sVal.Format(_T("%.2f"), yMin);  m_eLearnAreaSY.SetWindowText(sVal);
	//sVal.Format(_T("%.2f"), xMax);  m_eLearnAreaEX.SetWindowText(sVal);
	//sVal.Format(_T("%.2f"), yMax);  m_eLearnAreaEY.SetWindowText(sVal);
//	sVal.Format(_T("%.2f"), xMax - xMin);  m_eLearnAreaW.SetWindowText(sVal);
//	sVal.Format(_T("%.2f"), yMax - yMin);  m_eLearnAreaH.SetWindowText(sVal);

	AfxMessageBox(_T("Finished"));
}



void CDlgVScanCalib::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iCurLensIdx = m_pDoc->m_config.m_iCurrLens;

	g_pGrabImgRunner->m_pScanImgsProc->UseCalibration(m_pDoc->m_config.m_arCorrParam[iCurLensIdx].bUseCalib);
	CDialogEx::OnOK();
}


void CDlgVScanCalib::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}








void CDlgVScanCalib::OnBnClickedButtonChangeCenterPos()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgVScanCalib::OnBnClickedButtonMoveScaling()
{
	//if (m_pMergeCtrl->m_pMergeImg == NULL)
	//{
	//	AfxMessageBox(_T("Please try after creating the merged image."));
	//	return;
	//}
	//m_bMouseDown = FALSE;
	//imaqSetCurrentTool(IMAQ_LINE_TOOL);
	//m_bSetCenterPos = FALSE;
}


void CDlgVScanCalib::OnBnClickedButtonSaveData()
{

	BROWSEINFO BrInfo;
	TCHAR szBuffer[512];                                      // 경로저장 버퍼 

	::ZeroMemory(&BrInfo, sizeof(BROWSEINFO));
	::ZeroMemory(szBuffer, 512);

	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.lpszTitle = _T("Select Data-Directory");
	BrInfo.pidlRoot = NULL;
	BrInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_BROWSEINCLUDEURLS | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
	BrInfo.lpfn = BrowseCallbackProc;

	BrInfo.lParam = (LPARAM)_T("");
	LPITEMIDLIST pItemIdList = ::SHBrowseForFolder(&BrInfo);
	::SHGetPathFromIDList(pItemIdList, szBuffer);               // 파일경로 읽어오기
	CString sPath = CString(szBuffer);
	if (sPath.GetLength() < 1)
	{
		return;
	}

	CString strExePath = sPath+_T("\\");
	g_pGrabImgRunner->m_pScanImgsProc->SaveScanImageAll(strExePath);
}

BOOL CDlgVScanCalib::PreTranslateMessage(MSG* pMsg)
{

	return CDialogEx::PreTranslateMessage(pMsg);
}


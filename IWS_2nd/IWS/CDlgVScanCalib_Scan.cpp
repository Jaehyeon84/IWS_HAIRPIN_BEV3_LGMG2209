// CDlgVScanCalib_Scan.cpp: 구현 파일
//
#include "./VRSaux/VRS.h"
#include "stdafx.h"
#include "IWS.h"
#include "CDlgVScanCalib_Scan.h"

#include "afxdialogex.h"
#include "IWSDoc.h"
#include "CDlgVScanCalib.h"
#include "VRSaux/GrabImgRunner.h"
//#include "./VRSaux/GrabImgDlg.h"
#include "./VRSaux/ScanImgsProc.h"


// CDlgVScanCalib_Scan 대화 상자

IMPLEMENT_DYNAMIC(CDlgVScanCalib_Scan, CDialogEx)

CDlgVScanCalib_Scan::CDlgVScanCalib_Scan(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_VSCANING_CALIB_SCAN, pParent)
	, m_pDlgCalib(NULL)
	, m_bInitDlg(FALSE)
	, m_pImgWnd(NULL)
	, m_pDoc(NULL)
	, m_pSnapImg(NULL)
	, m_pMergeCtrl(NULL)

{
	m_fZoomScale = 1.0f;
	m_fZoomScaleUnit = 0.05f;
}

CDlgVScanCalib_Scan::~CDlgVScanCalib_Scan()
{

	if (m_pImgWnd != NULL)
	{
		delete m_pImgWnd;
		m_pImgWnd = NULL;
	}
	if (m_pSnapImg != NULL)
	{
		imaqDispose(m_pSnapImg);
		m_pSnapImg = NULL;
	}
}

void CDlgVScanCalib_Scan::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_STRENGTH, m_eStrength);
	DDX_Control(pDX, IDC_EDIT_GRID_XCNT, m_eGridXCnt);
	DDX_Control(pDX, IDC_EDIT_GRID_YCNT, m_eGridYCnt);
	DDX_Control(pDX, IDC_EDIT_GRID_SIZE_MM, m_eGridUnitSize_mm);
	DDX_Control(pDX, IDC_CHECK_FIND_DARK_OBJ, m_chkFindDarkObj);

	DDX_Control(pDX, IDC_EDIT_SCAN_XMIN, m_eScanXmin);
	DDX_Control(pDX, IDC_EDIT_SCAN_XMAX, m_eScanXmax);
	DDX_Control(pDX, IDC_EDIT_SCAN_YMIN, m_eScanYmin);
	DDX_Control(pDX, IDC_EDIT_SCAN_YMAX, m_eScanYmax);
	DDX_Control(pDX, IDC_EDIT_SCAN_OVERLAP_P, m_eOverlap_p);
	DDX_Control(pDX, IDC_EDIT_SCAN_OVERLAPX_MM, m_eOverlapX_mm);
	DDX_Control(pDX, IDC_EDIT_SCAN_OVERLAPY_MM, m_eOverlapY_mm);

	DDX_Control(pDX, IDC_STATIC_GRID_SIZE_PX, m_stGridUnitSize_px);
	DDX_Control(pDX, IDC_STATIC_LENS_POWER, m_stLensPower);
}


BEGIN_MESSAGE_MAP(CDlgVScanCalib_Scan, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_Load_LEARN_IMG, &CDlgVScanCalib_Scan::OnBnClickedButtonLoadLearnImg)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TO_ORIGIN, &CDlgVScanCalib_Scan::OnBnClickedButtonMoveToOrigin)
	ON_BN_CLICKED(IDC_BUTTON_SNAP_IMAGE, &CDlgVScanCalib_Scan::OnBnClickedButtonSnapImage)
	ON_BN_CLICKED(IDC_BUTTON_SET_STRENGTH, &CDlgVScanCalib_Scan::OnBnClickedButtonSetStrength)
	ON_BN_CLICKED(IDC_BUTTON_SET_GRID_INFO, &CDlgVScanCalib_Scan::OnBnClickedButtonSetGridInfo)
	ON_BN_CLICKED(IDC_BUTTON_SET_GRID_PARAM, &CDlgVScanCalib_Scan::OnBnClickedButtonSetGridParam)
	ON_BN_CLICKED(IDC_BUTTON_DO_SCAN, &CDlgVScanCalib_Scan::OnBnClickedButtonDoScan)
	ON_EN_CHANGE(IDC_EDIT_SCAN_OVERLAP_P, &CDlgVScanCalib_Scan::OnEnChangeEditScanOverlapP)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()

	ON_BN_CLICKED(IDC_BUTTON_ZOOM_P, &CDlgVScanCalib_Scan::OnBnClickedButtonZoomP)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_FIT, &CDlgVScanCalib_Scan::OnBnClickedButtonZoomFit)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_N, &CDlgVScanCalib_Scan::OnBnClickedButtonZoomN)
	ON_BN_CLICKED(IDC_CHECK_USE_VIRTUAL_MODE, &CDlgVScanCalib_Scan::OnBnClickedCheckUseVirtualMode)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDlgVScanCalib_Scan 메시지 처리기



void CDlgVScanCalib_Scan::OnDestroy()
{
	CDialogEx::OnDestroy();

}

void CDlgVScanCalib_Scan::OnOK()
{
	//CDialogEx::OnOK();
}


void CDlgVScanCalib_Scan::OnCancel()
{
	//CDialogEx::OnCancel();
}

BOOL CDlgVScanCalib_Scan::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pImgWnd = new CWorkImg(this);
	m_pImgWnd->AttachToParentWnd(GetDlgItem(IDC_STATIC_IMG_WND), TRUE, { 157, 157,   0, 0 });
	m_pImgWnd->_imaqShowScrollbars(TRUE);
	m_chkFindDarkObj.SetCheck(FALSE);
	GetDlgItem(IDC_BUTTON_Load_LEARN_IMG)->EnableWindow(FALSE);

	InitDlgData();




	m_bInitDlg = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgVScanCalib_Scan::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow && m_bInitDlg==TRUE)
	{
		InitDlgData();
	}
}

void CDlgVScanCalib_Scan::InitDlgData()
{

	CString sVal;
	sVal.Format(_T("%d"), m_pDlgCalib->m_iGridUnitSize_px); m_stGridUnitSize_px.SetWindowText(sVal);
	sVal.Format(_T("%.6f"), m_pDlgCalib->m_fLensPower); m_stLensPower.SetWindowText(sVal);


	sVal.Format(_T("%.3f"), m_pDlgCalib->m_fEdgeMinThreshold); m_eStrength.SetWindowText(sVal);
	m_eGridXCnt.SetWindowText(_T("2"));
	m_eGridYCnt.SetWindowText(_T("2"));
	m_eGridUnitSize_mm.SetWindowText(_T("5.0"));;

	float resolX_umPcnt, resolY_umPcnt;
	m_pDoc->GetCcdResolution(&resolX_umPcnt, &resolY_umPcnt);
	float fXmm = (100.0f - m_pDlgCalib->m_iOverlap) * 0.01f* g_pGrabImgRunner->m_nGrabWd*resolX_umPcnt*0.001f;
	float fYmm = (100.0f - m_pDlgCalib->m_iOverlap) * 0.01f* g_pGrabImgRunner->m_nGrabHt*resolX_umPcnt*0.001f;


	sVal.Format(_T("%.3f"), m_pDlgCalib->m_dScanXmin); m_eScanXmin.SetWindowText(sVal);
	sVal.Format(_T("%.3f"), m_pDlgCalib->m_dScanYmin); m_eScanYmin.SetWindowText(sVal);
	sVal.Format(_T("%.3f"), m_pDlgCalib->m_dScanXmax); m_eScanXmax.SetWindowText(sVal);
	sVal.Format(_T("%.3f"), m_pDlgCalib->m_dScanYmax); m_eScanYmax.SetWindowText(sVal);
	sVal.Format(_T("%d"), m_pDlgCalib->m_iOverlap);  m_eOverlap_p.SetWindowText(sVal);
	sVal.Format(_T("%.3f"), fXmm); m_eOverlapX_mm.SetWindowText(sVal);
	sVal.Format(_T("%.3f"), fYmm); m_eOverlapY_mm.SetWindowText(sVal);
	m_eOverlapX_mm.SetWindowText(_T("0"));
	m_eOverlapY_mm.SetWindowText(_T("0"));
}
void CDlgVScanCalib_Scan::MovePosScaner(double x, double y, BOOL bRel)
{
	CScanner *pScanner = m_pDoc->m_pScanner;
	CLaserPen *pOldPen = pScanner->GetLaserPen();
	CLaserPen *pPenMove = &(m_pDoc->m_pens[14]);
	pScanner->SetLaserPen(pPenMove);
	pScanner->SetLaserPenParam();
	pScanner->WaitMoving(TRUE);
	double cx, cy;
	m_pDoc->m_pScanner->GetLastPos(cx, cy);
	// 	if (cx + x >= m_pDoc->m_config.m_dWorkAreaX*0.5) x = m_pDoc->m_config.m_WorkAreaImg.x*0.5 - cx;
	// 	if (cy + y >= m_pDoc->m_config.m_dWorkAreaY*0.5) y = m_pDoc->m_config.m_WorkAreaImg.y*0.5 - cy;
	// 	if (cx + x <= -m_pDoc->m_config.m_WorkAreaImg.x*0.5)  x = -m_pDoc->m_config.m_WorkAreaImg.x*0.5 - cx;
	// 	if (cy + y <= -m_pDoc->m_config.m_WorkAreaImg.y  *0.5) y = -m_pDoc->m_config.m_WorkAreaImg.y*0.5 - cy;
	if (bRel) pScanner->MoveRel(x, y);
	else
		pScanner->Move(x, y);
	pScanner->WaitMoving(TRUE);
	long nx, ny;
	pScanner->GetEncoder(nx, ny);
	pScanner->SetLaserPen(pOldPen);
	pScanner->SetLaserPenParam();

	m_pDoc->m_pScanner->GetLastPos(x, y);
}
void CDlgVScanCalib_Scan::OnBnClickedButtonMoveToOrigin()
{
	MovePosScaner(0, 0, FALSE);
}



void CDlgVScanCalib_Scan::OnBnClickedButtonSnapImage()
{
	unsigned char *pSnap = g_pGrabImgRunner->SnapImg();
	if (m_pSnapImg != NULL)
	{
		imaqDispose(m_pSnapImg);
		m_pSnapImg = NULL;
	}

	m_pSnapImg = imaqCreateImage(IMAQ_IMAGE_U8, 7);
	if (imaqArrayToImage(m_pSnapImg, pSnap,
		g_pGrabImgRunner->m_nGrabWd, g_pGrabImgRunner->m_nGrabHt) == 0)
	{
		AfxMessageBox(_T("Error"));
		return;
	}


	CString sThreshold;
	m_eStrength.GetWindowText(sThreshold);
	float fMinThreshold = _ttof(sThreshold);

	imaqClearOverlay(m_pSnapImg, NULL);
	FindCornerNDraw(m_pSnapImg, &m_pDlgCalib->m_pOrgGridCorner, fMinThreshold, m_pDlgCalib->m_iGridUnitSize_px);
	imaqDisplayImage(m_pSnapImg, m_pImgWnd->GetNiWndId(), FALSE);
	imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
	imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);
	imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCen);

	CalcGridDimInfo();
	//m_enShowTarget = SHOW_IMG_NONE;
	//m_chkShowOrgin.SetCheck(FALSE);
	//m_chkShowCalib.SetCheck(FALSE);
	//m_chkShowMerge.SetCheck(FALSE);
}


void CDlgVScanCalib_Scan::OnBnClickedButtonSetStrength()
{
	if (m_pSnapImg == NULL)
	{
		AfxMessageBox(_T("Apply after Snap."));
		m_pDlgCalib->m_bInitGridParam = FALSE;
		return;
	}


	CString sThreshold;
	m_eStrength.GetWindowText(sThreshold);
	float fMinThreshold = _ttof(sThreshold);

	imaqClearOverlay(m_pSnapImg, NULL);
	FindCornerNDraw(m_pSnapImg, &m_pDlgCalib->m_pOrgGridCorner, fMinThreshold, m_pDlgCalib->m_iGridUnitSize_px);

	imaqDisplayImage(m_pSnapImg, m_pImgWnd->GetNiWndId(), FALSE);
	imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
	imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);
	CalcGridDimInfo();
}

void CDlgVScanCalib_Scan::FindCornerNDraw(Image* pImg, CornerData* pCorner, float fMinThreshold, int iGridUnitSize)
{
	BOOL bFindDarkObj = m_chkFindDarkObj.GetCheck();
	CornerData pCornerOrg;
	//if (CImageMerging::FindCornerInfo(pImg, iGridUnitSize * 0.94, &pCornerOrg, fMinThreshold,bFindDarkObj) < 0)
	//{
	//	m_bInitGridParam = FALSE;
	//	return;
	//}
	Rect rROI;
	//if (m_chkShowCalib.GetCheck())
	//{
	//	rROI = m_pDlgCalib->m_rCopyArea;
	//}
	//else
	//{
		int iW, iH;
		imaqGetImageSize(pImg, &iW, &iH);
		rROI = imaqMakeRect(0, 0, iH, iW);
	//}

	if (CImageMerging::FindGridInfo(pImg, rROI, iGridUnitSize*0.93, &pCornerOrg, fMinThreshold, bFindDarkObj) < 0)
	{
		m_pDlgCalib->m_bInitGridParam = FALSE;
		return;
	}




	RGBValue IMAQ_RGB_VIOLET = RGB_RED;
	if (pCornerOrg.bLT && pCornerOrg.bRT)
		imaqOverlayLine(pImg, PointFloat2Point(pCornerOrg.pLT),
			PointFloat2Point(pCornerOrg.pRT), &IMAQ_RGB_VIOLET, NULL);

	if (pCornerOrg.bRT && pCornerOrg.bRB)
		imaqOverlayLine(pImg, PointFloat2Point(pCornerOrg.pRT),
			PointFloat2Point(pCornerOrg.pRB), &IMAQ_RGB_VIOLET, NULL);

	if (pCornerOrg.bRB && pCornerOrg.bLB)
		imaqOverlayLine(pImg, PointFloat2Point(pCornerOrg.pRB),
			PointFloat2Point(pCornerOrg.pLB), &IMAQ_RGB_VIOLET, NULL);

	if (pCornerOrg.bLB && pCornerOrg.bLT)
		imaqOverlayLine(pImg, PointFloat2Point(pCornerOrg.pLB),
			PointFloat2Point(pCornerOrg.pLT), &IMAQ_RGB_VIOLET, NULL);
	*pCorner = pCornerOrg;
}

void CDlgVScanCalib_Scan::OnBnClickedButtonSetGridInfo()
{
	CalcGridDimInfo();
}

void CDlgVScanCalib_Scan::CalcGridDimInfo()
{
	if (!m_pDlgCalib->m_pOrgGridCorner.bLT
		|| !m_pDlgCalib->m_pOrgGridCorner.bLB
		|| !m_pDlgCalib->m_pOrgGridCorner.bRT
		|| !m_pDlgCalib->m_pOrgGridCorner.bRB
		)
	{
		m_pDlgCalib->m_bInitGridParam = FALSE;
		return;
	}

	float fW = 0.5f*(imaqCalcDistF(m_pDlgCalib->m_pOrgGridCorner.pLT, m_pDlgCalib->m_pOrgGridCorner.pRT) +
		imaqCalcDistF(m_pDlgCalib->m_pOrgGridCorner.pLB, m_pDlgCalib->m_pOrgGridCorner.pRB));

	float fH = 0.5f*(imaqCalcDistF(m_pDlgCalib->m_pOrgGridCorner.pLT, m_pDlgCalib->m_pOrgGridCorner.pLB) +
		imaqCalcDistF(m_pDlgCalib->m_pOrgGridCorner.pRT, m_pDlgCalib->m_pOrgGridCorner.pRB));

	CString sGridCntX;
	CString sGridCntY;

	m_eGridXCnt.GetWindowText(sGridCntX);
	m_eGridYCnt.GetWindowText(sGridCntY);

	int iGridX = _ttoi(sGridCntX);
	int iGridY = _ttoi(sGridCntY);

	float fUnitLngX_px = fW / (float)iGridX;
	float fUnitLngY_px = fH / (float)iGridY;

	float fUnitLng_px = 0.5f*(fUnitLngX_px + fUnitLngY_px);
	int iUnitLng_px = (int)(fUnitLng_px + 0.5f);
	CString sGridRealSize_mm;
	m_eGridUnitSize_mm.GetWindowText(sGridRealSize_mm);
	float fGridRealSize_mm = _ttof(sGridRealSize_mm);

	float fPixelSize_um = m_pDoc->m_config.m_fCcdResol_X;

	float fImgPixelSize_um = fGridRealSize_mm * (1000.0f / fUnitLng_px);

	float fMagnitude = fPixelSize_um / (fGridRealSize_mm / (0.001f*iUnitLng_px));     //fPixelSize_um / fImgPixelSize_um;


	CString sPixelSize;
	sPixelSize.Format(_T("%d"), iUnitLng_px);
	CString sLensPower;
	sLensPower.Format(_T("%.6f"), fMagnitude);

	m_stGridUnitSize_px.SetWindowText(sPixelSize);
	m_stLensPower.SetWindowText(sLensPower);



	OnEnChangeEditScanOverlapP();
}

void CDlgVScanCalib_Scan::OnBnClickedButtonSetGridParam()
{
	CString sPixelSize;
	m_stGridUnitSize_px.GetWindowText(sPixelSize);
	m_pDlgCalib->m_iGridUnitSize_px = _ttoi(sPixelSize);

	CString sGridSize_mm;
	m_eGridUnitSize_mm.GetWindowText(sGridSize_mm);
	m_pDlgCalib->m_fGridUnitSize_mm = _ttof(sGridSize_mm);


	CString sLensPower;
	m_stLensPower.GetWindowText(sLensPower);
	m_pDlgCalib->m_fLensPower = _ttof(sLensPower);


	CString sThreshold;
	m_eStrength.GetWindowText(sThreshold);
	m_pDlgCalib->m_fEdgeMinThreshold = _ttof(sThreshold);


	m_pDlgCalib->m_bInitGridParam = TRUE;
	m_pDlgCalib->m_bSetCenterPos = FALSE;
	m_pDlgCalib->m_bFindDarkObj = (BOOL) m_chkFindDarkObj.GetCheck();

	if (((CButton*)GetDlgItem(IDC_CHECK_USE_VIRTUAL_MODE))->GetCheck() == TRUE)
	{
		m_pDlgCalib->GetDlgItem(IDC_RADIO_LEARNING)->EnableWindow(TRUE);

		m_pDlgCalib->GetDlgItem(IDC_RADIO_Stiching)->EnableWindow(FALSE);
		m_pDlgCalib->GetDlgItem(IDC_RADIO_SAVE)->EnableWindow(FALSE);
	}
}


void CDlgVScanCalib_Scan::OnEnChangeEditScanOverlapP()
{
	//if (!m_bInitGridParam) {
	//	AfxMessageBox(_T("Grid 파라메터를 설정 후 시도하십시오"));
	//	return;
	//}
	float fResolution_umPpx = 1.0;
	if (!m_pDlgCalib->m_bInitGridParam) {
		CString sLensPower;
		m_stLensPower.GetWindowText(sLensPower);
		fResolution_umPpx = m_pDoc->m_config.m_fCcdResol_X / _ttof(sLensPower);
	}
	else
	{
		fResolution_umPpx = m_pDoc->m_config.m_fCcdResol_X / m_pDlgCalib->m_fLensPower;
	}




	CString sOver;
	m_eOverlap_p.GetWindowText(sOver);
	int   overlap = _ttoi(sOver);
	float fXmm = (100.0f - overlap) * 0.01f* g_pGrabImgRunner->m_nGrabWd*fResolution_umPpx*0.001f;
	float fYmm = (100.0f - overlap) * 0.01f* g_pGrabImgRunner->m_nGrabHt*fResolution_umPpx*0.001f;

	CString sVal;
	sVal.Format(_T("%.3f"), fXmm); m_eOverlapX_mm.SetWindowText(sVal);
	sVal.Format(_T("%.3f"), fYmm); m_eOverlapY_mm.SetWindowText(sVal);
}


void CDlgVScanCalib_Scan::OnBnClickedButtonDoScan()
{
	//if (!m_bInitGridParam){ 
	//	AfxMessageBox(_T("Grid 파라메터를 설정 후 시도하십시오")); 
	//	return;	
	//}

	CString sSX, sEX, sSY, sEY, sOver;
	m_eScanXmin.GetWindowText(sSX);
	m_eScanXmax.GetWindowText(sEX);
	m_eScanYmin.GetWindowText(sSY);
	m_eScanYmax.GetWindowText(sEY);
	m_eOverlap_p.GetWindowText(sOver);


	float sx, sy, ex, ey;
	int   overlap;
	sx = _ttof(sSX);
	ex = _ttof(sEX);
	sy = _ttof(sSY);
	ey = _ttof(sEY);
	overlap = _ttoi(sOver);

	if ((ex <= sx) || (ey <= sy) || ((sx*ex) > 0) || ((sy*ey) > 0) || overlap < 0 || overlap>100)
	{
		AfxMessageBox(_T("Check the scan range."));
		return;
	}

	m_pDlgCalib->m_dScanXmin = sx;
	m_pDlgCalib->m_dScanXmax = ex;
	m_pDlgCalib->m_dScanYmin = sy;
	m_pDlgCalib->m_dScanYmax = ey;
	m_pDlgCalib->m_iOverlap = overlap;

	int iCurLensIdx = m_pDoc->m_config.m_iCurrLens;
	//m_dOldLensPowerX = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].dblLensPowerX;
	//m_dOldLensPowerY = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].dblLensPowerY;
	//
	//m_pDoc->m_config.GetScanRng(&m_dOldScanSx, &m_dOldScanSy, &m_dOldScanEx, &m_dOldScanEy);


	m_pDoc->m_config.m_arCorrParam[iCurLensIdx].dblLensPowerX = (double)m_pDlgCalib->m_fLensPower;
	m_pDoc->m_config.m_arCorrParam[iCurLensIdx].dblLensPowerY = (double)m_pDlgCalib->m_fLensPower;

	float resolX_umPcnt, resolY_umPcnt;
	m_pDoc->GetCcdResolution(&resolX_umPcnt, &resolY_umPcnt);
	//resolX = 1000.0 / m_fResolution_cntPmm;
	//resolY = resolX;
	g_pGrabImgRunner->m_pScanImgsProc->UseCalibration(FALSE);
	g_pGrabImgRunner->m_pScanImgsProc->UseDisableScanLimit(TRUE);
	g_pGrabImgRunner->ChangeOverlap(m_pDlgCalib->m_iOverlap, m_pDlgCalib->m_iOverlap);
	g_pGrabImgRunner->ChangeScanRange(resolX_umPcnt, resolY_umPcnt, m_pDlgCalib->m_dScanXmin, m_pDlgCalib->m_dScanYmin, m_pDlgCalib->m_dScanXmax, m_pDlgCalib->m_dScanYmax);


	//==============================================
	m_pDoc->m_work.DoVScan();
	//==============================================
	m_pDlgCalib->m_dwScanTimeS = GetTickCount();


	m_pDlgCalib->m_bLoadedLearnImg = FALSE;
	GetDlgItem(IDC_BUTTON_SNAP_IMAGE)->EnableWindow(TRUE);

	m_pDlgCalib->GetDlgItem(IDC_RADIO_LEARNING)->EnableWindow(FALSE);
	m_pDlgCalib->GetDlgItem(IDC_RADIO_Stiching)->EnableWindow(FALSE);
	m_pDlgCalib->GetDlgItem(IDC_RADIO_SAVE)->EnableWindow(FALSE);
	m_pDlgCalib->m_bSetCenterPos = FALSE;

	SetTimer(2010, 100, NULL);

}

void CDlgVScanCalib_Scan::OnTimer(UINT_PTR nIDEvent)
{
	DWORD dwTime = GetTickCount() - m_pDlgCalib->m_dwScanTimeS;
	if (m_pDlgCalib->m_pGrabPos != NULL)
	{
		for (int i = 0; i < m_pDlgCalib->m_nBlockX; i++)
		{
			delete[] m_pDlgCalib->m_pGrabPos[i];
		}
		delete[] m_pDlgCalib->m_pGrabPos;
		m_pDlgCalib->m_pGrabPos = NULL;
	}

	if (m_pDlgCalib->m_pScanImage_org != NULL)
	{
#pragma omp parallel for 
		for (int x = 0; x < m_pDlgCalib->m_nBlockX; x++)
		{
			for (int y = 0; y < m_pDlgCalib->m_nBlockY; y++)
			{
				if (m_pDlgCalib->m_pScanImage_org[x][y] != NULL)
				{
					imaqDispose(m_pDlgCalib->m_pScanImage_org[x][y]);
					m_pDlgCalib->m_pScanImage_org[x][y] = NULL;
				}
			}
			delete[] m_pDlgCalib->m_pScanImage_org[x];
			m_pDlgCalib->m_pScanImage_org[x] = NULL;
		}

		delete[] m_pDlgCalib->m_pScanImage_org;
		m_pDlgCalib->m_pScanImage_org = NULL;
	}
	if (!m_pDoc->m_work.IsWorkOn())
	{
		CScanImgsProc* pImgProc = g_pGrabImgRunner->m_pScanImgsProc;
		m_pDlgCalib->m_nBlockX = pImgProc->m_nBlockX;
		m_pDlgCalib->m_nBlockY = pImgProc->m_nBlockY;
		m_pDlgCalib->m_rCopyArea = pImgProc->m_fEffectLocalFov.eLocalFovRect;

		m_pDlgCalib->m_pGrabPos = new PointFloat*[m_pDlgCalib->m_nBlockX];
		m_pDlgCalib->m_pScanImage_org = new Image**[m_pDlgCalib->m_nBlockX];

#pragma omp parallel for 
		for (int x = 0; x < m_pDlgCalib->m_nBlockX; x++)
		{
			m_pDlgCalib->m_pScanImage_org[x] = new Image*[m_pDlgCalib->m_nBlockY];
			m_pDlgCalib->m_pGrabPos[x] = new PointFloat[m_pDlgCalib->m_nBlockY];
			for (int y = 0; y < m_pDlgCalib->m_nBlockY; y++)
			{
				POINTF pt = pImgProc->m_ptScanCoord.at(y*m_pDlgCalib->m_nBlockX + x);
				m_pDlgCalib->m_pGrabPos[x][y].x = pt.x;
				m_pDlgCalib->m_pGrabPos[x][y].y = pt.y;
				m_pDlgCalib->m_pScanImage_org[x][y] = NULL;
				m_pDlgCalib->m_pScanImage_org[x][y] = imaqCreateImage(IMAQ_IMAGE_U8, 7);
				if (imaqArrayToImage(m_pDlgCalib->m_pScanImage_org[x][y],
					pImgProc->m_stitchImg.ppScanImgData[y*m_pDlgCalib->m_nBlockX + x], m_pDlgCalib->m_iImgW, m_pDlgCalib->m_iImgH) == 0)
				{
					int iErr = imaqGetLastError();
					TRACE(_T("\n Error = Create image(%d,%d) : %d \t\t"), x, y, iErr);
				}

			}
		}
		KillTimer(2010);
		//m_spinBlockX.SetRange(0, m_pDlgCalib->m_nBlockX - 1);
		//m_spinBlockY.SetRange(0, m_pDlgCalib->m_nBlockY - 1);
		//CString sBCnt;
		//sBCnt.Format(_T("/ %d"), m_pDlgCalib->m_nBlockX - 1); m_stBlockCntX.SetWindowText(sBCnt);
		//sBCnt.Format(_T("/ %d"), m_pDlgCalib->m_nBlockY - 1); m_stBlockCntY.SetWindowText(sBCnt);

		int iCurLensIdx = m_pDoc->m_config.m_iCurrLens;
		m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibX = m_pDlgCalib->m_nBlockX;
		m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibY = m_pDlgCalib->m_nBlockY;

		//OnBnClickedCheckShowOrigin();
		m_pDlgCalib->GetDlgItem(IDC_RADIO_LEARNING)->EnableWindow(TRUE);
		m_pDlgCalib->GetDlgItem(IDC_RADIO_Stiching)->EnableWindow(FALSE);
		m_pDlgCalib->GetDlgItem(IDC_RADIO_SAVE)->EnableWindow(FALSE);


		CString sMsg;
		sMsg.Format(_T("Finish (ScanTime = %ldms)"), dwTime);
		AfxMessageBox(sMsg);
	}
	else
	{
		if (dwTime > 50000)
		{
			KillTimer(2010);
			//AfxMessageBox(_T("Timeout"));
			TRACE(_T("\nTimeOut(%ld) \t\t"), (long)dwTime);
		}
	}




	CDialogEx::OnTimer(nIDEvent);
}


void CDlgVScanCalib_Scan::OnBnClickedButtonLoadLearnImg()
{

	if (m_pMergeCtrl->m_pBlock == NULL || m_pMergeCtrl->m_pBlock[0][0].pLearn.bLearnOK == FALSE) return;

	if (m_pDlgCalib->m_pGrabPos != NULL)
	{
		for (int i = 0; i < m_pDlgCalib->m_nBlockX; i++)
		{
			delete[] m_pDlgCalib->m_pGrabPos[i];
		}
		delete[] m_pDlgCalib->m_pGrabPos;
		m_pDlgCalib->m_pGrabPos = NULL;
	}

	if (m_pDlgCalib->m_pScanImage_org != NULL)
	{
#pragma omp parallel for 
		for (int x = 0; x < m_pDlgCalib->m_nBlockX; x++)
		{
			for (int y = 0; y < m_pDlgCalib->m_nBlockY; y++)
			{
				if (m_pDlgCalib->m_pScanImage_org[x][y] != NULL)
				{
					imaqDispose(m_pDlgCalib->m_pScanImage_org[x][y]);
					m_pDlgCalib->m_pScanImage_org[x][y] = NULL;
				}
			}
			delete[] m_pDlgCalib->m_pScanImage_org[x];
			m_pDlgCalib->m_pScanImage_org[x] = NULL;
		}

		delete[] m_pDlgCalib->m_pScanImage_org;
		m_pDlgCalib->m_pScanImage_org = NULL;
	}


	m_pDlgCalib->m_pGrabPos = new PointFloat*[m_pDlgCalib->m_nBlockX];
	m_pDlgCalib->m_pScanImage_org = new Image**[m_pDlgCalib->m_nBlockX];

#pragma omp parallel for 
	for (int x = 0; x < m_pDlgCalib->m_nBlockX; x++)
	{
		m_pDlgCalib->m_pScanImage_org[x] = new Image*[m_pDlgCalib->m_nBlockY];
		m_pDlgCalib->m_pGrabPos[x] = new PointFloat[m_pDlgCalib->m_nBlockY];
		for (int y = 0; y < m_pDlgCalib->m_nBlockY; y++)
		{
			ImgBlock* pBlock = m_pMergeCtrl->GetBlock(x, y);
			m_pDlgCalib->m_pGrabPos[x][y] = pBlock->pLearn.pInfo.pGrabPos;
			m_pDlgCalib->m_pScanImage_org[x][y] = imaqCreateImage(IMAQ_IMAGE_U8, 7);
			imaqDuplicate(m_pDlgCalib->m_pScanImage_org[x][y], pBlock->pLearn.pImgLearn);
		}
	}

	m_pDlgCalib->m_bLoadedLearnImg = TRUE;
	


	if (m_pSnapImg != NULL)
	{
		imaqDispose(m_pSnapImg);
		m_pSnapImg = NULL;
	}

	m_pSnapImg = imaqCreateImage(IMAQ_IMAGE_U8, 7);
	imaqDuplicate(m_pSnapImg, m_pDlgCalib->m_pScanImage_org[m_pDlgCalib->m_nBlockX / 2][m_pDlgCalib->m_nBlockY / 2]);
	int iW, iH;
	imaqGetImageSize(m_pSnapImg, &iW, &iH);
	m_pDlgCalib->m_rCopyArea.left = iW * 0.1;
	m_pDlgCalib->m_rCopyArea.top = iH * 0.1;
	m_pDlgCalib->m_rCopyArea.width = iW - (m_pDlgCalib->m_rCopyArea.left * 2);
	m_pDlgCalib->m_rCopyArea.height = iH - (m_pDlgCalib->m_rCopyArea.top * 2);


	imaqDisplayImage(m_pSnapImg, m_pImgWnd->GetNiWndId(), FALSE);
	imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
	imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);


}



void CDlgVScanCalib_Scan::OnBnClickedButtonZoomP()
{

	//if (m_chkShowMerge.GetCheck())
	//{
	//	m_fZoomScaleMerge += m_fZoomScaleUnit;
	//
	//	if (m_pSelBlock.x >= 0 && m_pSelBlock.x < m_nBlockX &&
	//		m_pSelBlock.y >= 0 && m_pSelBlock.y < m_nBlockY)
	//	{
	//		CRect cr = m_pMergeCtrl->GetBlock(m_pSelBlock)->rBlockArea;
	//		m_fZoomCenMerge.x = cr.CenterPoint().x;// r.left + r.width / 2;
	//		m_fZoomCenMerge.y = cr.CenterPoint().y;//r.top+r.height/2;
	//	}
	//	else
	//	{
	//		imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCenMerge);
	//	}
	//
	//	imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScaleMerge, m_fZoomScaleMerge, m_fZoomCenMerge);
	//}
	//else
	//{
		m_fZoomScale += m_fZoomScaleUnit;

		imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCen);
		imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScale, m_fZoomScale, m_fZoomCen);
	//}
}


void CDlgVScanCalib_Scan::OnBnClickedButtonZoomFit()
{
	imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);

	//if (m_chkShowMerge.GetCheck())
	//{
	//	imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCenMerge);
	//	imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScaleMerge, &m_fZoomScaleMerge);
	//}
	//else
	//{

		imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCen);
		imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);
	//}



}


void CDlgVScanCalib_Scan::OnBnClickedButtonZoomN()
{
	//if (m_chkShowMerge.GetCheck())
	//{
	//
	//	if ((m_fZoomScaleMerge - m_fZoomScaleUnit) < 0.01)
	//	{
	//	}
	//	else
	//	{
	//		m_fZoomScaleMerge -= m_fZoomScaleUnit;
	//	}
	//
	//
	//	if (m_pSelBlock.x >= 0 && m_pSelBlock.x < m_nBlockX &&
	//		m_pSelBlock.y >= 0 && m_pSelBlock.y < m_nBlockY)
	//	{
	//		CRect cr = m_pMergeCtrl->GetBlock(m_pSelBlock)->rBlockArea;
	//		m_fZoomCenMerge.x = cr.CenterPoint().x;// r.left + r.width / 2;
	//		m_fZoomCenMerge.y = cr.CenterPoint().y;//r.top+r.height/2;
	//	}
	//	else
	//	{
	//		imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCenMerge);
	//	}
	//
	//	imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScaleMerge, m_fZoomScaleMerge, m_fZoomCenMerge);
	//}
	//else
	//{

		if ((m_fZoomScale - m_fZoomScaleUnit) < 0.01)
		{
		}
		else
		{
			m_fZoomScale -= m_fZoomScaleUnit;
		}
		imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCen);
		imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScale, m_fZoomScale, m_fZoomCen);
	//}
}




void CDlgVScanCalib_Scan::OnBnClickedCheckUseVirtualMode()
{
	if (((CButton*)GetDlgItem(IDC_CHECK_USE_VIRTUAL_MODE))->GetCheck() == TRUE)
	{
		GetDlgItem(IDC_BUTTON_Load_LEARN_IMG)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SNAP_IMAGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_MOVE_TO_ORIGIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DO_SCAN)->EnableWindow(FALSE);
		
	}
	else
	{
		GetDlgItem(IDC_BUTTON_Load_LEARN_IMG)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SNAP_IMAGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_MOVE_TO_ORIGIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DO_SCAN)->EnableWindow(TRUE);
	}
}

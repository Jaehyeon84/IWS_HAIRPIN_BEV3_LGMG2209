// CamViewDlg.cpp : 구현 파일입니다.
//

#include "../stdafx.h"
#include "../IWS.h"
#include "../IWSDoc.h"
#include "WorkImg.h"
#include "DispImgDlg.h"
#include "GrabImgRunner.h"
#include "afxdialogex.h"
#include "Utility.h"

#define DISPLAY_TIMER		777 
#define DISPLAY_TIMER_UI	778
#define CALIB_STEP 45.f

/* main CCD */
HANDLE g_hEventDisplayStart;

/* 2nd CCD */
HANDLE g_hEventDisplayStart2nd;

/* */
DECLARE_USER_MESSAGE(UWM_GRIDWELDCAL);
// CDispImgDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDispImgDlg, CDialogEx)
CDispImgDlg::CDispImgDlg(CIWSDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CAMERA_VIEW_DLG, pParent)
	, m_dWeldPOffsetX(0)
	, m_dWeldPOffsetY(0)
{
	m_pDoc = pDoc;
	
	/* get ccd width & height */
	UINT ht, wd, ch;
	m_pDoc->GetCcdInfo(&wd, &ht, &ch, NULL, NULL);
	m_nDispHt = ht / DOWN_SAMPLE_FACTOR;
	m_nDispWd = wd / DOWN_SAMPLE_FACTOR;
	m_nDispCh = ch;
	
	/* get ccd pixel resolution including lens effect*/
	m_pDoc->GetCcdResolution(&m_fResolX, &m_fResolY);

	int dispImgSIze = m_nDispWd * m_nDispHt * m_nDispCh;

	m_pDisplayimgData = new unsigned char[dispImgSIze];
	memset(m_pDisplayimgData, 0, sizeof(unsigned char)*dispImgSIze);

	m_pDispNiImg = NULL;
	m_pResizeImg = NULL;
	m_iDispNiWindow = -1;

	m_bDisplayThredAlive = FALSE;
	/* Init global variable main CCD */
	g_hEventDisplayStart = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	m_hEventDisplayThreadEnd = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bShowLive = FALSE;
	m_rtBlink = imaqMakeRect(5, 30, 8, 8);
	m_histoInfo.pHisto = new int[NUM_HISTO_DIV];

	m_bMouseDown = FALSE;
	m_bClearOverlay = FALSE;
	m_iCurTool = IMAQ_NO_TOOL;
	m_pAttach = NULL;
	m_dCalibStep = CALIB_STEP;

	/* 2nd CCD: we currently use the same type of ccd */
	m_nDispHt2nd = m_nDispHt;
	m_nDispWd2nd = m_nDispWd;
	m_nDispCh2nd = m_nDispCh;

	m_pDoc->GetCcd2ImgInfo(&m_nImgSizeWd2nd, &m_nImgSizeHt2nd);

	/* get ccd pixel resolution including lens effect*/
	m_pDoc->GetCcdResolution(&m_fResolX2nd, &m_fResolY2nd);

	int dispImgSIze2nd = m_nDispWd2nd * m_nDispHt2nd * m_nDispCh2nd;

	m_pDisplayimgData2nd = new unsigned char[dispImgSIze2nd];
	memset(m_pDisplayimgData2nd, 0, sizeof(unsigned char)*dispImgSIze2nd);

	// 2022.11.24 jh.kim 추가 - CCD2 Live Image size 변경
	dispImgSIze2nd = (m_nImgSizeWd2nd/DOWN_SAMPLE_FACTOR) * (m_nImgSizeHt2nd/DOWN_SAMPLE_FACTOR) * m_nDispCh2nd;
	m_pDpData2nd = new unsigned char[dispImgSIze2nd];
	memset(m_pDpData2nd, 0, sizeof(unsigned char)*dispImgSIze2nd);

	m_pDispNiImg2nd = NULL;
	m_iDispNiWindow2nd = -1;

	m_bDisplayThredAlive2nd = FALSE;
	g_hEventDisplayStart2nd = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEventDisplayThreadEnd2nd = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bShowLive2nd = FALSE;

	m_pAttach2nd = NULL;
}

CDispImgDlg::~CDispImgDlg()
{
	DWORD ret;

	/* main CCD */
	m_bDisplayThredAlive = FALSE;
	g_Util.Logging(L"~CDispImgDlg(): m_bDisplayThredAlive flag was set to FALSE");

	/* set dummy data to smooth finish */
	memset(m_pDisplayimgData, 0, int(m_nDispWd*m_nDispHt) * sizeof(BYTE));
	::SetEvent(g_hEventDisplayStart);

	if (m_pDispThread != NULL)
	{
		Sleep(50);
		ret = ::WaitForSingleObject(m_hEventDisplayThreadEnd, 10000);
		
		if (ret == WAIT_TIMEOUT)
		{
			g_Util.Logging(L"~CDispImgDlg(): WaitObject(1st ThreadEnd): ret - time out");
			TerminateThread(m_pDispThread->m_hThread, 0);
			Sleep(50);
		}
		else
		{
			g_Util.Logging(L"~CDispImgDlg(): WaitObject(1st ThreadEnd): ret - ok");
		}
	}

	if (m_pAttach != NULL)
	{
		delete m_pAttach;
		m_pAttach = NULL;
	}

	::CloseHandle(g_hEventDisplayStart);
	::CloseHandle(m_hEventDisplayThreadEnd);
	DeleteObject(g_hBrushBk);
	delete[] m_histoInfo.pHisto;

	// clean up disp image buffer
	delete[] m_pDisplayimgData;

	// Ni clean up
	if (m_pDispNiImg != NULL)
		imaqDispose(m_pDispNiImg);
	if (m_pResizeImg != NULL)
		imaqDispose(m_pResizeImg);


	/* 2nd CCD */
	m_bDisplayThredAlive2nd = FALSE;
	g_Util.Logging(L"~CDispImgDlg(): m_bDisplayThredAlive2nd flag was set to FALSE");

	/* set dummy data to smooth finish */
	memset(m_pDisplayimgData2nd, 0, int(m_nDispWd2nd*m_nDispHt2nd) * sizeof(BYTE));
	::SetEvent(g_hEventDisplayStart2nd);


	if (m_pDispThread2nd != NULL)
	{
		Sleep(50);
		ret = ::WaitForSingleObject(m_hEventDisplayThreadEnd2nd, 10000);

		if (ret == WAIT_TIMEOUT)
			{
				g_Util.Logging(L"~CDispImgDlg(): WaitObject(2nd ThreadEnd): ret - time out");
				TerminateThread(m_pDispThread2nd->m_hThread, 0);
				Sleep(50);
			}
		else
		{
			g_Util.Logging(L"~CDispImgDlg(): WaitObject(2nd ThreadEnd): ret - ok");
		}
	}

	if (m_pAttach2nd != NULL)
	{
		delete m_pAttach2nd;
		m_pAttach2nd = NULL;
	}

	// clean up disp image buffer
	delete[] m_pDisplayimgData2nd;
	delete[] m_pDpData2nd;

	// Ni clean up
	if (m_pDispNiImg2nd != NULL)
		imaqDispose(m_pDispNiImg2nd);

	::CloseHandle(g_hEventDisplayStart2nd);
	::CloseHandle(m_hEventDisplayThreadEnd2nd);

	g_Util.Logging(L"============================================");
	g_Util.Logging(L"================= VISION END ==================");

}

int CDispImgDlg::GetWeldMark(int axis, double x, double y)
{
	double offset = m_dCalibStep *2.0f;
	int i = INT((x + offset) / m_dCalibStep);
	int j = INT((offset - y ) / m_dCalibStep);
	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i >= 4) i = 3;
	if (j >= 4)j = 3;
	double Fij, Fiij, Fiijj, Fijj;
	if (axis == 0) {
		Fij = m_calibX[i][j];
		Fiij = m_calibX[i + 1][j];
		Fiijj = m_calibX[i + 1][j + 1];
		Fijj = m_calibX[i][j + 1];
	}
	else {
		Fij = m_calibY[i][j];
		Fiij = m_calibY[i + 1][j];
		Fiijj = m_calibY[i + 1][j + 1];
		Fijj = m_calibY[i][j + 1];
	}
	double xi, yi;
	xi = m_dCalibStep * i - offset;
	yi = offset - m_dCalibStep * j;
	double fa, fb, fxy;

	fa = Fij + (yi - y) / m_dCalibStep * (Fijj - Fij);
	fb = Fiij + (yi - y) / m_dCalibStep * (Fiijj - Fiij);
	fxy = fa + (x - xi) / m_dCalibStep * (fb - fa);

	return int(fxy + 0.5);
}

void CDispImgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICT_HISTO, m_pictHisto);
	DDX_Control(pDX, IDC_STC_DISTANCE, m_stcDistance);
	DDX_Control(pDX, IDC_GB_UTILITY, m_gb2);
	DDX_Control(pDX, IDC_EDIT_SCAN_MOVESTEP, m_editScanStep);
	DDX_Control(pDX, IDC_STC_SCAN_CURRPOS, m_stScanCurrPos);
	DDX_Control(pDX, IDC_CHECK_WELDPOINT_CALIB, m_chkWeldPointCalib);
	DDX_Control(pDX, IDC_CHECK_WELDPOINT_OFFSET, m_chkWeldPointOffset);

	DDX_Control(pDX, IDC_EDIT_WELD_POINT_OFFSET_X, m_edtiWeldPOffsetX);
	DDX_Control(pDX, IDC_EDIT_WELD_POINT_OFFSET_Y, m_edtiWeldPOffsetY);
}


BEGIN_MESSAGE_MAP(CDispImgDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(UWM_DISPHISTO, &CDispImgDlg::OnDisplayHisto)
	//ON_MESSAGE(UWM_DISPIMG, &CDispImgDlg::OnDisplay)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_LINE_GAUGE, &CDispImgDlg::OnBnClickedBtnLineGauge)
	ON_WM_DRAWITEM()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_SCAN_MOVE_YP, &CDispImgDlg::OnBnClickedButtonScanMoveYp)
	ON_BN_CLICKED(IDC_BTN_SCAN_MOVE_YN, &CDispImgDlg::OnBnClickedButtonScanMoveYn)
	ON_BN_CLICKED(IDC_BTN_SCAN_MOVE_XP, &CDispImgDlg::OnBnClickedButtonScanMoveXp)
	ON_BN_CLICKED(IDC_BTN_SCAN_MOVE_XN, &CDispImgDlg::OnBnClickedButtonScanMoveXn)
	ON_BN_CLICKED(IDC_BTN_SCAN_MOVE_ORG, &CDispImgDlg::OnBnClickedButtonScanMoveOrg)
	//ON_BN_CLICKED(IDC_BTN_ACQ_WELDPT, &CDispImgDlg::OnBnClickedBtnAcqWeldpt)
	ON_BN_CLICKED(IDC_BTN_CALIB_MODIFY, &CDispImgDlg::OnBnClickedBtnCalibModify)
	ON_BN_CLICKED(IDC_BTN_CALIB_SAVE, &CDispImgDlg::OnBnClickedBtnCalibSave)
	ON_BN_CLICKED(IDC_BTN_CALIB_LOAD, &CDispImgDlg::OnBnClickedBtnCalibLoad)
	ON_BN_CLICKED(IDC_CHECK_WELDPOINT_CALIB, &CDispImgDlg::OnBnClickedCheckWeldpointCalib)
	ON_REGISTERED_MESSAGE(UWM_GRIDWELDCAL,&CDispImgDlg::OnUwmGridWeldCalib)
	ON_MESSAGE(UWM_WORKIMG_EVENT, &CDispImgDlg::OnWorkImgEvent)//DHChoi 20200623
	ON_BN_CLICKED(IDC_SNAP_IMG, &CDispImgDlg::OnBnClickedSnapImg)
	ON_BN_CLICKED(IDC_CHECK_WELDPOINT_OFFSET, &CDispImgDlg::OnBnClickedCheckWeldpointOffset)
	ON_BN_CLICKED(IDC_BTN_SET_WELDP_OFFSET, &CDispImgDlg::OnBnClickedBtnSetWeldpOffset)
END_MESSAGE_MAP()


// CDispImgDlg 메시지 처리기입니다.
//LRESULT CDispImgDlg::OnDisplay(WPARAM wp, LPARAM lp)
//{
//	int seq = 0;
////	 Remove redundant UWM_DISPIMG from message queue if any
//	MSG msg;
//	while (::PeekMessage(&msg, m_hWnd, UWM_DISPIMG, UWM_DISPIMG, PM_REMOVE))
//	{
//		//if (::GetMessage(&msg, m_hWnd, UWM_DISPIMG, UWM_DISPIMG))
//			seq++;
//	}
//	if (seq > 0)
//	{
//		CString str;
//		str.Format(L"Image display was not handled %d times", seq);
//		g_Util.Logging(str);
//	}
//
//	//if (g_pGrabImgRunner->m_bLveDisplay)
//	//{
//	//   // convert image data array to image
//	//	imaqArrayToImage(m_pDispNiImg, m_pDisplayimgData, m_nWdDispimg, m_nHtDispimg);
//	//	m_pAttach->AdjustImgSize(m_pDispNiImg);
//
//	//	g_Util.DrawCrossOverlay(m_pAttach->m_nWdWnd, 
//	//		m_pAttach->m_nHtWnd, CROSS_HAIR_SIZE, m_iDispNiWindow);
//
//	//	if (m_bShowLive)
//	//	{
//	//		imaqDrawShapeOnImage(m_pDispNiImg, 
//	//			m_pDispNiImg, m_rtBlink, IMAQ_PAINT_VALUE, IMAQ_SHAPE_OVAL, 3000);
//	//	}
//
//	//	g_Util.DrawTextOverlay(m_pDispNiImg, imaqMakePoint(5, 2), g_AvgFrmRate, RGB_YELLOW);
//	//	imaqDisplayImage(m_pDispNiImg, m_iDispNiWindow, FALSE);
//
//	//	HENHMETAFILE emf = g_baseAlgo.Histogram(m_pDispNiImg, TRUE, 85, 170);
//	//	m_pictLiveHisto.SetEnhMetaFile(emf);
//	//}
//
//	{
//		imaqArrayToImage(m_pDispNiImg2nd, m_pDisplayimgData2nd, m_nDispWd2nd, m_nDispHt2nd);
//		m_pAttach2nd->AdjustImgSize(m_pDispNiImg2nd, m_pResizeImg2nd);
//
//		//if (m_bClearOverlay)
//		//{
//		//	m_bClearOverlay = FALSE;
//		//	imaqClearOverlay(m_pResizeImg, NULL);
//		//	imaqSetCurrentTool(IMAQ_NO_TOOL);
//		//}
//
//		//g_Util.DrawCrossOverlay(m_pAttach2nd->m_nWdWnd,
//		//	m_pAttach2nd->m_nHtWnd, m_pAttach2nd->m_nWdWnd/*CROSS_HAIR_SIZE*/, m_iDispNiWindow2nd);
//
//		//if (m_bShowLive2nd)
//		//{
//		//	if (g_bCameraStateOK2nd)
//		//		clr = RGB_GREEN;
//		//	else
//		//		clr = RGB_RED;
//
//		//	imaqOverlayOval(m_pResizeImg2nd, imaqMakeRect(5, 70, 12, 12), &clr, IMAQ_PAINT_VALUE, NULL);
//		//}
//
//		imaqDisplayImage(m_pResizeImg2nd, 2, FALSE);
//		Sleep(20);
//		::SetEvent(g_hEventSrtCopyDispData2nd);
//
//	}
//
//	return 0;
//}


LRESULT CDispImgDlg::OnDisplayHisto(WPARAM wp, LPARAM lp)
{

	int seq = 0;
	MSG msg;
	while (::PeekMessage(&msg, m_hWnd, UWM_DISPHISTO, UWM_DISPHISTO, PM_REMOVE))
	{
		seq++;
	}
	if (seq > 0)
	{
		CString str;
		str.Format(L"Histogram display was not handled %d times", seq);
		g_Util.Logging(str);
	}

	if (g_bLiveDisplay)
	{
		{
			CRect rc;
			CDC* pDC = m_pictHisto.GetDC();
			m_pictHisto.GetClientRect(&rc);
			CDC MemDC;
			CBitmap* pOldBitmap;
			CBitmap bitmap;
			MemDC.CreateCompatibleDC(pDC);
			bitmap.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
			pOldBitmap = (CBitmap*)MemDC.SelectObject(&bitmap);
			
			DrawHistoGraph_Sobel(&MemDC, rc, m_pDispNiImg);
			pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &MemDC,0,0,SRCCOPY);

			MemDC.SelectObject(pOldBitmap);
			bitmap.DeleteObject();
			MemDC.DeleteDC();
			m_pictHisto.ReleaseDC(pDC);
		}
	}
	return 0;
}


void CDispImgDlg::DisplayImageThreadFunc()
{
	// to calc average frame rate
	double avgDispFrmRate;
	int dispFrameCnt = 0;
	double timeElasped;
	LARGE_INTEGER freq, timeBegin, timeEnd;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&timeBegin);
	
	//
	m_bDisplayThredAlive = TRUE;
	RGBValue clr;
	DWORD ret;

	RGBValue rgbBox = { 255,0,0,0 };//B,G,R,Alpha
	imaqSetWindowZoomToFit(m_iDispNiWindow, TRUE);
	while (m_bDisplayThredAlive)
	{
		ret = ::WaitForSingleObject(g_hEventDisplayStart, 50);
		switch (ret)
		{
		case WAIT_TIMEOUT:
			{
				clr = RGB_RED;
				imaqClearOverlay(m_pDispNiImg, NULL);
				memset(m_pDisplayimgData, 0, sizeof(unsigned char)*m_nDispWd*m_nDispHt);
				imaqArrayToImage(m_pDispNiImg, m_pDisplayimgData, m_nDispWd, m_nDispHt);
				imaqOverlayOval(m_pDispNiImg, imaqMakeRect(4, 38, 6, 6), &clr, IMAQ_DRAW_VALUE, NULL);
				imaqDisplayImage(m_pDispNiImg, m_iDispNiWindow, FALSE);
				break;
			}
		case WAIT_OBJECT_0:
			{
				if (!m_bDisplayThredAlive)
					break;

				if (!g_bLiveDisplay)
					break;

				// convert image data array to image
				imaqClearOverlay(m_pDispNiImg, NULL);
				imaqArrayToImage(m_pDispNiImg, m_pDisplayimgData, m_nDispWd, m_nDispHt);

				g_Util.DrawCrossLines(m_pDispNiImg, m_nDispWd, m_nDispHt, m_nDispWd, RGB_RED);

				if (m_bShowLive)
				{
					if (g_bCameraStateOK)
						clr = RGB_GREEN;
					else
						clr = RGB_RED;

					imaqOverlayOval(m_pDispNiImg, imaqMakeRect(4, 38, 6, 6), &clr, IMAQ_DRAW_VALUE, NULL);
				}

				QueryPerformanceCounter(&timeEnd);
				timeElasped = ((double)(timeEnd.QuadPart - timeBegin.QuadPart) / (double)freq.QuadPart); // in sec
				avgDispFrmRate = (int)((double)(++dispFrameCnt) / timeElasped);

				if (!g_bCameraStateOK)
					avgDispFrmRate = g_AvgFrmRate = 0.0;
				g_Util.DrawTextOverlay(m_pDispNiImg, imaqMakePoint(2, 12), avgDispFrmRate, g_AvgFrmRate, RGB_GREEN);


				if (dispFrameCnt >= 1e+6) // reset timer 
				{
					dispFrameCnt = 0;
					timeElasped = 0;
					QueryPerformanceCounter(&timeBegin);
				}

				// 2022.11.10 jh.kim  CCD Live : 1차 용접 포인트 표시
				// Get Current Scanner Position
				double x = 0;
				double y = 0;
				m_pDoc->m_pScanner->GetLastPos(x, y);

				// Get Current Scanner Position In CCD(Unit : pixel)
				int ix = GetWeldMark(0, x, y);// / 2;
				int iy = GetWeldMark(1, x, y);// / 2;
				TRACE(_T("[SJYI Test]Scanner Pos(x : %.3f, y : %.3f), Scanner Pos(ix : %d, iy : %d)\n"), x, y, ix, iy);
				
				// Get Center Position(P1, P2)
				Point3Dbl cenP1 = { 0.0, 0.0 }, cenP2 = { 0.0, 0.0 };
				if (m_pDoc->m_work.m_JobGroup.size() > 0) {
					cenP1 = m_pDoc->m_work.m_JobGroup[0]->m_jobs[0]->GetEntity()->m_cen;
					cenP2 = m_pDoc->m_work.m_JobGroup[0]->m_jobs[1]->GetEntity()->m_cen;
				}
				Point3Dbl pMark1 = { 0.0, 0.0 }, pMark2 = { 0.0, 0.0 }, pMarkScanner = { 0.0, 0.0 };
				Point3Dbl pDist1 = { 0.0, 0.0 }, pDist2 = { 0.0, 0.0 }, pDistScanner = { 0.0, 0.0 };
				TRACE(_T("[SJYI Test]P1(x : %.3f, y : %.3f), P2(x : %.3f, y : %.3f)\n"), cenP1.x, cenP1.y, cenP2.x, cenP2.y);

				// Get Scanner Position of P1, P2 In CCD(Unit : pixel)
				// sjyi 2024.03.30 LGMG2209는 /2 안함.
				int ix1 = GetWeldMark(0, cenP1.x, cenP1.y);// / 2;
				int iy1 = GetWeldMark(1, cenP1.x, cenP1.y);// / 2;
				int ix2 = GetWeldMark(0, cenP2.x, cenP2.y);// / 2;
				int iy2 = GetWeldMark(1, cenP2.x, cenP2.y);// / 2;
				TRACE(_T("[SJYI Test]P1 Pos(x : %.3f, y : %.3f), P1 Pos(ix : %d, iy : %d)\n"), cenP1.x, cenP1.y, ix1, iy1);
				TRACE(_T("[SJYI Test]P2 Pos(x : %.3f, y : %.3f), P2 Pos(ix : %d, iy : %d)\n"), cenP2.x, cenP2.y, ix2, iy2);

				// Calc Distance(Scanner to P1, P2)(include Offset)
				//pMark1.x = x - cenP1.x + 1.2;
				//pMark1.y = cenP1.y - y - 0.3;
				pDistScanner.x = m_dWeldPOffsetX;
				pDistScanner.y = m_dWeldPOffsetY;
				pDist1.x = x - cenP1.x + m_dWeldPOffsetX;
				pDist1.y = cenP1.y - y + m_dWeldPOffsetY;
				pDist2.x = x - cenP2.x + m_dWeldPOffsetX;
				pDist2.y = cenP2.y - y + m_dWeldPOffsetY;
				TRACE(_T("[SJYI Test]P1 Dist x : %.3f, y : %.3f\n"), pDist1.x, pDist1.y);
				TRACE(_T("[SJYI Test]P2 Dist x : %.3f, y : %.3f\n"), pDist2.x, pMark2.y);

				//==========================================================
				// Box그리기
				//==========================================================
				// sjyi 202310.21 Box Size 크기 변경 -> 작업 크기에 맞춰
				//float dBoxSizeW_mm = 0.5;
				//float dBoxSizeH_mm = 0.5;
				// 1차와 2차는 같으므로
				CJobWeld* pJobWeld = (CJobWeld*)m_pDoc->m_work.GetJob(0);
				float dBoxSizeW_mm = 2.0;
				float dBoxSizeH_mm = 2.0;
				if (pJobWeld)
				{
					dBoxSizeW_mm = pJobWeld->m_d1StLength * DOWN_SAMPLE_FACTOR;
					dBoxSizeH_mm = pJobWeld->m_d1StWidth * DOWN_SAMPLE_FACTOR;
				}

				// Calc Scale Factor
				float scaleX = (float)m_nDispWd / (float)(DOWN_SAMPLE_FACTOR * m_nDispWd);
				float scaleY = (float)m_nDispHt / (float)(DOWN_SAMPLE_FACTOR * m_nDispHt);

				// Calc Pixel Resolution(in um)
				float resolX_umPpx;
				float resolY_umPpx;
				m_pDoc->GetCcdResolution(&resolX_umPpx, &resolY_umPpx);

				// Calc Pixel Resolution(in mm)
				float resolX_mmPpx = resolX_umPpx / 1000.f;
				float resolY_mmPpx = resolY_umPpx / 1000.f;

				// Calc Box Size(in pixel)
				int iBoxSizeW_px = (int)(scaleX * dBoxSizeW_mm / resolX_mmPpx / DOWN_SAMPLE_FACTOR);
				int iBoxSizeH_px = (int)(scaleY * dBoxSizeH_mm / resolY_mmPpx / DOWN_SAMPLE_FACTOR);
				TRACE(_T("[SJYI Test]BoxSize W : %d, H : %d\n"), iBoxSizeW_px, iBoxSizeH_px);

				// Calc Relitive P1, P2 Position(in pixel)
				pDist1.x = (int)(scaleX * pDist1.x / resolX_mmPpx );
				pDist1.y = (int)(scaleY * pDist1.y / resolY_mmPpx );
				pDist2.x = (int)(scaleX * pDist2.x / resolX_mmPpx );
				pDist2.y = (int)(scaleY * pDist2.y / resolY_mmPpx );
				pDistScanner.x = (int)(scaleX * pDistScanner.x / resolX_mmPpx);
				pDistScanner.y = (int)(scaleY * pDistScanner.y / resolY_mmPpx);
				TRACE(_T("[SJYI Test]P1 Dist(Pixel) x : %.3f, y : %.3f\n"), pDist1.x, pDist1.y);
				TRACE(_T("[SJYI Test]P2 Dist(Pixel) x : %.3f, y : %.3f\n"), pDist2.x, pMark2.y);

				// Calc Mark1, Mark2(in Pixel)
				pMark1.x = (int)(ix1 - pDist1.x);
				pMark1.y = (int)(iy1 - pDist1.y);
				pMark2.x = (int)(ix2 - pDist2.x);
				pMark2.y = (int)(iy2 - pDist2.y);
				pMarkScanner.x = (int)(ix - pDistScanner.x);
				pMarkScanner.y = (int)(iy - pDistScanner.y);

				// Calc Rect
				Rect rBox;
				//rBox.left = ix - (iBoxSizeW_px / 2);
				//rBox.top = iy - (iBoxSizeH_px / 2);
				if (m_bUseCCD2ndPtnDisplay)
				{
					rBox.left = (int)(pMark1.x - (iBoxSizeW_px / 2));
					rBox.top = (int)(pMark1.y - (iBoxSizeH_px / 2));
					rBox.width = iBoxSizeW_px;
					rBox.height = iBoxSizeH_px;
					imaqOverlayRect(m_pDispNiImg, rBox, &rgbBox, IMAQ_DRAW_VALUE, NULL);
					TRACE(_T("[SJYI Test]Box1 left : %d, top : %d\n"), rBox.left, rBox.top);

					rBox.left = (int)(pMark2.x - (iBoxSizeW_px / 2));
					rBox.top = (int)(pMark2.y - (iBoxSizeH_px / 2));
					rBox.width = iBoxSizeW_px;
					rBox.height = iBoxSizeH_px;
					imaqOverlayRect(m_pDispNiImg, rBox, &rgbBox, IMAQ_DRAW_VALUE, NULL);
					TRACE(_T("[SJYI Test]Box2 left : %d, top : %d\n"), rBox.left, rBox.top);
				}
				else
				{
					rBox.left = ix - (iBoxSizeW_px / 2);
					rBox.top = iy - (iBoxSizeH_px / 2);
					rBox.width = iBoxSizeW_px;
					rBox.height = iBoxSizeH_px;
					imaqOverlayRect(m_pDispNiImg, rBox, &rgbBox, IMAQ_DRAW_VALUE, NULL);
				}

				//==========================================================
				// 용접 스팟 그리기
				//==========================================================
				RGBValue rgb = RGB_GREEN;
				RGBValue rgbS = RGB_RED;
				if (m_bUseCCD2ndPtnDisplay)
				{
					imaqOverlayOval(m_pDispNiImg, imaqMakeRect((int)(pMark1.y - 4), (int)(pMark1.x - 4), 8, 8), &rgb, IMAQ_PAINT_VALUE, NULL);
					imaqOverlayOval(m_pDispNiImg, imaqMakeRect((int)(pMark2.y - 4), (int)(pMark2.x - 4), 8, 8), &rgb, IMAQ_PAINT_VALUE, NULL);
					imaqOverlayOval(m_pDispNiImg, imaqMakeRect((int)(pMarkScanner.y - 2), (int)(pMarkScanner.x - 2), 4, 4), &rgbS, IMAQ_PAINT_VALUE, NULL); // imsi
				}
				else
				{
					imaqOverlayOval(m_pDispNiImg, imaqMakeRect(iy - 4, ix - 4, 8, 8), &rgb, IMAQ_PAINT_VALUE, NULL);
				}

				imaqDisplayImage(m_pDispNiImg, m_iDispNiWindow, FALSE);

				if (dispFrameCnt % 5 == 0)
					::PostMessageW(this->m_hWnd, UWM_DISPHISTO, 0, 0);

				::SetEvent(g_hEventSrtCopyDispData);

				break;
			}
		} // switch loop
	}
	::SetEvent(m_hEventDisplayThreadEnd);
	g_Util.Logging(L"CCD 1: Display ThreadEnd Event was set");
}

/* 2nd CCD */
void CDispImgDlg::DisplayImageThreadFunc2nd()
{
	// to calc average frame rate
	double avgDispFrmRate;
	int dispFrameCnt = 0;
	double timeElasped;
	LARGE_INTEGER freq, timeBegin, timeEnd;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&timeBegin);

	UINT nImgSizeW = m_nImgSizeWd2nd / DOWN_SAMPLE_FACTOR;
	UINT nImgSizeH = m_nImgSizeHt2nd / DOWN_SAMPLE_FACTOR;

	//
	m_bDisplayThredAlive2nd = TRUE;
	RGBValue clr;

	DWORD ret;
	imaqSetWindowZoomToFit(m_iDispNiWindow2nd, TRUE);

	while (m_bDisplayThredAlive2nd)
	{
		ret = ::WaitForSingleObject(g_hEventDisplayStart2nd, 3000);

		switch (ret)
		{
		case WAIT_TIMEOUT:
			{
				clr = RGB_RED;
				imaqClearOverlay(m_pDispNiImg2nd, NULL);
				//memset(m_pDisplayimgData2nd, 0, sizeof(unsigned char)*m_nDispWd2nd*m_nDispHt2nd);
				//imaqArrayToImage(m_pDispNiImg2nd, m_pDisplayimgData2nd, m_nDispWd2nd, m_nDispHt2nd);
				memset(m_pDpData2nd, 0, sizeof(unsigned char)*nImgSizeW*nImgSizeH);
				imaqArrayToImage(m_pDispNiImg2nd, m_pDpData2nd, nImgSizeW, nImgSizeH);
				//imaqOverlayOval(m_pDispNiImg2nd, imaqMakeRect(4, 38, 6, 6), &clr, IMAQ_DRAW_VALUE, NULL);
				imaqOverlayOval(m_pDispNiImg2nd, imaqMakeRect(2, 10, 3, 3), &clr, IMAQ_DRAW_VALUE, NULL);
				imaqDisplayImage(m_pDispNiImg2nd, m_iDispNiWindow2nd, FALSE);
				break;
			}
		case WAIT_OBJECT_0:
			{
				if (!m_bDisplayThredAlive2nd)
					break;

				if (!g_bLiveDisplay2nd)
					break;

				// convert image data array to image
				imaqClearOverlay(m_pDispNiImg2nd, NULL);
				//imaqArrayToImage(m_pDispNiImg2nd, m_pDisplayimgData2nd, m_nDispWd2nd, m_nDispHt2nd);
				imaqArrayToImage(m_pDispNiImg2nd, m_pDpData2nd, nImgSizeW, nImgSizeH);

				if (m_bShowLive2nd)
				{
					if (g_bCameraStateOK2nd)
						clr = RGB_GREEN;
					else
						clr = RGB_RED;

					//imaqOverlayOval(m_pDispNiImg2nd, imaqMakeRect(4, 38, 6, 6), &clr, IMAQ_DRAW_VALUE, NULL);
					imaqOverlayOval(m_pDispNiImg2nd, imaqMakeRect(2, 10, 3, 3), &clr, IMAQ_DRAW_VALUE, NULL);
				}

				//g_Util.DrawCrossLines(m_pDispNiImg2nd, m_nDispWd2nd, m_nDispHt2nd, m_nDispWd2nd, RGB_CYAN);
				g_Util.DrawCrossLines(m_pDispNiImg2nd, nImgSizeW, nImgSizeH, nImgSizeW, RGB_CYAN);
	
				QueryPerformanceCounter(&timeEnd);
				timeElasped = ((double)(timeEnd.QuadPart - timeBegin.QuadPart) / (double)freq.QuadPart); // in sec
				avgDispFrmRate = (int)((double)(++dispFrameCnt) / timeElasped);

				if (!g_bCameraStateOK2nd)
					avgDispFrmRate = g_AvgFrmRate2nd = 0.0;
				
				//g_Util.DrawTextOverlay(m_pDispNiImg2nd, imaqMakePoint(2, 12), avgDispFrmRate, g_AvgFrmRate2nd, RGB_GREEN);
				g_Util.DrawTextOverlay2(m_pDispNiImg2nd, imaqMakePoint(2, 10), avgDispFrmRate, g_AvgFrmRate2nd, RGB_GREEN);

				if (dispFrameCnt > 1e+6) // reset timer 
				{
					dispFrameCnt = 0;
					timeElasped = 0;
					QueryPerformanceCounter(&timeBegin);
				}

				imaqDisplayImage(m_pDispNiImg2nd, m_iDispNiWindow2nd, FALSE);

				::SetEvent(g_hEventSrtCopyDispData2nd);
				break;
			}
		} //switch
	} //while

	::SetEvent(m_hEventDisplayThreadEnd2nd);
	g_Util.Logging(L"CCD 2: Display ThreadEnd event was set");
}

void CDispImgDlg::DrawHistoGraph_Sobel(CDC* pDC, CRect rc, Image* pImg)
{
	CRect rcHistoGraph;
	rcHistoGraph.SetRect(0, 0, rc.right, rc.bottom);

	//Histogram을 그릴 영역 설정
	long lSobelValue=0;
	CalcHistogram_sobel(pImg, &m_histoInfo, &lSobelValue, NUM_HISTO_DIV);

	CPen penCyan;
	penCyan.CreatePen(PS_SOLID, 2, CYAN);

	CBrush brushBack;
	brushBack.CreateSolidBrush(BACK);

	CPen penGreen;
	penGreen.CreatePen(PS_SOLID, 1, GREEN);
	//
	CPen penCyanDot;
	penCyanDot.CreatePen(PS_DOT, 1, CYAN);

	// 1. 전체 DC 배경 back 칼라로 칠하고
//	pDC->FillSolidRect(rcHistoGraph, BACK);

	CPen *pOldPen;
	// 2. 히스토 그램 영역의 테두리는 CYAN 칼라로 그리고
	pOldPen = (CPen*)pDC->SelectObject(&penCyan);
	pDC->Rectangle(rcHistoGraph.left, rcHistoGraph.top, rcHistoGraph.right, rcHistoGraph.bottom);
	pDC->SelectObject(pOldPen);
	penCyan.DeleteObject();
	pDC->SetBkMode(TRANSPARENT);

	//// 3. 히스토 그램 영역을 브러쉬로 칠하고
	CBrush *pOldBrush;
	pOldBrush = (CBrush*)pDC->SelectObject(&brushBack);
	pDC->Rectangle(rcHistoGraph.left + 1, rcHistoGraph.top + 1, rcHistoGraph.right - 1, rcHistoGraph.bottom - 1);
	pDC->SelectObject(pOldBrush);
	brushBack.DeleteObject();

	// 4. Draw Histogram
	//int histoX = 0;
	//int preX = 0;
	//float step = (float)rcHistoGraph.Width() / (float)NUM_HISTO_DIV;
	//
	//pOldPen = (CPen*)pDC->SelectObject(&penGreen);
	//for (int i = 0; i < NUM_HISTO_DIV; i++)
	//{
	//	histoX = (int)((float)rcHistoGraph.left + step * (float)i);
	//	int histoY = (int)(rcHistoGraph.bottom - (rcHistoGraph.Height()*m_histoInfo.pHisto[i]) / (m_histoInfo.maxCnt + 1));
	//
	//	pDC->Rectangle(preX, histoY, (int)(histoX + step), rcHistoGraph.bottom);
	//	preX = (int)(histoX + step);
	//}
	//pDC->SelectObject(pOldPen);
	//penGreen.DeleteObject();
	int histoX = 0;
	int histoY = 0;
	int preX = 0;
	//float step = (float)rcHistoGraph.Width() / (float)256;
	float step = (float)rcHistoGraph.Height() / (float)256;// pHisto->maxCnt;

	CBrush brushGreen;
	brushGreen.CreateSolidBrush(GREEN);
	pOldBrush = (CBrush*)pDC->SelectObject(&brushGreen);

	pOldPen = (CPen*)pDC->SelectObject(&penGreen);
	double dLogMax = log(m_histoInfo.maxCnt + 1);
	double dGraphW = rcHistoGraph.Width();
	double dGraphH = rcHistoGraph.Height();
	CPoint p1, p2;
	for (int i = 0; i < 256; i++)
	{
		double dLogH = 0.0;//
		if (m_histoInfo.pHisto[i] > 0) dLogH = log(m_histoInfo.pHisto[i]);

		histoX = (int)((float)rcHistoGraph.left + (dGraphW*dLogH / dLogMax));
		histoY = (int)((float)rcHistoGraph.bottom - ((float)i*step));
		p1.x = rcHistoGraph.left;
		p1.y = histoY;
		p2.x = histoX;
		p2.y = (int)(histoY - step);
		pDC->Rectangle(p1.x, p1.y, p2.x, p2.y);
	}
	pDC->SelectObject(pOldPen);
	penGreen.DeleteObject();
	pDC->SelectObject(pOldBrush);
	brushGreen.DeleteObject();



	// 5. Grid 그리고
	pOldPen = (CPen*)pDC->SelectObject(&penCyanDot);

	int wSpan = rcHistoGraph.Width() / 4;
	//vertical grid
	for (int i = 1; i <= 3; i++)
	{
		pDC->MoveTo(rcHistoGraph.left + wSpan * i, rcHistoGraph.bottom);
		pDC->LineTo(rcHistoGraph.left + wSpan * i, rcHistoGraph.top);
	}
	//horizontal grid
	pDC->MoveTo(rcHistoGraph.left, rcHistoGraph.top + rcHistoGraph.Height() / 2);
	pDC->LineTo(rcHistoGraph.right, rcHistoGraph.top + rcHistoGraph.Height() / 2);
	pDC->SelectObject(pOldPen);
	penCyanDot.DeleteObject();


	// 6.Draw maximum frequency value
	CRect rcText;
	int left = rcHistoGraph.Width() / 2 - 200;
	int right = rcHistoGraph.Width() / 2 + 200;
	rcText.SetRect(left, rcHistoGraph.top + 10, right, rcHistoGraph.top + 30);

	CString str;
	//str.Format(_T("[Gray:%d, MaxCount:%d]"), m_histoInfo.grayAtMax, m_histoInfo.maxCnt);
	str.Format(_T("[Gray:%d, MaxCount:%d, Focus:%ld]"), m_histoInfo.grayAtMax, m_histoInfo.maxCnt, lSobelValue);
	DisplayTextOnDC(18, 0, rcText, YELLOW, str, 2, pDC);
}


void CDispImgDlg::DrawHistoGraph(CDC* pDC, CRect rc, Image* pImg)
{
	CRect rcHistoGraph;
	rcHistoGraph.SetRect(0, 0, rc.right, rc.bottom);
	
	//Histogram을 그릴 영역 설정
	CalcHistogram(pImg, &m_histoInfo, NUM_HISTO_DIV);
	
	CPen penCyan;
	penCyan.CreatePen(PS_SOLID, 2, CYAN);

	CBrush brushBack;
	brushBack.CreateSolidBrush(BACK);

	CPen penGreen;
	penGreen.CreatePen(PS_SOLID, 1, GREEN);
	//
	CPen penCyanDot;
	penCyanDot.CreatePen(PS_DOT, 1, CYAN);

	// 1. 전체 DC 배경 back 칼라로 칠하고
//	pDC->FillSolidRect(rcHistoGraph, BACK);

	CPen *pOldPen;
	// 2. 히스토 그램 영역의 테두리는 CYAN 칼라로 그리고
	pOldPen = (CPen*)pDC->SelectObject(&penCyan);
	pDC->Rectangle(rcHistoGraph.left, rcHistoGraph.top, rcHistoGraph.right, rcHistoGraph.bottom);
	pDC->SelectObject(pOldPen);
	penCyan.DeleteObject();
	pDC->SetBkMode(TRANSPARENT);

	//// 3. 히스토 그램 영역을 브러쉬로 칠하고
	CBrush *pOldBrush;
	pOldBrush = (CBrush*)pDC->SelectObject(&brushBack);
	pDC->Rectangle(rcHistoGraph.left+1, rcHistoGraph.top+1, rcHistoGraph.right-1, rcHistoGraph.bottom-1);
	pDC->SelectObject(pOldBrush);
	brushBack.DeleteObject();	

	// 4. Draw Histogram
	int histoX = 0;
	int preX = 0;
	float step = (float)rcHistoGraph.Width() / (float)NUM_HISTO_DIV;
	
	pOldPen = (CPen*)pDC->SelectObject(&penGreen);
	for (int i = 0; i < NUM_HISTO_DIV; i++)
	{
		histoX = (int)((float)rcHistoGraph.left + step * (float)i);
		int histoY = (int)(rcHistoGraph.bottom - (rcHistoGraph.Height()*m_histoInfo.pHisto[i]) / (m_histoInfo.maxCnt+1));

		pDC->Rectangle(preX, histoY, (int)(histoX + step), rcHistoGraph.bottom);
		preX = (int)(histoX + step);
	}
	pDC->SelectObject(pOldPen);
	penGreen.DeleteObject();

	// 5. Grid 그리고
	pOldPen = (CPen*)pDC->SelectObject(&penCyanDot);

	int wSpan = rcHistoGraph.Width() / 4;
	//vertical grid
	for (int i = 1; i <= 3; i++)
	{
		pDC->MoveTo(rcHistoGraph.left + wSpan * i, rcHistoGraph.bottom);
		pDC->LineTo(rcHistoGraph.left + wSpan * i, rcHistoGraph.top);
	}
	//horizontal grid
	pDC->MoveTo(rcHistoGraph.left, rcHistoGraph.top + rcHistoGraph.Height() / 2);
	pDC->LineTo(rcHistoGraph.right, rcHistoGraph.top + rcHistoGraph.Height() / 2);
	pDC->SelectObject(pOldPen);
	penCyanDot.DeleteObject();


	// 6.Draw maximum frequency value
	CRect rcText;
	int left = rcHistoGraph.Width() / 2 - 100;
	int right = rcHistoGraph.Width() / 2 + 100;
	rcText.SetRect(left, rcHistoGraph.top + 10, right, rcHistoGraph.top + 30);

	CString str;
	str.Format(_T("[Gray:%d, MaxCount:%d]"), m_histoInfo.grayAtMax, m_histoInfo.maxCnt);
	DisplayTextOnDC(18, 0, rcText, YELLOW, str, 2, pDC);
}


// 화면에 필요한 글자를 표기하는 함수
// PARAMETERS :
//	1. size: font size
//	2. bthick : thickness 0->normal, 1->bold
//	3. rc : region of text
//	4. txtclr: text color
//	5. str : text
//	6. bpos: align 0->left alignment, 1-> center alignment
//	7. pDC : pointer of DC
void CDispImgDlg::DisplayTextOnDC(int size, BOOL bthick, CRect rc, COLORREF txtclr, CString str, BOOL bpos, CDC *pDC)
{
	CFont font;
	CFont* pOldFont;

	int weight = FW_NORMAL;
	if (bthick)
	{
		weight = FW_BOLD;
	}

	CString strFontName;
	strFontName.Format(_T("Consolas"));

	font.CreateFont(size, 0, 0, 0, weight, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, CLEARTYPE_NATURAL_QUALITY | FF_SWISS, strFontName);

	pOldFont = (CFont*)pDC->SelectObject(&font);
	pDC->SetTextColor(txtclr);

	DWORD align = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
	switch (bpos)
	{
	case 0: align = DT_LEFT | DT_VCENTER | DT_SINGLELINE; break;
	case 1: align = DT_CENTER | DT_VCENTER | DT_SINGLELINE; break;
	}

	pDC->DrawText(str, &rc, align);
	pDC->SelectObject(pOldFont);
	font.DeleteObject();
}

void CDispImgDlg::MovePosScaner(double x, double y, BOOL bRel)
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
	if(bRel) pScanner->MoveRel(x, y);
	else 
		pScanner->Move(x, y);
	pScanner->WaitMoving(TRUE);
	long nx, ny;
	pScanner->GetEncoder(nx, ny);
	pScanner->SetLaserPen(pOldPen);
	pScanner->SetLaserPenParam();

	m_pDoc->m_pScanner->GetLastPos(x, y);
	int ix = GetWeldMark(0, x, y);
	int iy = GetWeldMark(1, x, y);
	TRACE("Weld Mark Pos = %d, %d\n", ix, iy);
}

//void CDispImgDlg::StopDisplay()
//{
//	// let grab thread -> disp thread stop
//	g_bLiveDisplay = FALSE;
////	ShowWindow(SW_HIDE);
//}

/* 2nd CCD  */
//void CDispImgDlg::StopDisplay2nd()
//{
//	// let grab thread -> disp thread stop
//	g_bLiveDisplay2nd = FALSE;
//	//	ShowWindow(SW_HIDE);
//}

void CDispImgDlg::StartDisplay()
{
	g_bLiveDisplay = TRUE;
	ShowWindow(SW_SHOW);
}

/* 2nd CCD */
void CDispImgDlg::StartDisplay2nd()
{
	g_bLiveDisplay2nd = TRUE;
	ShowWindow(SW_SHOW);
}

BOOL CDispImgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetWindowTheme(GetDlgItem(IDC_CHECK_WELDPOINT_CALIB)->m_hWnd, L"", L""); //yjko
	SetWindowTheme(GetDlgItem(IDC_CHECK_WELDPOINT_OFFSET)->m_hWnd, L"", L"");

	m_dCalibStep = m_pDoc->m_config.m_dWPCalibStep;
	m_gridCal.AttachGrid(this, IDC_GRID_WELDCAL);
	m_gridCal.m_hRcvHandle = this->GetSafeHwnd();

	// sjyi 2024.03.29 수정
	LoadCCDOffset();
	OnBnClickedCheckWeldpointOffset();

	OnBnClickedBtnCalibLoad();
	
	m_gb2.SetBoxColors(BACK, CYAN, CYAN);

	SetTimer(DISPLAY_TIMER, 1000, 0);
	SetTimer(DISPLAY_TIMER_UI, 200, 0);

	if (m_pAttach == NULL)
		m_pAttach = new CWorkImg(this);

	m_pAttach->AttachToParentWnd(GetDlgItem(IDC_CAM_VIEW), TRUE,RGB_DARKCYAN);
	m_iDispNiWindow = m_pAttach->GetNiWndId();

	// Ni variables
	m_pDispNiImg = imaqCreateImage(g_ImgType, 3);
	m_pResizeImg = imaqCreateImage(g_ImgType, 3);

	SetWindowTheme(GetDlgItem(IDC_CAM_VIEW)->m_hWnd, L"", L"");

	GetDlgItem(IDC_STATIC_DESC)->SetFont(&g_textFont);
	m_stcDistance.SetFont(&g_textFont);
	m_editScanStep.SetWindowTextW(L"10.0");

	m_pDispThread = NULL;
	if (g_pGrabImgRunner->m_bCCDFound[CCD_MAIN])
		m_pDispThread = AfxBeginThread(DisplayImageThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);

	/* 2nd CCD */
	if (m_pAttach2nd == NULL)
		m_pAttach2nd = new CWorkImg(this);

	m_pAttach2nd->AttachToParentWnd(GetDlgItem(IDC_CAM_VIEW_2ND), TRUE, RGB_DARKCYAN);
	m_iDispNiWindow2nd = m_pAttach2nd->GetNiWndId();

	// Ni variables
	m_pDispNiImg2nd = imaqCreateImage(g_ImgType2nd, 3);

	SetWindowTheme(GetDlgItem(IDC_CAM_VIEW_2ND)->m_hWnd, L"", L"");
	
	m_pDispThread2nd = NULL;

	if(g_pGrabImgRunner->m_bCCDFound[CCD_2ND])
		m_pDispThread2nd = AfxBeginThread(DisplayImageThread2nd, (LPVOID)this, THREAD_PRIORITY_NORMAL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

UINT CDispImgDlg::DisplayImageThread(LPVOID pArgs)
{
	CDispImgDlg* me = (CDispImgDlg*)pArgs;
	me->DisplayImageThreadFunc();
	return 0;
}

UINT CDispImgDlg::DisplayImageThread2nd(LPVOID pArgs)
{
	CDispImgDlg* me = (CDispImgDlg*)pArgs;
	me->DisplayImageThreadFunc2nd();
	return 0;
}

void CDispImgDlg::MoveNResizeCtrl(int cx, int cy)
{
	int top0, top1, top2, gap;
	int h0, h1, h2;

	gap = 5;
	top0 = 0;
	h0 = (int)((float)cy * 0.65f);


//	GetDlgItem(IDC_CAM_VIEW)->MoveWindow(0, 0, cx, h);
	m_pAttach->MoveWindowTo(0, top0, cx, h0);

	top1 = top0 + h0 + gap;
	h1 = (int)((float)cy * 0.25f);
	GetDlgItem(IDC_PICT_HISTO)->MoveWindow(1, top1, cx - 1, h1);

	top2 = top1 + h1 + 2*gap;
	h2 = cy - h0 - h1 - 3*gap;

	CRect rt;
	GetDlgItem(IDC_STATIC_DESC)->GetWindowRect(&rt);
	int wd = rt.Width();
	int ht = rt.Height();
	GetDlgItem(IDC_STATIC_DESC)->MoveWindow(20, top2 +20, wd, ht);

	wd += 30;
	m_stcDistance.GetWindowRect(&rt);
	m_stcDistance.MoveWindow(wd, top2 + 20, rt.Width(), rt.Height());

	wd += (rt.Width() + 10);
	GetDlgItem(IDC_BTN_LINE_GAUGE)->GetWindowRect(&rt);
	GetDlgItem(IDC_BTN_LINE_GAUGE)->MoveWindow(wd, top2 + 20, rt.Width(), rt.Height());
}

HBRUSH CDispImgDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	HBRUSH myhbr = g_Util.CtlColor(pDC, pWnd, nCtlColor, g_hBrushBk);
	if (myhbr == NULL)	return hbr;
	else return myhbr;
}

void CDispImgDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (0)
	{
		if(GetDlgItem(IDC_CAM_VIEW)->GetSafeHwnd())
			MoveNResizeCtrl(cx, cy);
	}
}


void CDispImgDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{
	case DISPLAY_TIMER:
		m_bShowLive = !m_bShowLive;
		m_bShowLive2nd = !m_bShowLive2nd;
		break;
	case DISPLAY_TIMER_UI:
		{
			if (!m_pDoc) break;
			CScanner *pScanner = m_pDoc->m_pScanner;
			double x, y;
			pScanner->GetLastPos(x, y);
			CString str;
			str.Format(L"%6.2f, %6.2f", x, y);
			m_stScanCurrPos.SetWindowTextW(str);

			UpdateCCDOffsetBtn();
		}
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CDispImgDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_bDisplayThredAlive = FALSE;
	::SetEvent(g_hEventDisplayStart);
	Sleep(100);
	KillTimer(DISPLAY_TIMER);

	///* 2nd CCD */
	//m_bDisplayThredAlive2nd = FALSE;
	//::SetEvent(g_hEventDisplayStart2nd);
}


void CDispImgDlg::OnBnClickedBtnLineGauge()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// change to line tool
	m_iCurTool = IMAQ_LINE_TOOL;
}

void CDispImgDlg::OnLButtonDown()
{
	if (m_iCurTool == IMAQ_POINT_TOOL) {
		if (!m_chkWeldPointCalib.GetCheck()) return;
		imaqGetMousePos(&m_ptMDown, &m_iDispNiWindow);
		int iRet = m_gridCal.SetCurrentData((double)m_ptMDown.x, (double)m_ptMDown.y);
		if (iRet >= 0) {
			int ix = iRet % 5;
			int iy = iRet / 5;
			m_calibX[ix][iy] = (double)m_ptMDown.x;
			m_calibY[ix][iy] = (double)m_ptMDown.y;
		}
		m_gridCal.RedrawAll();
		m_iCurTool = IMAQ_NO_TOOL;
	}
		
	if (m_iCurTool == IMAQ_LINE_TOOL)
	{
		imaqSetCurrentTool(m_iCurTool);
		m_bMouseDown = TRUE;
		imaqGetMousePos(&m_ptMDown, &m_iDispNiWindow);
	}
//	else
//		imaqSetCurrentTool(IMAQ_NO_TOOL);
}

void CDispImgDlg::OnMouseMove()
{
	Tool tool;
	imaqGetCurrentTool(&tool);
	if (tool == IMAQ_NO_TOOL)
		return;

	if (tool != IMAQ_LINE_TOOL)
	{
		imaqSetCurrentTool(IMAQ_NO_TOOL);
		return;
	}
	if (!m_bMouseDown)
		return;

	imaqGetMousePos(&m_ptMMove, &m_iDispNiWindow);
	int dx = m_ptMMove.x - m_ptMDown.x;
	int dy = m_ptMMove.y - m_ptMDown.y;

	float distance = ConvertPxlToRealDist(dx, dy);
	CString strDist;
	strDist.Format(L"%.2f mm", distance);
	m_stcDistance.SetWindowTextW(strDist);

}

void CDispImgDlg::OnLButtonUp()
{
	if (m_iCurTool == IMAQ_LINE_TOOL)
	{
		CString strDist;
		strDist.Format(L"%.2f mm", 0);
		m_stcDistance.SetWindowTextW(strDist);

		m_bMouseDown = FALSE;
		m_iCurTool = IMAQ_NO_TOOL;
		imaqSetCurrentTool(IMAQ_NO_TOOL);
	}
}

void CDispImgDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	g_Util.DrawItem(nIDCtl, lpDrawItemStruct);

	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


//void CDispImgDlg::OnBnClickedBtnRemoveOverlay()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
////	m_bClearOverlay = TRUE;
//	imaqClearOverlay(m_pDispNiImg, NULL);
//}

float CDispImgDlg::ConvertPxlToRealDist(int pxlDX, int pxlDY)
{
	float resolX, resolY;
	m_pDoc->GetCcdResolution(&resolX, &resolY);
	resolX /= 1000.f;	// um -> mm
	resolY /= 1000.f;	// um -> mm
	int	wndX = m_nDispWd;// m_pAttach->m_nWdWnd;
	int wndY = m_nDispHt;// m_pAttach->m_nHtWnd;
	int imgX = DOWN_SAMPLE_FACTOR * m_nDispWd;
	int	imgY = DOWN_SAMPLE_FACTOR * m_nDispHt;

	float scaleX = (float)wndX / (float)imgX;
	float scaleY = (float)wndY / (float)imgY;

	float dX = resolX * (float)pxlDX / scaleX;
	float dY = resolY * (float)pxlDY / scaleY;
	
	return sqrtf(dX*dX + dY*dY);
}


void CDispImgDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	/*CDialogEx::OnClose();*/
	ShowWindow(SW_HIDE);
}


void CDispImgDlg::OnBnClickedButtonScanMoveYp()
{
	CString str;
	m_editScanStep.GetWindowTextW(str);

	double dPos = _tstof(str);

	if (m_chkWeldPointOffset.GetCheck())
	{
		MoveCCDOffset(0, dPos);
	}
	else
	{
		MovePosScaner(0, dPos);
	}
}


void CDispImgDlg::OnBnClickedButtonScanMoveYn()
{
	CString str;
	m_editScanStep.GetWindowTextW(str);

	double dPos = _tstof(str);

	if (m_chkWeldPointOffset.GetCheck())
	{
		MoveCCDOffset(0, -dPos);
	}
	else
	{
		MovePosScaner(0, -dPos);
	}
}


void CDispImgDlg::OnBnClickedButtonScanMoveXp()
{
	CString str;
	m_editScanStep.GetWindowTextW(str);

	double dPos = _tstof(str);

	if (m_chkWeldPointOffset.GetCheck())
	{
		MoveCCDOffset(dPos, 0);
	}
	else
	{
		MovePosScaner(dPos, 0);
	}
}


void CDispImgDlg::OnBnClickedButtonScanMoveXn()
{
	CString str;
	m_editScanStep.GetWindowTextW(str);

	double dPos = _tstof(str);

	if (m_chkWeldPointOffset.GetCheck())
	{
		MoveCCDOffset(-dPos, 0);
	}
	else
	{
		MovePosScaner(-dPos, 0);
	}

}


void CDispImgDlg::OnBnClickedButtonScanMoveOrg()
{
	MovePosScaner(0, 0,FALSE);
}


// 
// void CDispImgDlg::OnBnClickedBtnAcqWeldpt()
// {
// 	
// 
// 	AfxMessageBox(L"Point out the 25 laser strike locations within the work area");
// 	CStringA strPath = CStringA(m_pDoc->m_strImgPath);
// 	CStringA filename;
// 
// // 	double x1, y1;
// // 	m_pDoc->m_pScanner->GetLastPos(x1, y1);
// // 	filename.Format("snap%.3f_%.3f.png", x1,y1); 
// // 	filename = strPath + filename;
// // 	imaqWritePNGFile2(m_pDispNiImg, filename, 750, NULL, FALSE);
// // 
// // 	return;
// 	int idx = 0;
// 	double xstep = m_dCalibStep;
// 	double ystep = m_dCalibStep;
// 	double x = -m_dCalibStep *2.0f, y = m_dCalibStep * 2.0f;
// 	for (int i = 0; i < 5; i++) {
// 		x = -m_dCalibStep * 2.0f;
// 		for (int j = 0; j < 5; j++) {		
// 			MovePosScaner(x, y, FALSE);
// 			TRACE("MovePosScaner = %f,%f\n", x, y);
// 			for (int ii = 0; ii < 20; ii++) {
// 				m_pDoc->PeekAndPump();
// 				Sleep(10);
// 			}
// 
// 			filename.Format("%d.png", idx); idx++;
// 			filename = strPath + filename;
// 			imaqWritePNGFile2(m_pDispNiImg, filename, 750, NULL, FALSE);
// 			x += xstep;
// 		}
// 		y -= ystep;
// 	}
// }


void CDispImgDlg::OnBnClickedBtnCalibModify()
{
	m_iCurTool = IMAQ_POINT_TOOL;
	imaqSetCurrentTool(m_iCurTool);
}


void CDispImgDlg::OnBnClickedBtnCalibSave()
{
	FILE *pf;
	int ilens = m_pDoc->m_config.m_iCurrLens;
	CStringA strFilename = CStringA(m_pDoc->m_config.m_arCorrParam[ilens].szCcdFile);// m_pDoc->m_strConfigPath + "weldpt_cal.dat";

	//CStringA strFilename = m_pDoc->m_strConfigPath + "weldpt_cal.dat";
	fopen_s(&pf, strFilename, "w+");
	if (!pf) {
		AfxMessageBox(L"file open fail : weld point cal file ");
	}
	double xstep = m_dCalibStep;
	double ystep = m_dCalibStep;
	double x = -m_dCalibStep * 2.0f, y = m_dCalibStep * 2.0f;
	for (int i = 0; i < 5; i++) {
		x = -m_dCalibStep * 2.0f;
		for (int j = 0; j < 5; j++) {
			fprintf_s(pf, "%.1f, %.1f, %.1f, %.1f\n", m_calibX[j][i], m_calibY[j][i],x,y);
			x += xstep;
		}
		y -= ystep;
	}
	fclose(pf);
}


void CDispImgDlg::OnBnClickedBtnCalibLoad()
{
	FILE *pf;
	int ilens = m_pDoc->m_config.m_iCurrLens;
	CStringA strFilename =CStringA( m_pDoc->m_config.m_arCorrParam[ilens].szCcdFile);// m_pDoc->m_strConfigPath + "weldpt_cal.dat";
	fopen_s(&pf, strFilename, "r+");
	if (!pf) {
		AfxMessageBox(_T("file open fail : weld point cal file "), MB_OK | MB_TOPMOST);
	}
	CFileFind finder;
	BOOL bRet = finder.FindFile(m_pDoc->m_config.m_arCorrParam[ilens].szCcdFile);
	if (!bRet) {
		CString str;
		str.Format(L"the ccd calibration file not found");
		return;
	}
	char buff[200];
	char *token, *context;
	char deli[] = ", \n";
	double sx, sy;
	int idx = 0;
	double xstep = m_dCalibStep;
	double ystep = m_dCalibStep;
	double x = -m_dCalibStep * 2.0f, y = m_dCalibStep * 2.0f;


	for (int i = 0; i < 5; i++) {
		x = -m_dCalibStep * 2.0f;
		for (int j = 0; j < 5; j++) {
			fgets(buff, sizeof(char) * 200, pf);
			token = strtok_s(buff, deli, &context);
			m_calibX[j][i] = atof(token);
			token = strtok_s(NULL, deli, &context);
			m_calibY[j][i] = atof(token);
			token = strtok_s(NULL, deli, &context);
			sx =  atof(token);
 			token = strtok_s(NULL, deli, &context);
			sy =  atof(token);
			token = strtok_s(NULL, deli, &context);
			
			m_gridCal.SetData(idx, m_calibX[j][i], m_calibY[j][i], sx, sy);
			idx++;
			x += xstep;
		}
		y -= ystep;
	}
	fclose(pf);
	m_gridCal.RedrawAll();
}


void CDispImgDlg::OnBnClickedCheckWeldpointCalib()
{
	BOOL bShow = m_chkWeldPointCalib.GetCheck();
	m_gridCal.ShowWindow(bShow);
	GetDlgItem(IDC_BTN_CALIB_MODIFY)->ShowWindow(bShow);
	GetDlgItem(IDC_BTN_CALIB_SAVE)->ShowWindow(bShow);
	GetDlgItem(IDC_BTN_CALIB_LOAD)->ShowWindow(bShow);
}

LRESULT CDispImgDlg::OnUwmGridWeldCalib(WPARAM wParam, LPARAM lParam)
{
	int iRow = (int)lParam;
	int ix; int iy;
	double xstep = m_dCalibStep;
	double ystep = m_dCalibStep;
	double x = -m_dCalibStep * 2.0f, y = m_dCalibStep * 2.0f;

	switch (wParam)
	{
	case 0:
		ix = iRow % 5;
		iy = iRow / 5;
		x = x + xstep * ix;
		y = y - ystep * iy;
		MovePosScaner(x, y, FALSE);
		break;
	}
	return LRESULT();
}
void CDispImgDlg::CalcHistogram_sobel(Image* pImg, HISTO_INFO* pHistoInfo, long* lSobelVal, int nBin /*= 256*/)
{
	Image* pTmp = imaqCreateImage(IMAQ_IMAGE_U8, 7);
	imaqEdgeFilter(pTmp, m_pDispNiImg, IMAQ_EDGE_SOBEL, NULL);
	HistogramReport* report = imaqHistogram(pImg, nBin, 0.f, (float)(nBin - 1), NULL);
	if (!report)
	{
		g_Util.GetImaqLastError(L"Runtime Error");
		imaqDispose(pTmp);
		return;
	}
	long lValue = 0;
	int min = INT_MAX;
	int max = -INT_MAX;
	int gray = -1;
	for (int j = 0; j < nBin; ++j) {
		lValue += (j*report->histogram[j]);
		pHistoInfo->pHisto[j] = report->histogram[j];
		if (max < pHistoInfo->pHisto[j])
		{
			max = pHistoInfo->pHisto[j];
			gray = j;
		}
		if (min > pHistoInfo->pHisto[j])
			min = pHistoInfo->pHisto[j];
	}
	*lSobelVal = lValue/1000;
	pHistoInfo->maxCnt = max;
	pHistoInfo->grayAtMax = gray;
	pHistoInfo->minCnt = min;
	imaqDispose(report);
	imaqDispose(pTmp);
}

void CDispImgDlg::CalcHistogram(Image* pImg, HISTO_INFO* pHistoInfo, int nBin)
{
	HistogramReport* report = imaqHistogram(pImg, nBin, 0.f, (float)(nBin - 1), NULL);
	if (!report)
	{
		g_Util.GetImaqLastError(L"Runtime Error");
		return;
	}

	int min = INT_MAX;
	int max = -INT_MAX;
	int gray = -1;
	for (int j = 0; j < nBin; ++j) {
		pHistoInfo->pHisto[j] = report->histogram[j];
		if (max < pHistoInfo->pHisto[j])
		{
			max = pHistoInfo->pHisto[j];
			gray = j;
		}
		if (min > pHistoInfo->pHisto[j])
			min = pHistoInfo->pHisto[j];
	}

	pHistoInfo->maxCnt = max;
	pHistoInfo->grayAtMax = gray;
	pHistoInfo->minCnt = min;
	imaqDispose(report);
	return;
}

HENHMETAFILE CDispImgDlg::Histogram(Image* pImg, BOOL bLoaded, int min_thresh, int max_thresh)
{
	int max = 1;
	//-----------------------------------------------------------------------
	//  We need to make a metafile with the picture of the histogram.
	//  Start off by creating our device context.  We want the attributes of
	//  our DC so set up the attribute DC.  
	//-----------------------------------------------------------------------
	CMetaFileDC m_dc;
	m_dc.CreateEnhanced(NULL, NULL, NULL, NULL);
	m_dc.m_hAttribDC = m_dc.m_hDC;
	//-----------------------------------------------------------------------
	//  If we have an image, we'll do the histogram
	//-----------------------------------------------------------------------
	CPen *oldPen;
	if (pImg && bLoaded) {
		//-------------------------------------------------------------------
		//  Do the histogram.  Figure out the maximum number of pixels in a bin.
		//-------------------------------------------------------------------
		HistogramReport* report = imaqHistogram(pImg, 256, 0, 255, NULL);
		if (!report)
		{
			g_Util.GetImaqLastError(L"Runtime Error");
			return NULL;
		}

		for (int i = 0; i < 256; ++i) {
			if (max < report->histogram[i])
				max = report->histogram[i];
		}
		//-------------------------------------------------------------------
		//  Make an array of points.  The x coordinate is simply the array
		//  index.  The y coordinate is the value.  Well, not exactly.  Since
		//  the DC has 0 at the top, we'll subtract the value from the max
		//-------------------------------------------------------------------
		CPoint points[256];
		for (int i = 0; i < 256; ++i) {
			points[i].x = i;
			points[i].y = max - report->histogram[i];
		}

		imaqDispose(report);

		// BOADER
		CPen borderPen(PS_SOLID, 1, CYAN);
		oldPen = (CPen*)m_dc.SelectObject(&borderPen);
		m_dc.MoveTo(0, 0);
		m_dc.LineTo(0, (int)(0.98*max));
		m_dc.LineTo(255, (int)(0.98*max));
		m_dc.LineTo(255, 0);
		m_dc.LineTo(0, 0);
		m_dc.SelectObject(oldPen);
		DeleteObject(borderPen.m_hObject);

		CPen greenPen(PS_SOLID, 1, GREEN);
		oldPen = (CPen*)m_dc.SelectObject(&greenPen);
		for (int i = 0; i < 256; ++i)
		{
			m_dc.MoveTo(points[i].x, (int)(0.97*max));
			m_dc.LineTo(points[i].x, (int)(0.97*points[i].y));
		}
		//		dc.Polyline(points, 256);
		m_dc.SelectObject(oldPen);
		DeleteObject(greenPen.m_hObject);
	}

	if (min_thresh != -1 && max_thresh != -1)
	{
		CPen cyanPen(PS_DOT, 1, CYAN);
		oldPen = (CPen*)m_dc.SelectObject(&cyanPen);
		for (int j = 1; j < 4; j++)
		{
			m_dc.MoveTo(0, (int)(0.25*j*max));
			m_dc.LineTo(255, (int)(0.25*j*max));

			m_dc.MoveTo((int)(0.25*j * 255), 0);
			m_dc.LineTo((int)(0.25*j * 255), max);

		}
		m_dc.SelectObject(oldPen);
		DeleteObject(cyanPen.m_hObject);

		// draw min, max threshold line
		CPen dCyanPen(PS_SOLID, 1, DARKCYAN);
		oldPen = (CPen*)m_dc.SelectObject(&dCyanPen);
		m_dc.MoveTo(min_thresh, 0);
		m_dc.LineTo(min_thresh, max);

		m_dc.MoveTo(max_thresh, 0);
		m_dc.LineTo(max_thresh, max);

		m_dc.SelectObject(oldPen);
		DeleteObject(dCyanPen.m_hObject);

	}

	//-----------------------------------------------------------------------
	//  We are done drawing.  Close the metafile, and set the histogram to
	//  the metafile
	//-----------------------------------------------------------------------
	HENHMETAFILE emf = m_dc.CloseEnhanced();
	DeleteDC(m_dc.m_hDC);

	return emf;
}

LRESULT CDispImgDlg::OnWorkImgEvent(WPARAM wParam, LPARAM lParam) //DHChoi 20200623
{
	int iNiWnd = (int)wParam;
	CWorkImg::NI_EVENT pEvt = (CWorkImg::NI_EVENT) lParam;

	if (iNiWnd != m_iDispNiWindow) return 1;

	switch (pEvt)
	{
	case CWorkImg::WORKIMG_EVENT_LBUTTON_DOWN: OnLButtonDown(); break;
	case CWorkImg::WORKIMG_EVENT_LBUTTON_UP: OnLButtonUp();  break;
	case CWorkImg::WORKIMG_EVENT_MOUSE_MOVE: OnMouseMove();  break;
	}
	return 0;
}



void CDispImgDlg::OnBnClickedSnapImg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	g_pGrabImgRunner->m_bSnapImg = TRUE;
}


void CDispImgDlg::OnBnClickedCheckWeldpointOffset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bShow = m_chkWeldPointOffset.GetCheck();
	GetDlgItem(IDC_EDIT_WELD_POINT_OFFSET_X)->ShowWindow(bShow);
	GetDlgItem(IDC_EDIT_WELD_POINT_OFFSET_Y)->ShowWindow(bShow);
	GetDlgItem(IDC_BTN_SET_WELDP_OFFSET)->ShowWindow(bShow);
}


void CDispImgDlg::OnBnClickedBtnSetWeldpOffset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_dWeldPOffsetX = m_edtiWeldPOffsetX.GetValue();
	m_dWeldPOffsetY = m_edtiWeldPOffsetY.GetValue();

	UpdateCCDOffset();

	int ilens = m_pDoc->m_config.m_iCurrLens;
	m_pDoc->m_config.m_arCorrParam[ilens].dblCCDPointOffsetX = m_dWeldPOffsetX;
	m_pDoc->m_config.m_arCorrParam[ilens].dblCCDPointOffsetY = m_dWeldPOffsetY;

	m_pDoc->m_config.WriteConfig();
}


void CDispImgDlg::MoveCCDOffset(double dX, double dY)
{
	m_dWeldPOffsetX += dX;
	m_dWeldPOffsetY += dY;

	UpdateCCDOffset();
}

void CDispImgDlg::UpdateCCDOffset()
{
	m_edtiWeldPOffsetX.SetValue(m_dWeldPOffsetX);
	m_edtiWeldPOffsetY.SetValue(m_dWeldPOffsetY);
}

void CDispImgDlg::LoadCCDOffset()
{
	int ilens = m_pDoc->m_config.m_iCurrLens;

	m_bUseCCDPtnOffset = m_pDoc->m_config.m_bUseCCDPtnOffset;
	m_bUseCCD2ndPtnDisplay = m_pDoc->m_config.m_bUseCCD2ndPtnDisplay;

	if (m_bUseCCDPtnOffset)
	{
		m_dWeldPOffsetX = m_pDoc->m_config.m_arCorrParam[ilens].dblCCDPointOffsetX;
		m_dWeldPOffsetY = m_pDoc->m_config.m_arCorrParam[ilens].dblCCDPointOffsetY;
		m_edtiWeldPOffsetX.SetValue(m_dWeldPOffsetX);
		m_edtiWeldPOffsetY.SetValue(m_dWeldPOffsetY);
	}
	else
	{
		m_dWeldPOffsetX = 0;
		m_dWeldPOffsetY = 0;
	}
}

void CDispImgDlg::UpdateCCDOffsetBtn()
{
	// CCD Ptn 사용 안할 경우, 화면 모두 숨김
	if (m_bUseCCDPtnOffset == FALSE)
	{
		m_chkWeldPointOffset.ShowWindow(SW_HIDE);
		m_chkWeldPointOffset.SetCheck(FALSE);
		OnBnClickedCheckWeldpointOffset();

		return;
	}

	if (m_pDoc->m_User.GetUser() != USER_OPERATOR)
	{
		m_chkWeldPointOffset.ShowWindow(SW_SHOW);
		OnBnClickedCheckWeldpointOffset();
	}
	else
	{
		m_chkWeldPointOffset.ShowWindow(SW_HIDE);
		m_chkWeldPointOffset.SetCheck(FALSE);
		OnBnClickedCheckWeldpointOffset();
	}
}
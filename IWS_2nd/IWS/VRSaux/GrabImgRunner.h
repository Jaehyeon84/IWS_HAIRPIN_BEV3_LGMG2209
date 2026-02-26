#pragma once
#include <afxwin.h>

#include <pylon/PylonIncludes.h>
#include <pylon/gige/BaslerGigeCamera.h>
#include <stdlib.h>
#include "VRS.h"
#include "Grab2DBuff.h"

//using namespace Basler_GigETLParams;
using namespace Pylon;
using namespace GenApi;

// settings to use Basler GigE cameras
typedef Pylon::CBaslerGigECamera Camera_t;
using namespace Basler_GigECameraParams;
using namespace Basler_GigEStreamParams;

/* -- CLASS: CGrabImgRunner -- */
class CIWSDoc;
class CDispImgDlg;
class CScanImgsProc;

class CGrabImgRunner :
	public CWnd
{
public:
	CGrabImgRunner(CIWSDoc* pDoc = NULL);
	~CGrabImgRunner();

	/* */
	BOOL m_bReCover;
	CString m_strSNCCD[NUM_MAX_CAMERA];

	/* */
	UINT m_nDevices; // got to global: # of found ccds !
	CTlFactory* m_pTlFactory;
	Camera_t* m_Camera[NUM_MAX_CAMERA];
	ITransportLayer* m_pTl;
	DeviceInfoList_t m_deviceInfoList;

	/* */
	BOOL m_bCCDFound[NUM_MAX_CAMERA] = { FALSE, };

	/* main CCD */
	UINT m_nGrabRate;
	UINT m_nGrabWd;
	UINT m_nGrabHt;
	UINT m_nGrabCh;

	float m_fResolX;
	float m_fResolY;

	// for acquisition cam thread
	//BOOL	m_bCamGrabThredAlive;
	HANDLE	m_hEventGrabThreadEnd;

	// for grab & display & pump thread sync control
	BOOL	m_bSnapImg;
	HANDLE	m_hEventSnapimg;

	BOOL	m_bDoScan;
	//	BOOL	m_bLveDisplay;	-> to global

	int		m_nTotalScanFrame;
	unsigned char*	m_pSnapData;				// for one snap image (still shot)

	// scanner wait event
	HANDLE	m_hEvScannerMoveDone;

	// Grab Thread Handling
	CWinThread* m_pGrapThread;
	//BOOL m_bCameraStateOK;  => declare as global
	BOOL m_bStitchDone;

	CDispImgDlg* m_pLiveCCDDlg;
	CScanImgsProc* m_pScanImgsProc;

	/* 2nd CCD */
	int m_nFR2Want;
	int m_nCamBuff2nd;
	UINT m_nGrabRate2nd;
	UINT m_nGrabWd2nd;
	UINT m_nGrabHt2nd;
	UINT m_nGrabCh2nd;
	float m_fResolX2nd;
	float m_fResolY2nd;

	UINT m_nImgSizeWd;
	UINT m_nImgSizeHt;

	// handle for grab thread
	HANDLE	m_hEventGrabThreadEnd2nd;
	// handle for snap action
	BOOL	m_bSnapImg2nd;
	HANDLE	m_hEventSnapimg2nd;
	unsigned char*	m_pSnapData2nd;				// for one snap image (still shot)
	// Grab Thread Pointer
	CWinThread* m_pGrapThread2nd;
	//BOOL m_bCameraStateOK2nd;  //=> declare as global
	//CDispImgDlg* m_pLiveCCDDlg2nd;

public:
	void InitPylonObject();	// Pylon init process
	void SetUpCameraAndGrabBuff(Camera_t *Camera, int iCamera,
		Camera_t::StreamGrabber_t* pStreamGrabber, std::vector<CGrab2DBuff*>* pBufferList); // Pylon intermediate process
	void CleanUpCameraAndGrabBuff(Camera_t *Camera, int iCamera,
		Camera_t::StreamGrabber_t* pStreamGrabber, std::vector<CGrab2DBuff*>* pBufferList); // Pylon intermediate process
	void FinalizePylonObject(); //Pylon final process
	/* main CCD */
	static UINT GrabImgThread(LPVOID pArgs);
	void GrabimgThreadBody();
	bool Grab(Camera_t::StreamGrabber_t *pStreamGrabber, GrabResult* pGrabResult);
	BOOL ReadImageOnDisk(Image* pImg);		// simulate grab(read jpg file on disk)

//	void DispImg(int viewerID, BYTE* pImgData, int wd, int ht, BOOL bOverlay = FALSE);
	void SnapAndSaveImg(CString strPath, CString strImgName);
	unsigned char* SnapImg();// mspark

	BOOL GetCameraState() { return g_bCameraStateOK; };

	void SetLiveCCDDlgPtr(CDispImgDlg* pLiveCCDDlg) { m_pLiveCCDDlg = pLiveCCDDlg; };
	void ScanStart();
	BOOL MoveScannerTo(double x, double y);
	void ScanAndSave(CString strStitchImg, BOOL bScan, int* widthPxl, int* heightPxl,
		float* widthLength, float* heightLength);

	void SetFlagStitchImageDone(BOOL bFlag);// { m_bStitchDone = bFlag; };
	BOOL IsDoneStitchImage();// { return m_bStitchDone; };
	int SnapAndInspect(CString strFilename);

	// one shot scan thread
	static UINT OneShotScan(LPVOID pArg);

	// doc <--> scan imgs proc interface
	void ChangeScanRange(float resol_x, float resol_y,
		double start_x, double start_y, double end_x, double end_y);

	//DHChoi 20200626
	void ChangeOverlap(UINT nOverlapX, UINT nOverlapY);
	void GetRefImgCnt(double *dcx, double* dcy);
	void GetCurFullFov(float* fovX, float* fovY);


	/* 2nd CCD */
	static UINT GrabImgThread2nd(LPVOID pArgs);
	void GrabimgThreadBody2nd();
	BOOL ReadImageOnDisk2nd(Image* pImg);		// simulate grab(read jpg file on disk)
	//
	//	void DispImg(int viewerID, BYTE* pImgData, int wd, int ht, BOOL bOverlay = FALSE);
	void SnapAndSaveImg2nd(CString strPath, CString strImgName);
	unsigned char* SnapImg2nd();// mspark
	BOOL GetCameraState2nd() { return g_bCameraStateOK2nd; };

	POINTF GetScanCenter() { return m_ScanCener; };

protected:
	CIWSDoc *m_pDoc;

	POINTF	m_ScanCener;
	int m_nCamBuff;
};


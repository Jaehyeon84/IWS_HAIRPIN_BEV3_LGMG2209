#pragma once
#include <pylon/PylonIncludes.h>
#include <pylon/gige/BaslerGigeCamera.h>
#include <stdlib.h>
#include "afxwin.h"
#include "VRS.h"
#include "Grab2DBuff.h"

//using namespace Basler_GigETLParams;
using namespace Pylon;
using namespace GenApi;

// settings to use Basler GigE cameras
typedef Pylon::CBaslerGigECamera Camera_t;
using namespace Basler_GigECameraParams;
using namespace Basler_GigEStreamParams;


/* -- CLASS: CGrabImgDlg -- */
// GrabImgDlg 대화 상자
class CIWSDoc;
class CDispImgDlg;
class CScanImgsProc;
class CGrabImgDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGrabImgDlg)

public:
	CGrabImgDlg(CIWSDoc* pDoc = NULL, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CGrabImgDlg();

	/* */
	CTlFactory* TlFactory;
	Camera_t* m_Camera[2];
	ITransportLayer *pTl;
	DeviceInfoList_t devices;
	/* main CCD */
	UINT m_nGrabRate;
	UINT m_nGrabWd;
	UINT m_nGrabHt;
	UINT m_nGrabCh;

	float m_fResolX;
	float m_fResolY;

	HANDLE	m_hEventInitDone;
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
	int m_nCamBuf2nd;
	UINT m_nGrabRate2nd;
	UINT m_nGrabWd2nd;
	UINT m_nGrabHt2nd;
	UINT m_nGrabCh2nd;
	float m_fResolX2nd;
	float m_fResolY2nd;

	// handle for grab thread
	HANDLE	m_hEventGrabThreadEnd2nd;
	// handle for snap action
	BOOL	m_bSnapImg2nd;
	HANDLE	m_hEventSnapimg2nd;
	unsigned char*	m_pSnapData2nd;				// for one snap image (still shot)
// Grab Thread Pointer
	CWinThread* m_pGrapThread2nd;
	//BOOL m_bCameraStateOK2nd;  //=> declare as global
	CDispImgDlg* m_pLiveCCDDlg2nd;

public:
	void AcqProcessStart();
	BOOL Is1stGrabThreadInitDone();
	/* main CCD */
	static UINT GrabImgThread(LPVOID pArgs);
	void GrabimgThreadBody();
	EGrabStatus Grab(Camera_t::StreamGrabber_t *pStreamGrabber, GrabResult* pGrabResult);
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
	EGrabStatus Grab2nd(Camera_t::StreamGrabber_t *pStreamGrabber, GrabResult* pGrabResult);
	BOOL ReadImageOnDisk2nd(Image* pImg);		// simulate grab(read jpg file on disk)
	//
	//	void DispImg(int viewerID, BYTE* pImgData, int wd, int ht, BOOL bOverlay = FALSE);
	void SnapAndSaveImg2nd(CString strPath, CString strImgName);
	unsigned char* SnapImg2nd();// mspark
	BOOL GetCameraState2nd() { return g_bCameraStateOK2nd; };

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GRAB_DLG };
#endif
	POINTF GetScanCenter() { return m_ScanCener; };
protected:
	CIWSDoc *m_pDoc;
	
	POINTF	m_ScanCener;
	int m_nCamBuf;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
//	afx_msg void OnDestroy();
};

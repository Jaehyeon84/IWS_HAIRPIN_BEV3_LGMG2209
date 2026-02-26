#pragma once
/* -- CLASS: CGrab2DBuffer -- */
#include <pylon/PylonIncludes.h>
// 박다현
#include <pylon/gige/PylonGigEIncludes.h>
//
#include <pylon/gige/BaslerGigeCamera.h>
#include <stdlib.h>
#include "afxwin.h"
#include "VRS.h"

//using namespace Basler_GigETLParams;
using namespace Pylon;
using namespace GenApi;

// settings to use Basler GigE cameras
//typedef Pylon::CBaslerGigECamera Camera_t;
//박다현
typedef Pylon::CBaslerGigEInstantCamera Camera_t;

// sjyi 2023.07.16 2D Float Vector 선언 
typedef std::vector<std::vector<float>> VEC_2D_FLOAT;


using namespace Basler_GigECameraParams;
using namespace Basler_GigEStreamParams;

class CGrab2DBuffer
{
public:
	CGrab2DBuffer(const size_t ImageSize);
	~CGrab2DBuffer();
	uint8_t* GetBufferPointer(void) { return m_pBuffer; }
	StreamBufferHandle GetBufferHandle(void) { return m_hBuffer; }
	void SetBufferHandle(StreamBufferHandle hBuffer) { m_hBuffer = hBuffer; };

protected:
	uint8_t *m_pBuffer;
	StreamBufferHandle m_hBuffer;
};

/* -- CLASS: CGrabImgDlg -- */

// GrabImgDlg 대화 상자
class CIWSDoc;
class CDispImgDlg;
class CScanImgsProc;
class CGigECamera;

class CGrabImgDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGrabImgDlg)

public:
	CGrabImgDlg(CIWSDoc* pDoc = NULL, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CGrabImgDlg();

	UINT m_nGrabRate;
	UINT m_nGrabWd;
	UINT m_nGrabHt;
	UINT m_nGrabCh;

	float m_fResolX;
	float m_fResolY;

	// for acquisition cam thread
//	BOOL	m_bCamGrabThredAlive; -> to global
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
	//
	CDispImgDlg* m_pLiveCCDDlg;
	CScanImgsProc* m_pScanImgsProc;

	//
public:
	// 2D camera grab thread
	static UINT GrabImgThread(LPVOID pArgs);
	void Grabimg();
	//EGrabStatus Grab(Camera_t::StreamGrabber_t *pStreamGrabber, GrabResult* pGrabResult);
	void Grabimg_Simul();					// simulate camera
	BOOL ReadImageOnDisk(Image* pImg);		// simulate grab(read jpg file on disk)
	//
//	void DispImg(int viewerID, BYTE* pImgData, int wd, int ht, BOOL bOverlay = FALSE);
	void SnapAndSaveImg(CString strPath, CString strImgName);
	unsigned char* SnapImg();// mspark
	//
	BOOL GetCameraState() { return g_bCameraStateOK; };

	//
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
	static UINT ReScan(LPVOID pArg);

	// doc <--> scan imgs proc interface
	void ChangeScanRange(float resol_x, float resol_y,
		double start_x, double start_y, double end_x, double end_y);

	//DHChoi 20200626
	void ChangeOverlap(UINT nOverlapX, UINT nOverlapY);
	void GetRefImgCnt(double *dcx, double* dcy);

	// 박다현
	CGigECamera* m_pCamera;
	void GigECameraConnect();

	long m_imageWidth;
	long m_imageHeight;
	long m_imageSize;

	BYTE* m_pbyVisionScanBlockImage;
	BYTE* m_pbyLiveImage;

	//Camera_t *m_pCamera;
	//bool Grab(long count);
	//void VisionScanGrab();
	//virtual void OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult);


	void GetCurFullFov(float* fovX, float* fovY);

	void SetDoScan(BOOL bDoScan);

	// sjyi 2023.07.07 스캔결과 검증 Process 추가
	//void RescanStart();		 // 재 스캔 시작 함수
	//void ScanModify();		 // 재스캔 후, 부분 이미지 수정(실질적인 재 스캔 함수)
	//void SetFlagRescanImageDone(BOOL bFlag);	// 재스캔 종료 확인 플래그 설정 함수
	//BOOL IsDoneRescanImage(); // 재스캔 종료 확인 플래그
	//BOOL CheckResultImage(int nRcpNo = 1); // 스캔 결과 확인 함수
	//void LoadRefImage(int nRcpNo); // Recipe Read시, 기준 이미지 등록하기

	//// 기준 이미지 등록 함수
	//void CalcStatFromRefImages(int nFrameX, int nFrameY,
	//	std::vector<std::vector<float>>& vecMean,
	//	std::vector<std::vector<float>>& vecStdDev, CString strRefStatorPath);
	//
	//// 디렉토리 내, 이미지 파일 찾는 함수
	//std::vector<CString> read_diectory(CString strPath); 

	//// 스캔결과 체크 함수
	//std::vector<int> DetectAnorm(Image* pImg, int nFrameX, int nFrameY,
	//	std::vector<std::vector<float>>& vecMean,
	//	std::vector<std::vector<float>>& vecStdDev,
	//	float meanLimit, float stdLimit, BOOL bSaveOverlayImg, CString strSaveFolderPath);

	//// 결과 확인용 오버레이 이미지 저장함수
	//int SaveJPEGIncludeOverlay(Image* pOverlayImg, CStringA strPath);

	//// sjyi 2023.07.16 함수 인자 선언한 2D Float Vector 변수로 변경 
	//// 기준 이미지 등록 함수(7/14 신규 추가)
	//void CalcBoundContiFromRefImages(int nFrameX, int nFrameY,
	//	VEC_2D_FLOAT& vecBoundContiX,
	//	VEC_2D_FLOAT& vecBoundContiY,
	//	VEC_2D_FLOAT& vecBoundContiX_Std,
	//	VEC_2D_FLOAT& vecBoundContiY_Std,
	//	CString strRefStatorPath);

	//// sjyi 2023.07.16 함수 인자 선언한 2D Float Vector 변수로 변경 
	//// 스캔결과 체크 함수(7/14 추가)
	//std::vector<int> DetectAnormByBoundConti(Image* pImg, int nFrameX, int nFrameY,
	//	VEC_2D_FLOAT vecBoundContiX,
	//	VEC_2D_FLOAT vecBoundContiY,
	//	VEC_2D_FLOAT vecBoundContiX_Std,
	//	VEC_2D_FLOAT vecBoundContiY_Std,
	//	float meanLimitX, float meanLimitY, float stdLimitX, float stdLimitY,
	//	BOOL bSaveOverlayImg, CString strSaveFolderPath);
	//BOOL IsRescan(int nBlkIndexX, int nBlkIndexY);


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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

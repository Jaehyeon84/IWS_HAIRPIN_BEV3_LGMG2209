// GrabImgDlg.cpp: 구현 파일
//

#include "../stdafx.h"
#include "../IWS.h"
#include "../IWSDoc.h"
#include "Queue.h"

#include "afxdialogex.h"
#include "GrabImgDlg.h"
#include "DispImgDlg.h"
#include "ScanImgsProc.h"
#include "Utility.h"

#include "WorkImg.h"
#include "GigE/GigECamera.h"


/* Init Global Variable */
CRITICAL_SECTION G_cs;

//int			g_nCPUCore = 0;
double		g_AvgFrmRate = 0;
BOOL		g_bLiveDisplay = FALSE;
BOOL		g_bGrabThreadAlive = FALSE;
BOOL		g_bCameraStateOK = FALSE;

ImageType	g_ImgType = IMAQ_IMAGE_U8;

CString		g_strLogPath = (L"");
CString		g_strSimulImagePath = (L"");

CFont		g_textFont;
HBRUSH		g_hBrushBk = CreateSolidBrush(BACK);
HANDLE		g_hEventPushQueueDone = ::CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE		g_hEventSrtCopyDispData = ::CreateEvent(NULL, FALSE, FALSE, NULL);// for data copy for display

/* -- CLASS: CGrab2DBuffer -- */
// Constructor allocates the image buffer
CGrab2DBuffer::CGrab2DBuffer(const size_t ImageSize):
	m_pBuffer(NULL)
{
	m_pBuffer = new uint8_t[ImageSize];
	if (NULL == m_pBuffer)
	{
		GenICam::GenericException e("Not enough memory to allocate image buffer", __FILE__, __LINE__);
		throw e;
	}
}

// Freeing the memory
CGrab2DBuffer::~CGrab2DBuffer()
{
	if (NULL != m_pBuffer)
		delete[] m_pBuffer;
}


// GrabImgDlg 대화 상자

IMPLEMENT_DYNAMIC(CGrabImgDlg, CDialogEx)

CGrabImgDlg::CGrabImgDlg(CIWSDoc *pDoc, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GRAB_DLG, pParent)
{

	m_pDoc = pDoc;

	/* get necessary path */
	CString strExePath = m_pDoc->m_strExePath + L"\\";
	g_strSimulImagePath = strExePath + SIMUL_IMG_PATH;
	g_strLogPath = strExePath + LOG_PATH;

	m_pDoc->GetCcdInfo(&m_nGrabWd, &m_nGrabHt, &m_nGrabCh);

	m_nCamBuf = NUM_INTERNAL_CAMBUF;

	// for grab thread
	g_bGrabThreadAlive = false;
	m_hEventGrabThreadEnd = CreateEvent(NULL, FALSE, FALSE, NULL);

	// for specific grab task
	m_bSnapImg = FALSE;
	g_bLiveDisplay = FALSE;
	m_bDoScan = FALSE;

	// for grab & display sync control
	m_hEventSnapimg = CreateEvent(NULL, FALSE, FALSE, NULL);

	int acqImgSize = m_nGrabHt * m_nGrabWd * m_nGrabCh;
	m_pSnapData = new unsigned char[acqImgSize];
	memset(m_pSnapData, 0, acqImgSize * sizeof(unsigned char));

	// scanner motion done check event
	m_hEvScannerMoveDone = CreateEvent(NULL, FALSE, FALSE, NULL);
	//
	float fMaxScanRangeX, fMaxScanRangeY;	
	m_pDoc->GetWorkRange(&fMaxScanRangeX, &fMaxScanRangeY); // get maximum allowable scanner range

	m_pScanImgsProc = new CScanImgsProc(m_nGrabWd, m_nGrabHt, m_nGrabCh, m_fResolX, m_fResolY, // Camera
		fMaxScanRangeX, fMaxScanRangeY);	// Scanner

	//DHChoi 20200626
	int iCurLensIdx = m_pDoc->m_config.m_iCurrLens;
	BOOL bUseCalib = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].bUseCalib;
	int  iOverlay = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iVSOverlap;
	m_pScanImgsProc->UseCalibration(FALSE);
	if (bUseCalib)
	{
		CStringA strFilename = CStringA(m_pDoc->m_config.m_arCorrParam[iCurLensIdx].szCalibDir);
		if (strFilename.GetLength() > 1)
		{
			m_pScanImgsProc->UseCalibration(TRUE);
			CString sCalibPath = CString(strFilename);
			int nBlockX = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibX;
			int nBlockY = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibY;
			if (m_pScanImgsProc->SetCalibData(sCalibPath, nBlockX, nBlockY) < 0)
			{
				m_pScanImgsProc->UseCalibration(FALSE);
			}
		}
	}




	double sx, sy, ex, ey;
	m_pDoc->m_config.GetScanRng(&sx, &sy, &ex, &ey);
	
	float resolX, resolY;
	m_pDoc->GetCcdResolution(&resolX, &resolY);

	//DHChoi 20200626
	//m_pScanImgsProc->ChangeScanRange(resolX, resolY, sx, sy, ex, ex, OVERLAP_X, OVERLAP_Y);
	m_pScanImgsProc->ChangeOverlap(iOverlay, iOverlay);
	//m_pScanImgsProc->ChangeScanRange(resolX, resolY, sx, sy, ex, ex);
	m_pScanImgsProc->ChangeScanRange(resolX, resolY, sx, sy, ex, ey); // sjyi 2023.07.30 스캔영역 관련 오류 수정(중요)

	// 카메라 클래스 신규 추가로 인한 내용 추가 
	// S/W 비정상 종료시 제대로 카메라를 close 안했을 경우 에도 5초 뒤면 카메라가 정상인 상태로 되돌려 놓는 코드
	_putenv("PYLON_GIGE_HEARTBEAT=5000");

	m_pCamera = nullptr;
	m_pbyVisionScanBlockImage = nullptr;
	m_pbyLiveImage = nullptr;
	
	GigECameraConnect();
	m_pGrapThread = AfxBeginThread(GrabImgThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);
	
	
	g_bCameraStateOK = FALSE;

	InitializeCriticalSection(&G_cs);

//	m_bCheckRescanDone = FALSE;

	// Reference Image 등록
}

CGrabImgDlg::~CGrabImgDlg()
{
	::CloseHandle(m_hEventSnapimg);
	
	g_bGrabThreadAlive = FALSE;
	
	if(!g_bCameraStateOK) // 카메라 강제 종료
		Pylon::PylonTerminate();

	DWORD dwRes = WaitForSingleObject(m_hEventGrabThreadEnd, 3000);		// wait 3 second
	if (dwRes == WAIT_TIMEOUT)	// Grab Thread 강제 종료
	{
		TerminateThread(m_pGrapThread->m_hThread, 0);
		Sleep(100);
	}

	::CloseHandle(m_hEventGrabThreadEnd);

	::CloseHandle(m_hEvScannerMoveDone);
	::CloseHandle(g_hEventPushQueueDone);
	::CloseHandle(g_hEventSrtCopyDispData);

	// clean up snap image buffer
	delete[] m_pSnapData;
	m_pSnapData = NULL;

	delete m_pScanImgsProc;

	// 카메라 클래스 변경을 인한 내용 추가 
	if(m_pbyVisionScanBlockImage != nullptr)
	{
		delete[] m_pbyVisionScanBlockImage;
		m_pbyVisionScanBlockImage = nullptr;
	}

	if(m_pbyLiveImage != nullptr)
	{
		delete[] m_pbyLiveImage;
		m_pbyLiveImage = nullptr;
	}

	if(m_pCamera != nullptr)
	{
		if(m_pCamera->IsOpen() == TRUE)
		{
			m_pCamera->Stop();

			delete m_pCamera;
			m_pCamera = nullptr;
		}
	}

	DeleteCriticalSection(&G_cs);
}

void CGrabImgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGrabImgDlg, CDialogEx)
//ON_WM_DESTROY()
ON_WM_TIMER()
END_MESSAGE_MAP()


// GrabImgDlg 메시지 처리기

// 카메라 클래스 추가로 인한 함수 추가(카메라 연결)
void CGrabImgDlg::GigECameraConnect()
{
	Pylon::PylonTerminate();
	Pylon::PylonAutoInitTerm autoInitTerm;

	CTlFactory& TlFactory = CTlFactory::GetInstance();

	// Create the transport layer object needed to enumerate or
	// create a camera object of type Camera_t::DeviceClass()
	ITransportLayer *pTl = TlFactory.CreateTl(Camera_t::DeviceClass());

	// Exit application if the specific transport layer is not available
	if (!pTl)
	{
		g_Util.Logging(L"Failed to create transport layer!");
		g_bCameraStateOK = FALSE;
		return;
	}

	// Get all attached cameras and exit application if no camera is found
	DeviceInfoList_t devices;
	if (0 == pTl->EnumerateDevices(devices))
	{
		g_Util.Logging(L"No camera present!");
		g_bCameraStateOK = FALSE;
		return;
	}

	CString strAddr = L"";
	strAddr = (CString)devices[0].GetAddress();
	
	m_pCamera = new CGigECamera(strAddr, 0);

	if (m_pCamera != nullptr)
	{
		if (m_pCamera->Open() == true)
		{
			m_pCamera->SetMode(MODE_CONTINUE);

			m_imageWidth = m_pCamera->GetWidth();
			m_imageHeight = m_pCamera->GetHeight();
			m_imageSize = m_imageWidth * m_imageHeight;

			m_pbyVisionScanBlockImage = new BYTE[m_imageSize];
			m_pbyLiveImage = new BYTE[m_imageSize];
			
		}
		else
		{
			if(m_pCamera != nullptr)
			{
				delete m_pCamera;
				m_pCamera = nullptr;
			}
		}
	}
}


UINT CGrabImgDlg::GrabImgThread(LPVOID pArgs)
{
	CGrabImgDlg* me = (CGrabImgDlg*)pArgs;
	me->Grabimg();
//	me->Grabimg_Simul();
	return 0;
}


void CGrabImgDlg::Grabimg()
{
// 카메라 클래스 변경으로 인해 아래 내용 모두 수정 ------------>
//	Pylon::PylonTerminate();
//	Sleep(200);
//	g_Util.Logging(L"-- Basler Acq. Thread Started ! -- \n");
//#ifdef _DEBUG
	/*
	* When debugging a pylon application using a GigE camera, heartbeat timeouts may be encountered.
	* Any pylon application is required to send special network packets, called heartbeat packets,
	* to the camera at regular intervals to hold the connection open. If the camera does not receive
	* these heartbeats packets for some predefined time (the 'heartbeat timeout'), it will assume that
	* the connection is broken, and will cease to accept further commands from the application.
	*
	* This is not a problem if the application is running normally, as pylon will take care of generating
	* the heartbeat. During debugging, however, the debugger may stop the application, including all its
	* threads, when a breakpoint is hit or single-stepping is done. This will also stop the heartbeat.
	*
	* The problem can be worked around by extending the heartbeat timeout during debugging. There are two ways
	* to achieve this: either by setting an environment variable named 'PYLON_GIGE_HEARTBEAT' (see documentation),
	* or by setting the transport layer's 'HeartbeatTimeout' parameter.
	*
	* The code below demonstrates the first method by setting a heartbeat timeout of 5 minutes. In a real-world
	* program it might be more appropriate to check whether PYLON_GIGE_HEARTBEAT is already set, and leave
	* it untouched if this is the case.
	*/

	//_putenv("PYLON_GIGE_HEARTBEAT=30000");  /* set to 300000 ms = 5 m */

//#endif
//	_putenv("PYLON_GIGE_HEARTBEAT=5000");  /* set to 300000 ms = 5 m */

											// Automagically call PylonInitialize and PylonTerminate to ensure the pylon runtime system
											// is initialized during the lifetime of this object
//	Pylon::PylonTerminate();
//	Pylon::PylonAutoInitTerm autoInitTerm;
//	bool bPaint = true;
//	try
//	{
//		// Get the transport layer factory
//		CTlFactory& TlFactory = CTlFactory::GetInstance();
//
//		// Create the transport layer object needed to enumerate or
//		// create a camera object of type Camera_t::DeviceClass()
//		ITransportLayer *pTl = TlFactory.CreateTl(Camera_t::DeviceClass());
//
//		// Exit application if the specific transport layer is not available
//		if (!pTl)
//		{
//			g_Util.Logging(L"Failed to create transport layer!");
//			g_bCameraStateOK = FALSE;
//			return;
//		}
//
//		// Get all attached cameras and exit application if no camera is found
//		DeviceInfoList_t devices;
//
//#define TIME_OUT 10000
//		long told = clock();
//		BOOL bRes = FALSE;
//		while (bRes == FALSE) {
//	
//			if (pTl->EnumerateDevices(devices) != 0) {
//				bRes = TRUE;
//			}
//
//			if (clock() - told > TIME_OUT) {
//				_tracer(TRACE_NORMAL, 1, _T("No camera present"));
//				bRes = FALSE;
//				break;
//			}
//			Sleep(500);
//		}
//		//if (0 == pTl->EnumerateDevices(devices))
//		if (bRes == FALSE)
//		{
//			g_Util.Logging(L"No camera present!");
//			g_bCameraStateOK = FALSE;
//			return;
//		}
//
//		// Create the camera object of the first available camera.
//		// The camera object is used to set and get all available
//		// camera features.
//		Camera_t Camera(pTl->CreateDevice(devices[0]));
//		INodeMap* pTlNodeMap = Camera.GetTLNodeMap();
//		if (NULL == pTlNodeMap)
//		{
//			g_Util.Logging(L"This camera class doesn’t provide a transport layer nodemap");
//			g_bCameraStateOK = FALSE;
//			return;
//		}
//		Camera_t::TlParams_t TlParams = pTlNodeMap;
//
//		TlParams.HeartbeatTimeout.SetValue(30000); // 30 sec timeout
//		TlParams.HeartbeatTimeout.SetValue(3000);
//												   // Open the camera
//		Camera.Open();
//		int nTempBinning = 1;
//		Camera.BinningHorizontal.SetValue((size_t)nTempBinning);
//		Camera.BinningVertical.SetValue((size_t)nTempBinning);
//		Camera.Height.SetValue(m_nGrabHt / nTempBinning);
//		Camera.Width.SetValue(m_nGrabWd / nTempBinning);
//		//		Camera.AcquisitionFrameRateAbs.SetValue((double)m_nFrameRate);
//		//		Camera.ExposureTimeRaw.SetValue((size_t)9000);
//
//				// Get the first stream grabber object of the selected camera
//		Camera_t::StreamGrabber_t StreamGrabber(Camera.GetStreamGrabber(0));
//
//		// Open the stream grabber
//		StreamGrabber.Open();
//
//		// Set the image format and AOI
//		Camera.PixelFormat.SetValue(PixelFormat_Mono8);
//		Camera.CenterX.SetValue(0);
//		Camera.CenterY.SetValue(0);
//		Camera.OffsetX.SetValue(0);
//		Camera.OffsetY.SetValue(0);
//
//		//Disable acquisition start trigger if available
//		{
//			GenApi::IEnumEntry* acquisitionStart = Camera.TriggerSelector.GetEntry(TriggerSelector_AcquisitionStart);
//			bool available = GenApi::IsAvailable(acquisitionStart);
//			if (acquisitionStart && GenApi::IsAvailable(acquisitionStart))
//			{
//				Camera.TriggerSelector.SetValue(TriggerSelector_AcquisitionStart);
//				Camera.TriggerMode.SetValue(TriggerMode_Off);
//			}
//		}
//
//		//Disable frame start trigger if available
//		{
//			GenApi::IEnumEntry* frameStart = Camera.TriggerSelector.GetEntry(TriggerSelector_FrameStart);
//			if (frameStart && GenApi::IsAvailable(frameStart))
//			{
//				Camera.TriggerSelector.SetValue(TriggerSelector_FrameStart);
//				Camera.TriggerMode.SetValue(TriggerMode_Off);
//			}
//		}
//
//		//Set acquisition mode
//		Camera.AcquisitionMode.SetValue(AcquisitionMode_Continuous);
//
//		// Get the image buffer size
//		const size_t ImageSize = (size_t)(Camera.PayloadSize.GetValue());
//
//		// We won't use image buffers greater than ImageSize
//		StreamGrabber.MaxBufferSize.SetValue(ImageSize);
//
//		// We won't queue more than c_nBuffers image buffers at a time
//		StreamGrabber.MaxNumBuffer.SetValue((uint32_t)m_nCamBuf);
//
//		// Allocate all resources for grabbing. Critical parameters like image
//		// size now must not be changed until FinishGrab() is called.
//		StreamGrabber.PrepareGrab();
//
//		// Buffers used for grabbing must be registered at the stream grabber.
//		// The registration returns a handle to be used for queuing the buffer.
//		std::vector<CGrab2DBuffer*> BufferList;
//		for (int i = 0; i < m_nCamBuf; ++i)
//		{
//			CGrab2DBuffer *pGrabBuffer = new CGrab2DBuffer(ImageSize);
//			pGrabBuffer->SetBufferHandle(StreamGrabber.RegisterBuffer(
//				pGrabBuffer->GetBufferPointer(), ImageSize));
//
//			// Put the grab buffer object into the buffer list
//			BufferList.push_back(pGrabBuffer);
//		}
//
//		for (std::vector<CGrab2DBuffer*>::const_iterator x = BufferList.begin(); x != BufferList.end(); ++x)
//		{
//			// Put buffer into the grab queue for grabbing
//			StreamGrabber.QueueBuffer((*x)->GetBufferHandle(), NULL);
//		}
//
//		// Let the camera acquire images continuously ( Acquisiton mode equals
//		// Continuous! )
//		Camera.AcquisitionStart.Execute();
//
//		//	g_nExposureTime = nExposureTime = (int)Camera.ExposureTimeRaw.GetValue();
//		//	g_nGainRaw = nGainRaw = (int)Camera.GainRaw.GetValue();
//		//
//		UINT nPushCnt = 0;
//		g_bGrabThreadAlive = true;
//
//		// to calc average frame rate
//		int accuFrameCnt = 0;
//		double timeElasped;
//		LARGE_INTEGER freq, timeBegin, timeEnd;
//		QueryPerformanceFrequency(&freq);
//
//		::SetEvent(g_hEventSrtCopyDispData);
//		DWORD ret;
//		GrabResult grabResult;
//		uint8_t *pImageBuffer;
//
//		EGrabStatus grab_status;
//		unsigned char *s, *d;
//
//		BOOL bGrabTried = FALSE;
//		BOOL bInitDisplay = FALSE;
//
// 카메라 클래스 변경으로 인해 아래 내용 모두 수정 <------------

	int nPushCnt = 0;
	BOOL bStatus = FALSE;

	g_bGrabThreadAlive = TRUE;

	if(m_pCamera != nullptr)
	{
		bStatus = m_pCamera->Grab(m_pbyLiveImage, 1);// m_pScanImgsProc->m_nBlockCntTotal);
	}

	while (g_bGrabThreadAlive)
	{
		// sjyi 20230607 comment Vision Scan 관련 내용 -------------->
		if (m_bDoScan)	// Vision Scan 활성화 되어 있으면
		{
			// 카메라 클래스 인스턴스 생성여부 확인 
			if(m_pCamera == nullptr)
			{
				continue;
			}

			// 스캐너 이동완료 이벤트 대기, Timeout 시간 : 5밀리초
			DWORD ret = ::WaitForSingleObject(m_hEvScannerMoveDone, 5);

			// 이동완료되면 이미지 획득(Grab)
			//grab_status = Grab(&StreamGrabber, &grabResult);
			m_pCamera->SetIsGrabDone(FALSE);
			
			while (1)
			{
				if (m_pCamera->IsGrabDone())
				{
					break;
				}
				Sleep(1);
			}

			memcpy(m_pbyVisionScanBlockImage, m_pCamera->GetGrabData(), m_imageSize);

			m_pCamera->SetIsGrabDone(FALSE);


			// Grab 시도 Flag TRUE로 설정
			//bGrabTried = TRUE;

			// 스캐너 이동완료 이벤트 대기 Return값 확인
			switch (ret)
			{
			case WAIT_TIMEOUT:		// Timeout 발생 : 아무것도 안함.
				//						Sleep(5);
				break;
			case WAIT_OBJECT_0:		// 정상 발생
				//if (grab_status == Grabbed) // 이미지 획득 결과 확인, 정상이면
				if (bStatus == TRUE) // 이미지 획득 결과 확인, 정상이면
				{
					// 카메라 상태 OK, TRUE로 설정
					g_bCameraStateOK = TRUE;
					//// Grab된 이미지 버퍼(메모리 값) 가져오기
					//pImageBuffer = (uint8_t *)grabResult.Buffer();
					// Grab된 이미지 버퍼(메모리 값)을 스캔 큐(m_pScanDataQue)에 추가
					//m_pScanImgsProc->m_pScanDataQue->Push(pImageBuffer);
					m_pScanImgsProc->m_pScanDataQue->Push(m_pbyVisionScanBlockImage);

					// PushQue 완료 이벤트 발생
					SetEvent(g_hEventPushQueueDone);

					// Push Count 값 0인지 확인(최초 그랩인지 확인)
					if (nPushCnt == 0)
						::SetEvent(g_hEventPopThreadSrt); // 최초 그랩이면 PopThreadSrt 이벤트 발생

					// Push Count 증가
					++nPushCnt;
				}
				else // 이미지 획득 결과 확인, 정상이 아니면
				{
					// 카메라 상태 NG, FALSE로 설정
					g_bCameraStateOK = FALSE;

					// Logging
					g_Util.Logging(L"Grab in Scan Process Failed !");
				}

				// Push Count가 전체 블럭 사이즈보다 크거나 같으면, 이미지를 다 얻었으므로 스캔 종료
//					if (nPushCnt >= m_pScanImgsProc->m_nBlockX * m_pScanImgsProc->m_nBlockY)
				//_tracer(TRACE_NORMAL, 9999, L"[GrabImg()] nPushCnt : %d, %d", nPushCnt, m_pScanImgsProc->m_nBlockCntTotal);
				if (nPushCnt >= m_pScanImgsProc->m_nBlockCntTotal)
				{
					_tracer(TRACE_NORMAL, 9999, L"[GrabImg()] nPushCnt : %d", nPushCnt);

					// m_bDoScan Flag FALSE로 설정(스캔 종료)
					m_bDoScan = FALSE;

					// 타이머 실행
					SetTimer(0, 50, NULL);
					
					// Push Count 초기화
					nPushCnt = 0;

					// 아래 내용 불필요
					//// Grab 시도 Flag가 TRUE이면
					//if (bGrabTried)
					//{
					//	// input Queue에 버퍼를 인큐
					//	StreamGrabber.QueueBuffer(grabResult.Handle(), NULL);
					//	// Grab 시도 Flag 초기화
					//	bGrabTried = FALSE;
					//}
				}
				break;
			}
		}
		else
		{
			// Vision Scan Flag가 비활성화 되어 있으면, PushCount 초기화
			nPushCnt = 0;
		}

//		if (g_bLiveDisplay)
//			{
//			if (!bInitDisplay)
//				{
//					QueryPerformanceCounter(&timeBegin);
//					bInitDisplay = TRUE;
//				}
//
//				ret = ::WaitForSingleObject(g_hEventSrtCopyDispData, 5);
//				if (!m_bDoScan)
//				{
//					grab_status = Grab(&StreamGrabber, &grabResult);
//					bGrabTried = TRUE;
//				}

//				switch (ret) // display 용 m_pDisplayimgData에 영상을 1/4로 줄여 복사
//				{
//				case WAIT_TIMEOUT:
//					//					Sleep(5);
//					break;
//				case WAIT_OBJECT_0:
//					if (grab_status == Grabbed)
//					{
//						g_bCameraStateOK = TRUE;
//
//						pImageBuffer = (uint8_t *)grabResult.Buffer();
//						s = pImageBuffer;	// address 초기화
//						d = m_pLiveCCDDlg->m_pDisplayimgData;  // address 초기화
//						if (g_ImgType == IMAQ_IMAGE_U8)
//						{
//							for (UINT j = 0; j < m_nGrabHt; j += 2)
//							{
//								for (UINT i = 0; i < m_nGrabWd; i += 2)
//								{
//									*d = *s;
//									d++;
//									s += 2;
//								}
//								s += m_nGrabWd;
//							}
//						}
//
//						else
//							g_Util.Logging(L"No support format !\n");
//
//					}
//					else
//					{
//						g_bCameraStateOK = FALSE;
//						memset(m_pLiveCCDDlg->m_pDisplayimgData, 0, int(m_nGrabWd*m_nGrabHt / 4) * sizeof(BYTE));
//						g_Util.Logging(L"Grab in Live Display Process Failed !");
//					}

//					// display on Ni window
//					//::PostMessage(m_pDispImgProc->m_hWnd, UWM_DISPIMG, 0, 0);
//					::SetEvent(g_hEventDisplayStart);
//					break;
//				} //switch loop
//
//				// calc acq frame rate
//				QueryPerformanceCounter(&timeEnd);
//				timeElasped = ((double)(timeEnd.QuadPart - timeBegin.QuadPart) / (double)freq.QuadPart); // in sec
//				g_AvgFrmRate = (double)(++accuFrameCnt) / timeElasped;
//				if (accuFrameCnt > 1e+6) // counter & timer reset
//				{
//					accuFrameCnt = 0;
//					timeElasped = 0;
//					QueryPerformanceCounter(&timeBegin);
//				}
//			}
//			else
//			{
//				Sleep(100); // doing nothing, so let a thread run slowly
//				if (bInitDisplay)
//					bInitDisplay = FALSE;
//			}

		if (m_bSnapImg)
		{
//			if (!bGrabTried)
//			{
//				grab_status = Grab(&StreamGrabber, &grabResult); //DHChoi 20200630
//				bGrabTried = TRUE;
//			}
//			if (grab_status == Grabbed)
			if (bStatus == TRUE)
			{
				g_bCameraStateOK = TRUE;
				//pImageBuffer = (uint8_t *)grabResult.Buffer();
				//memcpy(m_pSnapData, pImageBuffer, sizeof(unsigned char) * ImageSize);
				memcpy(m_pSnapData, m_pbyVisionScanBlockImage, sizeof(unsigned char) * m_imageSize);
				m_bSnapImg = FALSE;
			}
			else
			{
				g_bCameraStateOK = FALSE;
				memcpy(m_pSnapData, 0, sizeof(unsigned char) * m_imageSize);
				g_Util.Logging(L"Grab in Snap Process Failed !\n");
			}

			::SetEvent(m_hEventSnapimg);
		}

	}
			// g_Util.Logging(L"-----%d ---- %f----%f-\n ", accuFrameCnt, timeElasped, g_AvgFrmRate);
			// Reuse the buffer for grabbing the next image
			//if (n < c_ImagesToGrab - c_nBuffers)
//			if (bGrabTried)
//			{
//				StreamGrabber.QueueBuffer(grabResult.Handle(), NULL);
//				bGrabTried = FALSE;
//			}
//		}
//
//		// Get the pending buffer back (You are not allowed to deregister
//		// buffers when they are still queued)
//		StreamGrabber.CancelGrab();
//
//		// Get all buffers back
//		for (GrabResult r; StreamGrabber.RetrieveResult(r););
//
//		// Stop acquisition
//		Camera.AcquisitionStop.Execute();
//
//		// Clean up
//		// You must deregister the buffers before freeing the memory
//		for (std::vector<CGrab2DBuffer*>::iterator it = BufferList.begin(); it != BufferList.end(); it++)
//		{
//			StreamGrabber.DeregisterBuffer((*it)->GetBufferHandle());
//			delete *it;
//			*it = NULL;
//		}
//
//		// Free all resources used for grabbing
//		StreamGrabber.FinishGrab();
//
//		// Close stream grabber
//		StreamGrabber.Close();
//
//		// Close camera
//		Camera.Close();
//	}
//	catch (GenICam::GenericException &e)
//	{
//		// Error handling
//		CString err;
//		err.Format(L"An exception occurred!: %s\n", e.GetDescription());
//		g_Util.Logging(err);
//		g_bCameraStateOK = FALSE;
//		return;
//	}

	// Exit 
//	SetEvent(m_hEventGrabThreadEnd);
//	g_Util.Logging(L"-- Basler Acq. Thread Ended ! -- \n");
}



void CGrabImgDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
	case 0: // Live
	{
		if (g_bLiveDisplay)
		{
			if(m_pCamera != nullptr)
			{
				if (!m_bDoScan)
				{
					while(1)
					{
						if(m_pCamera->m_bGrabDone == TRUE)
						{
							break;
						}
						Sleep(1);
					}

					memcpy(m_pbyVisionScanBlockImage, m_pCamera->GetGrabData(), m_imageSize);

					m_pCamera->m_bGrabDone = FALSE;
				}
				uint8_t *pImageBuffer;

				EGrabStatus grab_status;
				unsigned char *s, *d;

				pImageBuffer = m_pbyVisionScanBlockImage;// 박다현 (uint8_t *)grabResult.Buffer();
				s = pImageBuffer;	// address 초기화
				d = m_pLiveCCDDlg->m_pDisplayimgData;  // address 초기화
				if(g_ImgType == IMAQ_IMAGE_U8)
				{
					for(UINT j = 0; j < m_nGrabHt; j += 2)
					{
						for(UINT i = 0; i < m_nGrabWd; i += 2)
						{
							*d = *s;
							d++;
							s += 2;
						}
						s += m_nGrabWd;
					}
				}

				else
					g_Util.Logging(L"No support format !\n");

				::SetEvent(g_hEventDisplayStart);
			}
		}
		else
		{
			Sleep(100); // doing nothing, so let a thread run slowly
		}
	}
	break;
	case 1:
		KillTimer(0);
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}
/*
void CGrabImgDlg::VisionScanGrab()
{
	// 스캐너 이동완료 이벤트 대기, Timeout 시간 : 5밀리초
	DWORD ret = ::WaitForSingleObject(m_hEvScannerMoveDone, 5);

	// 이동완료되면 이미지 획득(Grab)
	BOOL bStatus = m_pCamera->Grab(&m_pbyVisionScanBlockImage, 1);// m_pScanImgsProc->m_nBlockCntTotal);

	
	// 스캐너 이동완료 이벤트 대기 Return값 확인
	switch (ret)
	{
	case WAIT_TIMEOUT:		// Timeout 발생 : 아무것도 안함.
		//						Sleep(5);
		break;
	case WAIT_OBJECT_0:		// 정상 발생
		if (bStatus == TRUE) // 이미지 획득 결과 확인, 정상이면
		{
			// 카메라 상태 OK, TRUE로 설정
			g_bCameraStateOK = TRUE;
			// Grab된 이미지 버퍼(메모리 값)을 스캔 큐(m_pScanDataQue)에 추가
			m_pScanImgsProc->m_pScanDataQue->Push(m_pbyVisionScanBlockImage);

			// PushQue 완료 이벤트 발생
			SetEvent(g_hEventPushQueueDone);

			// Push Count 값 0인지 확인(최초 그랩인지 확인)
			if (nPushCnt == 0)
				::SetEvent(g_hEventPopThreadSrt); // 최초 그랩이면 PopThreadSrt 이벤트 발생

			// Push Count 증가
			++nPushCnt;
		}
		else // 이미지 획득 결과 확인, 정상이 아니면
		{
			// 카메라 상태 NG, FALSE로 설정
			g_bCameraStateOK = FALSE;

			// Logging
			g_Util.Logging(L"Grab in Scan Process Failed !");
		}

		// Push Count가 전체 블럭 사이즈보다 크거나 같으면, 이미지를 다 얻었으므로 스캔 종료
//					if (nPushCnt >= m_pScanImgsProc->m_nBlockX * m_pScanImgsProc->m_nBlockY)
		if (nPushCnt >= m_pScanImgsProc->m_nBlockCntTotal)
		{
			_tracer(TRACE_NORMAL, 9999, L"[GrabImg()] nPushCnt : %d", nPushCnt);

			// m_bDoScan Flag FALSE로 설정(스캔 종료)
			m_bDoScan = FALSE;

			// Push Count 초기화
			nPushCnt = 0;

			// Grab 시도 Flag가 TRUE이면
			if (bGrabTried)
			{
				// input Queue에 버퍼를 인큐
				//박다현StreamGrabber.QueueBuffer(grabResult.Handle(), NULL);
				// Grab 시도 Flag 초기화
				bGrabTried = FALSE;
			}
		}
		break;
	}
}*/

/////////////////////////
/////////////////////////
// 박다현

/*
//EGrabStatus CGrabImgDlg::Grab(Camera_t::StreamGrabber_t *pStreamGrabber, GrabResult* pGrabResult)
//{
//	// Wait for the grabbed image with timeout of 300 seconds
//	pStreamGrabber->GetWaitObject().Wait(30000); 
//
//	// Get the grab result from the grabber's result queue
//	pStreamGrabber->RetrieveResult(*pGrabResult);
//
//	return pGrabResult->Status();
//}
bool CGrabImgDlg::Grab(long count)
{
	bool res = false;

	try
	{
		m_pCamera->StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);

		res = true;

	}
	catch (GenICam::GenericException ex)
	{
		CString err;
		err.Format(L"Error : CGigECamera::Grab %s", (CString)CStringA(ex.GetDescription()));
		g_Util.Logging(err);

	}
	catch (...)//catch (CException* e)
	{
		CString err;
		err.Format(L"Error : CGigECamera::Grab failed : StartGrabbing");
		g_Util.Logging(err);
	}
	
	return res;
}


void CGrabImgDlg::OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult)
{
	if (ptrGrabResult->GrabSucceeded() == true)
	{
		int nWidth = ptrGrabResult->GetWidth();
		int nHeight = ptrGrabResult->GetHeight();

		memcpy(m_pbyVisionScanBlockImage, (BYTE *)ptrGrabResult->GetBuffer(), nWidth * nHeight);

		m_pCamera->StopGrabbing();
	}
}
*/

//박다현
/////////////////////////
/////////////////////////


//
void CGrabImgDlg::Grabimg_Simul()
{
	g_Util.Logging(L"-- Simulate(Basler Acq.) Thread Started ! -- \n");
	UINT nPushCnt = 0;
	g_bGrabThreadAlive = TRUE;

	// to calc average frame rate
	int accuFrameCnt = 0;
	double timeElasped;
	LARGE_INTEGER freq, timeBegin, timeEnd;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&timeBegin);

	::SetEvent(g_hEventSrtCopyDispData);
	DWORD ret;
	uint8_t *pImageBuffer = NULL;

	BOOL grab_status_ok;
	unsigned char *s, *d;

	Image* ptemp = imaqCreateImage(g_ImgType, 2);

	while (g_bGrabThreadAlive)
	{
		if (m_bDoScan)
		{
			ret = ::WaitForSingleObject(m_hEvScannerMoveDone, 0);
			grab_status_ok = ReadImageOnDisk(ptemp);

			switch (ret)
			{
			case WAIT_TIMEOUT:
				Sleep(5);
				break;
			case WAIT_OBJECT_0:
				if (grab_status_ok)
				{
					m_pScanImgsProc->m_pScanDataQue->Push(m_pSnapData);

					SetEvent(g_hEventPushQueueDone);
					if (nPushCnt == 0)	
						::SetEvent(g_hEventPopThreadSrt);

					++nPushCnt;
				}

				if (nPushCnt >= m_pScanImgsProc->m_nBlockX * m_pScanImgsProc->m_nBlockY)
				{
					m_bDoScan = FALSE;
					nPushCnt = 0;
				}
				break;
			}
		}

		if (g_bLiveDisplay)
		{
			ret = ::WaitForSingleObject(g_hEventSrtCopyDispData, 0);
			if (!m_bDoScan)
				grab_status_ok = ReadImageOnDisk(ptemp);

			switch (ret) // display 용 m_pDisplayimgData에 영상을 1/4로 줄여 복사
			{
			case WAIT_TIMEOUT:
				Sleep(1);
				break;
			case WAIT_OBJECT_0:
				if (grab_status_ok)
				{
					s = m_pSnapData;		// address 초기화
					d = m_pLiveCCDDlg->m_pDisplayimgData;		// address 초기화
					if (g_ImgType == IMAQ_IMAGE_U8)
					{
						for (UINT j = 0; j < m_nGrabHt; j += 2)
						{
							for (UINT i = 0; i < m_nGrabWd; i += 2)
							{
								*d = *s;
								d++;
								s += 2;
							}
							s += m_nGrabWd;
						}
					}
					else if (g_ImgType == IMAQ_IMAGE_RGB)
					{
						for (UINT j = 0; j < m_nGrabHt; j += 2)
						{
							for (UINT i = 0; i < m_nGrabWd; i += 2)
							{
								*d = *s;				// RGB B
								*(d + 1) = *(s + 1);	// RGB G
								*(d + 2) = *(s + 2);	// RGB R
								*(d + 3) = 0;			// RGB alpa
								d += 4;
								s += 6;
							}
							s += m_nGrabWd;
						}
					}
					else
						g_Util.Logging(L"No support format !\n");

					// display on Ni window
//					::PostMessage(m_pDispImgProc->m_hWnd, UWM_DISPIMG, 0, 0);
					::SetEvent(g_hEventDisplayStart);
					QueryPerformanceCounter(&timeEnd);
					timeElasped = ((double)(timeEnd.QuadPart - timeBegin.QuadPart) / (double)freq.QuadPart); // in sec
					g_AvgFrmRate = (double)(++accuFrameCnt) / timeElasped;
				}
				else
				{
					g_bCameraStateOK = FALSE;
					g_Util.Logging(L"grab_status_ok is False in Display loop !");
				}

				break;
			} //switch loop
		}
		
		if (m_bSnapImg)
		{
			grab_status_ok = ReadImageOnDisk(ptemp);
			if (grab_status_ok)
			{
				// already copied
				m_bSnapImg = FALSE;
				SetEvent(m_hEventSnapimg);
			}
			else
			{
				g_bCameraStateOK = FALSE;
				g_Util.Logging(L"Cant not snap image !");
			}
		}
		
		// g_Util.Logging(L"-----%d ---- %f----%f-\n ", accuFrameCnt, timeElasped, g_AvgFrmRate);
		if (accuFrameCnt > 1e+6) // counter & timer reset
		{
			accuFrameCnt = 0;
			timeElasped = 0;
		}
	}

	// Exit 
	imaqDispose(ptemp);
	SetEvent(m_hEventGrabThreadEnd);
	g_Util.Logging(L"-- Simulate(Basler Acq.) Thread Ended ! -- \n");
}

BOOL CGrabImgDlg::ReadImageOnDisk(Image* pImg)
{
	CString cstrFullName = g_strSimulImagePath + L"\\";
	cstrFullName += SIMUL_IMG_NAME;

	//	int width, height;
	//	ImageType type;
	//	if (!imaqGetFileInfo((CStringA)cstrFullName, NULL, NULL, NULL, &width, &height, &type))
	//	{
	//		g_Util.GetImaqLastError(L"FileInfo Reading Error !");
	//		//	SetCursor(oldCursor);
	//		return FALSE;
	//	}

	if (!imaqReadFile(pImg, (CStringA)cstrFullName, NULL, NULL))
	{
		g_Util.GetImaqLastError(L"Image Reading Error for Vision-Simulation Mode!");
		return FALSE;
	}

	// convert image to 1-d array data
	int col, row;
	unsigned char* _pData = (unsigned char*)imaqImageToArray(pImg, IMAQ_NO_RECT, &col, &row);

	if (m_pSnapData != NULL && _pData != NULL)
	{
		for (int j = 0; j < col*row; j++)
			m_pSnapData[j] = _pData[j];

		imaqDispose(_pData);
		//		Sleep(100);
		return TRUE;
	}
	else
	{

		if (_pData != NULL)	imaqDispose(_pData);
		return FALSE;
	}
}


//void CGrabImgDlg::DispImg(int nCtrlID, BYTE* pImg, int nWd, int nHt, BOOL bOverlay)
//{
//	CRect roi;
//	//	COLORREF linecolor;
//
//	if (!GetDlgItem(nCtrlID)->IsWindowEnabled())
//		return;
//
//	// draw image here !
//	CDC* pDcDest = GetDlgItem(nCtrlID)->GetDC();
//
//	CDC memDC;
//	memDC.CreateCompatibleDC(pDcDest);
//
//	CBitmap bitmap;
//	bitmap.CreateDiscardableBitmap(pDcDest, nWd, nHt);
//	CBitmap* pOldBitmap = (CBitmap*)memDC.SelectObject(&bitmap);
//
//	//
//	int widthbytes = WIDTHBYTES(nWd, 24);
//	BYTE *lpvBits = new BYTE[nHt*widthbytes];
//	//	BYTE *pscanline;
//
//	BITMAPINFO bmi;
//	ZeroMemory(&bmi, sizeof(bmi));
//	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//	bmi.bmiHeader.biPlanes = 1;
//	bmi.bmiHeader.biWidth = nWd;
//	bmi.bmiHeader.biHeight = -nHt;
//	bmi.bmiHeader.biBitCount = 24;
//	bmi.bmiHeader.biCompression = BI_RGB;
//	bmi.bmiHeader.biSizeImage = nHt*widthbytes;
//
//	int i, j, d_jump, s_jump;
//	if (NUM_IMG_CH == 1)
//	{
//		for (j = 0; j < nHt; j++) {
//			s_jump = j * nWd;
//			d_jump = j * widthbytes;
//			for (i = 0; i < nWd; i++)
//			{
//				lpvBits[d_jump + 3 * i + 0] =
//					lpvBits[d_jump + 3 * i + 1] =
//					lpvBits[d_jump + 3 * i + 2] = pImg[s_jump + i];
//			}
//		}
//	}
//	else if (NUM_IMG_CH == 3)
//	{
//		for (j = 0; j < nHt; j++) {
//			s_jump = j * nWd * 3;
//			d_jump = j * widthbytes;
//			for (i = 0; i < nWd; i++)
//			{
//				lpvBits[d_jump + 3 * i + 0] = pImg[s_jump + 3 * i + 0];
//				lpvBits[d_jump + 3 * i + 1] = pImg[s_jump + 3 * i + 1];
//				lpvBits[d_jump + 3 * i + 2] = pImg[s_jump + 3 * i + 2];
//			}
//		}
//	}
//	else
//		g_Util.Logging(L"No supported image depth !");
//
//	SetDIBitsToDevice(
//		memDC.m_hDC,     // handle of device context 
//		0,     // x-coordinate of upper-left corner of dest. rect. 
//		0,     // y-coordinate of upper-left corner of dest. rect. 
//		nWd,     // source rectangle width 
//		nHt,     // source rectangle height 
//		0,     // x-coordinate of lower-left corner of source rect. 
//		0,     // y-coordinate of lower-left corner of source rect. 
//		0,     // first scan line in array 
//		nHt,     // number of scan lines 
//		(CONST VOID *)lpvBits,     // address of array with DIB bits 
//		&bmi,     // address of structure with bitmap info. 
//		DIB_RGB_COLORS      // RGB or palette indices 
//	);
//
//	delete[] lpvBits;
//	//
//
//	int rect_wd = m_rectDisp.Width();
//	int rect_ht = m_rectDisp.Height();
//
//	CPoint pts, pte;
//	pts.y = pte.y = m_nHtDispImg / 2;
//	pts.x = 0;
//	pte.x = m_nWdDispImg;
//	G_imgProc.DrawLine(&memDC, &pts, &pte, PS_SOLID, 1, RGB_CYAN);// RGB(0, 255, 0));
//
//	pts.x = pte.x = m_nWdDispImg / 2;
//	pts.y = 0;
//	pte.y = m_nHtDispImg - 1;
//	G_imgProc.DrawLine(&memDC, &pts, &pte, PS_SOLID, 1, RGB_CYAN);// RGB(0, 255, 0));
//
//																  //GetDlgItem(ctrlID)->GetClientRect(&rect);
//
//	if (m_bDoScan)
//		G_imgProc.DispText(25, 0, CRect(255, 473, 499, 499), RGB_CYAN, L"SCANNING ...", 0, &memDC);
//
//	CRect rect(0, 0, m_nWdDispImg - 1, m_nHtDispImg - 1);
//	G_imgProc.DrawRect(&memDC, &rect, PS_SOLID, 1, RGB_CYAN);
//
//	int mode = pDcDest->SetStretchBltMode(HALFTONE);
//	pDcDest->StretchBlt(0, 0, rect_wd, rect_ht, &memDC, 0, 0, nWd, nHt, SRCCOPY);
//	pDcDest->SetStretchBltMode(mode);
//
//	memDC.SelectObject(pOldBitmap);
//	memDC.DeleteDC();
//	bitmap.DeleteObject();
//	ReleaseDC(pDcDest);
//
//	// Draw roi
//	//	G_imgProc.DrawRect(pDcDest, &roi, PS_SOLID, 1, linecolor);
//
//	if (bOverlay)
//	{
//		;
//	}
//	//G_imgProc.DrawCross(&m_rt3DResult, pDcDest, &(rect1.CenterPoint()), 5, PS_SOLID, 1, RGB_DARK_GRAY);
//
//	//G_imgProc.DrawRect(pDcDest, &rect2, PS_SOLID, 1, RGB_WHITE);
//	//G_imgProc.DrawCross(&m_rt3DResult, pDcDest, &(rect2.CenterPoint()), 5,  PS_SOLID, 1, RGB_DARK_GRAY);
//
//	//	G_imgProc.DrawLine(pDcDest, &(rect1.CenterPoint()), 
//	//							&(rect2.CenterPoint()), PS_SOLID, 1, RGB_DARK_GRAY);
//
//	// rectangle & circle
//	//G_imgProc.DrawRect(pDcDest, &rect3, PS_SOLID, 1, RGB_MAGENTA);
//	//G_imgProc.DrawCircle(pDcDest, &rect4, PS_SOLID, 1, RGB_GREEN);
//}

void CGrabImgDlg::SnapAndSaveImg(CString strPath, CString strImgName)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bSnapImg = TRUE;
	DWORD result = WaitForSingleObject(m_hEventSnapimg, 5000);
	Image* pTempImg = imaqCreateImage(g_ImgType, 2);
	if (result == WAIT_OBJECT_0)
	{
		imaqArrayToImage(pTempImg, m_pSnapData, m_nGrabWd, m_nGrabHt);
		CString fullName;
		fullName = strPath + L"\\";
		fullName += strImgName;
		imaqWriteFile(pTempImg, (CStringA)fullName, NULL);
	}
	imaqDispose(pTempImg);
}


unsigned char* CGrabImgDlg::SnapImg()
{
	m_bSnapImg = TRUE;
	DWORD result = WaitForSingleObject(m_hEventSnapimg, 2000);
	if (result == WAIT_OBJECT_0)
		return m_pSnapData;
	else
		return NULL;
}

BOOL CGrabImgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	// 카메라 클래스 생성되어 있으면 타이머 실행
	if(m_pCamera != nullptr)
	{
		SetTimer(0, 50, NULL);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

//void CGrabImgDlg::OnDestroy()
//{
//	CDialogEx::OnDestroy();
//
//	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
//	g_bLiveDisplay = FALSE;
//	g_bGrabThreadAlive = FALSE;
//	Sleep(100);
//}
BOOL CGrabImgDlg::MoveScannerTo(double x, double y)
{
	m_pDoc->m_pScanner->Move(x, y);
	m_pDoc->m_pScanner->WaitMoving(FALSE);
	return TRUE;
}

// sjyi 20230607 Vision Scan 관련 Comment ---------->
// 실질적인 Vision Scan 쓰레드 동작 함수
void CGrabImgDlg::ScanStart()
{	
	// Processing Time을 계산하기 위해, 현재 시간 저장
	DWORD dwStart = GetTickCount();

	// sjyi 2023.06.13 전체 블럭개수 계산
	// sjyi 2024.05.30 부분스캔맵 파일이 있는 경우에만, 부분스캔하도록 수정
	//if (m_pDoc->m_config.m_bUsePartialScan) // 부분 스캔 사용할 경우, 부분스캔맵의 값이 TRUE인 것만 Count
	if (m_pDoc->m_config.m_bUsePartialScan == TRUE && m_pDoc->m_config.m_vmapUseScan.size() > 0) // 부분 스캔 사용할 경우
	{
		m_pScanImgsProc->m_nBlockCntTotal = 0;
		for (int i = 0; i < m_pDoc->m_config.m_vmapUseScan.size(); i++) // Y
		{
			for (int j = 0; j < m_pDoc->m_config.m_vmapUseScan[i].size(); j++) // X
			{
				if (m_pDoc->m_config.m_vmapUseScan[i][j]) // 부분스캔맵의 값이 TRUE인 것만 Count(실제 스캔 이미지 개수 확인용)
				{
					m_pScanImgsProc->m_nBlockCntTotal++;
				}
			}
		}
	}
	else // 부분 스캔 사용안 경우, 전체 Count Up(실제 스캔이미지 = 전체 영역)
	{
		m_pScanImgsProc->m_nBlockCntTotal = m_pScanImgsProc->m_nBlockX * m_pScanImgsProc->m_nBlockY;
	}

	// before Vision Scan Killtimer
//	KillTimer(0);

	// m_bDoScan Flag TRUE로 설정
	SetDoScan(TRUE);

	// 블럭 개수 확인(X방향 블럭 개수, Y방향 블럭 개수)
	int nBlockX = m_pScanImgsProc->m_nBlockX;
	int nBlockY = m_pScanImgsProc->m_nBlockY;

	POINTF pt;

	int cnt = -1;
	BOOL bRet;

	_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] Start--------------->");
	_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] Scan Block Count : %d", m_pScanImgsProc->m_nBlockCntTotal);

	// 이미지 획득
	for (int j = 0; j < nBlockY; j++) // Y방향으로 블럭 개수 만큼 실행
	{
		if (j % 2 == 0) // Y방향 짝수번째 블럭일 경우(ZigZag로 스캔하기 위해 짝수번, 홀수번 확인)
		{
			for (int i = 0; i < nBlockX; i++)	// X+ 방향으로 블럭 개수 만큼 실행
			{
				// sjyi 2024.05.30 부분스캔맵 파일이 있는 경우에만, 부분스캔하도록 수정
				//if ((m_pDoc->m_config.m_bUsePartialScan == TRUE) && (m_pDoc->m_config.m_vmapUseScan[j][i] == FALSE)) // 스캔 안함.
				if ((m_pDoc->m_config.m_bUsePartialScan == TRUE) && (m_pDoc->m_config.m_vmapUseScan.size() > 0)) // 부분스캔 적용 여부
				{
					if (m_pDoc->m_config.m_vmapUseScan[j][i] == FALSE) // 부분 스캔 비활성화 되면 스캔 안함.
					{
						// Black Image 처리
						m_pScanImgsProc->PushQueBlackImage();
						continue;
					}
				}

				// 스캐너 중심 좌표(Laser 포인트 좌표) 획득(pt)
				pt = m_pScanImgsProc->m_ptScanCoord.at(j*nBlockX + i);

				// 스캐너 중심 좌표(Laser 포인트 좌표) 이동(to pt)
				bRet = MoveScannerTo((double)pt.x, (double)pt.y); // 스캐너 x,y 좌표 이동

				// 30 밀리초 대기(좌표 이동 안정 시간 확보)
				// if 참조건, 거짓조건 모두 Sleep(30)이므로 그냥 Sleep(30)으로 변경
				//if (i == 0)	Sleep(30);
				//else Sleep(30);
				//Sleep(30);
				Sleep(m_pDoc->m_config.m_nScanSleepTime);

				// 스캐너 이동 완료 이벤트 발생, Grab다이얼로그에서 화면 획득 이벤트 확인하기 위해.
				::SetEvent(m_hEvScannerMoveDone); // push implemented at grab thread(see GrabImgDlg.h)

				// g_hEventPushQueueDone 이벤트 발생될 때까지 대기
				// Grab다이얼로그에서 화면 획득 완료되면, g_hEventPushQueueDone 이벤트 발생
				// wait somehow push is done
				::WaitForSingleObject(g_hEventPushQueueDone, INFINITE);
			}
		}
		else // Y방향 홀수번째 블럭일 경우
		{
			for (int i = nBlockX - 1; i >= 0; i--) // X- 방향으로 블럭 개수 만큼 실행
			{
				//_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] (%d, %d) Scan", i, j);

				// sjyi 2024.05.30 부분스캔맵 파일이 있는 경우에만, 부분스캔하도록 수정
				//if ((m_pDoc->m_config.m_bUsePartialScan == TRUE) && (m_pDoc->m_config.m_vmapUseScan[j][i] == FALSE)) // 스캔 안함.
				if ((m_pDoc->m_config.m_bUsePartialScan == TRUE) && (m_pDoc->m_config.m_vmapUseScan.size() > 0)) // 부분스캔 적용 여부
				{
					if (m_pDoc->m_config.m_vmapUseScan[j][i] == FALSE)// 부분 스캔 비활성화 되면 스캔 안함.
					{
						// Black Image 처리
						m_pScanImgsProc->PushQueBlackImage();
						continue;
					}
				}

				// 스캐너 중심 좌표(Laser 포인트 좌표) 획득(pt)
				pt = m_pScanImgsProc->m_ptScanCoord.at(j*nBlockX + i);

				// 스캐너 중심 좌표(Laser 포인트 좌표) 이동(to pt)
				bRet = MoveScannerTo((double)pt.x, (double)pt.y); // 스캐너 x,y 좌표 이동

				// 30 밀리초 대기(좌표 이동 안정 시간 확보)
				// if 참조건, 거짓조건 모두 Sleep(30)이므로 그냥 Sleep(30)으로 변경
				//if (i == 0)	Sleep(30);
				//else Sleep(30);
				//Sleep(30);
				Sleep(m_pDoc->m_config.m_nScanSleepTime);

				// 스캐너 이동 완료 이벤트 발생, Grab다이얼로그에서 화면 획득 이벤트 확인하기 위해.
				::SetEvent(m_hEvScannerMoveDone); // push implemented at grab thread(see GrabImgDlg.h)

				// g_hEventPushQueueDone 이벤트 발생될 때까지 대기
				// Grab다이얼로그에서 화면 획득 완료되면, g_hEventPushQueueDone 이벤트 발생
				// wait somehow push is done
				::WaitForSingleObject(g_hEventPushQueueDone, INFINITE);
			}
		}
	}

	// 이미지 획득 시간 계산
	DWORD dwScanTime = GetTickCount() - dwStart;
	_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] Image Scan is Done");

	// 모든 이미지 데이터 Pop 처리 완료 될 때까지 대기
	// Wait until mosaic image is constructed	Poping and Stitching is handled in stitching thread(see StitchImgProc.h) 
	WaitForSingleObject(m_pScanImgsProc->m_hNPopProcessEnd, INFINITE);
	_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] PopProcess is Done");

	// 이미지 합성(Stitching)
	m_pScanImgsProc->Merging();
	m_pScanImgsProc->m_stitchImg.bMosaicImgAvailable = true;
	_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] Merging is Done");

	// save stitched img req by Main
	int newWidth = (int)((float)m_pScanImgsProc->m_stitchImg.nWd / m_pScanImgsProc->m_dsf);
	int newHeight = (int)((float)m_pScanImgsProc->m_stitchImg.nHt / m_pScanImgsProc->m_dsf);
	imaqResample(m_pScanImgsProc->m_stitchImg.pDownMosaicImg, m_pScanImgsProc->m_stitchImg.pMosaicImg, newWidth, newHeight, IMAQ_ZERO_ORDER, IMAQ_NO_RECT);
	_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] Resample is Done");

	if (m_pScanImgsProc->m_stitchImg.iSubAction == SUB_ACT_SAVE)
	{
		imaqWriteFile(m_pScanImgsProc->m_stitchImg.pDownMosaicImg, (CStringA)m_pScanImgsProc->m_stitchImg.strName4Save, NULL);
		SetFlagStitchImageDone(TRUE);
		_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] File Save is Done");

		if (m_pDoc->m_config.m_bSaveImage)
		{
			// 2024.02.11 jh.kim 추가 - 저장 이미지에 좌표 표시
			m_pDoc->SaveImageAndPos();
		}
	}
	DWORD dwScanAllTime = GetTickCount() - dwStart;
	TRACE(_T("\nScanCnt = %d, ScanTime = %d msec, Scan+calib+merge = %d msec\t\t"), nBlockX*nBlockY, (int)dwScanTime, (int)dwScanAllTime);
	_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] Complete <---------------\nScanCnt = %d, ScanTime = %d msec, Scan+calib+merge = %d msec\t\t", nBlockX*nBlockY, (int)dwScanTime, (int)dwScanAllTime)
		;
	//SetFlagStitchImageDone(TRUE);
}
// sjyi 20230607 Vision Scan 관련 Comment <----------

// sjyi 20230607 Vision Scan 관련 Comment ---------->
// Vision Scan 함수
void CGrabImgDlg::ScanAndSave(CString strStitchImg, BOOL bScan, int* widthPxl, int* heightPxl,
	float* widthLength, float* heightLength)
{
	// m_pScanImgsProc->m_stitchImg.nWd : 스티치 이미지 폭(X)
	// m_pScanImgsProc->m_stitchImg.nHt : 스티치 이미지 높이(Y)
	// m_pScanImgsProc->m_fCurFullFovX : FOV(X)
	// m_pScanImgsProc->m_fCurFullFovY : FOV(Y)
	*widthPxl = m_pScanImgsProc->m_stitchImg.nWd / (int)m_pScanImgsProc->m_dsf;
	*heightPxl = m_pScanImgsProc->m_stitchImg.nHt / (int)m_pScanImgsProc->m_dsf;
	*widthLength = m_pScanImgsProc->m_fCurFullFovX;
	*heightLength = m_pScanImgsProc->m_fCurFullFovY;

	// Vision Scan 저장파일명 설정
	m_pScanImgsProc->m_stitchImg.strName4Save = strStitchImg;

	// bScan : TRUE -> Vision Scan, FALSE??? -> 현재 사용 안함
	if (bScan || !m_pScanImgsProc->m_stitchImg.bMosaicImgAvailable) // Vision Scan 조건 확인, bScan이 TRUE 이거나, bMosicImgAvailable이 FALSE일 때
	{
		// 스캔 주목적 설정 : 그냥 스캔, 확인결과 다른 옵션 사용 안함.
		m_pScanImgsProc->m_stitchImg.iActionPurpose = ACT_SCAN_ONLY;
		// 스캔 부목적 설정 : 파일 저장 여부, 다른 옵션은 SUB_ACT_NONE 이거 하나 밖에 없음.
		m_pScanImgsProc->m_stitchImg.iSubAction = SUB_ACT_SAVE;
		// 쓰레드 시작
		// 이중 쓰레드 이유는?
		AfxBeginThread(OneShotScan, (LPVOID)this, THREAD_PRIORITY_NORMAL);
	}
	else
	{
		// already available, so just save
		int newWidth = m_pScanImgsProc->m_stitchImg.nWd / (int)m_pScanImgsProc->m_dsf;
		int newHeight = m_pScanImgsProc->m_stitchImg.nHt / (int)m_pScanImgsProc->m_dsf;
		imaqResample(m_pScanImgsProc->m_stitchImg.pDownMosaicImg, m_pScanImgsProc->m_stitchImg.pMosaicImg, newWidth, newHeight, IMAQ_ZERO_ORDER, IMAQ_NO_RECT);
		imaqWriteFile(m_pScanImgsProc->m_stitchImg.pDownMosaicImg, (CStringA)strStitchImg, NULL);
		SetFlagStitchImageDone(TRUE);

		_tracer(TRACE_NORMAL, 9999, L"Vision Scan Done");
	}
}
// sjyi 20230607 Vision Scan 관련 Comment <----------

// One Shot Scan Thread to obtain stitched full field image
UINT CGrabImgDlg::OneShotScan(LPVOID pArg)
{	
	CGrabImgDlg* me = (CGrabImgDlg*)pArg;
	me->ScanStart();
	return 0;
}

//DHChoi 20200626
void CGrabImgDlg::ChangeOverlap(UINT nOverlapX, UINT nOverlapY)
{
	m_pScanImgsProc->ChangeOverlap(nOverlapX, nOverlapY);
}

void CGrabImgDlg::ChangeScanRange(float resolX, float resolY,
	double startX, double startY, double endX, double endY)
{
	m_fResolX = resolX;
	m_fResolY = resolY;

	m_pScanImgsProc->ChangeScanRange(resolX, resolY, startX, startY, endX, endY);

	//DHChoi 20200629
	// Calibration코드에서 
	//  Center위치를 Config의 CorrParam - dblImgCenX, dblImgCenY를 사용하도록 수동되어 
	// m_ScanCener를 사용할 경우 센터위치가 맞지 않게됨... 
	// m_ScanCener를 (0,0)으로 일단 처리
	//
	//POINTF pt0 = m_pScanImgsProc->m_ptScanCoord.front();
	//POINTF pt1 = m_pScanImgsProc->m_ptScanCoord.back();
	//
	//m_ScanCener.x = (pt0.x + pt1.x) * 0.5;
	//m_ScanCener.y = (pt0.y + pt1.y) * 0.5;

	m_ScanCener.x = 0.0f;
	m_ScanCener.y = 0.0f;
}

void CGrabImgDlg::SetFlagStitchImageDone(BOOL bFlag)
{ 
	_tracer(TRACE_NORMAL, 9999, _T("SetFlagStitchImageDone() : %d\n"), bFlag);
	m_bStitchDone = bFlag;
}

BOOL CGrabImgDlg::IsDoneStitchImage()
{ 
	return m_bStitchDone;
}

int CGrabImgDlg::SnapAndInspect(CString strFilename)
{
	int retV = -1;
	int retVV = -1;
	Image *pImg = NULL;
	Image *pSaveImg = NULL;

	BYTE* pImgData = NULL;
	std::vector<string> predClass;
	std::vector<float> maxProb;

	BYTE* pSnapImgData = SnapImg();
	if (pSnapImgData == NULL)
	{
		_tracer(TRACE_ERROR, 1, L"Image Snap Failed");
		return retV;
	}
	else
	{
		pImg = imaqCreateImage(g_ImgType, 0);
		pSaveImg = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
		imaqArrayToImage(pImg, pSnapImgData, m_nGrabWd, m_nGrabHt);

		double x = 0, y = 0;
		if (m_pDoc->m_pScanner)
			m_pDoc->m_pScanner->GetLastPos(x, y);

		int ix = m_pLiveCCDDlg->GetWeldMark(0, x, y);
		int iy = m_pLiveCCDDlg->GetWeldMark(1, x, y);
		
		ix = int(double(ix) / double(m_pLiveCCDDlg->m_pAttach->m_nWdWnd) * double(NUM_CAM_PIXEL_X) + 0.5);
		iy = int(double(iy) / double(m_pLiveCCDDlg->m_pAttach->m_nHtWnd) * double(NUM_CAM_PIXEL_Y) + 0.5);

		RGBValue rgb = RGB_GREEN;;
		imaqOverlayOval(pImg, imaqMakeRect(iy - 5, ix - 5, 10, 10), &rgb, IMAQ_PAINT_VALUE, NULL);//imaqCopyRect(pImgCrop, pImg, rectCrop, imaqMakePoint(0, 0));
		imaqMergeOverlay(pSaveImg, pImg, NULL, 256, NULL);

		imaqWriteFile(pSaveImg, CStringA(strFilename), NULL);

		imaqDispose(pSaveImg);
		imaqDispose(pImg);

		return retV;
	}
	return retV;
}

void CGrabImgDlg::GetCurFullFov(float* fovX, float* fovY) 
{
	*fovX = m_pScanImgsProc->m_fCurFullFovX;
	*fovY = m_pScanImgsProc->m_fCurFullFovY;
}


void CGrabImgDlg::GetRefImgCnt(double *dcx, double* dcy)
{
	*dcx = (double)m_pScanImgsProc->m_stitchImg.pfCenterPos_mm.x;
	*dcy = (double)m_pScanImgsProc->m_stitchImg.pfCenterPos_mm.y;
}

void CGrabImgDlg::SetDoScan(BOOL bDoScan)
{
	m_bDoScan = bDoScan;
}


//void CGrabImgDlg::RescanStart()
//{
//	SetFlagRescanImageDone(FALSE);
//
//	// Rescan Thread 시작
//	// 스캔 주목적 설정 : 그냥 스캔, 확인결과 다른 옵션 사용 안함.
//	m_pScanImgsProc->m_stitchImg.iActionPurpose = ACT_SCAN_ONLY;
//	// 스캔 부목적 설정 : 파일 저장 여부, 다른 옵션은 SUB_ACT_NONE 이거 하나 밖에 없음.
//	m_pScanImgsProc->m_stitchImg.iSubAction = SUB_ACT_SAVE;
//	// 쓰레드 시작
//	AfxBeginThread(ReScan, (LPVOID)this, THREAD_PRIORITY_NORMAL);
//}
//
//UINT CGrabImgDlg::ReScan(LPVOID pArg)
//{
//	CGrabImgDlg* me = (CGrabImgDlg*)pArg;
//	me->ScanModify();
//	return 0;
//}
//
//// 실질적인 Vision Scan 쓰레드 동작 함수
//void CGrabImgDlg::ScanModify()
//{
//	// Processing Time을 계산하기 위해, 현재 시간 저장
//	DWORD dwStart = GetTickCount();
//
//	// BackUp Image
//	m_pScanImgsProc->BackUpImage();
//
//	// 스캔 카운트 계산
//	m_pScanImgsProc->m_nBlockCntTotal = 0;
//	//for (int i = 0; i < m_pDoc->m_config.m_vmapReScan.size(); i++) // Y
//	//{
//	//	for (int j = 0; j < m_pDoc->m_config.m_vmapReScan[i].size(); j++) // X
//	//	{
//	//		if (m_pDoc->m_config.m_vmapReScan[i][j])
//	//		{
//	//			m_pScanImgsProc->m_nBlockCntTotal++;
//	//		}
//	//	}
//	//}
//	m_pScanImgsProc->m_nBlockCntTotal = m_vecFrameIdx.size();
//
//	// before Vision Scan Killtimer
//	//KillTimer(0);
//
//	// m_bDoScan Flag TRUE로 설정
//	SetDoScan(TRUE);
//
//	// 블럭 개수 확인(X방향 블럭 개수, Y방향 블럭 개수)
//	int nBlockX = m_pScanImgsProc->m_nBlockX;
//	int nBlockY = m_pScanImgsProc->m_nBlockY;
//
//	POINTF pt;
//
//	int cnt = -1;
//	BOOL bRet;
//
//	_tracer(TRACE_NORMAL, 9999, L"[ScanModify()] Start--------------->");
//	_tracer(TRACE_NORMAL, 9999, L"[ScanModify()] Scan Block Count : %d", m_pScanImgsProc->m_nBlockCntTotal);
//
//	// 이미지 획득
//	for (int j = 0; j < nBlockY; j++) // Y방향으로 블럭 개수 만큼 실행
//	{
//		if (j % 2 == 0) // Y방향 짝수번째 블럭일 경우(ZigZag로 스캔하기 위해 짝수번, 홀수번 확인)
//		{
//			for (int i = 0; i < nBlockX; i++)	// X+ 방향으로 블럭 개수 만큼 실행
//			{
////				if (m_pDoc->m_config.m_vmapReScan[j][i] == FALSE) // 스캔 안함.
//				if (IsRescan(i, j) == FALSE) // 스캔 안함.
//				{
//					m_pScanImgsProc->PushQueBakupImage(j*nBlockX + i);
//					TRACE(L"[sjyi TEST] PushQueBakupImage(%d, %d)\n", i, j);
//					Sleep(1);
//					continue;
//				}
//
//				// 스캐너 중심 좌표(Laser 포인트 좌표) 획득(pt)
//				pt = m_pScanImgsProc->m_ptScanCoord.at(j*nBlockX + i);
//
//				// 스캐너 중심 좌표(Laser 포인트 좌표) 이동(to pt)
//				bRet = MoveScannerTo((double)pt.x, (double)pt.y); // 스캐너 x,y 좌표 이동
//
//				// 30 밀리초 대기(좌표 이동 안정 시간 확보)
//				// if 참조건, 거짓조건 모두 Sleep(30)이므로 그냥 Sleep(30)으로 변경
//				//if (i == 0)	Sleep(30);
//				//else Sleep(30);
//				//Sleep(30);
//				Sleep(m_pDoc->m_config.m_nScanSleepTime);
//
//				// 스캐너 이동 완료 이벤트 발생, Grab다이얼로그에서 화면 획득 이벤트 확인하기 위해.
//				::SetEvent(m_hEvScannerMoveDone); // push implemented at grab thread(see GrabImgDlg.h)
//				TRACE(L"[sjyi TEST] SetEvent - m_hEvScannerMoveDone\n");
//
//				// g_hEventPushQueueDone 이벤트 발생될 때까지 대기
//				// Grab다이얼로그에서 화면 획득 완료되면, g_hEventPushQueueDone 이벤트 발생
//				// wait somehow push is done
//				::WaitForSingleObject(g_hEventPushQueueDone, INFINITE);
//			}
//		}
//		else // Y방향 홀수번째 블럭일 경우
//		{
//			for (int i = nBlockX - 1; i >= 0; i--) // X- 방향으로 블럭 개수 만큼 실행
//			{
//				//_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] (%d, %d) Scan", i, j);
//
////				if (m_pDoc->m_config.m_vmapReScan[j][i] == FALSE) // 스캔 안함.
//				if (IsRescan(i, j) == FALSE) // 스캔 안함.
//				{
//					m_pScanImgsProc->PushQueBakupImage(j*nBlockX + i);
//					TRACE(L"[sjyi TEST] PushQueBakupImage(%d, %d)\n", i, j);
//					Sleep(1);
//					continue;
//				}
//
//				// 스캐너 중심 좌표(Laser 포인트 좌표) 획득(pt)
//				pt = m_pScanImgsProc->m_ptScanCoord.at(j*nBlockX + i);
//
//				// 스캐너 중심 좌표(Laser 포인트 좌표) 이동(to pt)
//				bRet = MoveScannerTo((double)pt.x, (double)pt.y); // 스캐너 x,y 좌표 이동
//
//				// 30 밀리초 대기(좌표 이동 안정 시간 확보)
//				// if 참조건, 거짓조건 모두 Sleep(30)이므로 그냥 Sleep(30)으로 변경
//				//if (i == 0)	Sleep(30);
//				//else Sleep(30);
//				//Sleep(30);
//				Sleep(m_pDoc->m_config.m_nScanSleepTime);
//
//				// 스캐너 이동 완료 이벤트 발생, Grab다이얼로그에서 화면 획득 이벤트 확인하기 위해.
//				::SetEvent(m_hEvScannerMoveDone); // push implemented at grab thread(see GrabImgDlg.h)
//				TRACE(L"[sjyi TEST] SetEvent - m_hEvScannerMoveDone\n");
//
//				// g_hEventPushQueueDone 이벤트 발생될 때까지 대기
//				// Grab다이얼로그에서 화면 획득 완료되면, g_hEventPushQueueDone 이벤트 발생
//				// wait somehow push is done
//				::WaitForSingleObject(g_hEventPushQueueDone, INFINITE);
//			}
//		}
//	}
//
//	// 이미지 획득 시간 계산
//	DWORD dwScanTime = GetTickCount() - dwStart;
//	_tracer(TRACE_NORMAL, 9999, L"[ScanModify()] Image Scan Is Done");
//
//	// 모든 이미지 데이터 Pop 처리 완료 될 때까지 대기
//	// Wait until mosaic image is constructed	Poping and Stitching is handled in stitching thread(see StitchImgProc.h) 
//	WaitForSingleObject(m_pScanImgsProc->m_hNPopProcessEnd, INFINITE);
//	_tracer(TRACE_NORMAL, 9999, L"[ScanModify()] PopProcess Is Done");
//
//	// 이미지 합성(Stitching)
//	m_pScanImgsProc->Merging();
//	m_pScanImgsProc->m_stitchImg.bMosaicImgAvailable = true;
//	_tracer(TRACE_NORMAL, 9999, L"[ScanModify()] Merging Is Done");
//
//	// save stitched img req by Main
//	int newWidth = (int)((float)m_pScanImgsProc->m_stitchImg.nWd / m_pScanImgsProc->m_dsf);
//	int newHeight = (int)((float)m_pScanImgsProc->m_stitchImg.nHt / m_pScanImgsProc->m_dsf);
//	imaqResample(m_pScanImgsProc->m_stitchImg.pDownMosaicImg, m_pScanImgsProc->m_stitchImg.pMosaicImg, newWidth, newHeight, IMAQ_ZERO_ORDER, IMAQ_NO_RECT);
//	_tracer(TRACE_NORMAL, 9999, L"[ScanModify()] Resample Is Done");
//
//	if (m_pScanImgsProc->m_stitchImg.iSubAction == SUB_ACT_SAVE)
//	{
//		imaqWriteFile(m_pScanImgsProc->m_stitchImg.pDownMosaicImg, (CStringA)m_pScanImgsProc->m_stitchImg.strName4Save, NULL);
//		//SetFlagStitchImageDone(TRUE);
//		_tracer(TRACE_NORMAL, 9999, L"[ScanModify()] File Save Is Done");
//
//		if (m_pDoc->m_config.m_bSaveImage)
//		{
//			TCHAR path[_MAX_PATH];
//			GetModuleFileName(NULL, path, sizeof path);
//
//			CString strStartUpPath = path;
//			strStartUpPath = strStartUpPath.Left(strStartUpPath.ReverseFind('\\'));
//
//
//			// 이미지 복사
//			SYSTEMTIME stTime;
//			GetLocalTime(&stTime);
//			CString strJig;
//			if (m_pDoc->m_plc.GetWorkJig() == 1)
//			{
//				strJig.Format(_T("JIGA"));
//			}
//			else if (m_pDoc->m_plc.GetWorkJig() == 2)
//			{
//				strJig.Format(_T("JIGB"));
//			}
//			else
//			{
//				strJig.Format(_T("Unknown"));
//			}
//			CString strStitchImg2;
//			strStitchImg2.Format(_T("%s\\image\\refImage_%s_%04d%02d%02d%02d%02d%02d.bmp"), strStartUpPath, strJig,
//				stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
//
//			CopyFile(m_pScanImgsProc->m_stitchImg.strName4Save, strStitchImg2, FALSE);
//
//			//// 검사 결과 저장
//			//strStartUpPath.Format(L"%s\\image\\%04d%02d%02d%02d%02d%02d.csv", strStartUpPath,
//			//	stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
//
//			//m_pDoc->SaveInspectData(strStartUpPath.GetBuffer());
//		}
//	}
//	DWORD dwScanAllTime = GetTickCount() - dwStart;
//	TRACE(_T("\nScanCnt = %d, ScanTime = %d msec, Scan+calib+merge = %d msec\t\t"), nBlockX*nBlockY, (int)dwScanTime, (int)dwScanAllTime);
//	_tracer(TRACE_NORMAL, 9999, L"[ScanModify()] Complete <---------------\nScanCnt = %d, ScanTime = %d msec, Scan+calib+merge = %d msec\t\t", nBlockX*nBlockY, (int)dwScanTime, (int)dwScanAllTime)
//		;
//
//	SetFlagRescanImageDone(TRUE);
//}
//
//void CGrabImgDlg::SetFlagRescanImageDone(BOOL bFlag)
//{
//	m_bCheckRescanDone = bFlag;
//}
//
//BOOL CGrabImgDlg::IsDoneRescanImage()
//{
//	return m_bCheckRescanDone;
//}
//
//std::vector<CString> CGrabImgDlg::read_diectory(CString strPath)
//{
//	HANDLE  hFind;
//	WIN32_FIND_DATA wfd;
//	CString fileName;
//	std::vector<CString> vecFileName;
//
//	hFind = FindFirstFile(strPath, &wfd);
//
//	if (hFind == INVALID_HANDLE_VALUE)
//		return vecFileName;
//	do {
//		fileName.Format(_T("%s"), wfd.cFileName);
//		vecFileName.push_back(fileName);
//	} while (FindNextFile(hFind, &wfd));
//	FindClose(hFind);
//
//	return vecFileName;
//}
//
//// sjyi 2023.07.16 수정, RecipeNo로 구분
//BOOL CGrabImgDlg::CheckResultImage(int nRcpNo)
//{
//	BOOL bCheckResult = FALSE;
//	m_vecFrameIdx.clear();
//
//	// Check Vector Size
//	if (m_BoundContiX[nRcpNo].size() <= 0)
//	{
//		_tracer(TRACE_ERROR, 9999, L"[CheckResultImage()] RcpNo : %d - VectorSize : %d", nRcpNo, m_BoundContiX[nRcpNo].size());
//		return FALSE;
//	}
//
//	// Read Vision Scan Image
//	Image* pCurVSImg = imaqCreateImage(IMAQ_IMAGE_U8, 3);
//	char szImageFilePath[512] = { 0 };
//	USES_CONVERSION;
//	sprintf_s(szImageFilePath, 512, "%s%s", W2A(m_pDoc->m_strImgPath), W2A(STR_REFIMG_FILE));
//	imaqReadFile(pCurVSImg, szImageFilePath, NULL, NULL);
//
//	// STEP 1.
//	int nFrameX, nFrameY;
//
//	float meanLimitX = 0.5f;
//	float meanLimitY = 0.8f;
//
//	float stdLimitX = 1.5f;
//	float stdLimitY = 2.0f;
//
//	int iCurLensIdx = m_pDoc->m_config.m_iCurrLens;
//	nFrameX = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibX;
//	nFrameY = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibY;
//	
//	//meanLimit = m_pDoc->m_config.m_dMeanLimit;
//	//stdLimit = m_pDoc->m_config.m_dStdLimit;
//
//	// Check Reslut
//	BOOL bSaveOverlayImg = TRUE;
//	CString strSaveFolderPath = m_pDoc->m_strImgPath;
//	//m_vecFrameIdx = DetectAnorm(pCurVSImg, nFrameX, nFrameY, m_Mean, m_stdDev, meanLimit, stdLimit,
//	//	bSaveOverlayImg, strSaveFolderPath);
//	m_vecFrameIdx = DetectAnormByBoundConti(pCurVSImg, nFrameX, nFrameY, 
//		m_BoundContiX[nRcpNo], m_BoundContiY[nRcpNo], m_BoundContiX_Std[nRcpNo], m_BoundContiY_Std[nRcpNo],
//		meanLimitX, meanLimitY, stdLimitX, stdLimitY,
//		bSaveOverlayImg, strSaveFolderPath);
//
//	imaqDispose(pCurVSImg);
//
//	// Make Rescan Map
//	if (m_vecFrameIdx.size() > 0) {
//
//		bCheckResult = FALSE;
//	}
//	else
//	{
//		m_vecFrameIdx.clear();
//		bCheckResult = TRUE;
//	}
//
//	return bCheckResult;
//}
//
//void CGrabImgDlg::LoadRefImage(int nRcpNo)
//{
//	CString strRefDir;
//
//	// sjyi 2023.07.16 경로 변경(ini파일 -> 기본 실행파일 밑에 RefImage 폴더
//	//strRefDir.Format(_T("%s\\%03d\\*.bmp"), m_pDoc->m_config.m_strVSChkDir, nRcpNo);
//	strRefDir.Format(_T("%s\\%03d\\*.bmp"), m_pDoc->m_strRefImgPath, nRcpNo);
//	_tracer(TRACE_NORMAL, 9999, L"[LoadRefImage()] RcpNo : %d - %s", nRcpNo, strRefDir);
//
//	int nBlockX, nBlockY;
//	int iCurLensIdx = m_pDoc->m_config.m_iCurrLens;
//	nBlockX = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibX;
//	nBlockY = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibY;
//
//	// CalcStatFromRefImages(nBlockX, nBlockY, m_Mean, m_stdDev, strRefDir);
//	// sjyi 2023.07.16 함수 인자에 Recipe별 Ref 벡터 데이터 사용하도록 수정
//	CalcBoundContiFromRefImages(nBlockX, nBlockY, 
//		m_BoundContiX[nRcpNo], m_BoundContiY[nRcpNo], m_BoundContiX_Std[nRcpNo], m_BoundContiY_Std[nRcpNo], strRefDir);
//
//}
//
///*
//1. 아래 두함수는 속도 계산을 위하여 update 한 함수 입니다.
// 적용 바랍니다.
//2. 처음 보내드린 DetectAnorm(..) 함수에서는 vecMean 과 vecStdDev를 인자로 받지 않았는데
//새 버젼에서는 인자로 받았습니다.
//이유는 CalcStatFromRefImages(..) 함수 와의 통일성을 기하고자 하였습니다.
// vecMean 는 G_Mean 을  vecStdDev는 G_stdDev 를 의미합니다.
//*/
//
//void CGrabImgDlg::CalcStatFromRefImages(int nFrameX, int nFrameY,
//	std::vector<std::vector<float>>& vecMean,
//	std::vector<std::vector<float>>& vecStdDev, CString strRefStatorPath)
//{
//
//	// Scale Factor(이미지 스케일 인자, 현재, 실제 크기의 1/2, 즉 0.5배 이미지로 검사함)
//	float fSF = 0.5f;
//
//	// Number of grid for internal analysis(4개)
//	int nNx = 4 * nFrameX;
//	int nNy = 4 * nFrameY;
//	// Init
//	vecMean.clear();
//	vecStdDev.clear();
//
//	//#pragma omp parallel for
//	// 벡터 초기화(0.0f값으로 생성)
//	for (int j = 0; j < nNy; j++)
//	{
//		std::vector<float> vecMeanRowM;
//		std::vector<float> vecStdRowM;
//
//		/* Corr */
//		std::vector<float> vecCorrM;
//
//		//#pragma omp parallel for
//		for (int i = 0; i < nNx; i++)
//		{
//			vecMeanRowM.push_back(0.f);
//			vecStdRowM.push_back(0.f);
//
//			/* Corr */
//			vecCorrM.push_back(0.f);
//		}
//		vecMean.push_back(vecMeanRowM);
//		vecStdDev.push_back(vecStdRowM);
//
//	}
//
//	// RefImage 폴더에서 이미지 파일 이름 가져오기
//	std::vector<CString> vecImgName = read_diectory(strRefStatorPath);
//
//	// RefImage 폴더 경로 가져오기
//	CString strPath = strRefStatorPath.Left(strRefStatorPath.ReverseFind('*'));
//
//	// 이미지 생성(NI 이미지)
//	Image* pImg = imaqCreateImage(IMAQ_IMAGE_U8, 3);
//	Image* pImgCrop = imaqCreateImage(IMAQ_IMAGE_U8, 3);
//
//	// 변수 선언
//	// 위치 포인트 구조체 생성
//	int nWd, nHt, nWdSub, nHtSub;
//	Point destLoc = imaqMakePoint(0, 0);
//	Rect rtCrop;
//
//	for (int iImg = 0; iImg < (int)vecImgName.size(); iImg++)
//	{
//		// 이미지 파일 Load
//		CString strImgFullName = strPath + vecImgName[iImg];
//		imaqReadFile(pImg, (CStringA)strImgFullName, NULL, NULL);
//
//		// Quantify for Sub Image Region
//		if (iImg == 0) // 첫번째 이미지일 경우
//		{
//			//imaqWriteJPEGFile(pImg, "../x64/data/stator_img/REF0.jpg", 750, NULL);
//
//			// 이미지 정보 획득
//			ImageInfo info;
//			imaqGetImageInfo(pImg, &info);
//
//			// Process 영상 해상도 계산
//			nWd = (int)((float)info.xRes * fSF);
//			nHt = (int)((float)info.yRes * fSF);
//
//			// 격자 크기로 나눔.(격자당 이미지 해상도 계산)
//			nWdSub = (int)(nWd / nNx);
//			nHtSub = (int)(nHt / nNy);
//			imaqSetImageSize(pImgCrop, nWdSub, nHtSub);
//		}
//
//		// 이미지 크기 수정
//		imaqResample(pImg, pImg, nWd, nHt, IMAQ_BILINEAR, IMAQ_NO_RECT);
//
//		// Edge Filter 적용(시그마 엣지)
//		imaqEdgeFilter(pImg, pImg, IMAQ_EDGE_SIGMA, NULL); //  IMAQ_EDGE_SOBEL, NULL);
//
//
//		// 해당 격자 위치의 Rect 설정(Height, Width)
//		rtCrop.height = nHtSub;
//		rtCrop.width = nWdSub;
//
//		// Compute
////#pragma omp parallel for
//		for (int j = 0; j < nNy; j++)
//		{
//			// 해당 격자 위치의 Rect 계산(top)
//			rtCrop.top = j * nHtSub;
//
//			// 벡터 포인터 획득
//			float* pMeanData = vecMean[j].data();
//			float* pStdDevData = vecStdDev[j].data();
//
//			//#pragma omp parallel for
//			for (int i = 0; i < nNx; i++)
//			{
//				// 해당 격자 위치의 Rect 계산(left)
//				rtCrop.left = i * nWdSub;
//
//				// 원본이미지에서 해당격자의 Image 복사
//				imaqCopyRect(pImgCrop, pImg, rtCrop, destLoc);
//
//				// Quntify 함수 사용하여 이미지 정보 획득
//				QuantifyReport* pQr = imaqQuantify(pImgCrop, NULL);
//
//				// Mean값, 표준편차값 stdDev값 저장 후 Sum
//				pMeanData[i] += pQr->global.mean;
//				pStdDevData[i] += pQr->global.stdDev;
//
//				// 이미지 메모리 해제
//				imaqDispose(pQr);
//			}
//		}
//	}
//
//	// Average, Mean, StdDev를 평균값으로 계산
//	for (int j = 0; j < nNy; j++) {
//		float* pMeanData = vecMean[j].data();
//		float* pStdDevData = vecStdDev[j].data();
//
//		for (int i = 0; i < nNx; i++) {
//			pMeanData[i] /= (float)vecImgName.size();
//			pStdDevData[i] /= (float)vecImgName.size();
//		}
//	}
//
//	// 이미지 데이터 메모리 해제(NI 이미지)
//	imaqDispose(pImg);
//	imaqDispose(pImgCrop);
//}
//
//std::vector<int> CGrabImgDlg::DetectAnorm(Image* pImg, int nFrameX, int nFrameY,
//	std::vector<std::vector<float>>& vecMean,
//	std::vector<std::vector<float>>& vecStdDev,
//	float meanLimit, float stdLimit, BOOL bSaveOverlayImg, CString strSaveFolderPath)
//{
//	// 이미지 데이터 포인터 획득(NI 이미지)
//	Image* pImgBackup = pImg;//imaqCreateImage(IMAQ_IMAGE_U8, 3);
////imaqDuplicate(pImgBackup, pImg);
//
//	// Scale Factor(영상 크기 스케일 인자)
//	float fSF = 0.5f;
//
//	// Number of grid for internal analysis(하나의 프레임당 4 x 4 격자)
//	int nNx = 4 * nFrameX;
//	int nNy = 4 * nFrameY;
//
//	// 이미지 정보 획득
//	ImageInfo info;
//	imaqGetImageInfo(pImg, &info);
//
//	// Process 이미지 해상도 계산(실제 해상도 * 스케일팩터)
//	int img_wd = (int)((float)info.xRes * fSF);
//	int img_ht = (int)((float)info.yRes * fSF);
//
//	// 격자 이미지 포인터 생성(NI 이미지)
//	int wd = (int)(img_wd / nNx);
//	int ht = (int)(img_ht / nNy);
//
//	Image* pImgCrop = imaqCreateImage(IMAQ_IMAGE_U8, 3);
//	imaqSetImageSize(pImgCrop, wd, ht);
//
//	/* Corr */
//	//Image* pImgRefCrop = imaqCreateImage(IMAQ_IMAGE_U8, 3);
//	//imaqSetImageSize(pImgRefCrop, wd, ht);
//
//	// 이미지 크기 수정
//	imaqResample(pImg, pImg, img_wd, img_ht, IMAQ_BILINEAR, IMAQ_NO_RECT);
//
//	// Edge Filter 적용(시그마 엣지)
//	imaqEdgeFilter(pImg, pImg, IMAQ_EDGE_SIGMA, NULL); // IMAQ_EDGE_SOBEL, NULL);
//
//	Point destLoc = imaqMakePoint(0, 0);
//
//	Rect rtCrop;
//	Rect rtAnom;
//	rtAnom.width = (int)((float)80 * fSF);
//	rtAnom.height = rtAnom.width;
//	RGBValue rgbRed, rgbCyan, rgbYellow;
//
//	rgbRed.R = 255;
//	rgbRed.G = rgbRed.B = rgbRed.alpha = 0;
//
//	rgbCyan.G = 221;
//	rgbCyan.R = 22;
//	rgbCyan.B = 237;
//	rgbCyan.alpha = 0;
//
//	rgbYellow.R = rgbYellow.G = 255;
//	rgbYellow.B = rgbYellow.alpha = 0;
//
//	std::vector<int> vecFrameIdx;
//	std::vector<Point> vecAnormCenLoc;
//	vecFrameIdx.clear();
//	vecAnormCenLoc.clear();
//
//	rtCrop.height = ht;
//	rtCrop.width = wd;
//
//	//#pragma omp parallel for
//
//	for (int j = 0; j < nNy; j++)
//	{
//		rtCrop.top = j * ht;
//
//		//#pragma omp parallel for
//		for (int i = 0; i < nNx; i++)
//		{
//			rtCrop.left = i * wd;
//
//			imaqCopyRect(pImgCrop, pImg, rtCrop, destLoc);
//
//			// Statical
//			QuantifyReport* pQr = imaqQuantify(pImgCrop, NULL);
//
//			float diffMean = fabs(vecMean[j][i] - pQr->global.mean);
//			float diffStd = fabs(vecStdDev[j][i] - pQr->global.stdDev);
//			if (diffMean > meanLimit && diffStd > stdLimit)
//			{
//				//rtAnom.left = rtCrop.left + wd / 2 - rtAnom.width / 2;
//				//rtAnom.top = rtCrop.top + ht / 2 - rtAnom.height / 2;
//
//				rtAnom.left = rtCrop.left + wd / 2 - rtAnom.width / 2;
//				rtAnom.top = rtCrop.top + ht / 2 - rtAnom.height / 2;
//
//				vecAnormCenLoc.push_back(imaqMakePoint((rtCrop.left + wd / 2), (rtCrop.top + ht / 2))); // save each crop image center location
//				imaqOverlayRect(pImgBackup, rtAnom, &rgbRed, IMAQ_DRAW_VALUE, NULL);
//			}
//			imaqDispose(pQr);
//		}
//	}
//
//	// identify anorm frame location(Frame Index)
//	int x, y;
//
//	int frame_wd = img_wd / nFrameX;
//	int frame_ht = img_ht / nFrameY;
//	for (int k = 0; k < (int)vecAnormCenLoc.size(); k++)
//	{
//		x = vecAnormCenLoc[k].x;
//		y = vecAnormCenLoc[k].y;
//		int indexI = x / frame_wd;
//		int indexJ = y / frame_ht;
//		int indexF = indexJ * nFrameX + indexI;
//
//		rtAnom.left = indexI * frame_wd + frame_wd / 2 - 40;
//		rtAnom.top = indexJ * frame_ht + frame_ht / 2 - 40;
//
//		//rtAnom.left = rtCrop.left + wd / 2 - rtAnom.width / 2;
//		//rtAnom.top = rtCrop.top + ht / 2 - rtAnom.height / 2;
//
//		// redundency check
//		BOOL bFoundSameIdx = TRUE;
//		if (vecFrameIdx.size() > 0)
//		{
//			for (int q = 0; q < (int)vecFrameIdx.size(); q++)
//			{
//				if (vecFrameIdx[q] == indexF)
//				{
//					bFoundSameIdx = TRUE;
//					break;
//				}
//				else
//					bFoundSameIdx = FALSE;
//			}
//
//			if (!bFoundSameIdx)
//			{
//				imaqOverlayRect(pImgBackup, rtAnom, &rgbYellow, IMAQ_PAINT_VALUE, NULL);
//				vecFrameIdx.push_back(indexF);
//			}
//		}
//		else
//		{
//			imaqOverlayRect(pImgBackup, rtAnom, &rgbYellow, IMAQ_PAINT_VALUE, NULL);
//			vecFrameIdx.push_back(indexF);
//		}
//	}
//
//	if (bSaveOverlayImg)
//	{
//		// grid line
//		Point ptS, ptE;
//		for (int j = 0; j < nFrameY; j++)
//		{
//			ptS.x = 0; ptE.x = img_wd;
//			ptS.y = j * frame_ht;
//			ptE.y = ptS.y;
//			imaqOverlayLine(pImgBackup, ptS, ptE, &rgbCyan, NULL);
//		}
//
//		for (int i = 0; i < nFrameX; i++)
//		{
//			ptS.y = 0; ptE.y = img_ht;
//			ptS.x = i * frame_wd;
//			ptE.x = ptS.x;
//			imaqOverlayLine(pImgBackup, ptS, ptE, &rgbCyan, NULL);
//		}
//
//		CString strImgFullName = strSaveFolderPath;
//		strImgFullName += L"\\";
//		strImgFullName += L"Anorm.jpg";
//
//		SaveJPEGIncludeOverlay(pImgBackup, (CStringA)strImgFullName);
//	}
//
//	imaqDispose(pImgCrop);
//	//imaqDispose(pImgRefCrop);
//	//imaqDispose(pImgBackup);
//
//	return vecFrameIdx;
//}
//
//BOOL CGrabImgDlg::IsRescan(int nBlkIndexX, int nBlkIndexY)
//{
//	BOOL bRescanEnable = FALSE;
//	int nBlockX, nBlockY;
//	int iCurLensIdx = m_pDoc->m_config.m_iCurrLens;
//	nBlockX = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibX;
//	nBlockY = m_pDoc->m_config.m_arCorrParam[iCurLensIdx].iCalibY;
//
//	int nBlockIndex = nBlkIndexY * nBlockX + nBlkIndexX;
//
//	for (int i = 0; i < m_vecFrameIdx.size(); i++)
//	{
//		if (nBlockIndex == m_vecFrameIdx[i])
//		{
//			bRescanEnable = TRUE;
//			break;
//		}
//	}
//
//	
//
//	return bRescanEnable;
//}
//
//int CGrabImgDlg::SaveJPEGIncludeOverlay(Image* pOverlayImg, CStringA strPath)
//{
//	if (pOverlayImg == NULL) return -1;
//	Image * pImg = imaqCreateImage(IMAQ_IMAGE_RGB, 3);
//	if (imaqMergeOverlay(pImg, pOverlayImg, NULL, 0, NULL) == 0)
//	{
//		imaqDispose(pImg);
//		return -2;
//	}
//	if (imaqWriteJPEGFile(pImg, strPath, 750, NULL) == 0)
//	{
//		imaqDispose(pImg);
//		return -3;
//	}
//	imaqDispose(pImg);
//	return 0;
//}
//
//// sjyi 2023.07.16 함수 인자 변경
//// 2D Float Vector 선연하여 사용 
//void CGrabImgDlg::CalcBoundContiFromRefImages(int nFrameX, int nFrameY,
//	VEC_2D_FLOAT& vecBoundContiX,
//	VEC_2D_FLOAT& vecBoundContiY,
//	VEC_2D_FLOAT& vecBoundContiX_Std,
//	VEC_2D_FLOAT& vecBoundContiY_Std,
//	CString strRefStatorPath)
//{
//	// Number of grid for internal analysis
//	float fSF = 0.5f;
//
//	int nNx = nFrameX;
//	int nNy = nFrameY;
//	// Init
//	vecBoundContiX.clear();
//	vecBoundContiY.clear();
//
//	vecBoundContiX_Std.clear();
//	vecBoundContiY_Std.clear();
//
//
//	//#pragma omp parallel for
//	for (int j = 0; j < nNy; j++)
//	{
//		std::vector<float> vecBCX;
//		std::vector<float> vecBCY;
//
//		std::vector<float> vecBCX_Std;
//		std::vector<float> vecBCY_Std;
//
//		//#pragma omp parallel for
//		for (int i = 0; i < nNx; i++)
//		{
//			vecBCX.push_back(0.f);
//			vecBCY.push_back(0.f);
//
//			vecBCX_Std.push_back(0.f);
//			vecBCY_Std.push_back(0.f);
//
//		}
//
//		vecBoundContiX.push_back(vecBCX);
//		vecBoundContiY.push_back(vecBCY);
//
//		vecBoundContiX_Std.push_back(vecBCX_Std);
//		vecBoundContiY_Std.push_back(vecBCY_Std);
//
//	}
//
//	std::vector<CString> vecImgName = read_diectory(strRefStatorPath);
//	CString strPath = strRefStatorPath.Left(strRefStatorPath.ReverseFind('*'));
//
//	Image* pImgRS = NULL;
//	Image* pImg = imaqCreateImage(IMAQ_IMAGE_U8, 3);
//	Image* pImgCrop = imaqCreateImage(IMAQ_IMAGE_U8, 3);
//
//	int nWd, nHt, nWdSub, nHtSub;
//	Point destLoc = imaqMakePoint(0, 0);
//	Rect rtCrop;
//
//	if (fSF < 1.0f)
//		pImgRS = imaqCreateImage(IMAQ_IMAGE_U8, 3);
//
//	for (int iImg = 0; iImg < (int)vecImgName.size(); iImg++)
//	{
//		CString strImgFullName = strPath + vecImgName[iImg];
//		imaqReadFile(pImg, (CStringA)strImgFullName, NULL, NULL);
//
//		// Quantify for Sub Image Region
//		if (iImg == 0)
//		{
//			ImageInfo info;
//			imaqGetImageInfo(pImg, &info);
//
//			nWd = (int)((float)info.xRes * fSF);
//			nHt = (int)((float)info.yRes * fSF);
//
//			nWdSub = (int)(nWd / nNx);
//			nHtSub = (int)(nHt / nNy);
//
//			imaqSetImageSize(pImgCrop, nWdSub, nHtSub);
//		}
//		imaqResample(pImg, pImg, nWd, nHt, IMAQ_BILINEAR, IMAQ_NO_RECT);
//		imaqEdgeFilter(pImg, pImg, IMAQ_EDGE_SIGMA, NULL); //  IMAQ_EDGE_SOBEL, NULL);
//
//
//		// 1. Compute Boundary Continuity Along X direction 
////#pragma omp parallel for
//		for (int j = 0; j < nNy; j++)
//		{
//			rtCrop.top = j * nHtSub;
//
//			rtCrop.height = nHtSub;
//			rtCrop.width = nWdSub;
//
//			float* pBCX_Data = vecBoundContiX[j].data();
//			float* pBCX_Std_Data = vecBoundContiX_Std[j].data();
//
//			//#pragma omp parallel for
//			for (int i = 0; i < nNx - 1; i++)
//			{
//				rtCrop.left = i * nWdSub + nWdSub / 2;
//
//				imaqCopyRect(pImgCrop, pImg, rtCrop, destLoc);
//				QuantifyReport* pQr = imaqQuantify(pImgCrop, NULL);
//
//				pBCX_Data[i] += pQr->global.mean;
//				pBCX_Std_Data[i] += pQr->global.stdDev;
//				imaqDispose(pQr);
//			}
//		}
//
//		// 2. Compute Boundary Continuity Along Y direction 
////#pragma omp parallel for
//		for (int j = 0; j < nNy - 1; j++)
//		{
//			rtCrop.top = j * nHtSub + nHtSub / 2;
//
//			rtCrop.height = nHtSub;
//			rtCrop.width = nWdSub;
//
//			float* pBCY_Data = vecBoundContiY[j].data();
//			float* pBCY_Std_Data = vecBoundContiY_Std[j].data();
//
//			//#pragma omp parallel for
//			for (int i = 0; i < nNx; i++)
//			{
//				rtCrop.left = i * nWdSub;
//
//				imaqCopyRect(pImgCrop, pImg, rtCrop, destLoc);
//				QuantifyReport* pQr = imaqQuantify(pImgCrop, NULL);
//
//				pBCY_Data[i] += pQr->global.mean;
//				pBCY_Std_Data[i] += pQr->global.stdDev;
//				imaqDispose(pQr);
//			}
//		}
//	}
//
//	// Average w.r.t # Of Ref Images
//	for (int j = 0; j < nNy; j++) {
//		float* pBCX_Data = vecBoundContiX[j].data();
//		float* pBCY_Data = vecBoundContiY[j].data();
//
//		float* pBCX_Std_Data = vecBoundContiX_Std[j].data();
//		float* pBCY_Std_Data = vecBoundContiY_Std[j].data();
//
//
//		for (int i = 0; i < nNx; i++) {
//			pBCX_Data[i] /= (float)vecImgName.size();
//			pBCY_Data[i] /= (float)vecImgName.size();
//			pBCX_Std_Data[i] /= (float)vecImgName.size();
//			pBCY_Std_Data[i] /= (float)vecImgName.size();
//
//		}
//	}
//
//	imaqDispose(pImg);
//	imaqDispose(pImgCrop);
//}
//
//// sjyi 2023.07.16 함수 인자 변경
//// 2D Float Vector 선연하여 사용 
//std::vector<int> CGrabImgDlg::DetectAnormByBoundConti(Image* pImg, int nFrameX, int nFrameY,
//	VEC_2D_FLOAT vecBoundContiX,
//	VEC_2D_FLOAT vecBoundContiY,
//	VEC_2D_FLOAT vecBoundContiX_Std,
//	VEC_2D_FLOAT vecBoundContiY_Std,
//	float meanLimitX, float meanLimitY, float stdLimitX, float stdLimitY,
//	BOOL bSaveOverlayImg, CString strSaveFolderPath)
//{
//	Image* pImgBackup = pImg;//imaqCreateImage(IMAQ_IMAGE_U8, 3);
//	//imaqDuplicate(pImgBackup, pImg);
//
//	float fSF = 0.5f;
//
//	// Number of grid for internal analysis
//	int nNx = nFrameX;
//	int nNy = nFrameY;
//
//	ImageInfo info;
//	imaqGetImageInfo(pImg, &info);
//
//	int img_wd = (int)((float)info.xRes * fSF);
//	int img_ht = (int)((float)info.yRes * fSF);
//
//	int wd = (int)(img_wd / nNx);
//	int ht = (int)(img_ht / nNy);
//
//	Image* pImgCrop = imaqCreateImage(IMAQ_IMAGE_U8, 3);
//	imaqSetImageSize(pImgCrop, wd, ht);
//
//	imaqResample(pImg, pImg, img_wd, img_ht, IMAQ_BILINEAR, IMAQ_NO_RECT);
//	imaqEdgeFilter(pImg, pImg, IMAQ_EDGE_SIGMA, NULL); //  IMAQ_EDGE_SOBEL, NULL);
//
//
//
//	Point destLoc = imaqMakePoint(0, 0);
//
//	Rect rtCrop;
//	Rect rtAnom;
//	rtAnom.width = (int)((float)80 * fSF);
//	rtAnom.height = rtAnom.width;
//	RGBValue rgbRed, rgbCyan, rgbYellow;
//
//	rgbRed.R = 255;
//	rgbRed.G = rgbRed.B = rgbRed.alpha = 0;
//
//	rgbCyan.G = 221;
//	rgbCyan.R = 22;
//	rgbCyan.B = 237;
//	rgbCyan.alpha = 0;
//
//	rgbYellow.R = rgbYellow.G = 255;
//	rgbYellow.B = rgbYellow.alpha = 0;
//
//	std::vector<int> vecFrameIdx;
//	std::vector<Point> vecAnormCenLoc;
//	std::vector<int> vecAnormCenLocDir; // 0:X-Dir, 1:Y-Dir
//
//
//	vecFrameIdx.clear();
//	vecAnormCenLoc.clear();
//	vecAnormCenLocDir.clear();
//
//	rtCrop.height = ht;
//	rtCrop.width = wd;
//
//	//#pragma omp parallel for
//	// 1. Compute BC Along X Dir
//	for (int j = 0; j < nNy; j++)
//	{
//		rtCrop.top = j * ht;
//
//		//#pragma omp parallel for
//		for (int i = 0; i < nNx - 1; i++)
//		{
//			rtCrop.left = i * wd + wd / 2;
//
//			imaqCopyRect(pImgCrop, pImg, rtCrop, destLoc);
//
//			// Statical
//			QuantifyReport* pQr = imaqQuantify(pImgCrop, NULL);
//
//			// sjyi 2023.07.16 함수 인자를 사용하여 계산하도록 변경 
//			//float diffBCX = fabs(m_BoundContiX[j][i] - pQr->global.mean);
//			//float diffBCX_Std = fabs(m_BoundContiX_Std[j][i] - pQr->global.stdDev);
//			float diffBCX = fabs(vecBoundContiX[j][i] - pQr->global.mean);
//			float diffBCX_Std = fabs(vecBoundContiX_Std[j][i] - pQr->global.stdDev);
//			if (diffBCX > meanLimitX || diffBCX_Std > stdLimitX)
//			{
//				rtAnom.left = rtCrop.left + wd / 2 - rtAnom.width / 2;
//				rtAnom.top = rtCrop.top + ht / 2 - rtAnom.height / 2;
//
//				vecAnormCenLoc.push_back(imaqMakePoint((rtCrop.left + wd / 2), (rtCrop.top + ht / 2))); // save each crop image center location
//				vecAnormCenLocDir.push_back(0);
//				if (bSaveOverlayImg)
//					imaqOverlayRect(pImgBackup, rtAnom, &rgbRed, IMAQ_DRAW_VALUE, NULL);
//			}
//			imaqDispose(pQr);
//		}
//	}
//
//	// 2. Compute BC Along Y Dir
//	for (int j = 0; j < nNy - 1; j++)
//	{
//		rtCrop.top = j * ht + ht / 2;
//
//		//#pragma omp parallel for
//		for (int i = 0; i < nNx; i++)
//		{
//			rtCrop.left = i * wd;
//
//			imaqCopyRect(pImgCrop, pImg, rtCrop, destLoc);
//
//			// Statical
//			QuantifyReport* pQr = imaqQuantify(pImgCrop, NULL);
//
//			// sjyi 2023.07.16 함수 인자를 사용하여 계산하도록 변경 
//			//float diffBCY = fabs(m_BoundContiY[j][i] - pQr->global.mean);
//			//float diffBCY_Std = fabs(m_BoundContiY_Std[j][i] - pQr->global.stdDev);
//			float diffBCY = fabs(vecBoundContiY[j][i] - pQr->global.mean);
//			float diffBCY_Std = fabs(vecBoundContiY_Std[j][i] - pQr->global.stdDev);
//			if (diffBCY > meanLimitY || diffBCY_Std > stdLimitY)
//			{
//				rtAnom.left = rtCrop.left + wd / 2 - rtAnom.width / 2;
//				rtAnom.top = rtCrop.top + ht / 2 - rtAnom.height / 2;
//
//				vecAnormCenLoc.push_back(imaqMakePoint((rtCrop.left + wd / 2), (rtCrop.top + ht / 2))); // save each crop image center location
//				vecAnormCenLocDir.push_back(1);
//
//				if (bSaveOverlayImg)
//					imaqOverlayRect(pImgBackup, rtAnom, &rgbRed, IMAQ_DRAW_VALUE, NULL);
//			}
//			imaqDispose(pQr);
//		}
//	}
//
//
//	// identify anorm frame location(Frame Index)
//	int x, y;
//
//	int frame_wd = img_wd / nFrameX;
//	int frame_ht = img_ht / nFrameY;
//	int indexI, indexJ, indexF1, indexF2;
//	int indexI1, indexI2, indexJ1, indexJ2;
//	Rect rtAnom1, rtAnom2;
//	rtAnom1.height = rtAnom1.width = rtAnom2.height = rtAnom2.width = rtAnom.height;
//	for (int k = 0; k < (int)vecAnormCenLoc.size(); k++)
//	{
//
//		x = vecAnormCenLoc[k].x;
//		y = vecAnormCenLoc[k].y;
//		if (vecAnormCenLocDir[k] == 0)
//		{
//			indexI = x / frame_wd;
//
//			if (indexI*frame_wd >= x)
//			{
//				indexI1 = indexI - 1;
//				indexI2 = indexI;
//				if (indexI1 <= 0)
//					indexI = 0;
//
//			}
//			else {
//
//				indexI1 = indexI;
//				indexI2 = indexI + 1;
//				if (indexI2 > nFrameX - 1)
//					indexI2 = nFrameX - 1;
//			}
//
//
//			indexJ = y / frame_ht;
//
//			indexF1 = indexJ * nFrameX + indexI1;
//			indexF2 = indexJ * nFrameX + indexI2;
//
//			rtAnom1.left = indexI1 * frame_wd + frame_wd / 2 - rtAnom1.width / 2;
//			rtAnom1.top = indexJ * frame_ht + frame_ht / 2 - rtAnom1.height / 2;
//			rtAnom2.left = indexI2 * frame_wd + frame_wd / 2 - rtAnom2.width / 2;
//			rtAnom2.top = indexJ * frame_ht + frame_ht / 2 - rtAnom2.height / 2;
//		}
//		else
//		{
//			indexI = x / frame_wd;
//
//			int indexJ = y / frame_ht;
//			if (indexJ*frame_ht >= y)
//			{
//				indexJ1 = indexJ - 1;
//				indexJ2 = indexJ;
//				if (indexJ1 <= 0)
//					indexJ1 = 0;
//
//			}
//			else {
//
//				indexJ1 = indexJ;
//				indexJ2 = indexJ + 1;
//				if (indexJ2 > nFrameY - 1)
//					indexJ2 = nFrameY - 1;
//			}
//
//			indexF1 = indexJ1 * nFrameX + indexI;
//			indexF2 = indexJ2 * nFrameX + indexI;
//
//			rtAnom1.left = indexI * frame_wd + frame_wd / 2 - rtAnom1.width / 2;
//			rtAnom1.top = indexJ1 * frame_ht + frame_ht / 2 - rtAnom1.height / 2;
//			rtAnom2.left = indexI * frame_wd + frame_wd / 2 - rtAnom2.width / 2;
//			rtAnom2.top = indexJ2 * frame_ht + frame_ht / 2 - rtAnom2.height / 2;
//		}
//
//		// redundency check
//		BOOL bFoundSameIdx1 = TRUE;
//		BOOL bFoundSameIdx2 = TRUE;
//		if (vecFrameIdx.size() > 0)
//		{
//			for (int q = 0; q < (int)vecFrameIdx.size(); q++)
//			{
//				if (vecFrameIdx[q] == indexF1)
//				{
//					bFoundSameIdx1 = TRUE;
//					break;
//				}
//				else
//					bFoundSameIdx1 = FALSE;
//
//				if (vecFrameIdx[q] == indexF2)
//				{
//					bFoundSameIdx2 = TRUE;
//					break;
//				}
//				else
//					bFoundSameIdx2 = FALSE;
//			}
//
//			if (!bFoundSameIdx1)
//			{
//				if (bSaveOverlayImg)
//					imaqOverlayRect(pImgBackup, rtAnom1, &rgbYellow, IMAQ_PAINT_VALUE, NULL);
//				vecFrameIdx.push_back(indexF1);
//			}
//
//			if (!bFoundSameIdx2)
//			{
//				if (bSaveOverlayImg)
//					imaqOverlayRect(pImgBackup, rtAnom2, &rgbYellow, IMAQ_PAINT_VALUE, NULL);
//				vecFrameIdx.push_back(indexF2);
//			}
//		}
//		else
//		{
//			if (bSaveOverlayImg)
//				imaqOverlayRect(pImgBackup, rtAnom1, &rgbYellow, IMAQ_PAINT_VALUE, NULL);
//			vecFrameIdx.push_back(indexF1);
//
//			if (bSaveOverlayImg)
//				imaqOverlayRect(pImgBackup, rtAnom2, &rgbYellow, IMAQ_PAINT_VALUE, NULL);
//			vecFrameIdx.push_back(indexF2);
//		}
//	}
//
//	if (bSaveOverlayImg)
//	{
//		// grid line
//		Point ptS, ptE;
//		for (int j = 0; j < nFrameY; j++)
//		{
//			ptS.x = 0; ptE.x = img_wd;
//			ptS.y = j * frame_ht;
//			ptE.y = ptS.y;
//			imaqOverlayLine(pImgBackup, ptS, ptE, &rgbCyan, NULL);
//		}
//
//		for (int i = 0; i < nFrameX; i++)
//		{
//			ptS.y = 0; ptE.y = img_ht;
//			ptS.x = i * frame_wd;
//			ptE.x = ptS.x;
//			imaqOverlayLine(pImgBackup, ptS, ptE, &rgbCyan, NULL);
//		}
//
//		CString strImgFullName = strSaveFolderPath;
//		strImgFullName += L"\\";
//		strImgFullName += L"Anorm.jpg";
//
//		SaveJPEGIncludeOverlay(pImgBackup, (CStringA)strImgFullName);
//	}
//
//	imaqDispose(pImgCrop);
//	//imaqDispose(pImgBackup);
//
//	return vecFrameIdx;
//}
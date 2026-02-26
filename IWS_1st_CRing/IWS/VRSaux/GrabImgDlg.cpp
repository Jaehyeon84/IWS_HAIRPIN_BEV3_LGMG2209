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
	m_pScanImgsProc->ChangeScanRange(resolX, resolY, sx, sy, ex, ex);

	/* */
	m_pGrapThread = AfxBeginThread(GrabImgThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);
	g_bCameraStateOK = FALSE;

	InitializeCriticalSection(&G_cs);
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

	DeleteCriticalSection(&G_cs);
}

void CGrabImgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGrabImgDlg, CDialogEx)
//ON_WM_DESTROY()
END_MESSAGE_MAP()


// GrabImgDlg 메시지 처리기
UINT CGrabImgDlg::GrabImgThread(LPVOID pArgs)
{
	CGrabImgDlg* me = (CGrabImgDlg*)pArgs;
	me->Grabimg();
//	me->Grabimg_Simul();
	return 0;
}

void CGrabImgDlg::Grabimg()
{
	Pylon::PylonTerminate();
	Sleep(200);
	g_Util.Logging(L"-- Basler Acq. Thread Started ! -- \n");
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
	_putenv("PYLON_GIGE_HEARTBEAT=3000");

//#endif

											// Automagically call PylonInitialize and PylonTerminate to ensure the pylon runtime system
											// is initialized during the lifetime of this object
	Pylon::PylonTerminate();
	Pylon::PylonAutoInitTerm autoInitTerm;
	bool bPaint = true;
	try
	{
		// Get the transport layer factory
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

#define TIME_OUT 10000
		long told = clock();
		BOOL bRes = FALSE;
		while (bRes == FALSE) {
	
			if (pTl->EnumerateDevices(devices) != 0) {
				bRes = TRUE;
			}

			if (clock() - told > TIME_OUT) {
				_tracer(TRACE_NORMAL, 1, _T("No camera present"));
				bRes = FALSE;
				break;
			}
			Sleep(500);
		}
		//if (0 == pTl->EnumerateDevices(devices))
		if (bRes == FALSE)
		{
			g_Util.Logging(L"No camera present!");
			g_bCameraStateOK = FALSE;
			return;
		}

		// Create the camera object of the first available camera.
		// The camera object is used to set and get all available
		// camera features.
		Camera_t Camera(pTl->CreateDevice(devices[0]));
		INodeMap* pTlNodeMap = Camera.GetTLNodeMap();
		if (NULL == pTlNodeMap)
		{
			g_Util.Logging(L"This camera class doesn’t provide a transport layer nodemap");
			g_bCameraStateOK = FALSE;
			return;
		}
		Camera_t::TlParams_t TlParams = pTlNodeMap;

		//TlParams.HeartbeatTimeout.SetValue(30000); // 30 sec timeout
		TlParams.HeartbeatTimeout.SetValue(3000);
												   // Open the camera
		Camera.Open();
		int nTempBinning = 1;
		Camera.BinningHorizontal.SetValue((size_t)nTempBinning);
		Camera.BinningVertical.SetValue((size_t)nTempBinning);
		Camera.Height.SetValue(m_nGrabHt / nTempBinning);
		Camera.Width.SetValue(m_nGrabWd / nTempBinning);
		//		Camera.AcquisitionFrameRateAbs.SetValue((double)m_nFrameRate);
		//		Camera.ExposureTimeRaw.SetValue((size_t)9000);

				// Get the first stream grabber object of the selected camera
		Camera_t::StreamGrabber_t StreamGrabber(Camera.GetStreamGrabber(0));

		// Open the stream grabber
		StreamGrabber.Open();

		// Set the image format and AOI
		Camera.PixelFormat.SetValue(PixelFormat_Mono8);
		Camera.CenterX.SetValue(0);
		Camera.CenterY.SetValue(0);
		Camera.OffsetX.SetValue(0);
		Camera.OffsetY.SetValue(0);

		//Disable acquisition start trigger if available
		{
			GenApi::IEnumEntry* acquisitionStart = Camera.TriggerSelector.GetEntry(TriggerSelector_AcquisitionStart);
			bool available = GenApi::IsAvailable(acquisitionStart);
			if (acquisitionStart && GenApi::IsAvailable(acquisitionStart))
			{
				Camera.TriggerSelector.SetValue(TriggerSelector_AcquisitionStart);
				Camera.TriggerMode.SetValue(TriggerMode_Off);
			}
		}

		//Disable frame start trigger if available
		{
			GenApi::IEnumEntry* frameStart = Camera.TriggerSelector.GetEntry(TriggerSelector_FrameStart);
			if (frameStart && GenApi::IsAvailable(frameStart))
			{
				Camera.TriggerSelector.SetValue(TriggerSelector_FrameStart);
				Camera.TriggerMode.SetValue(TriggerMode_Off);
			}
		}

		//Set acquisition mode
		Camera.AcquisitionMode.SetValue(AcquisitionMode_Continuous);

		// Get the image buffer size
		const size_t ImageSize = (size_t)(Camera.PayloadSize.GetValue());

		// We won't use image buffers greater than ImageSize
		StreamGrabber.MaxBufferSize.SetValue(ImageSize);

		// We won't queue more than c_nBuffers image buffers at a time
		StreamGrabber.MaxNumBuffer.SetValue((uint32_t)m_nCamBuf);

		// Allocate all resources for grabbing. Critical parameters like image
		// size now must not be changed until FinishGrab() is called.
		StreamGrabber.PrepareGrab();

		// Buffers used for grabbing must be registered at the stream grabber.
		// The registration returns a handle to be used for queuing the buffer.
		std::vector<CGrab2DBuffer*> BufferList;
		for (int i = 0; i < m_nCamBuf; ++i)
		{
			CGrab2DBuffer *pGrabBuffer = new CGrab2DBuffer(ImageSize);
			pGrabBuffer->SetBufferHandle(StreamGrabber.RegisterBuffer(
				pGrabBuffer->GetBufferPointer(), ImageSize));

			// Put the grab buffer object into the buffer list
			BufferList.push_back(pGrabBuffer);
		}

		for (std::vector<CGrab2DBuffer*>::const_iterator x = BufferList.begin(); x != BufferList.end(); ++x)
		{
			// Put buffer into the grab queue for grabbing
			StreamGrabber.QueueBuffer((*x)->GetBufferHandle(), NULL);
		}

		// Let the camera acquire images continuously ( Acquisiton mode equals
		// Continuous! )
		Camera.AcquisitionStart.Execute();

		//	g_nExposureTime = nExposureTime = (int)Camera.ExposureTimeRaw.GetValue();
		//	g_nGainRaw = nGainRaw = (int)Camera.GainRaw.GetValue();
		//
		UINT nPushCnt = 0;
		g_bGrabThreadAlive = true;

		// to calc average frame rate
		int accuFrameCnt = 0;
		double timeElasped;
		LARGE_INTEGER freq, timeBegin, timeEnd;
		QueryPerformanceFrequency(&freq);

		::SetEvent(g_hEventSrtCopyDispData);
		DWORD ret;
		GrabResult grabResult;
		uint8_t *pImageBuffer;

		EGrabStatus grab_status;
		unsigned char *s, *d;

		BOOL bGrabTried = FALSE;
		BOOL bInitDisplay = FALSE;
		while (g_bGrabThreadAlive)
		{
			if (m_bDoScan)
			{
				ret = ::WaitForSingleObject(m_hEvScannerMoveDone, 5);
				grab_status = Grab(&StreamGrabber, &grabResult);
				bGrabTried = TRUE;
				switch (ret)
				{
				case WAIT_TIMEOUT:
					//						Sleep(5);
					break;
				case WAIT_OBJECT_0:
					if (grab_status == Grabbed)
					{
						g_bCameraStateOK = TRUE;
						pImageBuffer = (uint8_t *)grabResult.Buffer();
						m_pScanImgsProc->m_pScanDataQue->Push(pImageBuffer);

						SetEvent(g_hEventPushQueueDone);
						if (nPushCnt == 0)
							::SetEvent(g_hEventPopThreadSrt);

						++nPushCnt;
					}
					else
					{
						g_bCameraStateOK = FALSE;
						g_Util.Logging(L"Grab in Scan Process Failed !");
					}

					if (nPushCnt >= m_pScanImgsProc->m_nBlockX * m_pScanImgsProc->m_nBlockY)
					{
						m_bDoScan = FALSE;
						nPushCnt = 0;
						if (bGrabTried)
						{
							StreamGrabber.QueueBuffer(grabResult.Handle(), NULL);
							bGrabTried = FALSE;
						}
					}
					break;
				}
			}

			if (g_bLiveDisplay)
			{
				if (!bInitDisplay)
				{
					QueryPerformanceCounter(&timeBegin);
					bInitDisplay = TRUE;
				}

				ret = ::WaitForSingleObject(g_hEventSrtCopyDispData, 5);
				if (!m_bDoScan)
				{
					grab_status = Grab(&StreamGrabber, &grabResult);
					bGrabTried = TRUE;
				}

				switch (ret) // display 용 m_pDisplayimgData에 영상을 1/4로 줄여 복사
				{
				case WAIT_TIMEOUT:
					//					Sleep(5);
					break;
				case WAIT_OBJECT_0:
					if (grab_status == Grabbed)
					{
						g_bCameraStateOK = TRUE;

						pImageBuffer = (uint8_t *)grabResult.Buffer();
						s = pImageBuffer;	// address 초기화
						d = m_pLiveCCDDlg->m_pDisplayimgData;  // address 초기화
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

						else
							g_Util.Logging(L"No support format !\n");

					}
					else
					{
						g_bCameraStateOK = FALSE;
						memset(m_pLiveCCDDlg->m_pDisplayimgData, 0, int(m_nGrabWd*m_nGrabHt / 4) * sizeof(BYTE));
						g_Util.Logging(L"Grab in Live Display Process Failed !");
					}

					// display on Ni window
					//::PostMessage(m_pDispImgProc->m_hWnd, UWM_DISPIMG, 0, 0);
					::SetEvent(g_hEventDisplayStart);
					break;
				} //switch loop

				// calc acq frame rate
				QueryPerformanceCounter(&timeEnd);
				timeElasped = ((double)(timeEnd.QuadPart - timeBegin.QuadPart) / (double)freq.QuadPart); // in sec
				g_AvgFrmRate = (double)(++accuFrameCnt) / timeElasped;
				if (accuFrameCnt > 1e+6) // counter & timer reset
				{
					accuFrameCnt = 0;
					timeElasped = 0;
					QueryPerformanceCounter(&timeBegin);
				}
			}
			else
			{
				Sleep(100); // doing nothing, so let a thread run slowly
				if (bInitDisplay)
					bInitDisplay = FALSE;
			}

			if (m_bSnapImg)
			{
				if (!bGrabTried)
				{
					grab_status = Grab(&StreamGrabber, &grabResult); //DHChoi 20200630
					bGrabTried = TRUE;
				}

				if (grab_status == Grabbed)
				{
					g_bCameraStateOK = TRUE;
					pImageBuffer = (uint8_t *)grabResult.Buffer();
					memcpy(m_pSnapData, pImageBuffer, sizeof(unsigned char) * ImageSize);
					m_bSnapImg = FALSE;
				}
				else
				{
					g_bCameraStateOK = FALSE;
					memcpy(m_pSnapData, 0, sizeof(unsigned char) * ImageSize);
					g_Util.Logging(L"Grab in Snap Process Failed !\n");
				}

				::SetEvent(m_hEventSnapimg);
			}

			// g_Util.Logging(L"-----%d ---- %f----%f-\n ", accuFrameCnt, timeElasped, g_AvgFrmRate);
			// Reuse the buffer for grabbing the next image
			//if (n < c_ImagesToGrab - c_nBuffers)
			if (bGrabTried)
			{
				StreamGrabber.QueueBuffer(grabResult.Handle(), NULL);
				bGrabTried = FALSE;
			}
		}

		// Get the pending buffer back (You are not allowed to deregister
		// buffers when they are still queued)
		StreamGrabber.CancelGrab();

		// Get all buffers back
		for (GrabResult r; StreamGrabber.RetrieveResult(r););

		// Stop acquisition
		Camera.AcquisitionStop.Execute();

		// Clean up
		// You must deregister the buffers before freeing the memory
		for (std::vector<CGrab2DBuffer*>::iterator it = BufferList.begin(); it != BufferList.end(); it++)
		{
			StreamGrabber.DeregisterBuffer((*it)->GetBufferHandle());
			delete *it;
			*it = NULL;
		}

		// Free all resources used for grabbing
		StreamGrabber.FinishGrab();

		// Close stream grabber
		StreamGrabber.Close();

		// Close camera
		Camera.Close();
	}
	catch (GenICam::GenericException &e)
	{
		// Error handling
		CString err;
		err.Format(L"An exception occurred!: %s\n", e.GetDescription());
		g_Util.Logging(err);
		g_bCameraStateOK = FALSE;
		return;
	}

	// Exit 
	SetEvent(m_hEventGrabThreadEnd);
	g_Util.Logging(L"-- Basler Acq. Thread Ended ! -- \n");
}

EGrabStatus CGrabImgDlg::Grab(Camera_t::StreamGrabber_t *pStreamGrabber, GrabResult* pGrabResult)
{
	// Wait for the grabbed image with timeout of 300 seconds
	pStreamGrabber->GetWaitObject().Wait(30000); 

	// Get the grab result from the grabber's result queue
	pStreamGrabber->RetrieveResult(*pGrabResult);

	return pGrabResult->Status();
}

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

void CGrabImgDlg::ScanStart()
{	
	DWORD dwStart = GetTickCount();
	m_bDoScan = TRUE;

	_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] Start--------------->");

	int nBlockX = m_pScanImgsProc->m_nBlockX;
	int nBlockY = m_pScanImgsProc->m_nBlockY;
	POINTF pt;

	int cnt = -1;
	BOOL bRet;
	for (int j = 0; j < nBlockY; j++)
	{
		if (j % 2 == 0)
		{
			for (int i = 0; i < nBlockX; i++)
			{
				pt = m_pScanImgsProc->m_ptScanCoord.at(j*nBlockX + i);

				// Scanner Move
				bRet = MoveScannerTo((double)pt.x, (double)pt.y); // x,y 이송 좌표
				if (i == 0)	Sleep(30);
				else Sleep(30);

				::SetEvent(m_hEvScannerMoveDone); // push implemented at grab thread(see GrabImgDlg.h)

				// wait somehow push is done
				::WaitForSingleObject(g_hEventPushQueueDone, INFINITE);
			}
		}
		else
		{
			for (int i = nBlockX - 1; i >= 0; i--)
			{
				pt = m_pScanImgsProc->m_ptScanCoord.at(j*nBlockX + i);

				// Scanner Move
				bRet = MoveScannerTo((double)pt.x, (double)pt.y); // x,y 이송 좌표
				if (i == nBlockX - 1)	Sleep(30);
				else Sleep(30);

				::SetEvent(m_hEvScannerMoveDone); // push implemented at grab thread(see GrabImgDlg.h)

				// wait somehow push is done
				::WaitForSingleObject(g_hEventPushQueueDone, INFINITE);
			}
		}
	}
	DWORD dwScanTime = GetTickCount() - dwStart;
	_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] Image Scan is Done");

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

		if (m_pDoc->m_config.m_bSaveImage)
		{
			// 2024.02.11 jh.kim 추가 - 저장 이미지에 좌표 표시
			m_pDoc->SaveImageAndPos();
			_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] Save Image and Pos");
		}

		SetFlagStitchImageDone(TRUE);
	}
	DWORD dwScanAllTime = GetTickCount() - dwStart;
	TRACE(_T("\nScanCnt = %d, ScanTime = %d msec, Scan+calib+merge = %d msec\t\t"), nBlockX*nBlockY, (int)dwScanTime, (int)dwScanAllTime);
	_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] ScanCnt = %d, ScanTime = %d msec, Scan+calib+merge = %d msec\t\t", nBlockX*nBlockY, (int)dwScanTime, (int)dwScanAllTime);
	_tracer(TRACE_NORMAL, 9999, L"[ScanStart()] Complete <---------------");
}

void CGrabImgDlg::ScanAndSave(CString strStitchImg, BOOL bScan, int* widthPxl, int* heightPxl,
	float* widthLength, float* heightLength)
{
	*widthPxl = m_pScanImgsProc->m_stitchImg.nWd / (int)m_pScanImgsProc->m_dsf;
	*heightPxl = m_pScanImgsProc->m_stitchImg.nHt / (int)m_pScanImgsProc->m_dsf;
	*widthLength = m_pScanImgsProc->m_fCurFullFovX;
	*heightLength = m_pScanImgsProc->m_fCurFullFovY;

	m_pScanImgsProc->m_stitchImg.strName4Save = strStitchImg;
	if (bScan || !m_pScanImgsProc->m_stitchImg.bMosaicImgAvailable)
	{
		m_pScanImgsProc->m_stitchImg.iActionPurpose = ACT_SCAN_ONLY;
		m_pScanImgsProc->m_stitchImg.iSubAction = SUB_ACT_SAVE;
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



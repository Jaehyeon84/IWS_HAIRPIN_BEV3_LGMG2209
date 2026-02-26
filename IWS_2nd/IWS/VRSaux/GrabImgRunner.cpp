#include "../stdafx.h"
#include "GrabImgRunner.h"

#include "../IWS.h"
#include "../IWSDoc.h"
#include "Queue.h"

#include "DispImgDlg.h"
#include "ScanImgsProc.h"
#include "Utility.h"

#include "WorkImg.h"

/* Main CCD  */
CRITICAL_SECTION G_cs;

double		g_AvgFrmRate;
BOOL		g_bLiveDisplay;
BOOL		g_bGrabThreadAlive;
BOOL		g_bCameraStateOK;

ImageType	g_ImgType;

CString		g_strLogPath;
CString		g_strSimulImagePath;

CFont		g_textFont;
HBRUSH		g_hBrushBk = CreateSolidBrush(BACK);
HANDLE		g_hEventPushQueueDone;
HANDLE		g_hEventSrtCopyDispData;

/* 2nd CCD */
CRITICAL_SECTION G_cs2nd;

double		g_AvgFrmRate2nd;
BOOL		g_bLiveDisplay2nd;
BOOL		g_bGrabThreadAlive2nd;
BOOL		g_bCameraStateOK2nd;

ImageType	g_ImgType2nd;;
CString		g_strSimulImagePath2nd;
HANDLE		g_hEventSrtCopyDispData2nd;

CGrabImgRunner::CGrabImgRunner(CIWSDoc *pDoc)
{
	m_bReCover = TRUE;

	/* manin CCD */
	m_pDoc = pDoc;

	/* get necessary path */
	CString strExePath = m_pDoc->m_strExePath + L"\\";
	g_strSimulImagePath = strExePath + SIMUL_IMG_PATH;
	g_strLogPath = strExePath + LOG_PATH;

	m_pDoc->GetCcdInfo(&m_nGrabWd, &m_nGrabHt, &m_nGrabCh, m_strSNCCD, &m_nFR2Want);

	m_nCamBuff = NUM_INTERNAL_CAMBUF;

	// for grab thread
	g_ImgType = IMAQ_IMAGE_U8;
	g_bGrabThreadAlive = FALSE;
	g_hEventPushQueueDone = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	g_hEventSrtCopyDispData = ::CreateEvent(NULL, FALSE, FALSE, NULL);// for data copy for display

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
	m_pScanImgsProc->ChangeScanRange(resolX, resolY, sx, sy, ex, ey); // sjyi 2023.07.17 스캔영역 관련 오류 수정(중요)

	g_bCameraStateOK = FALSE;
	InitializeCriticalSection(&G_cs);

	/* 2nd CCD */
	m_nCamBuff2nd = NUM_INTERNAL_CAMBUF;
	// for grab thread
	g_ImgType2nd = IMAQ_IMAGE_U8;
	g_bGrabThreadAlive2nd = FALSE;
	g_hEventSrtCopyDispData2nd = ::CreateEvent(NULL, FALSE, FALSE, NULL);// for data copy for display

	m_hEventGrabThreadEnd2nd = CreateEvent(NULL, FALSE, FALSE, NULL);

	// for specific grab task
	m_bSnapImg2nd = FALSE;
	g_bLiveDisplay2nd = FALSE;

	// for grab & display sync control
	m_hEventSnapimg2nd = CreateEvent(NULL, FALSE, FALSE, NULL);
	//m_pDoc->GetCcdInfo(&m_nGrabWd2nd, &m_nGrabHt2nd, &m_nGrabCh2nd);
	//acqImgSize = m_nGrabHt2nd * m_nGrabWd2nd * m_nGrabCh2nd;
	m_nGrabWd2nd = m_nGrabWd;
	m_nGrabHt2nd = m_nGrabHt;
	m_nGrabCh2nd = m_nGrabCh;

	m_pDoc->GetCcd2ImgInfo(&m_nImgSizeWd, &m_nImgSizeHt);

	m_pSnapData2nd = new unsigned char[acqImgSize];
	memset(m_pSnapData2nd, 0, acqImgSize * sizeof(unsigned char));

	/* init pylon object */
	InitPylonObject();

	/* launch two grab thread */
	
	m_pGrapThread = NULL;
	if(m_bCCDFound[CCD_MAIN])
		m_pGrapThread = AfxBeginThread(GrabImgThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);

	g_bCameraStateOK2nd = FALSE;
	m_pGrapThread2nd = NULL;

	if(m_bCCDFound[CCD_2ND])
		m_pGrapThread2nd = AfxBeginThread(GrabImgThread2nd, (LPVOID)this, THREAD_PRIORITY_NORMAL);

	InitializeCriticalSection(&G_cs2nd);
}


CGrabImgRunner::~CGrabImgRunner()
{
	DWORD dwRes;
	m_bReCover = FALSE;
	
	/* main CCD */
	if (m_Camera[CCD_MAIN] != NULL)
	{
		g_bGrabThreadAlive = FALSE;
		g_Util.Logging(L"~CGrabImgRunner(): g_bGrabThreadAlive flag was set to FALSE");
		dwRes = WaitForSingleObject(m_hEventGrabThreadEnd, 5000);		// wait 3 second

		if (dwRes == WAIT_TIMEOUT && m_pGrapThread != NULL)	// Grab Thread 강제 종료
		{
			g_Util.Logging(L"~CGrabImgRunner(): WaitObject(1st ThreadEnd): ret- time out");
			TerminateThread(m_pGrapThread->m_hThread, 0);
			Sleep(100);
		}
		else
			g_Util.Logging(L"~CGrabImgRunner():WaitObject(1st ThreadEnd): ret- ok");

	}
	
	::CloseHandle(m_hEventGrabThreadEnd);
	::CloseHandle(m_hEvScannerMoveDone);
	::CloseHandle(g_hEventPushQueueDone);
	::CloseHandle(g_hEventSrtCopyDispData);
	::CloseHandle(m_hEventSnapimg);


	// clean up snap image buffer
	delete[] m_pSnapData;
	m_pSnapData = NULL;

	delete m_pScanImgsProc;
	DeleteCriticalSection(&G_cs);

	/* 2nd CCD */
	if (m_Camera[CCD_2ND] != NULL)
	{
		g_bGrabThreadAlive2nd = FALSE;
		g_Util.Logging(L"~CGrabImgRunner(): g_bGrabThreadAlive2nd flag was set to FALSE");
		dwRes = WaitForSingleObject(m_hEventGrabThreadEnd2nd, 5000);		// wait 3 second
		if (dwRes == WAIT_TIMEOUT && m_pGrapThread != NULL)	// Grab Thread 강제 종료
		{
			g_Util.Logging(L"~CGrabImgRunner():WaitObject(2nd ThreadEnd): ret- time out");
			TerminateThread(m_pGrapThread2nd->m_hThread, 0);
			Sleep(100);
		}
		else
			g_Util.Logging(L"~CGrabImgRunner():WaitObject(2nd ThreadEnd): ret- ok");
	}

	::CloseHandle(m_hEventGrabThreadEnd2nd);
	::CloseHandle(g_hEventSrtCopyDispData2nd);
	::CloseHandle(m_hEventSnapimg2nd);


	// clean up snap image buffer
	delete[] m_pSnapData2nd;
	m_pSnapData2nd = NULL;

	DeleteCriticalSection(&G_cs2nd);

	/* delete pylon object */
	FinalizePylonObject();
}

// GrabImgDlg 메시지 처리기
UINT CGrabImgRunner::GrabImgThread(LPVOID pArgs)
{
	CGrabImgRunner* me = (CGrabImgRunner*)pArgs;
	me->GrabimgThreadBody();
	return 0;
}


void CGrabImgRunner::InitPylonObject()
{
	for (int j = 0; j < NUM_MAX_CAMERA; j++)
		m_Camera[j] = NULL;

	Pylon::PylonTerminate();

	Sleep(50);
	Pylon::PylonInitialize();
	try {
		// Get the transport layer factory
		m_pTlFactory = &CTlFactory::GetInstance();

		// Create the transport layer object needed to enumerate or
		// create a camera object of type Camera_t::DeviceClass()
		m_pTl = m_pTlFactory->CreateTl(Camera_t::DeviceClass());
		
		// Exit application if the specific transport layer is not available
		if (!m_pTl)
		{
			g_Util.Logging(L"Failed to create transport layer!");
			g_bCameraStateOK = FALSE;
			return;
		}

		// Get all attached cameras and exit application if no camera is found
		//DeviceInfoList_t devices;
		int nCCDs = m_pTlFactory->EnumerateDevices(m_deviceInfoList);
		CString str;
		str.Format(L"Number of camera: %d", nCCDs);
		g_Util.Logging(str);
		if (0 == nCCDs)
		{
			g_Util.Logging(L"No camera present!");
			g_bCameraStateOK = g_bCameraStateOK2nd = FALSE;
			return;
		}

		// Create the camera object of the first available camera.
		// The camera object is used to set and get all available camera features.
		for (int i = 0; i < nCCDs; i++)
		{
			Camera_t* pCamera = new Camera_t(m_pTl->CreateDevice(m_deviceInfoList[i]));
			CDeviceInfo device_info = pCamera->GetDeviceInfo();
			std::string strSN_Found = device_info.GetSerialNumber();

			BOOL bMatchFound = FALSE;
			for (int j = 0; j < nCCDs; j++)
			{
				CString strSN_Config = m_strSNCCD[j];
				if ((CString)strSN_Found.c_str() == strSN_Config)
				{
					m_Camera[j] = pCamera;
					bMatchFound = TRUE;
				}

				if (bMatchFound)
				{
					m_bCCDFound[j] = TRUE;
					str.Format(L"CCD[%d] found: %ls, Config: %ls", j, (CString)strSN_Found.c_str(), strSN_Config);
					g_Util.Logging(str);
					TRACE("\t\tCCD[%d] found: %ls, Config: %ls \n", j, (CString)strSN_Found.c_str(), strSN_Config);
					break;
				}
				else
				{
					str.Format(L"CCD[%d] miss-match: Found: %ls , Config: %ls", j, (CString)strSN_Found.c_str(), strSN_Config);
					g_Util.Logging(str);
					TRACE("\t\tCCD[%d]:miss-match: Found: %ls , Config: %ls \n", j, (CString)strSN_Found.c_str(), strSN_Config);
				}
			}

		}
	}
	catch (GenICam::GenericException &e)
	{
		// Error handling
		CString err;
		TRACE(L"An exception occurred!: %s\n", e.GetDescription());
		g_Util.Logging(err);
		g_bCameraStateOK = g_bCameraStateOK2nd = FALSE;
		return;
	}
}


void CGrabImgRunner::SetUpCameraAndGrabBuff(Camera_t *pCamera, int iCamera,
	Camera_t::StreamGrabber_t* pStreamGrabber, std::vector<CGrab2DBuff*>* pBufferList) // Pylon intermediate process
{
	CString str;

	INodeMap* pTlNodeMap = pCamera->GetTLNodeMap();
	/////
	if (NULL == pTlNodeMap)
	{
		str.Format(L"CCD %d: does not provide a transport layer node map", iCamera);	g_Util.Logging(str);
		g_bCameraStateOK = FALSE;
		return;
	}

	Camera_t::TlParams_t TlParams = pTlNodeMap;

	TlParams.HeartbeatTimeout.SetValue(30000); // 30 sec timeout
	str.Format(L"CCD %d: Set HeartbeatTimeout", iCamera);	g_Util.Logging(str);

	pCamera->Open();
	str.Format(L"CCD %d: Camera open ", iCamera);	g_Util.Logging(str);

	//Disable acquisition start trigger if available
	{
		GenApi::IEnumEntry* acquisitionStart = pCamera->TriggerSelector.GetEntry(TriggerSelector_AcquisitionStart);
		bool available = GenApi::IsAvailable(acquisitionStart);
		if (acquisitionStart && GenApi::IsAvailable(acquisitionStart))
		{
			pCamera->TriggerSelector.SetValue(TriggerSelector_AcquisitionStart);
			pCamera->TriggerMode.SetValue(TriggerMode_Off);
		}
	}
	str.Format(L"CCD %d: Disable acquisition start trigger if available ", iCamera);	g_Util.Logging(str);

	//Disable frame start trigger if available
	{
		GenApi::IEnumEntry* frameStart = pCamera->TriggerSelector.GetEntry(TriggerSelector_FrameStart);
		if (frameStart && GenApi::IsAvailable(frameStart))
		{
			pCamera->TriggerSelector.SetValue(TriggerSelector_FrameStart);
			pCamera->TriggerMode.SetValue(TriggerMode_Off);
		}
	}
	str.Format(L"CCD %d: Disable frame start trigger if available ", iCamera);	g_Util.Logging(str);

	//Set acquisition mode
	pCamera->AcquisitionMode.SetValue(AcquisitionMode_Continuous);
	str.Format(L"CCD %d: Set AcquisitionMode as AcquisitionMode_Continuous ", iCamera);	g_Util.Logging(str);

	// Open the stream grabber
	pStreamGrabber->Open();
	str.Format(L"CCD %d: StreamGrabber open ", iCamera);	g_Util.Logging(str);

	// Get the image buffer size
	const size_t ImageSize = (size_t)(pCamera->PayloadSize.GetValue());

	// We won't use image buffers greater than ImageSize
	pStreamGrabber->MaxBufferSize.SetValue(ImageSize);

	// We won't queue more than c_nBuffers image buffers at a time
	pStreamGrabber->MaxNumBuffer.SetValue((uint32_t)m_nCamBuff);

	// Allocate all resources for grabbing. Critical parameters like image
	// size now must not be changed until FinishGrab() is called.
	pStreamGrabber->PrepareGrab();
	str.Format(L"CCD %d: StreamGrabber prepare grab ", iCamera);	g_Util.Logging(str);

	// Buffers used for grabbing must be registered at the stream grabber.
	// The registration returns a handle to be used for queuing the buffer.
	for (int i = 0; i < m_nCamBuff; ++i)
	{
		CGrab2DBuff *pGrabBuffer = new CGrab2DBuff(ImageSize);
		pGrabBuffer->SetBufferHandle(pStreamGrabber->RegisterBuffer(
			pGrabBuffer->GetBuffPointer(), ImageSize));

		// Put the grab buffer object into the buffer list
		pBufferList->push_back(pGrabBuffer);
	}
	str.Format(L"CCD %d: StreamGrabber register buffer ", iCamera);	g_Util.Logging(str);

	int iBuffIdx = 0;
	for (std::vector<CGrab2DBuff*>::const_iterator x = pBufferList->begin(); x != pBufferList->end(); ++x)
	{
		// Put buffer into the grab queue for grabbing
		pStreamGrabber->QueueBuffer((*x)->GetBuffHandle(), (void*)iBuffIdx);
		iBuffIdx++;
	}
	str.Format(L"CCD %d: StreamGrabber Put buffer into the grab queue for grabbing ", iCamera);	g_Util.Logging(str);


	// Let the camera acquire images continuously ( Acquisiton mode equals
	// Continuous! )
	pCamera->AcquisitionStart.Execute();
	str.Format(L"CCD %d: Camera acquisition start ", iCamera);	g_Util.Logging(str);

}

void CGrabImgRunner::CleanUpCameraAndGrabBuff(Camera_t* pCamera, int iCamera, 
	Camera_t::StreamGrabber_t* pStreamGrabber, std::vector<CGrab2DBuff*>* pBufferList) // Pylon intermediate process
{
	CString str;
	// Stop acquisition
	pCamera->AcquisitionStop.Execute();
	str.Format(L"CCD %d: Camera acquisition stop", iCamera);	g_Util.Logging(str);

	// Get the pending buffer back (You are not allowed to deregister
	// buffers when they are still queued)
	pStreamGrabber->CancelGrab();
	str.Format(L"CCD %d: StreamGrabber cancel grab", iCamera);	g_Util.Logging(str);

	// Get all buffers back
	for (GrabResult r; pStreamGrabber->RetrieveResult(r););
	str.Format(L"CCD %d: StreamGrabber retrieve result", iCamera);	g_Util.Logging(str);

	// Clean up
	// You must deregister the buffers before freeing the memory
	for (std::vector<CGrab2DBuff*>::iterator it = pBufferList->begin(); it != pBufferList->end(); it++)
	{
		pStreamGrabber->DeregisterBuffer((*it)->GetBuffHandle());
		delete *it;
		*it = NULL;
	}
	str.Format(L"CCD %d: StreamGrabber deregister & delete buffer", iCamera);	g_Util.Logging(str);

	// Free all resources used for grabbing
	pStreamGrabber->FinishGrab();
	str.Format(L"CCD %d: StreamGrabber finish grab", iCamera);	g_Util.Logging(str);

	// Close stream grabber
	pStreamGrabber->Close();
	str.Format(L"CCD %d: StreamGrabber close", iCamera);	g_Util.Logging(str);

	// Close camera
	pCamera->Close();
	str.Format(L"CCD 1: Camera close", iCamera);	g_Util.Logging(str);
}

void CGrabImgRunner::FinalizePylonObject()
{
	// delete created pylon object
	for (UINT j = 0; j < NUM_MAX_CAMERA; j++)
	{
		if (m_Camera[j] != NULL)
			delete m_Camera[j];
	}

	Pylon::PylonTerminate();
}

void CGrabImgRunner::GrabimgThreadBody()
{
	g_Util.Logging(L"/* - */");
	g_Util.Logging(L"CCD 1: ===== Grab Thread Started ! =====");

	CString str;

	// This outermost while-loop is added to recover dead camera 
	// when exception, multiple timeout or grab fail occur.

	// Set up main Camera and StreamGrabber for this thread
	int iCamera = CCD_MAIN + 1;
	Camera_t* pCamera = m_Camera[CCD_MAIN];

	int nFailCount = 0;
	Camera_t::StreamGrabber_t StreamGrabber = pCamera->GetStreamGrabber(0);
	std::vector<CGrab2DBuff*> BufferList;

	try
	{
		// Set Up camera & Stream Grabber
		g_Util.Logging(L"CCD 1: ----- Setting Up -----");
		SetUpCameraAndGrabBuff(pCamera, iCamera, &StreamGrabber, &BufferList);

		// for scan process
		UINT nPushCnt = 0;
		UINT nSnapCount = 0;

		// to calc average frame rate
		int accuFrameCnt = 0;
		double timeElasped;
		LARGE_INTEGER freq, timeBegin, timeEnd;
		QueryPerformanceFrequency(&freq);

		GrabResult grabResult;
		uint8_t *pImageBuffer;

		unsigned char *s, *d;

		g_bGrabThreadAlive = TRUE;
		BOOL bGrabTried = FALSE;

		::SetEvent(g_hEventSrtCopyDispData);

		DWORD wait_scan, wait_copy;
		bool ret_grab;
		
		QueryPerformanceCounter(&timeBegin);
		while (g_bGrabThreadAlive)
		{
			if (m_bDoScan)
			{
				wait_scan = ::WaitForSingleObject(m_hEvScannerMoveDone, 5);
				ret_grab = Grab(&StreamGrabber, &grabResult); // try grab
				bGrabTried = TRUE;

				switch (wait_scan)
				{
				case WAIT_TIMEOUT:
					//						Sleep(5);
					break;
				case WAIT_OBJECT_0: // Ready to grab

					if (grabResult.Status() == Grabbed && ret_grab)
					{
						g_bCameraStateOK = TRUE;
						pImageBuffer = (uint8_t *)grabResult.Buffer();
						m_pScanImgsProc->m_pScanDataQue->Push(pImageBuffer);

						SetEvent(g_hEventPushQueueDone);
						if (nPushCnt == 0)
						{
							m_pGrapThread2nd->SuspendThread();
							m_pLiveCCDDlg->m_pDispThread2nd->SuspendThread();
							::SetEvent(g_hEventPopThreadSrt);
						}

						++nPushCnt;
					}
					else
					{
						g_bCameraStateOK = FALSE;
						str.Format(L"CCD 1: Grab Fail in Scan Process - ret_grab: %d - EGrabStatus: %d", ret_grab, grabResult.Status());
						g_Util.Logging(str);
					}

					if (nPushCnt >= m_pScanImgsProc->m_nBlockX * m_pScanImgsProc->m_nBlockY)
					{
						m_bDoScan = FALSE;
						nPushCnt = 0;

						m_pGrapThread2nd->ResumeThread();
						m_pLiveCCDDlg->m_pDispThread2nd->ResumeThread();
						if (bGrabTried)
						{
							if (ret_grab) // 정산적인 return인 경우만 queueing
								StreamGrabber.QueueBuffer(grabResult.Handle(), grabResult.Context());

							bGrabTried = FALSE;
						}
					}
					break;
				}
			}

			if (g_bLiveDisplay)
			{
				wait_copy = ::WaitForSingleObject(g_hEventSrtCopyDispData, 5);

				switch (wait_copy) // display 용 m_pDisplayimgData에 영상을 1/4로 줄여 복사
				{
				case WAIT_TIMEOUT:
					//					Sleep(5);
					break;
				case WAIT_OBJECT_0:  // ready to grab
					if (!m_bDoScan)
					{
						ret_grab = Grab(&StreamGrabber, &grabResult);
						bGrabTried = TRUE;
					}

					if (grabResult.Status() == Grabbed && ret_grab)
					{
						g_bCameraStateOK = TRUE;

						pImageBuffer = (uint8_t *)grabResult.Buffer();
						s = pImageBuffer;	// address 초기화
						d = m_pLiveCCDDlg->m_pDisplayimgData;  // address 초기화

						for (UINT j = 0; j < m_nGrabHt; j += DOWN_SAMPLE_FACTOR)
						{
							for (UINT i = 0; i < m_nGrabWd; i += DOWN_SAMPLE_FACTOR)
							{
								*d = *s;
								d++;
								s += DOWN_SAMPLE_FACTOR;
							}
							s += m_nGrabWd;
						}

						accuFrameCnt++;
					}
					else if (grabResult.Status() == Failed)
					{
						Sleep(2000);
						g_bCameraStateOK2nd = FALSE;
						memset(m_pLiveCCDDlg->m_pDisplayimgData, 0, int(m_nGrabWd*m_nGrabHt / 4) * sizeof(BYTE));
						str.Format(L"CCD 1: Grab Failed in Display Process -- ret_grab: %d - EGrabStatus: Failed, check cables ", ret_grab);
						g_Util.Logging(str);
					}
					else
					{
						Sleep(20);
						g_bCameraStateOK = FALSE;
						memset(m_pLiveCCDDlg->m_pDisplayimgData, 0, int(m_nGrabWd*m_nGrabHt / 4) * sizeof(BYTE));
						str.Format(L"CCD 1: Grab Queue/Cancel in Display Process - ret_grab: %d - EGrabStatus: Queue/Cancel",	ret_grab);
						g_Util.Logging(str);
					}

					// display on Ni window
					//::PostMessage(m_pDispImgProc->m_hWnd, UWM_DISPIMG, 0, 0);
					::SetEvent(g_hEventDisplayStart);
					break;
				} //switch loop

				// calc acq frame rate
				QueryPerformanceCounter(&timeEnd);
				timeElasped = ((double)(timeEnd.QuadPart - timeBegin.QuadPart) / (double)freq.QuadPart); // in sec
				g_AvgFrmRate = (double)(accuFrameCnt) / timeElasped;
				if (accuFrameCnt > 1e+6) // counter & timer reset
				{
					accuFrameCnt = 0;
					timeElasped = 0;
					QueryPerformanceCounter(&timeBegin);
				}
			}

			if (m_bSnapImg)
			{
				if (!bGrabTried)
				{
					ret_grab = Grab(&StreamGrabber, &grabResult); //DHChoi 20200630
					bGrabTried = TRUE;
				}

				if (grabResult.Status() == Grabbed && ret_grab)
				{
					g_bCameraStateOK = TRUE;
					pImageBuffer = (uint8_t *)grabResult.Buffer();
					memcpy(m_pSnapData, pImageBuffer, m_nGrabWd * m_nGrabHt * sizeof(BYTE));

					/* save snap image */
					Image* pSnapImg = imaqCreateImage(IMAQ_IMAGE_U8, 0);
					imaqArrayToImage(pSnapImg, m_pSnapData, m_nGrabWd, m_nGrabHt);

					CString strImgName;
					strImgName.Format(L"\\snapImg_%d.bmp", nSnapCount);

					CString strImgFullName = m_pDoc->m_strExePath + +L"\\"; 
					strImgFullName += SNAP_IMG_PATH;
					strImgFullName += strImgName;
					imaqWriteFile(pSnapImg, (CStringA)strImgFullName, NULL);

					imaqDispose(pSnapImg);

					nSnapCount++;
					m_bSnapImg = FALSE;
				}
				else
				{
					Sleep(20);
					g_bCameraStateOK = FALSE;
					memcpy(m_pSnapData, 0, m_nGrabWd * m_nGrabHt * sizeof(BYTE));
					str.Format(L"CCD 1: Grab Fail in Snap Process - ret_grab: %d - EGrabStatus: %d", ret_grab, grabResult.Status());
					g_Util.Logging(str);
				}
				
				::SetEvent(m_hEventSnapimg);
			}

			// Reuse the buffer for grabbing the next image
			if (bGrabTried)
			{
				if (ret_grab) // 전상적인 return만 queueing
					StreamGrabber.QueueBuffer(grabResult.Handle(), grabResult.Context());
				else
					nFailCount++;

				bGrabTried = FALSE;
			}

			// caheck # of fail counter to restart
			if (nFailCount >= m_nCamBuff)
				g_bGrabThreadAlive = FALSE; // for smooth-ending of inner while loop
		} // Inner While Loop
		g_Util.Logging(L"CCD 1: Inner while-loop ends");

	}
	catch (GenICam::GenericException &e)
	{
		// Error handling
		str.Format(L"CCD 1: An exception occurred!: %s", (CString)(e.GetDescription()));
		g_Util.Logging(str);
		g_bCameraStateOK = FALSE;
		g_bGrabThreadAlive = FALSE;
	}

	// Clean up 
	g_Util.Logging(L"CCD 1: ----- Cleaning Up -----");
	CleanUpCameraAndGrabBuff(pCamera, iCamera, &StreamGrabber, &BufferList);

	// Exit 
	::SetEvent(m_hEventGrabThreadEnd);
	g_Util.Logging(L"CCD 1: GrabThreadBody(): ThreadEnd event was set exited !\n");
}

bool CGrabImgRunner::Grab(Camera_t::StreamGrabber_t *pStreamGrabber, GrabResult* pGrabResult)
{
	// Wait for the grabbed image with timeout of 3 seconds
	bool ret = pStreamGrabber->GetWaitObject().Wait(500);

	// Get the grab result from the grabber's result queue
	pStreamGrabber->RetrieveResult(*pGrabResult);

	return ret;
}

//
BOOL CGrabImgRunner::ReadImageOnDisk(Image* pImg)
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


//void CGrabImgRunner::DispImg(int nCtrlID, BYTE* pImg, int nWd, int nHt, BOOL bOverlay)
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

void CGrabImgRunner::SnapAndSaveImg(CString strPath, CString strImgName)
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


unsigned char* CGrabImgRunner::SnapImg()
{
	m_bSnapImg = TRUE;
	DWORD result = WaitForSingleObject(m_hEventSnapimg, 2000);
	if (result == WAIT_OBJECT_0)
		return m_pSnapData;
	else
		return NULL;
}

BOOL CGrabImgRunner::MoveScannerTo(double x, double y)
{
	m_pDoc->m_pScanner->Move(x, y);
	m_pDoc->m_pScanner->WaitMoving(FALSE);
	return TRUE;
}

void CGrabImgRunner::ScanStart()
{
	DWORD dwStart = GetTickCount();
	m_bDoScan = TRUE;

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

	// Wait until mosaic image is constructed	Poping and Stitching is handled in stitching thread(see StitchImgProc.h) 
	WaitForSingleObject(m_pScanImgsProc->m_hNPopProcessEnd, INFINITE);
	m_pScanImgsProc->Merging();
	m_pScanImgsProc->m_stitchImg.bMosaicImgAvailable = true;

	// sjyi 2023.07.17 메모리 오류 관련 수정
	m_pScanImgsProc->m_pScanDataQue->QClear();

	// save stitched img req by Main
	int newWidth = (int)((float)m_pScanImgsProc->m_stitchImg.nWd / m_pScanImgsProc->m_dsf);
	int newHeight = (int)((float)m_pScanImgsProc->m_stitchImg.nHt / m_pScanImgsProc->m_dsf);
	imaqResample(m_pScanImgsProc->m_stitchImg.pDownMosaicImg, m_pScanImgsProc->m_stitchImg.pMosaicImg, newWidth, newHeight, IMAQ_ZERO_ORDER, IMAQ_NO_RECT);

	if (m_pScanImgsProc->m_stitchImg.iSubAction == SUB_ACT_SAVE)
	{
		imaqWriteFile(m_pScanImgsProc->m_stitchImg.pDownMosaicImg, (CStringA)m_pScanImgsProc->m_stitchImg.strName4Save, NULL);
		SetFlagStitchImageDone(TRUE);
	}
	DWORD dwScanAllTime = GetTickCount() - dwStart;
	TRACE(_T("\nScanCnt = %d, ScanTime = %d msec, Scan+calib+merge = %d msec\t\t"), nBlockX*nBlockY, (int)dwScanTime, (int)dwScanAllTime);
}

void CGrabImgRunner::ScanAndSave(CString strStitchImg, BOOL bScan, int* widthPxl, int* heightPxl,
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
UINT CGrabImgRunner::OneShotScan(LPVOID pArg)
{
	CGrabImgRunner* me = (CGrabImgRunner*)pArg;
	me->ScanStart();
	return 0;
}

//DHChoi 20200626
void CGrabImgRunner::ChangeOverlap(UINT nOverlapX, UINT nOverlapY)
{
	m_pScanImgsProc->ChangeOverlap(nOverlapX, nOverlapY);
}

void CGrabImgRunner::ChangeScanRange(float resolX, float resolY,
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

void CGrabImgRunner::SetFlagStitchImageDone(BOOL bFlag)
{
	m_bStitchDone = bFlag;
}

BOOL CGrabImgRunner::IsDoneStitchImage()
{
	return m_bStitchDone;
}

int CGrabImgRunner::SnapAndInspect(CString strFilename)
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

void CGrabImgRunner::GetCurFullFov(float* fovX, float* fovY)
{
	*fovX = m_pScanImgsProc->m_fCurFullFovX;
	*fovY = m_pScanImgsProc->m_fCurFullFovY;
}


void CGrabImgRunner::GetRefImgCnt(double *dcx, double* dcy)
{
	*dcx = (double)m_pScanImgsProc->m_stitchImg.pfCenterPos_mm.x;
	*dcy = (double)m_pScanImgsProc->m_stitchImg.pfCenterPos_mm.y;
}


/* 2nd CCD */
UINT CGrabImgRunner::GrabImgThread2nd(LPVOID pArgs)
{
	CGrabImgRunner* me = (CGrabImgRunner*)pArgs;
	me->GrabimgThreadBody2nd();
	return 0;
}

void CGrabImgRunner::GrabimgThreadBody2nd()
{
	g_Util.Logging(L"/* - */");
	g_Util.Logging(L"nCCD 2: ===== Grab Thread Started ! =====");
	/* Calc Sleep time to run this thread slowly */
	DWORD sleepTime = (DWORD)(1000 / m_nFR2Want);
	if (sleepTime < 15)
		sleepTime = 0;


	// This outermost while-loop is added to recover dead camera 
	// when exception, multiple timeout or grab fail occur.
	// Set up main 2nd Camera and StreamGrabber for this thread
	CString str;

	int iCamera = CCD_2ND + 1;
	Camera_t* pCamera = m_Camera[CCD_2ND];

	Camera_t::StreamGrabber_t StreamGrabber = pCamera->GetStreamGrabber(0);
	std::vector<CGrab2DBuff*> BufferList;

	try
	{
		// Set up 2nd Camera and StreamGrabber for this thread
		g_Util.Logging(L"CCD 2: ----- Setting Up -----");

		SetUpCameraAndGrabBuff(pCamera, iCamera, &StreamGrabber, &BufferList);

		// to calc average frame rate
		int accuFrameCnt = 0;
		double timeElasped;
		LARGE_INTEGER freq, timeBegin, timeEnd;
		QueryPerformanceFrequency(&freq);

		GrabResult grabResult;
		uint8_t *pImageBuffer;

		unsigned char *s, *d;
		BOOL bGrabTried = FALSE;

		// 2022.11.24 jh.kim 추가 - CCD2 Live Image size 변경
		unsigned char *n;
		int nHtStart = (m_nGrabHt2nd - m_nImgSizeHt) / 2;
		int nHtEnd   = nHtStart + m_nImgSizeHt;
		int nWdStart = (m_nGrabWd2nd - m_nImgSizeWd) / 2;
		int nWdEnd   = nWdStart + m_nImgSizeWd;

		g_bGrabThreadAlive2nd = TRUE;
		::SetEvent(g_hEventSrtCopyDispData2nd);

		DWORD wait_copy;
		bool ret_grab;

		QueryPerformanceCounter(&timeBegin);
		while (g_bGrabThreadAlive2nd)
		{
			//Sleep(sleepTime);
			if (g_bLiveDisplay2nd)
			{

				wait_copy = ::WaitForSingleObject(g_hEventSrtCopyDispData2nd, 5);
				ret_grab = Grab(&StreamGrabber, &grabResult);
				bGrabTried = TRUE;

				switch (wait_copy) // display 용 m_pDisplayimgData에 영상을 1/4로 줄여 복사
				{
				case WAIT_TIMEOUT:
					//					Sleep(5);
					break;
				case WAIT_OBJECT_0:  // ready to grab

					if (grabResult.Status() == Grabbed && ret_grab)
					{
						g_bCameraStateOK2nd = TRUE;

						pImageBuffer = (uint8_t *)grabResult.Buffer();
						s = pImageBuffer;	// address 초기화
						d = m_pLiveCCDDlg->m_pDisplayimgData2nd;  // address 초기화
						n = m_pLiveCCDDlg->m_pDpData2nd;

						for (UINT j = 0; j < m_nGrabHt2nd; j += DOWN_SAMPLE_FACTOR)
						{
							for (UINT i = 0; i < m_nGrabWd2nd; i += DOWN_SAMPLE_FACTOR)
							{
								*d = *s;
								d++;

								if (j >= (UINT)nHtStart && j < (UINT)nHtEnd) {
									if (i >= (UINT)nWdStart && i < (UINT)nWdEnd) {
										*n = *s;
										n++;
									}
								}
								s += DOWN_SAMPLE_FACTOR;
							}
							s += m_nGrabWd2nd;
						}

						accuFrameCnt++;
						//str.Format(L" ------------- Grab BUFF Index: %d", grabResult.Context());
						//g_Util.Logging(str);
					}
					else if (grabResult.Status() == Failed)
					{
						Sleep(2000);
						g_bCameraStateOK2nd = FALSE;
						memset(m_pLiveCCDDlg->m_pDisplayimgData2nd, 0, int(m_nGrabWd2nd*m_nGrabHt2nd / 4) * sizeof(BYTE));
						memset(m_pLiveCCDDlg->m_pDpData2nd, 0, int(m_nImgSizeWd*m_nImgSizeHt / 4) * sizeof(BYTE));
						str.Format(L"CCD 2: Grab Fail in Display Process -- ret_grab: %d -- EGrabStatus:Failed, Check cables ", ret_grab);
						g_Util.Logging(str);
					}
					else
					{
						Sleep(2000);
						g_bCameraStateOK2nd = FALSE;
						memset(m_pLiveCCDDlg->m_pDisplayimgData2nd, 0, int(m_nGrabWd2nd*m_nGrabHt2nd / 4) * sizeof(BYTE));
						memset(m_pLiveCCDDlg->m_pDisplayimgData2nd, 0, int(m_nImgSizeWd*m_nImgSizeHt / 4) * sizeof(BYTE));
						str.Format(L"CCD 2: Grab Queue/Cancel in Display Process -- ret_grab: %d -- EGrabStatus: Queue/Cancel", ret_grab);
						g_Util.Logging(str);
					}

					// display on Ni window
					//::PostMessage(m_pDispImgProc->m_hWnd, UWM_DISPIMG, 0, 0);
					::SetEvent(g_hEventDisplayStart2nd);
					break;
				} //switch loop

				// calc acq frame rate
				QueryPerformanceCounter(&timeEnd);
				timeElasped = ((double)(timeEnd.QuadPart - timeBegin.QuadPart) / (double)freq.QuadPart); // in sec
				g_AvgFrmRate2nd = (double)(accuFrameCnt) / timeElasped;
				if (accuFrameCnt > 1e+6) // counter & timer reset
				{
					accuFrameCnt = 0;
					timeElasped = 0;
					QueryPerformanceCounter(&timeBegin);
				}
			}

			// Reuse the buffer for grabbing the next image
			if (bGrabTried)
			{
				if (ret_grab) // 정상적인 return이 있었다면 queueing
					StreamGrabber.QueueBuffer(grabResult.Handle(), grabResult.Context());
					
				bGrabTried = FALSE;
			}
		} // Inner While Loop(Grab Loop)

		g_Util.Logging(L"CCD 2: Inner while-loop ends");

	}
	catch (GenICam::GenericException &e)
	{
		// Error handling
		str.Format(L"CCD 2: An exception occurred!: %s", (CString)(e.GetDescription()));
		g_Util.Logging(str);
		g_bCameraStateOK2nd = FALSE;
		g_bGrabThreadAlive2nd = FALSE;
	}

	// Clean up 
	g_Util.Logging(L"CCD 2: ----- Cleaning Up -----");
	CleanUpCameraAndGrabBuff(pCamera, iCamera, &StreamGrabber, &BufferList);

	// Exit 
	::SetEvent(m_hEventGrabThreadEnd2nd);
	g_Util.Logging(L"CCD 2: GrabThreadBody2nd(): ThreadEnd Event was set & exited !\n");
}


BOOL CGrabImgRunner::ReadImageOnDisk2nd(Image* pImg)
{
	CString cstrFullName = g_strSimulImagePath2nd + L"\\";
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

	if (m_pSnapData2nd != NULL && _pData != NULL)
	{
		for (int j = 0; j < col*row; j++)
			m_pSnapData2nd[j] = _pData[j];

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


void CGrabImgRunner::SnapAndSaveImg2nd(CString strPath, CString strImgName)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bSnapImg2nd = TRUE;
	DWORD result = WaitForSingleObject(m_hEventSnapimg2nd, 5000);
	Image* pTempImg = imaqCreateImage(g_ImgType2nd, 2);
	if (result == WAIT_OBJECT_0)
	{
		imaqArrayToImage(pTempImg, m_pSnapData2nd, m_nGrabWd2nd, m_nGrabHt2nd);
		CString fullName;
		fullName = strPath + L"\\";
		fullName += strImgName;
		imaqWriteFile(pTempImg, (CStringA)fullName, NULL);
	}
	imaqDispose(pTempImg);
}

unsigned char* CGrabImgRunner::SnapImg2nd()
{
	m_bSnapImg2nd = TRUE;
	DWORD result = WaitForSingleObject(m_hEventSnapimg2nd, 2000);
	if (result == WAIT_OBJECT_0)
		return m_pSnapData2nd;
	else
		return NULL;
}
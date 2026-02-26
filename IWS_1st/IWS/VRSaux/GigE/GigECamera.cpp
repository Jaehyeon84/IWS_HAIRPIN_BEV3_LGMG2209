#include "stdafx.h"
#include "GigECamera.h"


using namespace GenApi;
using namespace Basler_GigECameraParams;

#define IS_MONO		0
#define INI_TRIG	0

bool CGigECamera::PersistentIpAddr(CString sSerialNumber, CString sIpAddr, CString sSubnetMask, CString sGateway)
{

	IGigETransportLayer* const gigeTL = dynamic_cast<IGigETransportLayer*>(CTlFactory::GetInstance().CreateTl(BaslerGigEDeviceClass));

	if ( gigeTL )
	{
		CString sMacAddr = _T("");
		DeviceInfoList_t dil;
		int nCamera = gigeTL->EnumerateAllDevices(dil);
		CTlFactory& TlFactory = CTlFactory::GetInstance();

		for(int i = 0; i < nCamera; i++)
		{
			CString sDevSerialNumber = (CString)(String_t)dil[i].GetSerialNumber();
			if (sDevSerialNumber == sSerialNumber)
			{
				CBaslerGigEDeviceInfo &bdi = static_cast<CBaslerGigEDeviceInfo&>(dil[i]);
				sMacAddr = bdi.GetMacAddress();
			}
		}

		if (sMacAddr != _T(""))
		{
			gigeTL->ForceIp((String_t)sMacAddr.GetBuffer(), (String_t)sIpAddr.GetBuffer(), (String_t)sSubnetMask.GetBuffer(), "0.0.0.0");

			CBaslerGigEDeviceInfo devInfo;
			devInfo.SetIpAddress((String_t)sIpAddr.GetBuffer());
			devInfo.SetSubnetMask((String_t)sSubnetMask.GetBuffer());

			Camera_t camera;
			camera.Attach(TlFactory.CreateDevice(devInfo));
			camera.Open();

			camera.ChangeIpConfiguration(true, true);
			camera.SetPersistentIpAddress((String_t)sIpAddr.GetBuffer(), (String_t)sSubnetMask.GetBuffer(), (String_t)sGateway.GetBuffer());
			camera.Close();
		}
	}

	return true;
}

CGigECamera::CGigECamera(CString sIpAddr, int nContext)
	: m_nLogIndex(0)
	, m_pBuffer(NULL)
	, m_pBufferC(NULL)
	, m_nCount(0)
	, m_nCurrentCount(0)
{
	m_pBaslerCamera = NULL;
	m_nMode = MODE_SOFTTRIGGER;

	m_sIpAddr = sIpAddr;
	m_nContext = nContext;

	m_hView = NULL;

	m_pDC = NULL;

	m_fFPS = 0;
	m_nGrabCount = 0;
	QueryPerformanceFrequency(&TimerFrequency);
	QueryPerformanceCounter(&StartTime);

	m_hGrabDone = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_bGrabDone = FALSE;
}

CGigECamera::~CGigECamera(void)
{
	::CloseHandle(m_hGrabDone);

	Close();
}

bool CGigECamera::Open(void)
{
	bool bRet = false;

	/********************************************************************************/
	/* Set Device Info                                                              */
	CTlFactory& tlFactory = CTlFactory::GetInstance(); 
	CBaslerGigEDeviceInfo di;
	di.SetIpAddress(String_t(m_sIpAddr.GetBuffer()));
	/* Set Device Info                                                              */
	/********************************************************************************/

	try
	{
		IPylonDevice* pIPylonDevice= tlFactory.CreateDevice(di);
		m_pBaslerCamera =  new Camera_t(pIPylonDevice);
		m_pBaslerCamera->SetCameraContext(m_nContext);
		m_pBaslerCamera->RegisterImageEventHandler( this, RegistrationMode_Append, Cleanup_None);
		m_pBaslerCamera->RegisterConfiguration(this, RegistrationMode_ReplaceAll, Cleanup_None);
		
		m_pBaslerCamera->Open();


		bRet = true;
	}
	catch(...)
	{
		Close();
		bRet = false;
	}

	return bRet;
}
DWORD dwStart;// = GetTickCount();

bool CGigECamera::Grab(BYTE* pbyImage, long count)
{
	bool res = false;

	if (m_pBaslerCamera != NULL && pbyImage != NULL)
	{
		try
		{
			m_pbyImage = pbyImage;

			m_nCount = count;
			m_nCurrentCount = 0;
			
			m_pBaslerCamera->StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);

			//CGrabResultPtr ptrGrabResult;

			//m_pBaslerCamera->MaxNumBuffer = 10;
			//m_pBaslerCamera->StartGrabbing(10, GrabStrategy_OneByOne, GrabLoop_ProvidedByUser);


			//m_pBaslerCamera->RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);

			//int nWidth = ptrGrabResult->GetWidth();
			//int nHeight = ptrGrabResult->GetHeight();

			//memcpy(m_pbyImage, (BYTE *)ptrGrabResult->GetBuffer(), nWidth * nHeight);

			//m_pBaslerCamera->StopGrabbing();

		
			res = true;
			//dwStart = GetTickCount();

			//열거 :
			//GrabStrategy_OneByOne.			: 이미지 순서대로 처리
			//GrabStrategy_LatestImageOnly.	: 마지막 이미지만 저장
			//GrabStrategy_UpcomingImage.		: 입력버퍼에 계속 갱신처리
		}
		catch (GenICam::GenericException ex)
		{
			AddLog(_T("Error"), _T("CGigECamera::Grab"), (CString)CStringA(ex.GetDescription()));
		}
		catch (...)//catch (CException* e)
		{
			AddLog(_T("Error"), _T("CGigECamera::Grab"), _T("failed : StartGrabbing"));
		}
	}

	return res;
}

void CGigECamera::Stop(void)
{
	if (m_pBaslerCamera != NULL)
	{
		try
		{
			if (IsGrabbing())
			{
				m_pBaslerCamera->StopGrabbing();
			}
		}
		catch (...)
		{
			AddLog(_T("Error"), _T("CGigECamera::Stop"), _T("failed : StopGrabbing"));
		}
	}
}

BYTE* CGigECamera::GetGrabData()
{
	//CGrabResultPtr ptrGrabResult;

	//bool bStatus = m_pBaslerCamera->RetrieveResult(5000, ptrGrabResult, TimeoutHandling_Return);
	//
	//if (bStatus != TRUE)
	//{
	//	return NULL;
	//}

	//int nWidth = ptrGrabResult->GetWidth();
	//int nHeight = ptrGrabResult->GetHeight();

	//memcpy(m_pbyImage, (BYTE *)ptrGrabResult->GetBuffer(), nWidth * nHeight);

	return m_pbyImage;
}


void CGigECamera::Close(void)
{
	if (m_pBaslerCamera != NULL)
	{
		Stop();

		m_pBaslerCamera->Close();

		m_pBaslerCamera->DeregisterImageEventHandler(this);
		m_pBaslerCamera->DeregisterConfiguration(this);

		delete m_pBaslerCamera;
		m_pBaslerCamera = NULL;

	}
}

bool CGigECamera::IsGrabbing(void)
{
	bool res = false;

	if (m_pBaslerCamera != NULL)
	{
		try
		{
			res = m_pBaslerCamera->IsGrabbing();
		}
		catch (...)
		{
		}
	}

	return res;
}



bool CGigECamera::SetMode(eMODE nMode)
{
	bool bRet = false;

	if (m_pBaslerCamera != NULL)
	{
		INodeMap &Control = m_pBaslerCamera->GetNodeMap();
		CEnumerationPtr triggerSelector(Control.GetNode("TriggerSelector"));
		CEnumerationPtr triggerMode(Control.GetNode("TriggerMode"));
		CEnumerationPtr triggerSource(Control.GetNode("TriggerSource"));

		switch (nMode)
		{
		case MODE_SOFTTRIGGER:
			triggerMode->FromString("On");
			triggerSource->FromString("Software");
			break;

		case MODE_HARDTRIGGER:
			triggerMode->FromString("On");
			triggerSource->FromString("Line1");
			break;

		case MODE_CONTINUE:
			triggerSelector->FromString("FrameStart");
			triggerMode->FromString("Off");
			break;
		}

		m_nMode = nMode;
	}

	return bRet;
}
#include "../../IWSDoc.h"
void CGigECamera::OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult)
{
	//DWORD dwEnd = GetTickCount();

	//CString strTemp;
	//strTemp.Format(_T("%dms"), dwEnd - dwStart);
	//_tracer(TRACE_NORMAL, 9999, L"%s", strTemp);

	if (m_bGrabDone == FALSE)
	{
		if (ptrGrabResult->GrabSucceeded() == true)
		{
			int nWidth = ptrGrabResult->GetWidth();
			int nHeight = ptrGrabResult->GetHeight();

			//memcpy(m_pbyImage[m_nCurrentCount], (BYTE *)ptrGrabResult->GetBuffer(), nWidth * nHeight);
		
			memcpy(m_pbyImage, (BYTE *)ptrGrabResult->GetBuffer(), nWidth * nHeight);

			//m_pBaslerCamera->StopGrabbing();

			m_bGrabDone = TRUE;
		
			//::SetEvent(m_hGrabDone);

			//m_nCurrentCount++;

			//if(m_nCount <= m_nCurrentCount)
			//{
			//	m_pBaslerCamera->StopGrabbing();
			//}
		}
	}
}

void CGigECamera::SetView(CDC* pDC)
{
	m_pDC = pDC;
}

void CGigECamera::SoftTriggerSend()
{
	if (m_pBaslerCamera != NULL)
	{
		m_pBaslerCamera->ExecuteSoftwareTrigger();
	}
}

void CGigECamera::OnCameraEvent( CBaslerGigEInstantCamera& camera, intptr_t userProvidedId, GenApi::INode* pNode)
{
	CString str;
	str.Format(_T("%d"), camera.GetCameraContext());
	switch (userProvidedId)
	{
	case eExposureEndEvent:
		break;

	default:
		break;
	}
}

void CGigECamera::OnCameraDeviceRemoved(CInstantCamera& camera)
{
	AddLog(_T("Error"), _T("CGigECamera::OnCameraDeviceRemoved"), _T("Camera Disconnected."));
	TRACE("OnCameraDeviceRemoved\n");
}

void CGigECamera::SetExpose(int64_t nValue)
{
	if (m_pBaslerCamera != NULL)
		m_pBaslerCamera->ExposureTimeRaw.SetValue(nValue);
}

void CGigECamera::SetPacketSize(int64_t nValue)
{
	if (m_pBaslerCamera != NULL)
	{
		INodeMap &Control = m_pBaslerCamera->GetNodeMap();
		CIntegerPtr packetSize(Control.GetNode("GevSCPSPacketSize"));

		packetSize->SetValue(nValue);
	}
}

void CGigECamera::SetAOI(int64_t nX, int64_t nY, int64_t nWidth, int64_t nHeight)
{
	if (m_pBaslerCamera != NULL)
	{
		if (m_pBaslerCamera->IsOpen() == true)
		{
			bool bGrabbing = m_pBaslerCamera->IsGrabbing();

			if (bGrabbing == true)
			{
				m_pBaslerCamera->StopGrabbing();
			}
			m_pBaslerCamera->OffsetX.SetValue(nX);
			m_pBaslerCamera->OffsetY.SetValue(nY);
			m_pBaslerCamera->Width.SetValue(nWidth);
			m_pBaslerCamera->Height.SetValue(nHeight);
			//m_pBaslerCamera->PixelFormat.SetValue(PixelFormat_BayerBG8);
			m_pBaslerCamera->PixelFormat.SetValue(PixelFormat_Mono8);
			m_pBaslerCamera->AcquisitionFrameRateEnable.SetValue(false);

			if (bGrabbing == true)
			{
				m_pBaslerCamera->StartGrabbing( GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
			}
		}
	}
}

void CGigECamera::AddLog(CString arg_log)
{
	TRACE(arg_log);
	TRACE("\n");
}

void CGigECamera::AddLog(CString arg_category, CString arg_function, CString arg_msg)
{
	CString sTmp;
	sTmp.Format(_T("[%06d] [%-20s]: [%s] > %s"), m_nLogIndex++, arg_category, arg_function, arg_msg);

	AddLog(sTmp);
}

void CGigECamera::OnOpened(CInstantCamera& camera)
{
	using namespace GenApi;

	try
	{
		// Get the camera control object.
		INodeMap &Control = camera.GetNodeMap();

		// Get the trigger selector node.
		CEnumerationPtr triggerSelector(Control.GetNode("TriggerSelector"));
		// Get the trigger mode node.
		CEnumerationPtr triggerMode(Control.GetNode("TriggerMode"));

		// Check the available camera trigger mode(s) to select the appropriate one: acquisition start trigger mode 
		// (used by previous cameras, i.e. for cameras supporting only the legacy image acquisition control mode;
		// do not confuse with acquisition start command) or frame start trigger mode
		// (used by newer cameras, i.e.for cameras using the standard image acquisition control mode;
		// equivalent to the acquisition start trigger mode in the leagcy image acquisition control mode).
		bool frameStartAvailable = false;
		bool acquisitionStartAvailable = false;
		{
			// Frame start trigger mode available?
			GenApi::IEnumEntry* frameStart = triggerSelector->GetEntryByName("FrameStart");
			frameStartAvailable = frameStart && IsAvailable(frameStart);

			// Acquisition start trigger mode available?
			GenApi::IEnumEntry* acquisitionStart = triggerSelector->GetEntryByName("AcquisitionStart");
			acquisitionStartAvailable = acquisitionStart && IsAvailable(acquisitionStart);
		}

		// Preselect the trigger mode for image acquisition.
		const char* triggerSelectorValue = "FrameStart";

		// Check to see if the camera implements the acquisition start trigger mode only.
		if ( acquisitionStartAvailable && !frameStartAvailable)
		{
			// Camera uses the acquisition start trigger as the only trigger mode.
			triggerSelector->FromString("AcquisitionStart");
			triggerMode->FromString("On");
			triggerSelectorValue = "AcquisitionStart";
		}
		else  
		{   
			// Camera may have the acquisition start trigger mode and the frame start trigger mode implemented.
			// In this case, the acquisition trigger mode must be switched off.
			if ( acquisitionStartAvailable )
			{
				triggerSelector->FromString("AcquisitionStart");
				triggerMode->FromString("Off");
			}
			// To trigger each single frame by software or external hardware trigger: Enable the frame start trigger mode.
			assert( frameStartAvailable); //Frame start trigger mode must be available here.
			triggerSelector->FromString("FrameStart");
			//triggerMode->FromString("On");
			triggerMode->FromString("Off");
		}

		// Note: The trigger selector must be set to the appropriate trigger mode 
		// before setting the trigger source or issuing software triggers.
		// Frame start trigger mode for newer cameras (i.e. for cameras supporting the standard image acquisition control mode).
		triggerSelector->FromString( triggerSelectorValue);

		// The trigger source must be set to 'Software'.
		CEnumerationPtr(Control.GetNode("TriggerSource"))->FromString("Software");

		//// Alternative hardware trigger configuration:
		//// This configuration can be copied and modified to create a hardware trigger configuration.
		//// Remove setting the 'TriggerSource' to 'Software' (see above) and 
		//// use the commented lines as a starting point.
		//// The camera user's manual contains more information about available configurations.
		//// The pylon Viewer tool can be used to test the selected settings first.

		//// The trigger source must be set to the trigger input, e.g. 'Line1'.
		//CEnumerationPtr(Control.GetNode("TriggerSource"))->FromString("Line1");

		////The trigger activation must be set to e.g. 'RisingEdge'.
		//CEnumerationPtr(Control.GetNode("TriggerActivation"))->FromString("RisingEdge");

		// Enable the continuous acquisition mode.
		CEnumerationPtr(Control.GetNode("AcquisitionMode"))->FromString("Continuous");
	}
	catch (GenICam::GenericException& e)
	{
		throw RUNTIME_EXCEPTION( "Could not apply configuration. GenICam::GenericException caught in OnOpened method msg=%hs", e.what());
	}
	catch (std::exception& e)
	{
		throw RUNTIME_EXCEPTION( "Could not apply configuration. std::exception caught in OnOpened method msg=%hs", e.what());
	}
	catch (...)
	{
		throw RUNTIME_EXCEPTION( "Could not apply configuration. Unknown exception caught in OnOpened method.");
	}
}

void CGigECamera::CalculateFPS()
{

	if (m_nGrabCount > 1)
	{
		LARGE_INTEGER CurrentTime;

		//현재 시간을 얻는다.
		QueryPerformanceCounter(&CurrentTime);

		// 여기서 드뎌 아까 공식이 나온다.
		m_fFPS = (float)m_nGrabCount / ((float)(CurrentTime.QuadPart - StartTime.QuadPart) / 
			(float)TimerFrequency.QuadPart);
		//TRACE("FPS : %f\n", fps);
		// 윈도우 타이틀에 프레임률이 나온다.
		//sprintf(buf, "%0.1f", fps);
		//SetWindowText(hWnd, buf);

		m_nGrabCount = 0;
		QueryPerformanceCounter(&StartTime);
	}

}

long CGigECamera::GetWidth()
{
	return m_pBaslerCamera->Width.GetValue();
}

long CGigECamera::GetHeight()
{
	return m_pBaslerCamera->Height.GetValue();
}

void CGigECamera::SetNodeMap(char* szPathFile)
{
	CFeaturePersistence::Load(szPathFile, &m_pBaslerCamera->GetNodeMap(), true);
}

BOOL CGigECamera::IsOpen()
{
	if(m_pBaslerCamera != nullptr)
	{
		return m_pBaslerCamera->IsOpen();
	}

	return FALSE;
}

BOOL CGigECamera::IsGrabDone()
{
	return m_bGrabDone;
}

void CGigECamera::SetIsGrabDone(BOOL bValue)
{
	m_bGrabDone = bValue;
}
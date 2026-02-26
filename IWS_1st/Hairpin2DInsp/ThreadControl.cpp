#include "pch.h"
#include "DetectionAlgorithm.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CDetectionAlgorithm::InitThreads()
{
	// Method Thread...
	m_nLatestMethodThreadID = 0;

	m_Control_Method.resize(REMOVE_NOISE_THREAD_COUNT);
	m_Method_IndexInfo.resize(REMOVE_NOISE_THREAD_COUNT);

	for(long nThread = 0; nThread < REMOVE_NOISE_THREAD_COUNT; nThread++)
	{
		auto &control = m_Control_Method[nThread];

		control.m_hSignal_Kill = CreateEvent(NULL, FALSE, FALSE, NULL);
		control.m_hSignal_Ready = CreateEvent(NULL, FALSE, FALSE, NULL);
		control.m_hSignal_Start = CreateEvent(NULL, FALSE, FALSE, NULL);
		control.m_hSignal_Finish = CreateEvent(NULL, FALSE, FALSE, NULL);
		control.m_Handle_Thread = INVALID_HANDLE_VALUE;

		HANDLE hThread = ::AfxBeginThread(ThreadMethodFunc, LPVOID(this))->m_hThread;
		::DuplicateHandle(::GetCurrentProcess(), hThread, ::GetCurrentProcess(), &control.m_Handle_Thread, DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_SAME_ACCESS);
	}

	// Data Save...
	m_Control_DataSave.m_hSignal_Start = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_Control_DataSave.m_hSignal_Kill = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_Control_DataSave.m_Handle_Thread = INVALID_HANDLE_VALUE;

	HANDLE hDataSaveThread = ::AfxBeginThread(ThreadDataSaveFunc, (void *)this)->m_hThread;
	::DuplicateHandle(::GetCurrentProcess(), hDataSaveThread, ::GetCurrentProcess(), &m_Control_DataSave.m_Handle_Thread, DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_SAME_ACCESS);
}

void CDetectionAlgorithm::FreeThreads()
{
	// Method Thread...
	m_Method_IndexInfo.clear();

	// 모든 쓰레드에게 일을 그만하라고 알린다.
	for(long nThread = long(m_Control_Method.size() - 1); nThread >= 0; nThread--)
	{
		::SetEvent(m_Control_Method[nThread].m_hSignal_Kill);
	}

	// 쓰레드가 종료되기를 기다린다.
	for(long nThread = long(m_Control_Method.size() - 1); nThread >= 0; nThread--)
	{
		::WaitForSingleObject(m_Control_Method[nThread].m_Handle_Thread, 1000);
	}

	// 관련 핸들을 모두 닫는다.
	for(long nThread = long(m_Control_Method.size() - 1); nThread >= 0; nThread--)
	{
		::CloseHandle(m_Control_Method[nThread].m_hSignal_Kill);
		::CloseHandle(m_Control_Method[nThread].m_hSignal_Ready);
		::CloseHandle(m_Control_Method[nThread].m_hSignal_Start);
		::CloseHandle(m_Control_Method[nThread].m_hSignal_Finish);
		::CloseHandle(m_Control_Method[nThread].m_Handle_Thread);
	}

	// Data Save...
	// 모든 Thread에게 일을 그만하라고 알린다.
	::SetEvent(m_Control_DataSave.m_hSignal_Kill);

	// Thread가 종료되기를 기다린다.
	::WaitForSingleObject(m_Control_DataSave.m_Handle_Thread, 1000);

	// 관련 핸들을 모두 닫는다.
	::CloseHandle(m_Control_DataSave.m_hSignal_Kill);
	::CloseHandle(m_Control_DataSave.m_Handle_Thread);
}

void CDetectionAlgorithm::BeginThread(const THREAD_METHOD method)
{
	m_Method = method;

	auto roiParams = ::g_RecipeManager.m_sROIParams;

	auto unrolledImageHeight = m_nUnrolledImageHeight;

	std::vector<HANDLE> vechFilteringFinish(REMOVE_NOISE_THREAD_COUNT);
	HANDLE* pFilteringFinishHandles = &vechFilteringFinish[0];

	for(long thread = 0; thread < REMOVE_NOISE_THREAD_COUNT; thread++)
	{
		vechFilteringFinish[thread] = m_Control_Method[thread].m_hSignal_Finish;
	}

	long div(0);
	if(m_Method == THREAD_METHOD::UNROLLED_IMAGE)
	{
		div = (long)((float)unrolledImageHeight / (float)REMOVE_NOISE_THREAD_COUNT);
	}
	else if(m_Method == THREAD_METHOD::REMOVE_NOISE)
	{
		div = (long)((float)roiParams.nSlotCount / (float)REMOVE_NOISE_THREAD_COUNT);
	}

	for(long thread = 0; thread < REMOVE_NOISE_THREAD_COUNT; thread++)
	{
		auto &info = m_Method_IndexInfo[thread];

		info.startIndex = thread * div;
		info.endIndex = info.startIndex + div;
	}

	for(long thread = 0; thread < REMOVE_NOISE_THREAD_COUNT; thread++)
	{
		::SetEvent(m_Control_Method[thread].m_hSignal_Start);
	}

	::WaitForMultipleObjects(REMOVE_NOISE_THREAD_COUNT, pFilteringFinishHandles, TRUE, INFINITE);

	// 다시 Ready 상태로 돌려 놓는다.
	for(long thread = 0; thread < REMOVE_NOISE_THREAD_COUNT; thread++)
	{
		::SetEvent(m_Control_Method[thread].m_hSignal_Ready);
	}
}

// Method Thread
UINT CDetectionAlgorithm::ThreadMethodFunc(LPVOID pParam)
{
	CDetectionAlgorithm *pMakeUnrolledThread = (CDetectionAlgorithm *)pParam;

	pMakeUnrolledThread->ThreadMethod();

	return 0;
}

void CDetectionAlgorithm::ThreadMethod()
{
	static CCriticalSection cs;

	cs.Lock();
	const long nThreadID = m_nLatestMethodThreadID++;
	cs.Unlock();

	auto &control = m_Control_Method[nThreadID];

	::SetEvent(control.m_hSignal_Ready);

	HANDLE pHandles[] = {control.m_hSignal_Start, control.m_hSignal_Kill};
	const DWORD nSignalCount = sizeof(pHandles) / sizeof(HANDLE);

	while(1)
	{
		BOOL bQuit = FALSE;

		DWORD dwRet = ::WaitForMultipleObjects(nSignalCount, pHandles, FALSE, INFINITE);

		switch(dwRet)
		{
		case WAIT_OBJECT_0: // 계산 시작
		{
			auto indexInfo = m_Method_IndexInfo[nThreadID];

			if(m_Method == THREAD_METHOD::UNROLLED_IMAGE)
			{
				CalcUnrolledImage(indexInfo.startIndex, indexInfo.endIndex);
			}
			else if(m_Method == THREAD_METHOD::REMOVE_NOISE)
			{
				CalcRemoveNoise(indexInfo.startIndex, indexInfo.endIndex);
			}

			// 계산 끝
			::SetEvent(control.m_hSignal_Finish);

			break;
		}
		case WAIT_OBJECT_0 + 1:	// Quit Thread
			bQuit = TRUE;
			break;
		}

		if(bQuit == TRUE)
		{
			break;
		}
	}
}

// DataSave Thread
UINT CDetectionAlgorithm::ThreadDataSaveFunc(LPVOID pParam)
{
	CDetectionAlgorithm *pDataSave = (CDetectionAlgorithm *)pParam;

	pDataSave->ThreadDataSave();

	return 0;
}


void CDetectionAlgorithm::ThreadDataSave()
{
	CString strTemp;

	HANDLE pHandles[] = {m_Control_DataSave.m_hSignal_Start, m_Control_DataSave.m_hSignal_Kill};
	const DWORD nSignalCount = sizeof(pHandles) / sizeof(HANDLE);

	while(1)
	{
		DWORD dwRet = ::WaitForMultipleObjects(nSignalCount, pHandles, FALSE, INFINITE);

		// DataSave 시작
		if(dwRet == WAIT_OBJECT_0)
		{
			CString strPath(_T(""));
			if(::g_RecipeManager.m_sPublicParams.strRawImageSavePath != _T(""))
			{
				strPath = ::g_RecipeManager.m_sPublicParams.strRawImageSavePath;
			}
			else if(::g_RecipeManager.m_sPublicParams.strResultDataSavePath != _T(""))
			{
				strPath = ::g_RecipeManager.m_sPublicParams.strResultDataSavePath;
			}
			else if(::g_RecipeManager.m_sPublicParams.strResultImageSavePath != _T(""))
			{
				strPath = ::g_RecipeManager.m_sPublicParams.strResultImageSavePath;
			}

			DWORDLONG nSize(0);
			if(strPath != _T(""))
			{
				nSize = MyGetDiskFreeSpaceEx(CStringA(strPath));
			}
			
			// 하드용량 5기가 이상일 경우에만 Data Save 를 진행함...
			if(nSize > 5368709120)
			{
				auto publicParams = ::g_RecipeManager.m_sPublicParams;

				// 시간 가져오기...
				COleDateTime currentTime;
				CString strTime;
				currentTime = COleDateTime::GetCurrentTime();
				strTime = currentTime.Format(_T("%Y%m%d%H%M%S")); // 날짜, 시간, 분, 초

				if(publicParams.bSaveRawImage == TRUE)
				{
					RawImageSave(strTime);
				}

				if(publicParams.bSaveResultData == TRUE)
				{
					ResultDataSave(strTime);
				}

				if(publicParams.bSaveResultImage == TRUE)
				{
					ResultImageSave(strTime);
				}
			}
		}
		else // DataSave Thread 종료
		{
			break;
		}
	}
}
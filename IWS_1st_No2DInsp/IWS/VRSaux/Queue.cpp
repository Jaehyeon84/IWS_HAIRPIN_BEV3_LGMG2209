#include "../stdafx.h"
#include "Queue.h"
#include "Utility.h"

CQueue::CQueue()
{
	::InitializeCriticalSection(&m_csQueue);
}

CQueue::CQueue(int nImg, int nSize)
{
	::InitializeCriticalSection(&m_csQueue);
	m_imgSize = nSize;
	m_frontIdx = m_rearIdx = 0;
	m_nQueue = nImg;
	m_pQueue = new unsigned char* [nImg];
	for (int j = 0; j < nImg; j++)
	{
		m_pQueue[j] = new unsigned char[nSize];
		memset(m_pQueue[j] , 0, sizeof(unsigned char)*nSize);
	}
}

CQueue::~CQueue()
{
	::DeleteCriticalSection(&m_csQueue);

	for (int j = 0; j < m_nQueue; j++)
		delete[] m_pQueue[j];

	delete[] m_pQueue;
}

BOOL CQueue::QisEmpty()
{
	if (m_frontIdx == m_rearIdx)
	{
		//_tracer(TRACE_NORMAL, 9999, L"[QisEmpty()] Empty !(%d) ", m_frontIdx);
		return TRUE;
	}
	else
		return FALSE;
}

int  CQueue::NextPosIdx(int pos)
{
	return ((pos + 1) % m_nQueue);
}

BOOL CQueue::Push(unsigned char* pImg)
{
	::EnterCriticalSection(&m_csQueue);

	int nextIdx = NextPosIdx(m_rearIdx);
	if (NextPosIdx(m_rearIdx) == m_frontIdx) // full 이니 넣을 수 없고
	{
		g_Util.Logging(L"Queue Memory is full ! \n"); 
		//_tracer(TRACE_NORMAL, 9999, L"[Push()] Queue Memory is full ! ");

		::LeaveCriticalSection(&m_csQueue);
		return FALSE;
	}

	m_rearIdx = nextIdx;	// update rear index

	//_tracer(TRACE_NORMAL, 9999, L"[Push()] m_rearIdx %d", m_rearIdx);
	memcpy(m_pQueue[m_rearIdx], pImg, sizeof(unsigned char)*m_imgSize);

	::LeaveCriticalSection(&m_csQueue);
	return TRUE;
}

unsigned char* CQueue::Pop()
{
	::EnterCriticalSection(&m_csQueue);

	if (QisEmpty())						// empty 이니 뺄 수 없고
	{
		g_Util.Logging(L"Queue memory is empty!\n");	
		//_tracer(TRACE_NORMAL, 9999, L"[Pop()] Queue memory is empty!");

		::LeaveCriticalSection(&m_csQueue);
		return NULL;
	}


	m_frontIdx = NextPosIdx(m_frontIdx);	// update front index

	//_tracer(TRACE_NORMAL, 9999, L"[Pop()] m_frontIdx %d", m_frontIdx);

	::LeaveCriticalSection(&m_csQueue);
	return m_pQueue[m_frontIdx];
}

void CQueue::QClear()
{
	m_frontIdx = m_rearIdx = 0;
}

void CQueue::CopyQueue(unsigned char* pQueueBuffer, int nImageIndex)
{
	if (nImageIndex >= m_nQueue) // 오류
	{
		return;
	}

	// 메모리 복사

	if (pQueueBuffer)
	{
		memcpy(m_pQueue[nImageIndex], pQueueBuffer, sizeof(unsigned char)*m_imgSize);
	}
}

unsigned char* CQueue::GetQueBuffer(int nQueIndex)
{
	if (nQueIndex >= m_nQueue) // 오류
	{
		return NULL;
	}

	return m_pQueue[nQueIndex];
}
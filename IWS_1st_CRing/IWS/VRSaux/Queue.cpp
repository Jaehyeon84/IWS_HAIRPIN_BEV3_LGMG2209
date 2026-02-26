#include "../stdafx.h"
#include "Queue.h"
#include "Utility.h"

CQueue::CQueue()
{
}

CQueue::CQueue(int nImg, int nSize)
{
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
	for (int j = 0; j < m_nQueue; j++)
		delete[] m_pQueue[j];

	delete[] m_pQueue;
}

BOOL CQueue::QisEmpty()
{
	if (m_frontIdx == m_rearIdx)
		return TRUE;
	else
		return FALSE;
}

int  CQueue::NextPosIdx(int pos)
{
	return ((pos + 1) % m_nQueue);
}

BOOL CQueue::Push(unsigned char* pImg)
{
	int nextIdx = NextPosIdx(m_rearIdx);
	if (NextPosIdx(m_rearIdx) == m_frontIdx) // full 이니 넣을 수 없고
	{
		g_Util.Logging(L"Queue Memory is full ! \n"); 
		return FALSE;
	}

	m_rearIdx = nextIdx;	// update rear index
	memcpy(m_pQueue[m_rearIdx], pImg, sizeof(unsigned char)*m_imgSize);
	return TRUE;
}

unsigned char* CQueue::Pop()
{
	if (QisEmpty())						// empty 이니 뺄 수 없고
	{
		g_Util.Logging(L"Queue memory is empty!\n");	
		return NULL;
	}

	m_frontIdx = NextPosIdx(m_frontIdx);	// update front index
	return m_pQueue[m_frontIdx];
}

void CQueue::QClear()
{
	m_frontIdx = m_rearIdx = 0;
}
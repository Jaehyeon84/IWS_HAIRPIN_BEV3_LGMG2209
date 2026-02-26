#pragma once
#include "vrs.h"

class CQueue
{
public:
	CQueue();
	CQueue(int nImg, int nSize);
	~CQueue();

public:
	int m_imgSize;
	int m_nQueue;
	int m_frontIdx;
	int m_rearIdx;
	unsigned char **m_pQueue;

	CRITICAL_SECTION m_csQueue;

public:
	void QClear();
	BOOL QisEmpty();
	int  NextPosIdx(int pos);
	BOOL Push(unsigned char* pImg);
	unsigned char* Pop();

	// sjyi 2023.07.07 Vision Scan 오류 수정을 위한 함수(특정 큐에 메모리 값 저장)
	void CopyQueue(unsigned char* pQueueBuffer, int nImageIndex);
	unsigned char* GetQueBuffer(int nQueIndex);
};


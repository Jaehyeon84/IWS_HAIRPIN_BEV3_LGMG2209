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

public:
	void QClear();
	BOOL QisEmpty();
	int  NextPosIdx(int pos);
	BOOL Push(unsigned char* pImg);
	unsigned char* Pop();
};


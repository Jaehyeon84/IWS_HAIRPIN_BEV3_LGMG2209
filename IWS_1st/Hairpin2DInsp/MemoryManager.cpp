#include "pch.h"
#include "MemoryManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMemoryManager g_MemoryManager;

CMemoryManager::CMemoryManager()
{
	m_nOriginalImageWidth = 0;
	m_nOriginalImageHeight = 0;
	m_nUnrolledImageWidth = 0;
	m_nUnrolledImageHeight = 0;

	m_pptOriginalPos = nullptr;
	m_pptUnrolledPos = nullptr;
	m_nPosSizeX = 0;
	m_nPosSizeY = 0;

	ResetMemoryUse();
}

CMemoryManager::~CMemoryManager()
{
	Clear();
}

void CMemoryManager::Clear()
{
	for(long i = 0; i < (long)m_vecByteImage.size(); i++)
	{
		BYTE* pData = m_vecByteImage[i];
		delete[] pData;
	}
	m_vecByteImage.clear();
	for(long i = 0; i < (long)m_vecLongImage.size(); i++)
	{
		long* pData = m_vecLongImage[i];
		delete[] pData;
	}
	m_vecLongImage.clear();
	for(long i = 0; i < (long)m_vecFloatImage.size(); i++)
	{
		float* pData = m_vecFloatImage[i];
		delete[] pData;
	}
	m_vecFloatImage.clear();

	if(m_pptOriginalPos != nullptr)
	{
		delete[] m_pptOriginalPos;
		m_pptOriginalPos = nullptr;
	}

	if(m_pptUnrolledPos != nullptr)
	{
		delete[] m_pptUnrolledPos;
		m_pptUnrolledPos = nullptr;
	}

	ResetMemoryUse();
}

void CMemoryManager::SetImageSize(const long originalImageX, const long originalImageY, const long unrolledImageX, const long unrolledImageY)
{
	if(m_nOriginalImageWidth != originalImageX || m_nOriginalImageHeight != originalImageY || m_nUnrolledImageWidth != unrolledImageX || m_nUnrolledImageHeight != unrolledImageY)
	{
		Clear();

		m_nOriginalImageWidth = originalImageX;
		m_nOriginalImageHeight = originalImageY;
		m_nUnrolledImageWidth = unrolledImageX;
		m_nUnrolledImageHeight = unrolledImageY;

		m_nPosSizeX = 1500;
		m_nPosSizeY = m_nUnrolledImageHeight;

		SetReserveByteOriginalImage(2);
		SetReserveByteImage(15);
		SetReserveLongImage(1);
		SetReservePosBuffer();
	}
}

void CMemoryManager::ResetMemoryUse()
{
	m_nUseByteImage = 0;
	m_nUseLongImage = 0;
	m_nUseFloatImage = 0;
}

void CMemoryManager::SetReserveByteOriginalImage(long nNum)
{
	long i;
	for(i = (long)m_vecByteImage.size(); i < nNum; i++)
	{
		BYTE* pData = new BYTE[m_nOriginalImageWidth * m_nOriginalImageHeight];
		m_vecByteImage.push_back(pData);
	}
}

void CMemoryManager::SetReserveByteImage(long nNum)
{
	long i;
	for(i = (long)m_vecByteImage.size(); i < nNum; i++)
	{
		BYTE* pData = new BYTE[m_nUnrolledImageWidth * m_nUnrolledImageHeight];
		m_vecByteImage.push_back(pData);
	}
}

void CMemoryManager::SetReserveLongImage(long nNum)
{
	long i;
	for(i = (long)m_vecLongImage.size(); i < nNum; i++)
	{
		long* pData = new long[m_nUnrolledImageWidth * m_nUnrolledImageHeight];
		m_vecLongImage.push_back(pData);
	}
}

void CMemoryManager::SetReserveFloatImage(long nNum)
{
	long i;
	for(i = (long)m_vecFloatImage.size(); i < nNum; i++)
	{
		float* pData = new float[m_nUnrolledImageWidth * m_nUnrolledImageHeight];
		m_vecFloatImage.push_back(pData);
	}
}

void CMemoryManager::SetReservePosBuffer()
{
	m_pptOriginalPos = new CPoint[m_nPosSizeX * m_nPosSizeY];
	m_pptUnrolledPos = new CPoint[m_nPosSizeX * m_nPosSizeY];
}

BYTE* CMemoryManager::GetNextByteImage()
{
	BYTE* pData;
	if(m_nUseByteImage >= (long)m_vecByteImage.size())
	{
		pData = new BYTE[m_nUnrolledImageWidth * m_nUnrolledImageHeight];
		m_vecByteImage.push_back(pData);
	}

	pData = m_vecByteImage[m_nUseByteImage];

	m_nUseByteImage++;

	return pData;
}

long* CMemoryManager::GetNextLongImage()
{
	long* pData;
	if(m_nUseLongImage >= (long)m_vecLongImage.size())
	{
		pData = new long[m_nUnrolledImageWidth * m_nUnrolledImageHeight];
		m_vecLongImage.push_back(pData);
	}

	pData = m_vecLongImage[m_nUseLongImage];

	m_nUseLongImage++;

	return pData;
}

float* CMemoryManager::GetNextFloatImage()
{
	float* pData;
	if(m_nUseFloatImage >= (long)m_vecFloatImage.size())
	{
		pData = new float[m_nUnrolledImageWidth * m_nUnrolledImageHeight];
		m_vecFloatImage.push_back(pData);
	}

	pData = m_vecFloatImage[m_nUseFloatImage];

	m_nUseFloatImage++;

	return pData;
}

BYTE* CMemoryManager::GetByteImage(long nOrder)
{
	if(nOrder >= (long)m_vecByteImage.size())
	{
		SetReserveByteImage(nOrder + 1);
	}

	m_nUseByteImage = nOrder + 1;

	return m_vecByteImage[nOrder];
}

long* CMemoryManager::GetLongImage(long nOrder)
{
	if(nOrder >= (long)m_vecLongImage.size())
	{
		SetReserveLongImage(nOrder + 1);
	}

	m_nUseLongImage = nOrder + 1;

	return m_vecLongImage[nOrder];
}

float* CMemoryManager::GetFloatImage(long nOrder)
{
	if(nOrder >= (long)m_vecFloatImage.size())
	{
		SetReserveFloatImage(nOrder + 1);
	}

	m_nUseFloatImage = nOrder + 1;

	return m_vecFloatImage[nOrder];
}

CPoint* CMemoryManager::GetOriginalPos()
{
	return m_pptOriginalPos;
}

CPoint* CMemoryManager::GetUnrolledPos()
{
	return m_pptUnrolledPos;
}

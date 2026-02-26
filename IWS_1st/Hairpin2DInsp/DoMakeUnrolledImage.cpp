#include "pch.h"
#include "DetectionAlgorithm.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CDetectionAlgorithm::DoMakeUnrolledImage()
{
	BYTE* pbyUnrolledImage = ::g_MemoryManager.GetByteImage((long)IMAGE::UNROLLED);
	memset(pbyUnrolledImage, 0, m_nUnrolledImageSize);

	m_pptOriginalPos = ::g_MemoryManager.GetOriginalPos();
	m_pptUnrolledPos = ::g_MemoryManager.GetUnrolledPos();

	MakeRotateImage();

	BeginThread(THREAD_METHOD::UNROLLED_IMAGE);

	return TRUE;
};

BOOL CDetectionAlgorithm::MakeRotateImage()
{
	auto params = ::g_RecipeManager.m_sUnrolledParams;

	BYTE* pbyImage = ::g_MemoryManager.GetByteImage((long)IMAGE::SUB_ORIG);
	BYTE* pbyRotateImage = ::g_MemoryManager.GetByteImage((long)IMAGE::ROTATE);

	if(params.fUnrolledStartAngle != 0)
	{
		HUBIP::RotateImage(pbyImage, m_nSubOriginalImageWidth, m_nSubOriginalImageHeight, params.fUnrolledStartAngle, 0, 0, pbyRotateImage);
	}
	else // 입력한 각도가 0이면 회전할 필요없다.
	{
		memcpy(pbyRotateImage, pbyImage, m_nSubOriginalImageSize);
	}

	return TRUE;
}


void CDetectionAlgorithm::CalcUnrolledImage(const long startIndex, const long endIndex)
{
	auto params = ::g_RecipeManager.m_sUnrolledParams;

	BYTE* pbyRotateImage = ::g_MemoryManager.GetByteImage((long)IMAGE::ROTATE);
	BYTE* pbyUnrolledImage = ::g_MemoryManager.GetByteImage((long)IMAGE::UNROLLED);

	CPoint ptStart = CPoint(m_nSubOriginalImageWidth / 2, params.nUnrolledStartOffsetY);
	CPoint ptEnd = CPoint(m_nSubOriginalImageWidth / 2, params.nUnrolledStartOffsetY + params.nUnrolledLength);
	CPoint ptCenter = CPoint(m_nSubOriginalImageWidth / 2, m_nSubOriginalImageHeight / 2);

	HUBIP::RolledImageToUnrolledImage(pbyRotateImage, m_nSubOriginalImageWidth, m_nSubOriginalImageHeight, ptStart, ptEnd, ptCenter, UNROLLED_ANGLE, startIndex, endIndex, m_nUnrolledImageWidth, m_nUnrolledImageHeight, pbyUnrolledImage, m_pptOriginalPos, m_pptUnrolledPos);
}
#include "pch.h"
#include "DetectionAlgorithm.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CDetectionAlgorithm::DoMakeSubOriginalImage()
{
	auto params = ::g_RecipeManager.m_sUnrolledParams;

	BYTE* pbyOriImage = ::g_MemoryManager.GetByteImage((long)IMAGE::ORIG);
	BYTE* pbySubOriImage = ::g_MemoryManager.GetByteImage((long)IMAGE::SUB_ORIG);

	CPoint ptCenter = CPoint(m_nOriginalImageWidth / 2 + params.ptCenterPosCorrection.x, m_nOriginalImageHeight / 2 + params.ptCenterPosCorrection.y);

	// Original Image를 imageWidth, imageHeight 가 210mm 가 되도록 자른다.
	m_nSubOriginalImageWidth = (long)(params.fSubOriginalWidth_mm / (m_fResolX / 1000.f));
	if(m_nSubOriginalImageWidth % 2 != 0)
	{
		m_nSubOriginalImageWidth -= 1;
	}
	m_nSubOriginalImageHeight = (long)(params.fSubOriginalHeight_mm / (m_fResolY / 1000.f));
	if(m_nSubOriginalImageHeight % 2 != 0)
	{
		m_nSubOriginalImageHeight -= 1;
	}
	m_nSubOriginalImageSize = m_nSubOriginalImageWidth * m_nSubOriginalImageHeight;

	auto subOriginalHalfWidth = m_nSubOriginalImageWidth / 2;
	auto subOriginalHalfHeight = m_nSubOriginalImageHeight / 2;

	CRect rtRoi(0, 0, 0, 0);
	rtRoi.left = ptCenter.x - subOriginalHalfWidth;
	rtRoi.top = ptCenter.y - subOriginalHalfHeight;
	rtRoi.right = ptCenter.x + subOriginalHalfWidth;
	rtRoi.bottom = ptCenter.y + subOriginalHalfHeight;

	HUBIP::ImageCopy(pbyOriImage, m_nOriginalImageWidth, m_nOriginalImageHeight, rtRoi, pbySubOriImage, m_nSubOriginalImageWidth, m_nSubOriginalImageHeight, CRect(0, 0, m_nSubOriginalImageWidth, m_nSubOriginalImageHeight));

	return TRUE;
}
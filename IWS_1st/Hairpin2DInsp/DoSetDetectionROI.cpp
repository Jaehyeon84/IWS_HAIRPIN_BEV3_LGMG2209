#include "pch.h"
#include "DetectionAlgorithm.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CDetectionAlgorithm::DoSetDetectionROI()
{
	auto params = ::g_RecipeManager.m_sROIParams;

	long slotCount = params.nSlotCount;
	long layerCount = params.nLayerCount;
	long layerPair = layerCount / 2;

	m_vecrtHairpin.clear();
	m_vecrtHairpin.resize(params.nSlotCount);
	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerPair; layer++)
		{
			CRect rtROI;
			rtROI.left = layer * params.nLayerWidth + params.nOffsetX + params.pnEachROIOffsetX[slot];
			rtROI.top = slot * params.nSlotHeight;
			rtROI.right = rtROI.left + params.nLayerWidth;
			rtROI.bottom = rtROI.top + params.nSlotHeight;

			m_vecrtHairpin[slot].push_back(rtROI);
		}
	}

	for(long layer = 0; layer < layerPair; layer++)
	{
		m_vecrtHairpin[0][layer].top += 1;
		m_vecrtHairpin[slotCount - 1][layer].bottom -= 1;
	}

	if(slotCount > 0 && layerCount > 0)
	{
		m_rtHairpinWhole = m_vecrtHairpin[0][0];
		for(long slot = 0; slot < slotCount; slot++)
		{
			for(long layer = 0; layer < layerPair; layer++)
			{
				m_rtHairpinWhole |= m_vecrtHairpin[slot][layer];
			}
		}
	}

	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerPair; layer++)
		{
			auto &roi = m_vecrtHairpin[slot][layer];

			roi.top += params.nSlotTopOffset;
			roi.bottom -= params.nSlotBottomOffset;
		}
	}

	if(params.bUseAutoROI == TRUE)
	{
		for(long idx = 0; idx < params.nAutoROIProgressCount; idx++)
		{
			if(DoSetAutoROI() == FALSE)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CDetectionAlgorithm::DoSetAutoROI()
{
	auto params = ::g_RecipeManager.m_sROIParams;

	BYTE* pbyUnrolledImage = ::g_MemoryManager.GetByteImage((long)IMAGE::UNROLLED);
	CRect rtImage(0, 0, m_nUnrolledImageWidth, m_nUnrolledImageHeight);

	long slotCount = params.nSlotCount;
	long layerCount = params.nLayerCount;
	long layerPair = layerCount / 2;

	std::vector<std::vector<CRect>> vecrtProfileROI; // Profile ∞ËªÍ«“ ROI
	vecrtProfileROI.clear();
	vecrtProfileROI.resize(slotCount);

	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerPair; layer++)
		{
			CRect rtProfile(0, 0, 0, 0);
			auto &hairpinRoi = m_vecrtHairpin[slot][layer];

			rtProfile.left = hairpinRoi.left - hairpinRoi.Width() * ((float)params.nSearchROIWidthIncreaseRatio / 100.f);
			rtProfile.top = hairpinRoi.top;
			rtProfile.right = hairpinRoi.right + hairpinRoi.Width() * ((float)params.nSearchROIWidthIncreaseRatio / 100.f);
			rtProfile.bottom = hairpinRoi.bottom;

			HUBSDK::ImageCondition::VerifyROI2(rtImage, rtProfile);

			vecrtProfileROI[slot].push_back(rtProfile);
		}
	}
	
	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerPair; layer++)
		{
			float fProfileCenX(0.f), fProfileCenY(0.f);
			HUBIP::GetProfileCenter(pbyUnrolledImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, vecrtProfileROI[slot][layer], fProfileCenX, fProfileCenY);

			long diffX(0), diffY(0);
			if(fProfileCenX != 0)
			{
				diffX = (long)fProfileCenX - m_vecrtHairpin[slot][layer].CenterPoint().x;
			}
			if(fProfileCenY != 0)
			{
				diffY = (long)fProfileCenY - m_vecrtHairpin[slot][layer].CenterPoint().y;
			}

			HUBSDK::Geometry::ShiftRect(m_vecrtHairpin[slot][layer], CPoint(diffX, diffY));

			HUBSDK::ImageCondition::VerifyROI2(rtImage, m_vecrtHairpin[slot][layer]);
		}
	}

	// «—π¯¥ı
	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerPair; layer++)
		{
			float fProfileCenX(0.f), fProfileCenY(0.f);
			HUBIP::GetProfileCenter(pbyUnrolledImage, m_nUnrolledImageWidth, m_nUnrolledImageHeight, m_vecrtHairpin[slot][layer], fProfileCenX, fProfileCenY);

			long diffX(0), diffY(0);
			if(fProfileCenX != 0)
			{
				diffX = (long)fProfileCenX - m_vecrtHairpin[slot][layer].CenterPoint().x;
			}
			if(fProfileCenY != 0)
			{
				diffY = (long)fProfileCenY - m_vecrtHairpin[slot][layer].CenterPoint().y;
			}

			HUBSDK::Geometry::ShiftRect(m_vecrtHairpin[slot][layer], CPoint(diffX, diffY));

			HUBSDK::ImageCondition::VerifyROI2(rtImage, m_vecrtHairpin[slot][layer]);
		}
	}

	for(long slot = 0; slot < slotCount; slot++)
	{
		for(long layer = 0; layer < layerPair - 1; layer++)
		{
			CRect rtOverlap = m_vecrtHairpin[slot][layer] & m_vecrtHairpin[slot][layer + 1];
			auto overlapWidth = rtOverlap.Width();
			auto overlapHeight = rtOverlap.Height();

			// æ»∞„√∆¿ª ∞ÊøÏ
			if(overlapWidth == 0 && overlapHeight == 0)
			{
				auto dist = m_vecrtHairpin[slot][layer + 1].left - m_vecrtHairpin[slot][layer].right;

				m_vecrtHairpin[slot][layer].right += (dist / 2);
				m_vecrtHairpin[slot][layer + 1].left -= (dist / 2);
			}
			else if(overlapWidth != 0) // ∞„√∆¿ª ∞ÊøÏ
			{
				m_vecrtHairpin[slot][layer].right -= (overlapWidth / 2);
				m_vecrtHairpin[slot][layer + 1].left += (overlapWidth / 2);
			}
		}
	}

	for(long layer = 0; layer < layerPair; layer++)
	{
		for(long slot = 0; slot < slotCount - 1; slot++)
		{
			CRect rtOverlap = m_vecrtHairpin[slot][layer] & m_vecrtHairpin[slot + 1][layer];
			auto overlapWidth = rtOverlap.Width();
			auto overlapHeight = rtOverlap.Height();

			// æ»∞„√∆¿ª ∞ÊøÏ
			if(overlapWidth == 0 && overlapHeight == 0)
			{
				continue;
			}

			// ∞„√∆¿ª ∞ÊøÏ
			if(overlapHeight != 0)
			{
				m_vecrtHairpin[slot][layer].bottom -= (overlapHeight / 2);
				m_vecrtHairpin[slot + 1][layer].top += (overlapHeight / 2);
			}
		}
	}

	return TRUE;
}

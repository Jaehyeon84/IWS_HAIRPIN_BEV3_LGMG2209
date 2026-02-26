#include "pch.h"
#include "RecipeManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRecipeManager g_RecipeManager;

CRecipeManager::CRecipeManager()
{
}

CRecipeManager::~CRecipeManager()
{
}

void CRecipeManager::InitParameters()
{
	m_sUnrolledParams.Init();
	m_sROIParams.Init();
	m_sRemoveNoiseParams.Init();
	m_sInterHairpinParams.Init();
	m_sDetectBlobParams.Init();
	m_sHairpinCenterParams.Init();
	m_sCriteriaParams.Init();
	m_sPublicParams.Init();
}

BOOL CRecipeManager::LoadRecipeParameters(const CString strRecipePathName)
{
	CString strTemp;

	if(strRecipePathName == _T(""))
	{
		return FALSE;
	}

	// m_sUnrolledParams
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("UNROLLED"), _T("Sub Original Width mm"), &m_sUnrolledParams.fSubOriginalWidth_mm, m_sUnrolledParams.fSubOriginalWidth_mm);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("UNROLLED"), _T("Sub Original Height mm"), &m_sUnrolledParams.fSubOriginalHeight_mm, m_sUnrolledParams.fSubOriginalHeight_mm);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("UNROLLED"), _T("Center Pos. Correction"), &m_sUnrolledParams.ptCenterPosCorrection, m_sUnrolledParams.ptCenterPosCorrection);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("UNROLLED"), _T("Unrolled Start Angle"), &m_sUnrolledParams.fUnrolledStartAngle, m_sUnrolledParams.fUnrolledStartAngle);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("UNROLLED"), _T("Unrolled Start Offset Y"), &m_sUnrolledParams.nUnrolledStartOffsetY, m_sUnrolledParams.nUnrolledStartOffsetY);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("UNROLLED"), _T("Unrolled Length"), &m_sUnrolledParams.nUnrolledLength, m_sUnrolledParams.nUnrolledLength);

	// m_sROIParams
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Slot Count"), &m_sROIParams.nSlotCount, m_sROIParams.nSlotCount);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Layer Count"), &m_sROIParams.nLayerCount, m_sROIParams.nLayerCount);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Layer Width"), &m_sROIParams.nLayerWidth, m_sROIParams.nLayerWidth);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Slot Height"), &m_sROIParams.nSlotHeight, m_sROIParams.nSlotHeight);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Slot Top Offset"), &m_sROIParams.nSlotTopOffset, m_sROIParams.nSlotTopOffset);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Slot Bottom Offset"), &m_sROIParams.nSlotBottomOffset, m_sROIParams.nSlotBottomOffset);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Offset X"), &m_sROIParams.nOffsetX, m_sROIParams.nOffsetX);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Hairpin Area ROI Center To Left Offset"), &m_sROIParams.nHairpinAreaROICenterToLeftOffset, m_sROIParams.nHairpinAreaROICenterToLeftOffset);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Hairpin Area ROI Center To Top Offset"), &m_sROIParams.nHairpinAreaROICenterToTopOffset, m_sROIParams.nHairpinAreaROICenterToTopOffset);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Hairpin Area ROI Center To Right Offset"), &m_sROIParams.nHairpinAreaROICenterToRightOffset, m_sROIParams.nHairpinAreaROICenterToRightOffset);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Hairpin Area ROI Center To Bottom Offset"), &m_sROIParams.nHairpinAreaROICenterToBottomOffset, m_sROIParams.nHairpinAreaROICenterToBottomOffset);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Use Auto ROI"), &m_sROIParams.bUseAutoROI, m_sROIParams.bUseAutoROI);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Search ROI Width Increase Ratio"), &m_sROIParams.nSearchROIWidthIncreaseRatio, m_sROIParams.nSearchROIWidthIncreaseRatio);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Auto ROI Progress Count"), &m_sROIParams.nAutoROIProgressCount, m_sROIParams.nAutoROIProgressCount);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), _T("Start Slot Index"), &m_sROIParams.nStartSlotIndex, m_sROIParams.nStartSlotIndex);

	for(long idx = 0; idx < MAX_SLOT_NUM; idx++)
	{
		strTemp.Format(_T("Each ROI Offset X %d"), idx);
		HUBSDK::IniReadWriter::Read(strRecipePathName, _T("ROI"), strTemp, &m_sROIParams.pnEachROIOffsetX[idx], m_sROIParams.pnEachROIOffsetX[idx]);
	}

	// m_sRemoveNoiseParams
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Image Proc. Num"), &m_sRemoveNoiseParams.sProcParam.nImageProcNum, m_sRemoveNoiseParams.sProcParam.nImageProcNum);
	m_sRemoveNoiseParams.sProcParam.m_vecnProc.resize(m_sRemoveNoiseParams.sProcParam.nImageProcNum);
	for(long idx = 0; idx < m_sRemoveNoiseParams.sProcParam.nImageProcNum; idx++)
	{
		strTemp.Format(_T("Image_Proc_%d"), idx);
		HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), strTemp, &m_sRemoveNoiseParams.sProcParam.m_vecnProc[idx], m_sRemoveNoiseParams.sProcParam.m_vecnProc[idx]);
	}

	m_sRemoveNoiseParams.sProcParam.m_vecnProcParams.resize(m_sRemoveNoiseParams.sProcParam.nImageProcNum);
	for(long idx = 0; idx < m_sRemoveNoiseParams.sProcParam.nImageProcNum; idx++)
	{
		strTemp.Format(_T("Image_Proc_Param_%d"), idx);
		HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), strTemp, &m_sRemoveNoiseParams.sProcParam.m_vecnProcParams[idx], m_sRemoveNoiseParams.sProcParam.m_vecnProcParams[idx]);
	}

	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Select Threshold"), &m_sRemoveNoiseParams.nSelectThreshold, m_sRemoveNoiseParams.nSelectThreshold);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Threshold Value"), &m_sRemoveNoiseParams.nThresholdValue, m_sRemoveNoiseParams.nThresholdValue);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Threshold Low"), &m_sRemoveNoiseParams.nThresholdLow, m_sRemoveNoiseParams.nThresholdLow);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Threshold high"), &m_sRemoveNoiseParams.nThresholdHigh, m_sRemoveNoiseParams.nThresholdHigh);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Adaptive Offset"), &m_sRemoveNoiseParams.nAdaptiveOffset, m_sRemoveNoiseParams.nAdaptiveOffset);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Hysteresis Low"), &m_sRemoveNoiseParams.nHysteresisLow, m_sRemoveNoiseParams.nHysteresisLow);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Hysteresis High"), &m_sRemoveNoiseParams.nHysteresisHigh, m_sRemoveNoiseParams.nHysteresisHigh);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("High Threshold Region Noise Filtering Size"), &m_sRemoveNoiseParams.nHighThresholdRegionNoiseFilteringSize, m_sRemoveNoiseParams.nHighThresholdRegionNoiseFilteringSize);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Threshold Reverse"), &m_sRemoveNoiseParams.bThresholdReverse, m_sRemoveNoiseParams.bThresholdReverse);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Min Blob Size"), &m_sRemoveNoiseParams.nMinBlobSize, m_sRemoveNoiseParams.nMinBlobSize);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Max Blob Count"), &m_sRemoveNoiseParams.nMaxBlobCount, m_sRemoveNoiseParams.nMaxBlobCount);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Blob Merge"), &m_sRemoveNoiseParams.bBlobMerge, m_sRemoveNoiseParams.bBlobMerge);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Blob Merge Dist"), &m_sRemoveNoiseParams.nBlobMergeDist, m_sRemoveNoiseParams.nBlobMergeDist);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Use Remove Blob"), &m_sRemoveNoiseParams.bUseRemoveBlob, m_sRemoveNoiseParams.bUseRemoveBlob);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Aspect Ratio Min"), &m_sRemoveNoiseParams.fAspectRatioMin, m_sRemoveNoiseParams.fAspectRatioMin);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Aspect Ratio Max"), &m_sRemoveNoiseParams.fAspectRatioMax, m_sRemoveNoiseParams.fAspectRatioMax);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Use Mask Convolutioin"), &m_sRemoveNoiseParams.bUseMaskConvolution, m_sRemoveNoiseParams.bUseMaskConvolution);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Mask1Width"), &m_sRemoveNoiseParams.nMask1Width, m_sRemoveNoiseParams.nMask1Width);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Mask1Height"), &m_sRemoveNoiseParams.nMask1Height, m_sRemoveNoiseParams.nMask1Height);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Mask2Width"), &m_sRemoveNoiseParams.nMask2Width, m_sRemoveNoiseParams.nMask2Width);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("Mask2Height"), &m_sRemoveNoiseParams.nMask2Height, m_sRemoveNoiseParams.nMask2Height);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("fRef1"), &m_sRemoveNoiseParams.fRef1, m_sRemoveNoiseParams.fRef1);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Remove Noise"), _T("fRef2"), &m_sRemoveNoiseParams.fRef2, m_sRemoveNoiseParams.fRef2);

	// m_sInterHairpinParams
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("Image Proc. Num"), &m_sInterHairpinParams.sProcParam.nImageProcNum, m_sInterHairpinParams.sProcParam.nImageProcNum);
	m_sInterHairpinParams.sProcParam.m_vecnProc.resize(m_sInterHairpinParams.sProcParam.nImageProcNum);
	for(long idx = 0; idx < m_sInterHairpinParams.sProcParam.nImageProcNum; idx++)
	{
		strTemp.Format(_T("Image_Proc_%d"), idx);
		HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), strTemp, &m_sInterHairpinParams.sProcParam.m_vecnProc[idx], m_sInterHairpinParams.sProcParam.m_vecnProc[idx]);
	}

	m_sInterHairpinParams.sProcParam.m_vecnProcParams.resize(m_sInterHairpinParams.sProcParam.nImageProcNum);
	for(long idx = 0; idx < m_sInterHairpinParams.sProcParam.nImageProcNum; idx++)
	{
		strTemp.Format(_T("Image_Proc_Param_%d"), idx);
		HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), strTemp, &m_sInterHairpinParams.sProcParam.m_vecnProcParams[idx], m_sInterHairpinParams.sProcParam.m_vecnProcParams[idx]);
	}

	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("Select Threshold"), &m_sInterHairpinParams.nSelectThreshold, m_sInterHairpinParams.nSelectThreshold);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("Threshold Value"), &m_sInterHairpinParams.nThresholdValue, m_sInterHairpinParams.nThresholdValue);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("Adaptive Offset"), &m_sInterHairpinParams.nAdaptiveOffset, m_sInterHairpinParams.nAdaptiveOffset);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("Threshold Reverse"), &m_sInterHairpinParams.bThresholdReverse, m_sInterHairpinParams.bThresholdReverse);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("Hysteresis Low"), &m_sInterHairpinParams.nHysteresisLow, m_sInterHairpinParams.nHysteresisLow);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("Hysteresis High"), &m_sInterHairpinParams.nHysteresisHigh, m_sInterHairpinParams.nHysteresisHigh);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("High Threshold Region Noise Filtering Size"), &m_sInterHairpinParams.nHighThresholdRegionNoiseFilteringSize, m_sInterHairpinParams.nHighThresholdRegionNoiseFilteringSize);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("Min Blob Size"), &m_sInterHairpinParams.nMinBlobSize, m_sInterHairpinParams.nMinBlobSize);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("Max Blob Count"), &m_sInterHairpinParams.nMaxBlobCount, m_sInterHairpinParams.nMaxBlobCount);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("Blob Merge"), &m_sInterHairpinParams.bBlobMerge, m_sInterHairpinParams.bBlobMerge);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("Blob Merge Dist"), &m_sInterHairpinParams.nBlobMergeDist, m_sInterHairpinParams.nBlobMergeDist);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("Interpolation X"), &m_sInterHairpinParams.bInterpolationX, m_sInterHairpinParams.bInterpolationX);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("InterPolation Hairpin"), _T("Interpolation Y"), &m_sInterHairpinParams.bInterpolationY, m_sInterHairpinParams.bInterpolationY);
	
	// m_sDetectBlobParams
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Detect Blob"), _T("Min Blob Size"), &m_sDetectBlobParams.nMinBlobSize, m_sDetectBlobParams.nMinBlobSize);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Detect Blob"), _T("Max Blob Count"), &m_sDetectBlobParams.nMaxBlobCount, m_sDetectBlobParams.nMaxBlobCount);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Detect Blob"), _T("Blob Merge"), &m_sDetectBlobParams.bBlobMerge, m_sDetectBlobParams.bBlobMerge);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Detect Blob"), _T("Blob Merge Dist"), &m_sDetectBlobParams.nBlobMergeDist, m_sDetectBlobParams.nBlobMergeDist);

	// m_sHairpinCenterParams
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Width Size 1 Blob Min Size"), &m_sHairpinCenterParams.nWidthSize_1BlobMinSize, m_sHairpinCenterParams.nWidthSize_1BlobMinSize);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Width Size 2 Blob Min Size"), &m_sHairpinCenterParams.nWidthSize_2BlobMinSize, m_sHairpinCenterParams.nWidthSize_2BlobMinSize);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Width Size 2 More Blob Min Size"), &m_sHairpinCenterParams.nWidthSize_2MoreBlobMinSize, m_sHairpinCenterParams.nWidthSize_2MoreBlobMinSize);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Use Complement Hairpin Center"), &m_sHairpinCenterParams.bUseComplementHairpinCenter, m_sHairpinCenterParams.bUseComplementHairpinCenter);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Complement Height Size"), &m_sHairpinCenterParams.nComplementHeightSize, m_sHairpinCenterParams.nComplementHeightSize);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Complement Height Limit"), &m_sHairpinCenterParams.nComplementHeightLimit, m_sHairpinCenterParams.nComplementHeightLimit);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Min Hairpin Width"), &m_sHairpinCenterParams.nMinHairpinWidth, m_sHairpinCenterParams.nMinHairpinWidth);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Min Hairpin Height"), &m_sHairpinCenterParams.nMinHairpinHeight, m_sHairpinCenterParams.nMinHairpinHeight);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Max Hairpin Width"), &m_sHairpinCenterParams.nMaxHairpinWidth, m_sHairpinCenterParams.nMaxHairpinWidth);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Max Hairpin Height"), &m_sHairpinCenterParams.nMaxHairpinHeight, m_sHairpinCenterParams.nMaxHairpinHeight);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Min Hairpin Width for 1 Blob"), &m_sHairpinCenterParams.nMinHairpinWidthfor1Blob, m_sHairpinCenterParams.nMinHairpinWidthfor1Blob);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Use Cal Center Offset replace3D"), &m_sHairpinCenterParams.bUseCalCenterOffset_replace3D, m_sHairpinCenterParams.bUseCalCenterOffset_replace3D);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Use Correction Offset replace3D"), &m_sHairpinCenterParams.bUseCorrectionOffset_replace3D, m_sHairpinCenterParams.bUseCorrectionOffset_replace3D);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("2D 3D Posion Limit X"), &m_sHairpinCenterParams.f2D3DPositionLimitX, m_sHairpinCenterParams.f2D3DPositionLimitX);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("2D 3D Posion Limit Y"), &m_sHairpinCenterParams.f2D3DPositionLimitY, m_sHairpinCenterParams.f2D3DPositionLimitY);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Hairpin Center"), _T("Use LayerPair Replace"), &m_sHairpinCenterParams.bUseLayerPairReplace, m_sHairpinCenterParams.bUseLayerPairReplace);

	// m_sCriteriaParams
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Criteria"), _T("Min_H12"), &m_sCriteriaParams.fMin_H12, m_sCriteriaParams.fMin_H12);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Criteria"), _T("Max_H12"), &m_sCriteriaParams.fMax_H12, m_sCriteriaParams.fMax_H12);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Criteria"), _T("Max_H3"), &m_sCriteriaParams.fMax_H3, m_sCriteriaParams.fMax_H3);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Criteria"), _T("Min_C"), &m_sCriteriaParams.fMin_C, m_sCriteriaParams.fMin_C);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Criteria"), _T("Max_C"), &m_sCriteriaParams.fMax_C, m_sCriteriaParams.fMax_C);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Criteria"), _T("Min_T"), &m_sCriteriaParams.fMin_T, m_sCriteriaParams.fMin_T);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Criteria"), _T("Max_T"), &m_sCriteriaParams.fMax_T, m_sCriteriaParams.fMax_T);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Criteria"), _T("Min_J"), &m_sCriteriaParams.fMin_J, m_sCriteriaParams.fMin_J);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Criteria"), _T("Max_J"), &m_sCriteriaParams.fMax_J, m_sCriteriaParams.fMax_J);

	// m_sPublicParams
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Public"), _T("Save Result Image"), &m_sPublicParams.bSaveResultImage, m_sPublicParams.bSaveResultImage);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Public"), _T("Result Image Save Path"), &m_sPublicParams.strResultImageSavePath, m_sPublicParams.strResultImageSavePath);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Public"), _T("Save Result Data"), &m_sPublicParams.bSaveResultData, m_sPublicParams.bSaveResultData);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Public"), _T("Result Data Save Path"), &m_sPublicParams.strResultDataSavePath, m_sPublicParams.strResultDataSavePath);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Public"), _T("Replace 3D Result"), &m_sPublicParams.bReplace3DResult, m_sPublicParams.bReplace3DResult);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Public"), _T("Save Raw Image"), &m_sPublicParams.bSaveRawImage, m_sPublicParams.bSaveRawImage);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Public"), _T("Raw Image Save Path"), &m_sPublicParams.strRawImageSavePath, m_sPublicParams.strRawImageSavePath);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Public"), _T("Save Result Image For JigA"), &m_sPublicParams.bSaveResultImageForJigA, m_sPublicParams.bSaveResultImageForJigA);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Public"), _T("Raw Image Save Path For JigA"), &m_sPublicParams.strResultImageSavePathForJigA, m_sPublicParams.strResultImageSavePathForJigA);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Public"), _T("Save Result Image For JigB"), &m_sPublicParams.bSaveResultImageForJigB, m_sPublicParams.bSaveResultImageForJigB);
	HUBSDK::IniReadWriter::Read(strRecipePathName, _T("Public"), _T("Raw Image Save Path For JigB"), &m_sPublicParams.strResultImageSavePathForJigB, m_sPublicParams.strResultImageSavePathForJigB);

	return TRUE;
}

BOOL CRecipeManager::SaveRecipeParameters(const CString strRecipePathName)
{
	CString strTemp;

	if(strRecipePathName == _T(""))
	{
		return FALSE;
	}

	// m_sUnrolledParams
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("UNROLLED"), _T("Sub Original Width mm"), &m_sUnrolledParams.fSubOriginalWidth_mm);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("UNROLLED"), _T("Sub Original Height mm"), &m_sUnrolledParams.fSubOriginalHeight_mm);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("UNROLLED"), _T("Center Pos. Correction"), &m_sUnrolledParams.ptCenterPosCorrection);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("UNROLLED"), _T("Unrolled Start Angle"), &m_sUnrolledParams.fUnrolledStartAngle);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("UNROLLED"), _T("Unrolled Start Offset Y"), &m_sUnrolledParams.nUnrolledStartOffsetY);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("UNROLLED"), _T("Unrolled Length"), &m_sUnrolledParams.nUnrolledLength);

	// m_sROIParams
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Slot Count"), &m_sROIParams.nSlotCount);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Layer Count"), &m_sROIParams.nLayerCount);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Layer Width"), &m_sROIParams.nLayerWidth);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Slot Height"), &m_sROIParams.nSlotHeight);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Slot Top Offset"), &m_sROIParams.nSlotTopOffset);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Slot Bottom Offset"), &m_sROIParams.nSlotBottomOffset);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Offset X"), &m_sROIParams.nOffsetX);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Hairpin Area ROI Center To Left Offset"), &m_sROIParams.nHairpinAreaROICenterToLeftOffset);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Hairpin Area ROI Center To Top Offset"), &m_sROIParams.nHairpinAreaROICenterToTopOffset);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Hairpin Area ROI Center To Right Offset"), &m_sROIParams.nHairpinAreaROICenterToRightOffset);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Hairpin Area ROI Center To Bottom Offset"), &m_sROIParams.nHairpinAreaROICenterToBottomOffset);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Use Auto ROI"), &m_sROIParams.bUseAutoROI);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Search ROI Width Increase Ratio"), &m_sROIParams.nSearchROIWidthIncreaseRatio);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Auto ROI Progress Count"), &m_sROIParams.nAutoROIProgressCount);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), _T("Start Slot Index"), &m_sROIParams.nStartSlotIndex);

	for(long idx = 0; idx < MAX_SLOT_NUM; idx++)
	{
		strTemp.Format(_T("Each ROI Offset X %d"), idx);
		HUBSDK::IniReadWriter::Write(strRecipePathName, _T("ROI"), strTemp, &m_sROIParams.pnEachROIOffsetX[idx]);
	}

	// m_sRemoveNoiseParams
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Image Proc. Num"), &m_sRemoveNoiseParams.sProcParam.nImageProcNum);
	m_sRemoveNoiseParams.sProcParam.m_vecnProc.resize(m_sRemoveNoiseParams.sProcParam.nImageProcNum);
	for(long idx = 0; idx < m_sRemoveNoiseParams.sProcParam.nImageProcNum; idx++)
	{
		strTemp.Format(_T("Image_Proc_%d"), idx);
		HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), strTemp, &m_sRemoveNoiseParams.sProcParam.m_vecnProc[idx]);
	}

	m_sRemoveNoiseParams.sProcParam.m_vecnProcParams.resize(m_sRemoveNoiseParams.sProcParam.nImageProcNum);
	for(long idx = 0; idx < m_sRemoveNoiseParams.sProcParam.nImageProcNum; idx++)
	{
		strTemp.Format(_T("Image_Proc_Param_%d"), idx);
		HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), strTemp, &m_sRemoveNoiseParams.sProcParam.m_vecnProcParams[idx]);
	}

	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Select Threshold"), &m_sRemoveNoiseParams.nSelectThreshold);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Threshold Value"), &m_sRemoveNoiseParams.nThresholdValue);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Threshold Low"), &m_sRemoveNoiseParams.nThresholdLow);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Threshold high"), &m_sRemoveNoiseParams.nThresholdHigh);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Adaptive Offset"), &m_sRemoveNoiseParams.nAdaptiveOffset);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Hysteresis Low"), &m_sRemoveNoiseParams.nHysteresisLow);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Hysteresis High"), &m_sRemoveNoiseParams.nHysteresisHigh);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("High Threshold Region Noise Filtering Size"), &m_sRemoveNoiseParams.nHighThresholdRegionNoiseFilteringSize);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Threshold Reverse"), &m_sRemoveNoiseParams.bThresholdReverse);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Min Blob Size"), &m_sRemoveNoiseParams.nMinBlobSize);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Max Blob Count"), &m_sRemoveNoiseParams.nMaxBlobCount);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Blob Merge"), &m_sRemoveNoiseParams.bBlobMerge);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Blob Merge Dist"), &m_sRemoveNoiseParams.nBlobMergeDist);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Use Remove Blob"), &m_sRemoveNoiseParams.bUseRemoveBlob);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Aspect Ratio Min"), &m_sRemoveNoiseParams.fAspectRatioMin);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Aspect Ratio Max"), &m_sRemoveNoiseParams.fAspectRatioMax);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Use Mask Convolutioin"), &m_sRemoveNoiseParams.bUseMaskConvolution);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Mask1Width"), &m_sRemoveNoiseParams.nMask1Width);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Mask1Height"), &m_sRemoveNoiseParams.nMask1Height);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Mask2Width"), &m_sRemoveNoiseParams.nMask2Width);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("Mask2Height"), &m_sRemoveNoiseParams.nMask2Height);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("fRef1"), &m_sRemoveNoiseParams.fRef1);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Remove Noise"), _T("fRef2"), &m_sRemoveNoiseParams.fRef2);

	// m_sInterHairpinParams
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("Image Proc. Num"), &m_sInterHairpinParams.sProcParam.nImageProcNum);
	m_sInterHairpinParams.sProcParam.m_vecnProc.resize(m_sInterHairpinParams.sProcParam.nImageProcNum);
	for(long idx = 0; idx < m_sInterHairpinParams.sProcParam.nImageProcNum; idx++)
	{
		strTemp.Format(_T("Image_Proc_%d"), idx);
		HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), strTemp, &m_sInterHairpinParams.sProcParam.m_vecnProc[idx]);
	}

	m_sInterHairpinParams.sProcParam.m_vecnProcParams.resize(m_sInterHairpinParams.sProcParam.nImageProcNum);
	for(long idx = 0; idx < m_sInterHairpinParams.sProcParam.nImageProcNum; idx++)
	{
		strTemp.Format(_T("Image_Proc_Param_%d"), idx);
		HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), strTemp, &m_sInterHairpinParams.sProcParam.m_vecnProcParams[idx]);
	}

	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("Select Threshold"), &m_sInterHairpinParams.nSelectThreshold);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("Threshold Value"), &m_sInterHairpinParams.nThresholdValue);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("Adaptive Offset"), &m_sInterHairpinParams.nAdaptiveOffset);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("Hysteresis Low"), &m_sInterHairpinParams.nHysteresisLow);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("Hysteresis High"), &m_sInterHairpinParams.nHysteresisHigh);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("High Threshold Region Noise Filtering Size"), &m_sInterHairpinParams.nHighThresholdRegionNoiseFilteringSize);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("Threshold Reverse"), &m_sInterHairpinParams.bThresholdReverse);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("Min Blob Size"), &m_sInterHairpinParams.nMinBlobSize);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("Max Blob Count"), &m_sInterHairpinParams.nMaxBlobCount);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("Blob Merge"), &m_sInterHairpinParams.bBlobMerge);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("Blob Merge Dist"), &m_sInterHairpinParams.nBlobMergeDist);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("Interpolation X"), &m_sInterHairpinParams.bInterpolationX);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("InterPolation Hairpin"), _T("Interpolation Y"), &m_sInterHairpinParams.bInterpolationY);
	
	// m_sDetectBlobParams
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Detect Blob"), _T("Min Blob Size"), &m_sDetectBlobParams.nMinBlobSize);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Detect Blob"), _T("Max Blob Count"), &m_sDetectBlobParams.nMaxBlobCount);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Detect Blob"), _T("Blob Merge"), &m_sDetectBlobParams.bBlobMerge);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Detect Blob"), _T("Blob Merge Dist"), &m_sDetectBlobParams.nBlobMergeDist);

	// m_sHairpinCenterParams
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Width Size 1 Blob Min Size"), &m_sHairpinCenterParams.nWidthSize_1BlobMinSize);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Width Size 2 Blob Min Size"), &m_sHairpinCenterParams.nWidthSize_2BlobMinSize);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Width Size 2 More Blob Min Size"), &m_sHairpinCenterParams.nWidthSize_2MoreBlobMinSize);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Use Complement Hairpin Center"), &m_sHairpinCenterParams.bUseComplementHairpinCenter);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Complement Height Size"), &m_sHairpinCenterParams.nComplementHeightSize);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Complement Height Limit"), &m_sHairpinCenterParams.nComplementHeightLimit);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Min Hairpin Width"), &m_sHairpinCenterParams.nMinHairpinWidth);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Min Hairpin Height"), &m_sHairpinCenterParams.nMinHairpinHeight);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Max Hairpin Width"), &m_sHairpinCenterParams.nMaxHairpinWidth);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Max Hairpin Height"), &m_sHairpinCenterParams.nMaxHairpinHeight);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Min Hairpin Width for 1 Blob"), &m_sHairpinCenterParams.nMinHairpinWidthfor1Blob);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Use Cal Center Offset replace3D"), &m_sHairpinCenterParams.bUseCalCenterOffset_replace3D);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Use Correction Offset replace3D"), &m_sHairpinCenterParams.bUseCorrectionOffset_replace3D);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("2D 3D Posion Limit X"), &m_sHairpinCenterParams.f2D3DPositionLimitX);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("2D 3D Posion Limit Y"), &m_sHairpinCenterParams.f2D3DPositionLimitY);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Hairpin Center"), _T("Use LayerPair Replace"), &m_sHairpinCenterParams.bUseLayerPairReplace);

	// m_sCriteriaParams
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Criteria"), _T("Min_H12"), &m_sCriteriaParams.fMin_H12);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Criteria"), _T("Max_H12"), &m_sCriteriaParams.fMax_H12);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Criteria"), _T("Max_H3"), &m_sCriteriaParams.fMax_H3);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Criteria"), _T("Min_C"), &m_sCriteriaParams.fMin_C);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Criteria"), _T("Max_C"), &m_sCriteriaParams.fMax_C);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Criteria"), _T("Min_T"), &m_sCriteriaParams.fMin_T);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Criteria"), _T("Max_T"), &m_sCriteriaParams.fMax_T);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Criteria"), _T("Min_J"), &m_sCriteriaParams.fMin_J);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Criteria"), _T("Max_J"), &m_sCriteriaParams.fMax_J);

	// m_sPublicParams
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Public"), _T("Save Result Image"), &m_sPublicParams.bSaveResultImage);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Public"), _T("Result Image Save Path"), &m_sPublicParams.strResultImageSavePath);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Public"), _T("Save Result Data"), &m_sPublicParams.bSaveResultData);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Public"), _T("Result Data Save Path"), &m_sPublicParams.strResultDataSavePath);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Public"), _T("Replace 3D Result"), &m_sPublicParams.bReplace3DResult);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Public"), _T("Save Raw Image"), &m_sPublicParams.bSaveRawImage);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Public"), _T("Raw Image Save Path"), &m_sPublicParams.strRawImageSavePath);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Public"), _T("Save Result Image For JigA"), &m_sPublicParams.bSaveResultImageForJigA);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Public"), _T("Raw Image Save Path For JigA"), &m_sPublicParams.strResultImageSavePathForJigA);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Public"), _T("Save Result Image For JigB"), &m_sPublicParams.bSaveResultImageForJigB);
	HUBSDK::IniReadWriter::Write(strRecipePathName, _T("Public"), _T("Raw Image Save Path For JigB"), &m_sPublicParams.strResultImageSavePathForJigB);

	return TRUE;
}


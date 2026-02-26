#pragma once

#include <algorithm>
#include <vector>
#include <list>
#include <string.h>
#include <fstream>
#include <iostream>
#include <string>
#include <memory>

#include "Algorithm/ImageProcessing.h"
#include "Algorithm/ImageProcessingHelper.h"
#include "Algorithm/ImageCondition.h"
#include "Algorithm/Geometry.h"
#include "Algorithm/BlobInfo.h"
#include "BaseMethod/IniReadWriter.h"
#include "BaseMethod/ImageInterface.h"
#include "BaseMethod/ThreadedProgress.h"
#include "BaseMethod/Label.h"
#include "ImageViewer/ImageDisplay.h"
#include "ImageViewer/EventMessage.h"
#include "GridControl/GridCtrl.h"

#pragma comment(lib, "HubisSDK.Library.Algorithm.lib")
#pragma comment(lib, "HubisSDK.Library.BaseMethod.lib")
#pragma comment( lib, "HubisSDK.Library.ImageViewer.lib" )

#ifdef _DEBUG
#pragma comment(lib, "HubisSDK.Library.GridControl_d.lib")
#else
#pragma comment(lib, "HubisSDK.Library.GridControl.lib")
#endif

#define PI 3.141592
#define TwoPI 2 * PI;

#define IsSHIFTPressed() ((GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT) * 8 - 1))) != 0)
#define IsCTRLPressed()  ((GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT) * 8 - 1))) != 0)

#define REMOVE_EXT_RECIPE_FROM_FILENAME(x)		( (x).Left ( (x).Find(_T(".recipe")) ) )
#define REMOVE_EXT_SURF_FROM_FILENAME(x)			( (x).Left ( (x).Find(_T(".surf")) ) )
#define REMOVE_EXT_BMP_FROM_FILENAME(x)			( (x).Left ( (x).Find(_T(".bmp")) ) )
#define REMOVE_EXT_JPG_FROM_FILENAME(x)			( (x).Left ( (x).Find(_T(".jpg")) ) )
#define REMOVE_FILE_FROM_PATHNAME(x)				( (x).Left((x).ReverseFind('\\')) )
#define REMOVE_FOLDER_FROM_PATHNAME(x)			( (x).Right( (x).GetLength() - (x).ReverseFind('\\') - 1 ) )
#define REMOVE_EXTENSION_FROM_PATHNAME(x)		( (x).Right( (x).GetLength() - (x).ReverseFind('.') - 1) )
#define REMOVE_EXT_FROM_FILENAME(x)					( (x).Left ( (x).ReverseFind('.') ) )

#define UNROLLED_IMAGE_WIDTH 10000
#define UNROLLED_IMAGE_HEIGHT 14400
#define UNROLLED_ANGLE 0.025f
#define MAX_SLOT_NUM 96
#define MAX_LAYER_NUM 10
#define REMOVE_NOISE_THREAD_COUNT 8

#define VECTOR_SIZE(v) (long)v.size()

enum class IMAGE : long
{
	ORIG,
	SUB_ORIG,
	ROTATE,
	UNROLLED,
	REMOVE_NOISE_PROC,
	REMOVE_NOISE_THRESHOLD,
	REMOVE_NOISE_HYSTERESIS_TEMP,
	REMOVE_NOISE_HYSTERESIS_MERGE,
	REMOVE_NOISE_MASK,
	REMOVE_NOISE_SUB,
	REMOVE_NOISE_RESULT,
	INTER_PROC,
	INTER_THRESHOLD,
	INTER_HYSTERESIS_TEMP,
	INTER_HYSTERESIS_MERGE,
};

enum class THRESHOLD : long
{
	NORMAL,
	DOUBLE,
	ADAPTIVE,
	HYSTERISIS,
};

enum class THREAD_METHOD : long
{
	UNROLLED_IMAGE,
	REMOVE_NOISE,
};

enum
{
	INSP_ERR_OK = 1,	//OK
	INSP_ERR = 0,
	INSP_ERR_F1 = -1,	// Not found HP1
	INSP_ERR_F2 = -2,	// Not found HP2
	INSP_ERR_F12 = -3,	// Not found HP1&HP2 => -3 = INSP_ERR_F1 + INSP_ERR_F2 = -1 -2 
	INSP_ERR_H1 = -4,	// Inner hairpin spec. NG
	INSP_ERR_H2 = -5,	// Outer hairpin spec. NG
	INSP_ERR_H3 = -6,	// Inner-Outer step-height spec. NG
	INSP_ERR_C = -7,	// Inner-Outer distance spec.NG
	INSP_ERR_T = -8,	//
	INSP_ERR_J = -9,	// [curruent layer-pair Outer]  -- [next layer pair Inner] distance spec.NG
	INSP_ERR_A1 = -10,	//
	INSP_ERR_A2 = -11	//
};

typedef struct SProcImageParams
{
	long nImageProcNum;
	std::vector<short> m_vecnProc;
	std::vector<short> m_vecnProcParams;

	void Init()
	{
		nImageProcNum = 0;
		m_vecnProc.clear();
		m_vecnProcParams.clear();
	}
}SProc;

typedef struct SUnrolledParams
{
	float fSubOriginalWidth_mm;
	float fSubOriginalHeight_mm;
	CPoint ptCenterPosCorrection;
	float fUnrolledStartAngle;
	long nUnrolledStartOffsetY;
	long nUnrolledLength;

	void Init()
	{
		fSubOriginalWidth_mm = 210.f;
		fSubOriginalHeight_mm = 210.f;
		ptCenterPosCorrection = CPoint(0, 0);
		fUnrolledStartAngle = 0.f;
		nUnrolledStartOffsetY = 200;
		nUnrolledLength = 1000;
	}
}SUnrolledParams;

typedef struct SROIParams
{
	long nSlotCount;
	long nLayerCount;
	long nLayerWidth;
	long nSlotHeight;
	long nSlotTopOffset;
	long nSlotBottomOffset;
	long nOffsetX;
	long nHairpinAreaROICenterToLeftOffset;
	long nHairpinAreaROICenterToTopOffset;
	long nHairpinAreaROICenterToRightOffset;
	long nHairpinAreaROICenterToBottomOffset;
	long pnEachROIOffsetX[MAX_SLOT_NUM];
	BOOL bUseAutoROI;
	long nSearchROIWidthIncreaseRatio;
	long nAutoROIProgressCount;
	long nStartSlotIndex;

	void Init()
	{
		nSlotCount = 96;
		nLayerCount = 8;
		nLayerWidth = 220;
		nSlotHeight = 150;
		nSlotTopOffset = 0;
		nSlotBottomOffset = 0;
		nOffsetX = 0;
		nHairpinAreaROICenterToLeftOffset = 10;
		nHairpinAreaROICenterToTopOffset = 10;
		nHairpinAreaROICenterToRightOffset = 10;
		nHairpinAreaROICenterToBottomOffset = 10;
		bUseAutoROI = FALSE;
		nSearchROIWidthIncreaseRatio = 10;
		nAutoROIProgressCount = 1;
		nStartSlotIndex = 1;

		for(long idx = 0; idx < MAX_SLOT_NUM; idx++)
		{
			pnEachROIOffsetX[idx] = 0;
		}
	}
}SROIParams;

typedef struct SRemoveNoiseParams
{
	SProc sProcParam;
	long nSelectThreshold;
	long nThresholdValue;
	long nThresholdLow;
	long nThresholdHigh;
	long nAdaptiveOffset;
	long nHysteresisLow;
	long nHysteresisHigh;
	long nHighThresholdRegionNoiseFilteringSize;
	BOOL bThresholdReverse;
	long nMinBlobSize;
	long nMaxBlobCount;
	BOOL bBlobMerge;
	long nBlobMergeDist;
	BOOL bUseRemoveBlob;
	float fAspectRatioMin;
	float fAspectRatioMax;
	BOOL bUseMaskConvolution;
	long nMask1Width;
	long nMask1Height;
	long nMask2Width;
	long nMask2Height;
	float fRef1;
	float fRef2;

	void Init()
	{
		sProcParam.Init();
		nSelectThreshold = 0;
		nThresholdValue = 128;
		nThresholdLow = 100;
		nThresholdHigh = 200;
		nAdaptiveOffset = 10;
		nHysteresisLow = 30;
		nHysteresisHigh = 225;
		nHighThresholdRegionNoiseFilteringSize = 10;
		bThresholdReverse = FALSE;
		nMinBlobSize = 100;
		nMaxBlobCount = 10000;
		bBlobMerge = FALSE;
		nBlobMergeDist = 1;
		bUseRemoveBlob = FALSE;
		fAspectRatioMin = 0.f;
		fAspectRatioMax = 100.f;
		bUseMaskConvolution = FALSE;
		nMask1Width = 100;
		nMask1Height = 45;
		nMask2Width = 45;
		nMask2Height = 35;
		fRef1 = 0.9f;
		fRef2 = 0.8f;
	}
}SRemoveNoiseParams;

typedef struct SInterpolationHairpinParams
{
	SProc sProcParam;
	long nSelectThreshold;
	long nThresholdValue;
	long nAdaptiveOffset;
	long nHysteresisLow;
	long nHysteresisHigh;
	BOOL bThresholdReverse;
	long nHighThresholdRegionNoiseFilteringSize;
	long nMinBlobSize;
	long nMaxBlobCount;
	BOOL bBlobMerge;
	long nBlobMergeDist;
	BOOL bInterpolationX;
	BOOL bInterpolationY;

	void Init()
	{
		sProcParam.Init();
		nSelectThreshold = 0;
		nThresholdValue = 128;
		nAdaptiveOffset = 10;
		nHysteresisLow = 30;
		nHysteresisHigh = 225;
		bThresholdReverse = FALSE;
		nHighThresholdRegionNoiseFilteringSize = 10;
		nMinBlobSize = 100;
		nMaxBlobCount = 10000;
		bBlobMerge = FALSE;
		nBlobMergeDist = 1;
		bInterpolationX = FALSE;
		bInterpolationY = FALSE;
	}
}SInterHairpinParams;

typedef struct SDetectBlobParams
{
	long nMinBlobSize;
	long nMaxBlobCount;
	BOOL bBlobMerge;
	long nBlobMergeDist;

	void Init()
	{
		nMinBlobSize = 100;
		nMaxBlobCount = 10000;
		bBlobMerge = FALSE;
		nBlobMergeDist = 1;
	}
}SDetectBlobParams;

typedef struct SHairpinCenterParams
{
	long nWidthSize_1BlobMinSize;
	long nWidthSize_2BlobMinSize;
	long nWidthSize_2MoreBlobMinSize;
	BOOL bUseComplementHairpinCenter;
	long nComplementHeightSize;
	long nComplementHeightLimit;
	long nMinHairpinWidth;
	long nMinHairpinHeight;
	long nMaxHairpinWidth;
	long nMaxHairpinHeight;
	long nMinHairpinWidthfor1Blob;
	BOOL bUseCalCenterOffset_replace3D;
	BOOL bUseCorrectionOffset_replace3D;
	float f2D3DPositionLimitX;
	float f2D3DPositionLimitY;
	BOOL bUseLayerPairReplace;

	void Init()
	{
		nWidthSize_1BlobMinSize = 145;
		nWidthSize_2BlobMinSize = 145;
		nWidthSize_2MoreBlobMinSize = 145;
		bUseComplementHairpinCenter = TRUE;
		nComplementHeightSize = 42;
		nComplementHeightLimit = 30;
		nMinHairpinWidth = 50;
		nMinHairpinHeight = 35;
		nMaxHairpinWidth = 80;
		nMaxHairpinHeight = 50;
		nMinHairpinWidthfor1Blob = 120;
		bUseCalCenterOffset_replace3D = TRUE;
		bUseCorrectionOffset_replace3D = TRUE;
		f2D3DPositionLimitX = 2.0f;
		f2D3DPositionLimitY = 2.0f;
		bUseLayerPairReplace = FALSE;
	}
}SHairpinCenterParams;

typedef struct SCriteriaParams
{
	float fMin_H12;
	float fMax_H12;
	float fMax_H3;
	float fMin_C;
	float fMax_C;
	float fMin_T;
	float fMax_T;
	float fMin_J;
	float fMax_J;

	void Init()
	{
		fMin_H12 = 1.f;
		fMax_H12 = 10.f;
		fMax_H3 = 10.f;
		fMin_C = 1.f;
		fMax_C = 2.5;
		fMin_T = 0.f;
		fMax_T = 360.f;
		fMin_J = 0.f;
		fMax_J = 20.f;
	}
}SCriteriaParams;

typedef struct SPublicParams
{
	BOOL bReplace3DResult;
	BOOL bSaveRawImage;
	CString strRawImageSavePath;
	BOOL bSaveResultData;
	CString strResultDataSavePath;
	BOOL bSaveResultImage;
	CString strResultImageSavePath;
	BOOL bSaveResultImageForJigA;
	CString strResultImageSavePathForJigA;
	BOOL bSaveResultImageForJigB;
	CString strResultImageSavePathForJigB;

	void Init()
	{
		bReplace3DResult = FALSE;
		bSaveRawImage = FALSE;
		strRawImageSavePath = _T("");
		bSaveResultData = FALSE;
		strResultDataSavePath = _T("");
		bSaveResultImage = FALSE;
		strResultImageSavePath = _T("");
		bSaveResultImageForJigA = FALSE;
		strResultImageSavePathForJigA = _T("");
		bSaveResultImageForJigB = FALSE;
		strResultImageSavePathForJigB = _T("");
	}
}SPublicParams;

typedef struct SResult
{
	long nErrorCode;
	float fC;
	float fT;
	float fJ;
	float fH1;
	float fH2;
	float fH3;

	void Init()
	{
		nErrorCode = INSP_ERR_OK;
		fC = 0.f;
		fT = 0.f;
		fJ = 0.f;
		fH1 = 0.f;
		fH2 = 0.f;
		fH3 = 0.f;
	}
}SResult;
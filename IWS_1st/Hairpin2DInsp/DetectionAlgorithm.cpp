#include "pch.h"
#include "DetectionAlgorithm.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef BOOL(WINAPI *PGETDISKFREESPACEEX)(LPCSTR,
	PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

CDetectionAlgorithm::CDetectionAlgorithm()
{
	m_pAlgorithm = new CAlgorithm;

	m_nOriginalImageWidth = 0;
	m_nOriginalImageHeight = 0;
	m_nOriginalImageSize = 0;
	m_nSubOriginalImageWidth = 0;
	m_nSubOriginalImageHeight = 0;
	m_nSubOriginalImageSize = 0;
	m_nUnrolledImageWidth = UNROLLED_IMAGE_WIDTH;
	m_nUnrolledImageHeight = UNROLLED_IMAGE_HEIGHT;
	m_nUnrolledImageSize = m_nUnrolledImageWidth * m_nUnrolledImageHeight;
	m_strCurRecipeFilePath = _T("No Recipe");
	m_bLoadOriginalImage = FALSE;

	m_strJigName = _T("");
	m_strBarcodeID = _T("");
	m_nModelNo = 0;

	// Make Unrolled Image Parameters...
	m_pptOriginalPos = nullptr;
	m_pptUnrolledPos = nullptr;
	m_fResolX = 0.f;
	m_fResolY = 0.f;

	// ROI Parameters...
	m_rtHairpinWhole = CRect(0, 0, 0, 0);
	m_vecrtHairpin.clear();

	// Remove Noise parameters...
	m_vecbProcessing.clear();
	m_nMaskWidth = 0;
	m_nMaskHeight = 0;
	m_nMaskSize = 0;
	m_fRefValue = 0.f;

	// Interpolation Parameters...
	m_vecInterpolationBlobInfoList.clear();;
	m_nInterpolationBlobTotalCount = 0;

	// Detect Blob Parameters...
	m_vecDetectBlobBlobInfoList.clear();
	m_nBlobTotalCount = 0;
	m_vecptCriteriaGravityCenter.clear();
	m_vecptResult.clear();

	// Criteria Parameters...
	m_vecsCriteriaResult.clear();
	m_strTotalResult = _T("");
	m_nOKCount = 0;
	m_nNGCount = 0;

	// Calc Slot Overlay Parameters...
	m_vecptSlotLineOverlayStartPoint.clear();
	m_vecptSlotLineOverlayEndPoint.clear();
	m_vecptSlotNo1OverlayPoint.clear();
	m_vecptSlotNo2OverlayPoint.clear();

	InitThreads();
}

CDetectionAlgorithm::~CDetectionAlgorithm()
{
	delete m_pAlgorithm;

	FreeThreads();
}

BOOL CDetectionAlgorithm::SetInspInfo(const BYTE* src, const long imageWidth, const long imageHeight, const float resolX, const float resolY, const float imageCenterXAfterCalibration, const float imageCenterYAfterCalibration, short* pnIWIResult, float* pfIWIH1, float* pfIWIH2, float* pfIWIH3, float* pfIWIX_afterMatrix, float* pfIWIY_afterMatrix, const LPCTSTR szRecipePath, CString strJig)
{
	CFileFind Find;
	BOOL bRet = Find.FindFile(szRecipePath);

	// 임시...
	m_fResolX = resolX;
	m_fResolY = resolY;
	m_fimageCenterXAfterCalibration = imageCenterXAfterCalibration;
	m_fimageCenterYAfterCalibration = imageCenterYAfterCalibration;
	m_strJigName = strJig;

	if(szRecipePath == _T("") || bRet == FALSE)
	{
		return FALSE;
	}

	if(src == nullptr)
	{
		return FALSE;
	}

	if(imageWidth == 0 || imageHeight == 0)
	{
		return FALSE;
	}

	m_strCurRecipeFilePath = szRecipePath;
	m_fResolX = resolX;
	m_fResolY = resolY;
	m_fimageCenterXAfterCalibration = imageCenterXAfterCalibration;
	m_fimageCenterYAfterCalibration = imageCenterYAfterCalibration;
	m_strJigName = strJig;

	SetInspInfo(imageWidth, imageHeight);

	::g_RecipeManager.InitParameters();
	::g_RecipeManager.LoadRecipeParameters(m_strCurRecipeFilePath);

	auto params = ::g_RecipeManager.m_sROIParams;
	auto slotCount = params.nSlotCount;
	auto layerCount = params.nLayerCount;
	auto layerPair = layerCount / 2;

	if(pnIWIResult != nullptr && pfIWIH1 != nullptr && pfIWIH2 != nullptr && pfIWIH3 != nullptr && pfIWIX_afterMatrix != nullptr && pfIWIY_afterMatrix != nullptr)
	{
		m_pAlgorithm->ResizeVector(m_pnIWIResult, slotCount, layerPair);
		m_pAlgorithm->ResizeVector(m_vecfIWIH1, slotCount, layerPair);
		m_pAlgorithm->ResizeVector(m_vecfIWIH2, slotCount, layerPair);
		m_pAlgorithm->ResizeVector(m_vecfIWIH3, slotCount, layerPair);
		m_pAlgorithm->ResizeVector(m_vecfIWIX_afterMatrix, slotCount, layerCount);
		m_pAlgorithm->ResizeVector(m_vecfIWIY_afterMatrix, slotCount, layerCount);

#pragma omp parallel for
		for(long slot = 0; slot < slotCount; slot++)
		{
			// 3D 데이터는 이미 IWI 에서 start index가 변경되어 있으니
			// 여기서는 start index가 변경되지 않은 상태의 데이터를 넣어야 한다.
			long index = params.nSlotCount - params.nStartSlotIndex + 1 + slot; // ehjang. start slot index
			if(index >= params.nSlotCount)
			{
				index -= params.nSlotCount;
			}

			long sum1 = index * layerPair;
			long sum2 = index * layerCount;

			for(long layer = 0; layer < layerPair; layer++)
			{
				m_pnIWIResult[slot][layer] = pnIWIResult[sum1 + layer];
				m_vecfIWIH1[slot][layer] = pfIWIH1[sum1 + layer];
				m_vecfIWIH2[slot][layer] = pfIWIH2[sum1 + layer];
				m_vecfIWIH3[slot][layer] = pfIWIH3[sum1 + layer];
			}

			if(pfIWIX_afterMatrix != nullptr && pfIWIY_afterMatrix != nullptr)
			{
				for(long layer = 0; layer < layerCount; layer++)
				{
					m_vecfIWIX_afterMatrix[slot][layer] = pfIWIX_afterMatrix[sum2 + layer];
					m_vecfIWIY_afterMatrix[slot][layer] = pfIWIY_afterMatrix[sum2 + layer];
				}
			}
		}
	}

	BYTE* pbyImage = ::g_MemoryManager.GetByteImage();
	memcpy(pbyImage, src, m_nOriginalImageSize);

	m_bLoadOriginalImage = TRUE;

	return TRUE;
}

BOOL CDetectionAlgorithm::SetInspInfo(const long imageWidth, const long imageHeight)
{
	m_nOriginalImageWidth = imageWidth;
	m_nOriginalImageHeight = imageHeight;
	m_nOriginalImageSize = imageWidth * imageHeight;

	::g_MemoryManager.SetImageSize(m_nOriginalImageWidth, m_nOriginalImageHeight, m_nUnrolledImageWidth, m_nUnrolledImageHeight);

	return TRUE;
}


BOOL CDetectionAlgorithm::DoInspection()
{
	CString strTemp;

	if(m_bLoadOriginalImage == FALSE)
	{
		return FALSE;
	}

	if(DoMakeSubOriginalImage() != TRUE)
	{
		return FALSE;
	}

	if(DoMakeUnrolledImage() != TRUE)
	{
		return FALSE;
	}

	if(DoSetDetectionROI() != TRUE)
	{
		return FALSE;
	}

	if(DoMakeOverlay() != TRUE)
	{
		return FALSE;
	}
	
	if(DoRemoveNoise() != TRUE)
	{
		return FALSE;
	}

	if(DoInterpolation() != TRUE)
	{
		return FALSE;
	}

	if(DoDetectBlob() != TRUE)
	{
		return FALSE;
	}

	if(DoCalcHairpinCenter() != TRUE)
	{
		return FALSE;
	}

	if(DoDataExchange() != TRUE)
	{
		return FALSE;
	}

	if(DoCriteria() != TRUE)
	{
		return FALSE;
	}

	// 결과 데이터 저장(csv, image)
	auto publicParams = ::g_RecipeManager.m_sPublicParams;

	if(publicParams.bSaveResultImage == TRUE || publicParams.bSaveResultData == TRUE)
	{
		::SetEvent(m_Control_DataSave.m_hSignal_Start);
	}

	return TRUE;
}

DWORDLONG CDetectionAlgorithm::MyGetDiskFreeSpaceEx(LPCSTR pszDrive)
{
	PGETDISKFREESPACEEX pGetDiskFreeSpaceEx;
	__int64 i64FreeBytesToCaller, i64TotalBytes, i64FreeBytes;

	DWORD dwSectPerClust,
		dwBytesPerSect,
		dwFreeClusters,
		dwTotalClusters;

	BOOL fResult;

	pGetDiskFreeSpaceEx = (PGETDISKFREESPACEEX)GetProcAddress(
		GetModuleHandle(_T("kernel32.dll")),
		"GetDiskFreeSpaceExA");

	if(pGetDiskFreeSpaceEx)
	{
		fResult = pGetDiskFreeSpaceEx(pszDrive,
			(PULARGE_INTEGER)&i64FreeBytesToCaller,
			(PULARGE_INTEGER)&i64TotalBytes,
			(PULARGE_INTEGER)&i64FreeBytes);

		// Process GetDiskFreeSpaceEx results.

		return i64FreeBytes;
	}

	else
	{
		fResult = GetDiskFreeSpaceA(pszDrive,
			&dwSectPerClust,
			&dwBytesPerSect,
			&dwFreeClusters,
			&dwTotalClusters);

		// Process GetDiskFreeSpace results.
		return dwFreeClusters * dwSectPerClust*dwBytesPerSect;
	}
	return 0;
}
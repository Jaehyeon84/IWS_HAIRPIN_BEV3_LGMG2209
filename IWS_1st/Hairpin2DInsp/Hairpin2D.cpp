#include "pch.h"
#include "Hairpin2D.h"
#include "Hairpin2DInspDlg.h"
#include "DlgResultViewer.h"
#include "DetectionAlgorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHairpin2D::CHairpin2D()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pDetectionAlgorithm = new CDetectionAlgorithm;

	m_pDlgResultViewer = new CDlgResultViewer(m_pDetectionAlgorithm);
	m_pDlgResultViewer->Create(CDlgResultViewer::IDD);

	m_pbyImage = nullptr;
}

CHairpin2D::~CHairpin2D()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pDlgResultViewer->ShowWindow(SW_HIDE);

	delete m_pDetectionAlgorithm;
	delete m_pDlgResultViewer;

	if(m_pbyImage != nullptr)
	{
		delete[] m_pbyImage;
	}
}

INT_PTR CHairpin2D::DoModal()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHairpin2DInspDlg dlg(m_pDetectionAlgorithm);

	return dlg.DoModal();
}

void CHairpin2D::ResultViewerDoModal()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pDlgResultViewer->ShowResult();
	m_pDlgResultViewer->ShowWindow(SW_SHOW);
}

BOOL CHairpin2D::InitParameters(const BYTE* src, const long imageWidth, const long imageHeight, const float resolX, const float resolY, const float imageCenterXAfterCalibration, const float imageCenterYAfterCalibration, short* pnIWIResult, float* pfIWIH1, float* pfIWIH2, float* pfIWIH3, float* pfIWIX_afterMatrix, float* pfIWIY_afterMatrix, const LPCTSTR szRecipePath, CString strJig)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// 임시
	//long width(0), height(0);
	//ImageOpen2D(_T("C:\\Work\\IWS_1st\\bin\\Image\\refImage.bmp"), width, height);
	////ImageOpen2D(_T("C:\\Work\\VisionScan2DHairpin\\Image\\refImage.bmp"), width, height);
	//m_pDetectionAlgorithm->SetInspInfo(m_pbyImage, width, height, resolX, resolY, imageCenterXAfterCalibration, imageCenterYAfterCalibration, pnIWIResult, pfIWIPosX, pfIWIPosY, szRecipePath);
	//delete[] m_pbyImage;

	if(m_pDetectionAlgorithm->SetInspInfo(src, imageWidth, imageHeight, resolX, resolY, imageCenterXAfterCalibration, imageCenterYAfterCalibration, pnIWIResult, pfIWIH1, pfIWIH2, pfIWIH3, pfIWIX_afterMatrix, pfIWIY_afterMatrix, szRecipePath, strJig) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CHairpin2D::DoInspection()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pDetectionAlgorithm->DoInspection();

	return TRUE;
}

CPoint CHairpin2D::GetStatorCenterOffset()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return ::g_RecipeManager.m_sUnrolledParams.ptCenterPosCorrection;
}

BOOL CHairpin2D::GetResult(float* pfResultX, float* pfResultY, long* pnResult, float* pfResultC, float* pfResultT, float* pfResultJ)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(pfResultX == nullptr || pfResultY == nullptr || pnResult == nullptr || pfResultC == nullptr || pfResultT == nullptr || pfResultJ == nullptr)
	{
		return FALSE;
	}

	auto params = ::g_RecipeManager.m_sROIParams;
	auto slotCount = params.nSlotCount;
	auto layerCount = params.nLayerCount;
	auto layerPair = layerCount / 2;

	auto fResultX = m_pDetectionAlgorithm->m_vecfResultX_cad;
	auto fResultY = m_pDetectionAlgorithm->m_vecfResultY_cad;
	auto sResult = m_pDetectionAlgorithm->m_vecsCriteriaResult;

	if((long)fResultX.size() != slotCount || (long)fResultY.size() != slotCount || (long)sResult.size() != slotCount)
	{
		return FALSE;
	}

	for(long slot = 0; slot < slotCount; slot++)
	{
		long index = params.nSlotCount - params.nStartSlotIndex + 1 + slot; // ehjang. start slot index
		if(index >= params.nSlotCount)
		{
			index -= params.nSlotCount;
		}

		long sum1 = index * layerPair;
		long sum2 = index * layerCount;

		for(long layer = 0; layer < layerCount; layer++)
		{
			pfResultX[sum2 + layer] = fResultX[slot][layer];
			pfResultY[sum2 + layer] = fResultY[slot][layer];
		}

		for(long layer = 0; layer < layerPair; layer++)
		{
			pnResult[sum1 + layer] = sResult[slot][layer].nErrorCode;
			pfResultC[sum1 + layer] = sResult[slot][layer].fC;
			pfResultT[sum1 + layer] = sResult[slot][layer].fT;
			pfResultJ[sum1 + layer] = sResult[slot][layer].fJ;
		}
	}

	return TRUE;
}

void CHairpin2D::GetTotalResult(CString &strResult, long &okCount, long &ngCount)
{
	strResult = m_pDetectionAlgorithm->m_strTotalResult;
	okCount = m_pDetectionAlgorithm->m_nOKCount;
	ngCount = m_pDetectionAlgorithm->m_nNGCount;
}

void CHairpin2D::SetBarcodeID(CString strBarcodeID)
{
	m_pDetectionAlgorithm->m_strBarcodeID = strBarcodeID;
}


void CHairpin2D::SetModelNo(int nModelNo)
{
	m_pDetectionAlgorithm->m_nModelNo = nModelNo;
}



BOOL CHairpin2D::ImageOpen2D(const CString strPath, long &width, long &height)
{
	long bpp(0);

	BYTE* pbyBuffer = nullptr;
	if(!HUBSDK::ImageInterface::ImageOpenBMP(strPath, pbyBuffer, width * height, &width, &height, &bpp))
	{
		return FALSE;
	}

	if(m_pbyImage != nullptr)
	{
		delete[] m_pbyImage;
	}

	m_pbyImage = new BYTE [width * height];

	// 불러온 image를 모든 buffer에 복사
	memcpy(m_pbyImage, pbyBuffer, width * height);

	HUBIPHelper::FreeMemory(pbyBuffer);

	return TRUE;
}
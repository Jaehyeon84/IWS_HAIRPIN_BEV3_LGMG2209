#pragma once

#ifdef INSP_2D_DLL_EXPORT
#define INSP_2D_DLL_API	__declspec(dllexport)
#else
#define INSP_2D_DLL_API	__declspec(dllimport)
#endif

class CDetectionAlgorithm;
class CDlgResultViewer;

class INSP_2D_DLL_API CHairpin2D
{
public:
	CHairpin2D();
	~CHairpin2D();

	CDetectionAlgorithm *m_pDetectionAlgorithm;
	CDlgResultViewer* m_pDlgResultViewer;

	INT_PTR DoModal();
	void ResultViewerDoModal();

	BOOL InitParameters(const BYTE* src, const long imageWidth, const long imageHeight, const float resolX, const float resolY, const float imageCenterXAfterCalibration, const float imageCenterYAfterCalibration, short* pnIWIResult, float* pfIWIH1, float* pfIWIH2, float* pfIWIH3, float* pfIWIX_afterMatrix, float* pfIWIY_afterMatrix, const LPCTSTR szRecipePath, CString strJig);
	BOOL DoInspection();
	CPoint GetStatorCenterOffset();
	BOOL GetResult(float* pfResultX, float* pfResultY, long* pnResult, float* pfResultC, float* pfResultT, float* pfResultJ);
	void GetTotalResult(CString &strResult, long &okCount, long &ngCount);
	void SetBarcodeID(CString strBarcodeID);
	void SetModelNo(int nModelNo);

	// 임시...
	BOOL ImageOpen2D(const CString strPath, long &width, long &height);
	BYTE* m_pbyImage;
};


#pragma once

class CAlgorithm;

class CDetectionAlgorithm
{
public:
	CDetectionAlgorithm();
	~CDetectionAlgorithm();

	CAlgorithm* m_pAlgorithm;

	long m_nOriginalImageWidth;
	long m_nOriginalImageHeight;
	long m_nOriginalImageSize;
	long m_nSubOriginalImageWidth;
	long m_nSubOriginalImageHeight;
	long m_nSubOriginalImageSize;
	long m_nUnrolledImageWidth;
	long m_nUnrolledImageHeight;
	long m_nUnrolledImageSize;
	BOOL m_bLoadOriginalImage;

	void InitThreads();
	void FreeThreads();

	CString m_strJigName;
	CString m_strBarcodeID;
	int m_nModelNo;

	// IWS Parameters...
	float m_fResolX;
	float m_fResolY;
	float m_fimageCenterXAfterCalibration;
	float m_fimageCenterYAfterCalibration;
	CString m_strCurRecipeFilePath;
	std::vector<std::vector<short>> m_pnIWIResult;
	std::vector<std::vector<float>> m_vecfIWIH1;
	std::vector<std::vector<float>> m_vecfIWIH2;
	std::vector<std::vector<float>> m_vecfIWIH3;
	std::vector<std::vector<float>> m_vecfIWIX_afterMatrix;
	std::vector<std::vector<float>> m_vecfIWIY_afterMatrix;

	// Make Unrolled Image Parameters...
	CPoint* m_pptOriginalPos;
	CPoint* m_pptUnrolledPos;

	// ROI Parameters...
	CRect m_rtHairpinWhole;
	std::vector<std::vector<CRect>> m_vecrtHairpin;

	// Remove Noise parameters...
	std::vector<std::vector<BOOL>> m_vecbProcessing;
	long m_nMaskWidth;
	long m_nMaskHeight;
	long m_nMaskSize;
	float m_fRefValue;
	std::vector<HUBSDK::BlobInfo> m_vecRemoveNoiseBlobInfoList;
	long m_nRemoveNoiseBlobTotalCount;

	// Interpolation Parameters...
	std::vector<HUBSDK::BlobInfo> m_vecInterpolationBlobInfoList;
	long m_nInterpolationBlobTotalCount;

	// Detect Blob Parameters...
	std::vector<std::vector<std::vector<HUBSDK::BlobInfo>>> m_vecDetectBlobBlobInfoList;
	long m_nBlobTotalCount;
	std::vector<std::vector<CPoint>> m_vecptCriteriaGravityCenter;
	std::vector<std::vector<CPoint>> m_vecptCriteriaGravityCenter_forTheta;
	std::vector<std::vector<CPoint>> m_vecptResult; // Sub Original Image 에서의 최종 헤어핀 중심 위치
	std::vector<std::vector<float>> m_vecfResultX_cad; // 캐드좌표계에서의 최종 헤어핀 중심 위치
	std::vector<std::vector<float>> m_vecfResultY_cad; // 캐드좌표계에서의 최종 헤어핀 중심 위치
	std::vector<std::vector<BOOL>> m_vecbReplace3DPosition; // 캐드좌표계에서의 최종 헤어핀 중심 위치

	// Hairpin Center Parameters...
	std::vector<std::vector<CString>> m_vecstrCenterAlgorithm;
	std::vector<std::vector<long>> m_vecnHairpinWidth;
	std::vector<std::vector<long>> m_vecnHairpinHeight;

	// Criteria Parameters...
	std::vector<std::vector<SResult>> m_vecsCriteriaResult; // 검사결과
	CString m_strTotalResult;
	long m_nOKCount;
	long m_nNGCount;

	// Calc Slot Overlay Parameters...
	std::vector<CPoint> m_vecptSlotLineOverlayStartPoint;
	std::vector<CPoint> m_vecptSlotLineOverlayEndPoint;
	std::vector<CPoint> m_vecptSlotNo1OverlayPoint;
	std::vector<CPoint> m_vecptSlotNo2OverlayPoint;
	std::vector<std::vector<CPoint>> m_vecptLayerPairOverlayPoint;

	BOOL SetInspInfo(const BYTE* src, const long imageWidth, const long imageHeight, const float resolX, const float resolY, const float imageCenterXAfterCalibration, const float imageCenterYAfterCalibration, short* pnIWIResult, float* pfIWIH1, float* pfIWIH2, float* pfIWIH3, float* pfIWIX_afterMatrix, float* pfIWIY_afterMatrix, const LPCTSTR szRecipePath, CString strJig);
	BOOL SetInspInfo(const long imageWidth, const long imageHeight);

	BOOL DoInspection();
	BOOL DoMakeSubOriginalImage();
	BOOL DoMakeUnrolledImage();
	BOOL MakeRotateImage();
	void CalcUnrolledImage(const long startIndex, const long endIndex);
	BOOL DoSetDetectionROI();
	BOOL DoSetAutoROI();
	BOOL DoRemoveNoise();
	void CalcMaskConvolution();
	void CalcRemoveNoise(const long startIndex, const long endIndex);
	BOOL DoInterpolation();
	BOOL DoDetectBlob();
	BOOL DoCalcHairpinCenter();
	BOOL DoCriteria();
	BOOL DoRemoveNoiseThresholdImage();
	BOOL DoRemoveNoiseProcImage();
	BOOL DoInterThresholdImage();
	BOOL DoInterProcImage();
	BOOL DoDataExchange();
	BOOL DoMakeOverlay();
	void SearchLostHairpin(const BYTE* image, long* labelImage, const long imageWidth, const long imageHeight, const CRect searchRoi, const CRect hairpinArea, CPoint &point);
	void CalcHairpinCenter(const BYTE* image, long* labelImage, const long imageWidth, const long imageHeight, const CRect hairpinRoi, const long slot, const long layer1, const long layer2, const long hairpinSize, const long label1, const long label2, const BOOL bLeft, const BOOL bCalcOneHairpin, CPoint &point1, CPoint &point2);
	void CalcDivisionHairpinPair(const BYTE* image, long* labelImage, const long imageWidth, const long imageHeight, const CRect blobRect, const long label1, const long label2, CPoint &point1, CPoint &point2);
	//void InterpolationHairpinPoints(const BYTE* image, const long imageWidth, const long imageHeight, const CRect roi, const CRect roi1, const CRect roi2, CPoint &point1, CPoint &point2);
	//void InterpolationHairpinPoints2(const BYTE* image, const long imageWidth, const long imageHeight, const CRect roi, const CRect roi1, const CRect roi2, CPoint &point1, CPoint &point2);
	void InterpolationHairpinPoints3(const BYTE* image, long* labelImage, const long imageWidth, const long imageHeight, const CRect roi, const long slot, const long layer1, const long layer2, const CRect roiLeft, const CRect roiRight, CPoint &point1, CPoint &point2);
	void InterpolationHairpinPoints4(const BYTE* image, long* labelImage, const long imageWidth, const long imageHeight, const CRect roi, const long slot, const long layer, CPoint &point);
	void InterpolationHairpinPoints5(const BYTE* image, long* labelImage, const long imageWidth, const long imageHeight, const CRect roi, const long slot, const long layer1, const long layer2, const CRect roiLeft, const CRect roiRight, CPoint &point1, CPoint &point2);
	void ResultDataSave(const CString strTime);
	void ResultImageSave(const CString strTime);
	void RawImageSave(const CString strTime);
	CString CreateSaveFolder(CString strPath);
	void GetClosedPosition(const CPoint src, const long yStart, const long yEnd, const CPoint* pPoints, long &xIndex, long &yIndex);

	DWORDLONG MyGetDiskFreeSpaceEx(LPCSTR pszDrive);

	////////////////////////////////////////////////
	// Method Thread...
	struct SMethodHandle
	{
		HANDLE m_hSignal_Ready;
		HANDLE m_hSignal_Start;
		HANDLE m_hSignal_Finish;
		HANDLE m_hSignal_Kill;
		HANDLE m_Handle_Thread;
	};
	std::vector<SMethodHandle> m_Control_Method;

	long m_nLatestMethodThreadID;
	THREAD_METHOD m_Method;
	static UINT ThreadMethodFunc(LPVOID pParam);
	void ThreadMethod();
	void BeginThread(const THREAD_METHOD method);

	struct SMethodIndexInfo
	{
		long startIndex;
		long endIndex;
	};
	std::vector<SMethodIndexInfo> m_Method_IndexInfo;
	////////////////////////////////////////////////

	////////////////////////////////////////////////
	// Data Save Thread...
	struct SDataSaveHandle
	{
		HANDLE m_hSignal_Start;
		HANDLE m_hSignal_Kill;
		HANDLE m_Handle_Thread;
	};
	SDataSaveHandle m_Control_DataSave;

	static UINT ThreadDataSaveFunc(LPVOID pParam);
	void ThreadDataSave();
	////////////////////////////////////////////////
};

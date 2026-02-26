#pragma once
#include "VRSaux/NIVision/Include/nivision.h"
#define LT_MARGIN 70

typedef struct stGridLineParam
{
	Rect	rIn;
	Rect	rOut;
	BOOL			bFindDarkObj;
	SearchDirection nFindDir;
	float fEdgeStrength;
}GridLineParam;
typedef struct stGridRectParam
{
	GridLineParam left		;
	GridLineParam top		;
	GridLineParam right		;
	GridLineParam bottom	;
}GridRectParam;




typedef struct stGridLineInfo
{
	RotatedRect	rrIn;
	RotatedRect	rrOut;
	LineFloat edgeOut;
	LineFloat edgeIn;
	LineFloat edgeCen;
	BOOL bFindIn;
	BOOL bFindOut;
	stGridLineInfo()
	{
		rrIn = imaqMakeRotatedRect(0, 0, 0, 0, 0.0);
		rrOut = rrIn;
		edgeOut = { {0.0f, 0.0f},{0.0f, 0.0f} };
		edgeIn = edgeOut;
		edgeCen = edgeOut;
		bFindIn = FALSE;
		bFindOut = FALSE;
	}
}GridLineInfo;

typedef struct stCornerData
{
	PointFloat pLT;
	PointFloat pRT;
	PointFloat pRB;
	PointFloat pLB;

	BOOL bLT;
	BOOL bRT;
	BOOL bLB;
	BOOL bRB;

	int nDivW;
	int nDivH;
	float fDivLngW_px;
	float fDivLngH_px;
	float fDivLngWUnit_px;
	float fDivLngHUnit_px;
	stCornerData()
	{
		pLT = {0.0f, 0.0f};
		pRT = pLT;
		pRB = pLT;
		pLB = pLT;

		bLT = FALSE;
		bRT = FALSE;
		bLB = FALSE;
		bRB = FALSE;

		nDivW = 0;
		nDivH = 0;
		fDivLngW_px		= 0.0f;
		fDivLngH_px		= 0.0f;
		fDivLngWUnit_px	= 0.0f;
		fDivLngHUnit_px	= 0.0f;
	}
}CornerData;


#define IMAQ_HUBIS_DATA_KEY "HBUBIS"
#define IMAQ_HUBIS_DATA_VER_1 1
#define IMAQ_HUBIS_DATA_VER_2 2
#define IMAQ_HUBIS_DATA_VER_CUR IMAQ_HUBIS_DATA_VER_2
typedef struct stHubisCalibInfo1
{
	char  structVersion;
	Point pMergePos;
	Rect  rCopyArea;
	int   iScaleSizeW;
	int   iScaleSizeH;
}HubisCalibInfo1;

typedef struct stHubisCalibInfo2
{
	char  structVersion;
	Point pMergePos;
	Rect  rCopyArea;
	int   iScaleSizeW;
	int   iScaleSizeH;
	PointFloat pGrabPos;
}HubisCalibInfo2;

typedef struct stHubisCalibInfo3
{
	char  structVersion;
	Point pMergePos;
	Rect  rCopyArea;
	int   iScaleSizeW;
	int   iScaleSizeH;
	PointFloat pGrabPos;
	PointFloat pStartPos_mm;
	PointFloat pEndPos_mm;
}HubisCalibInfo3;

typedef struct stLearningData
{
	HubisCalibInfo3 pInfo;
	BOOL bLearnOK;
	Image* pImgLearn;
}LearningData;

typedef struct stImgBlock
{
	//PointFloat pGrabPos;
	CString sCalibDataFile;
	//CornerData corner;
	CRect rBlockArea;

	Image* pCalibImg;
	BOOL bCalibOK;

	float fMeanIntensity;
	float fEdgeStrength;

	LearningData pLearn;

	GridRectParam gridLearn;
	GridRectParam gridScaling;
	GridRectParam gridStitching;
	int iCalibStatus;

	Point pMLT;
	Point pMRT;
	Point pMRB;
	Point pMLB;
}ImgBlock;

double     imaqCalcDistF(PointFloat p1, PointFloat p2);
int imaqFindIntersection(LineFloat L1, LineFloat L2, PointFloat* pCrossPos);
void vImaqMakePixelValue(PixelValue*  pixelValue, ImageType imageType, float grayscale, unsigned int red, unsigned int green, unsigned int blue);
Point PointFloat2Point(PointFloat pPF);

class CImageMerging
{
public:
	CImageMerging();
	~CImageMerging();
	void RemoveBlockData();

	static StraightEdgeReport2* FindEdeg(Image* pImg, ROI* pROI, SearchDirection pDir, float fMinThreshold, BOOL bFindDarkObj);
	static GridLineInfo FindGridLine(Image* pImg, Rect rRoi, SearchDirection pDir, float fMinThreshold, BOOL bFindDarkObj);
	static int FindCornerInfo(Image* pImg, int iDivSize, CornerData* pCorner, float fMinThreshold, BOOL bFindDarkObj);
	static int LearnImage(Image* pImg, float fGridSize_px, float fMinThreshold, BOOL bFindDarkObj);
	static int ApplyCalib(Image* pCalibImg, Image** pTarget, BOOL bFindDarkObj);
	static int FindGridInfo(Image* pImg, Rect roi, int iDivSize, CornerData* pCorner, float fMinThreshold, BOOL bFindDarkObj);
	static void CornerShift(float x, float y, CornerData* p);
	ImgBlock* GetBlock(int x, int y);
	ImgBlock* GetBlock(Point pBlockIdx);

	//int initSystemParam(CString sInfoDirPath);
	int SetInfoDirPath(CString sInfoDirPath);

	int CreateMerginInfo3_InitParam(int nAllBlockCntX, int nAllBlockCntY,
		Image*** pBlockAll, PointFloat** pGrabPos,
		Rect rValidArea,
		int GridSize_px,
		//float fMinThreshold,
		float fEdgeStrengthMin,
		float fEdgeStrengthMax,
		BOOL bFindDarkObj);
	int CreateMerginInfo3_LearningAndScaling(int nAllBlockCntX, int nAllBlockCntY,
		Image*** pBlockAll, PointFloat** pGrabPos,
		Rect rValidArea,
		int GridSize_px,
		//float fMinThreshold,
		float fEdgeStrengthMin,
		float fEdgeStrengthMax,
		BOOL bFindDarkObj);
	int CreateMerginInfo3_CalcStitchingParam(int nAllBlockCntX, int nAllBlockCntY,
		Image*** pBlockAll, PointFloat** pGrabPos,
		Rect rValidArea,
		int GridSize_px,
		//float fMinThreshold,
		float fEdgeStrengthMin,
		float fEdgeStrengthMax,
		BOOL bFindDarkObj);

	static GridLineInfo FindGridLine3(Image* pImg, GridLineParam* lineParam, StraightEdgeReport2** pResultOut = NULL, StraightEdgeReport2** pResultIn = NULL);
	static int FindCornerInfo3(Image* pImg, GridRectParam* gridParam, int iDivSize, CornerData* pCorner);
	static int FindGridInfo3(Image* pImg, GridRectParam* gridParam, int iDivSize, CornerData* pCorner);
	static int LearnImage3(Image* pImg, GridRectParam* gridParam, float fGridSize_px);
	int CalcCopyPos(int ix, int iy);
	


	int CreateMerginInfo(int nAllBlockCntX, int nAllBlockCntY,
		Image*** pBlockAll, PointFloat** pGrabPos,
		Rect rValidArea,
		int GridSize_px,
		//float fMinThreshold,
		float fEdgeStrengthMin,
		float fEdgeStrengthMax,
		BOOL bFindDarkObj);


	int DoCalibrationAndScaling(int x, int y, Image* pImg, BOOL bFindDarkObj=TRUE);
	int MakeMergedImage(int iBlockSx, int iBlockSy, int xImgCnt, int yImgCnt, Image*** pScanImage, BOOL bFindDarkObj);
	int MakeMergedImage_Small(Image* pTarget, Point pStartIdx, Point pEndIdx, Image*** pScanImage);
	int MakeMergedImage_Small(Image* pTarget, Point pStartIdx, Point pEndIdx, BYTE** pBYTEScanImage, int nGrabWd, int nGrabHt);
	int SaveMerginInfo();
	int LoadMerginInfo(int nBlockX, int nBlockY);
	//int LoadSystemFile();

	Point PtInAreaIdx(Point pPos_px);


	CString m_sInfoDirPath;
	CString m_sSystemFilePath;



	ImgBlock** m_pBlock;
	int   m_nBlockX;
	int   m_nBlockY;
	CSize m_szMergeImg;
	Rect  m_rValidArea;
	int   m_GridSize_px;

	Image* m_pMergeImg;

	Point m_pCenterBlockIdx;
	PointFloat m_pfCenterPos;
	int SetMergeCenter(PointFloat pCenter, double dResol_umPpx);



	Point PtInBlockIdx(PointFloat pos_mm);
	int GetAreaIdx(PointFloat pS_mm, PointFloat pE_mm, Point* pStartBlockIdx, Point* pEndBlockIdx);
	int   GetMergeImgInfo(Point pSIdx, Point pEIdx,
		PointFloat* fStartPos_mm, PointFloat* fEndPos_mm,
		int* iImgW, int* iImgH,
		int* nBlockX, int* nBlockY,
		PointFloat*** pfScanPos);

	int AdjustGridPos(int iLastBlockShiftX, int iLastBlockShiftY);
};


#pragma once
#include "afxwin.h"
#include "VRS.h"
#include "../CImageMerging.h"

class CQueue;
class CScanImgsProc
{
public:
	CScanImgsProc(UINT imgWd, UINT imgHt, UINT imgCh, float xResol, float yResol,
		float maxScanRangeX, float maxScanRangeY);
	~CScanImgsProc();

	CQueue*	m_pScanDataQue;
	float	m_fResolX;
	float	m_fResolY;

	float	m_fMaxScanRangeY;
	float	m_fMaxScanRangeX;

	float	m_fCurFullFovX;
	float	m_fCurFullFovY;

	double	m_dScanSrtPosX;
	double	m_dScanSrtPosY;
	double	m_dScanEndPosX;
	double	m_dScanEndPosY;

	// overlap percentile
	UINT	m_nOverlapX;
	UINT	m_nOverlapY;
	UINT	m_nBlockX;
	UINT	m_nBlockY;


	double	m_dsf;	// down sampling factor
	// 
	EFFECTIVE_LOCAL_FOV m_fEffectLocalFov;

	UINT	m_nGrabWd;	// camera's image width
	UINT	m_nGrabHt;	// camera's image height
	UINT	m_nGrabCh;	// camera's image channel
	STITCH_IMG	m_stitchImg;
	STITCH_IMG	m_stitchImgBackUp;

	//
	std::vector<POINTF> m_ptScanCoord;	// Scanner scan coordinate
	// BOOL	m_bStitchDone;

	// sjyi 2023.06.13 부분 스캔할 경우 블럭 개수 
	UINT	m_nBlockCntTotal;

public:
	// actually stitched processed image count
	int		m_nProcessedPopCnt;

	/* -- Poping Thread: Scan(or Load) Data Queue --- */
	BOOL	m_bPopThreadAlive;
	HANDLE	m_hPopThreadEnd;
	static	UINT PopThread(LPVOID pArgs);
	void	PopThreadFunc();
	/* ---------------------------------------------- */

	/* -- After each pop, subpop thread process each popped image -- */
	typedef struct _nPopProcessParam {
		int		nWd = 0;
		int		nHt = 0;
		Image*	pNiImg = NULL;
		BYTE*	pData = NULL;
		int		threadID = -1;
		int		blockIdxY = -1;
		int		blockIdxX = -1;
		int		iActionPurpose = ACT_NONE;
	} NPOP_PROCESS_PARAM;

	HANDLE	m_hNPopProcessEnd;

	NPOP_PROCESS_PARAM m_NPopParam;
	void ProcessEachPopData(NPOP_PROCESS_PARAM* tp);
	/* ----------------------------------------- */

	void Merging();

	void CreateScanQueueBuff(UINT nQbuff, UINT nWd, UINT nHt, UINT nCh);

	//DHChoi 20200626
	//void ChangeScanRange(float resol_x, float resol_y,
	//	double start_x, double start_y, double end_x, double end_y,
	//	UINT nOverlapX = OVERLAP_X, UINT nOverlapY = OVERLAP_Y);
	void ChangeOverlap(UINT nOverlapX, UINT nOverlapY);
	void ChangeScanRange(float resol_x, float resol_y,
		double start_x, double start_y, double end_x, double end_y);


	// Calc Scan Pos & Re-create memory (ni image, data buffer ... )
	EFFECTIVE_LOCAL_FOV CalcNewPatchSize4Stitching(UINT nWd, UINT nHt, float ratioX, float ratioY);
	void ReCalcScanPosAndReCreateMemory();
	void UpdateScanPosMapAndReCalcFov(int* pNumBlockX, int* pNumBlockY);
	void ReCreateNiImage(int newNblockX, int newNblockY);

	//DHChoi 20200626
	int SaveScanImageAll(CString sSavePath);
	CImageMerging m_pMergeCtrl;
	BOOL m_bUseCalibration;
	Point m_pStrBlocIdx;
	Point m_pEndBlocIdx;
	BOOL m_bDisableLimit;
	void UseCalibration(BOOL bUse) { m_bUseCalibration = bUse; }
	void UseDisableScanLimit(BOOL bDisable) { m_bDisableLimit = bDisable; }
	int SetCalibData(CString strCalibPath, int nBlockX, int nBlockY);
	int SetScanArea(double sx, double sy, double ex, double ey);

	// sjyi 20230612 부분 스캔 관련 내용 추가 ------->
	BYTE* m_pBlackBuffer;
	void PushQueBlackImage();

	// sjyi 20230707 기존 스캔 이미지 저장 부분 추가 --->
	void BackUpImage();
	void PushQueBakupImage(int nIndex);
};


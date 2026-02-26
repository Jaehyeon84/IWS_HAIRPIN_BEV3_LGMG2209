#pragma once

typedef std::vector<std::vector <BOOL>> VECMAP_BOOL;
typedef std::vector<BOOL> VEC_BOOL;

class CConfigIWS
{
public:
	CConfigIWS();
	~CConfigIWS();
	
	void SetPath(CString strPath) {
		m_strConfigPath = strPath;
	m_strConfigPath = m_strConfigPath + STR_CONFIG_FILE;
	};
	void ReadConfig();
	void WriteConfig();
	
	CConfigIWS &operator=(const CConfigIWS &rhs);
	int GetNumOfCard() { return m_nCard; };
	int GetNumOfLens() { return m_nLens; }; // 
	int GetCurrentLens() { return m_iCurrLens; };

	CorrParam *pGetCorrParam(int iLens);
	CorrParam GetCorrParam(int iLens);
	// 2024.02.11 jh.kim
	void GetScannerRotation(double *theta);
	void GetWorkRng(double *x, double *y);
	void GetWorkCnt(double *cx, double *cy);
	void GetRefImgScale(double *x, double *y);
	void GetRefImgCnt(double *cx, double *cy);
	void GetScanRng(double *sx, double *sy, double *ex, double *ey);
	void SetScanRng(double sx, double sy, double ex, double ey);
	void SetCenterPos(double m1x, double m1y, double m2x, double m2y, double m3x, double m3y, double m4x, double m4y);
	void SetRefImgCnt(double cx, double cy);//DHChoi 20200629

	int Encrypt(LPCSTR lpszIn, LPSTR lpszOut, int nBufLen);
	int Decrypt(LPCSTR lpszIn, LPSTR lpszOut);

	void ReadMatrix(int nModelNo);
	void WriteMatrix(int nModelNo);
	
	BOOL ReadVScanMap();

	void BackUpConfig(); // sjyi 2024.06.14 추가, 멕시코 현장에서 수정 후, 바로 용접 진행(확인 없이 진행), 안전 및 수정사항 내용 확인을 위해 추가
	BOOL _CreateDirectory(CString sDirPath);
protected:
	void Tokenize(std::vector<CString> &VecStr, CString strSource, TCHAR tToken);
	void ParseVScanMap(TCHAR* szBufferData, int nRow);

public:
	BOOL m_bUseInspect;
	CString m_strConfigPath;
	CArray<CorrParam,CorrParam&> m_arCorrParam;
	CStringArray m_arWorkList;
	int m_nWorkList;

	BOOL m_bUsePLC;
	CString m_strPLC_IP;
	int m_iPLC_Port;
	int m_nPLC_Read;
	int m_nPLC_Write;

	BOOL m_bUseWritePattern;

	int m_nJigNo;
	int m_nCard;
	int m_iCurrLens; 
	int m_nLens;
	double m_dDetectRange;
	BOOL m_bUseHairpin;
	BOOL m_bUse2DProc;

	BOOL m_bUseCircleFit;

	CString m_strPassword;
	
	int m_iLaser;
	double m_dWPCalibStep; // Weld Point calib 
	
	IWS_CENTER_POS m_cp;
	//IWS_HAIRPIN_INFO m_hp;
	//BOOL m_bWeldAllAtOnce;

	float   m_fCcdResol_X;// VRS camera pixel resolution in X(um~mm/pxl)
	float	m_fCcdResol_Y;// VRS camera pixel resolution in Y(um~mm/pxl)
	int		m_nCcdWidth;					// VRS camera number of pixel in x(horizontal)
	int		m_nCcdHeight;				// VRS camera number of pixel in Y(vertical)
	int		m_nLightPort;
	int		m_nLightPower;
	BOOL	m_bUseLight;

	BOOL    m_bUseStartRoom;
	BOOL    m_bUseLegend;

	double m_dHpLegendStartAng;
	double m_dHpLegendDir; // CW : 1, CCW : -1
	double m_dHpLegendDia; //#define HP_TXT_DIA		120.0f

	double m_dRcpScale[6] = { 0 };
	double m_dRcpAngle[6] = { 0 };
	double m_dRcpOffsetX[6] = { 0 };
	double m_dRcpOffsetY[6] = { 0 };

	// 2023.07.12 2D Setting add
	BOOL m_b3DUse;
	double m_d2DRcpScale[6] = { 0 };
	double m_d2DRcpAngle[6] = { 0 };
	double m_d2DRcpOffsetX[6] = { 0 };
	double m_d2DRcpOffsetY[6] = { 0 };

	//double m_dPenPower[10] = { 0 };
	//double m_dPenSpeed[10] = { 0 };
	//21.05.27
	double m_dJumpSpdmin[MAX_PENS];
	double m_dJumpSpdmax[MAX_PENS];

	double m_dWeldSpdmin[MAX_PENS];
	double m_dWeldSpdmax[MAX_PENS];	
	unsigned char m_ckey = 172;	

	//CRing 내, 외경 개별 용접 시 내경에서 외경으로 넘어갈때 주는 딜레이
	//int m_nInOutAirSleep;

	//Calibration Metrix
	/*double m_dCalibMtx_a11;
	double m_dCalibMtx_a12;
	double m_dCalibMtx_a13;
	double m_dCalibMtx_a21;
	double m_dCalibMtx_a22;
	double m_dCalibMtx_a23;*/

	//1St Pattern Limit(Length, Width, Repeat)	
	double m_d1StWidth;
	int m_n1StRepeat;

	//2nd Pattern Limit(Width, OverLength, Repeat)
	double m_d2ndWidth;	
	int m_n2ndRepeat;

	double m_dLimitEnergy;

	//Laser 출력 Spec
	int m_nLaserMax;

	// sjyi 2022-07-08 JigA와 JigB의 결과를 따로 저장하기 위해 변수 변경
	// sjyi 2022-07-07 Matrix 관련 변수 추가--->
	//Calibration Metrix 
	//double m_dCalibMtx_a11;
	//double m_dCalibMtx_a12;
	//double m_dCalibMtx_a13;
	//double m_dCalibMtx_a21;
	//double m_dCalibMtx_a22;
	//double m_dCalibMtx_a23;
	//IWS_MATRIX_RESULT m_MatrixJigA;
	//IWS_MATRIX_RESULT m_MatrixJigB;
	// sjyi 2022-07-07 Matrix 관련 변수 추가<---

	CALI_MATRIX m_MatrixA[MATRIX_DIVISION];
	CALI_MATRIX m_MatrixB[MATRIX_DIVISION];

	BOOL m_bUseOneByOnMatrix;
	double m_dOnebyOneMatchingX_JigA[MAX_HAIRPIN_SLOT * MAX_HAIRPIN_LAYER];
	double m_dOnebyOneMatchingY_JigA[MAX_HAIRPIN_SLOT * MAX_HAIRPIN_LAYER];
	double m_dOnebyOneMatchingX_JigB[MAX_HAIRPIN_SLOT * MAX_HAIRPIN_LAYER];
	double m_dOnebyOneMatchingY_JigB[MAX_HAIRPIN_SLOT * MAX_HAIRPIN_LAYER];

	// 2022.08.31 jh.kim 추가 - 분할 용접 사용 유무
	BOOL m_bDivisionWelding;

	// 2023.01.18 sjyi 추가 - 모델별 Matching 분리 여부
	BOOL m_bMatrixByModel;

	// 2023.02.14 sjyi 추가 - 시간동기화 사용 여부
	BOOL m_bTimeSync;

	// 2023.02.18 sjyi 추가 - PLC 통신 재연결 시도 여부
	BOOL m_bReconnectPLC;
	
	// 2024.02.05 jh.kim 추가 - 용접 후 좌표 저장 유무
	BOOL m_bUseWeldPosSave;

	// 2023.04.07 sjyi 높이 변화에 따른 좌표 보상 사용여부
	//#####################################################################
	//##[HeightCompensation]	높이 보상 파라미터(헤어핀 높이에 따라 X,Y 좌표를 보상해줌)
	//##
	//##   UseComp : 높이 보상 적용 여부(1 = 적용, 0 = 적용안함)
	//##   ReferenceHeight : 높이 보상 기준 높이(단위 mm)
	//##   UseDistanceNGCheck : NGCheck 사용 여부, Recipe 기준점 보다 많이 벗어날 경우 NG 발생(1 = NG Check 사용, 0 = NG Check 안함)
	//##   NG Distance : NGCheck 기준(단위 mm)
	//##   Save Result : 높이 보상 결과 저장 여부(1 = 저장, 0 = 저장안함)
	/////////////////////// 아래는 개발자외 설정 금지 /////////////////////////////////
	//##   Center(X) : 기준 원점좌표 X(단위 mm)
	//##   Center(Y) : 기준 원점좌표 Y(단위 mm)
	//##   Working Distance : F-Theta 렌즈의 Working Distance(단위 mm)
	//##   Telecentric Angle(MAX) : F-Theta렌즈의 Telecentric Error Angle의 최대 값(단위 Degree)
	//##   Working Radius : F-Theta렌즈의 작업 반경(단위 mm)
	//##   View ManualSave : 높이 보상 데이터 수동저장 버튼 보이기(1 = 보임, 0 = 안보임)
	//##
	//#####################################################################

	// 2023.04.07 sjyi 높이 변화에 따른 좌표 보상 사용여부
	BOOL m_bUseHeightComp;		// 높이보상 사용여부
//	double m_dHCompRefHeight;	// 기준 높이
	double m_dHCompRefHeight[2][2];	// 기준 높이
	BOOL	m_bSaveHeightComp;	// HComp 결과 저장 여부
	//////////////// 아래는 개발자외 설정 금지 ////////////////////////
	double m_dHCompCenterX;		// 중심점(스캐너 중심점) X ->
	double m_dHCompCenterY;		// 중심점(스캐너 중심점) Y ->
	double m_dHCompWD;			// 스캐너 Working Distance
	double m_dHCompTCMaxAngle;	// Telecentric Error Angle Max값
	double m_dHCompWorkRadius;	// F-Theta Lens Working Radius
	BOOL	m_bViewHCompManual; // 수동저장 버튼 보이기

//	int		m_nLightMode;		// 2023.04.26 sjyi Light Mode, 0 : Serial, 1 : DIO

// 2023.06.12 sjyi Vision Scan 부분 스캔 설정
//#####################################################################
//##[VisionScan] Vision 스캔 사용 설정
//#####################################################################
//##   UseAlwaysScan : 자동시 항상 스캔(1 = 적용, 0 = 적용안함)
//##   UsePartialScan : 부분 스캔 사용 여부(1 = 적용, 0 = 적용안함)
//##   ScanMapFilePath : 부분 스캔 Map 파일 경로
//##   ScanSleepTime : 스캐너 대기 시간(단위, msec)
//##   SaveImage : 스캔시 이미지 저장(1 = 적용, 0 = 적용안함)
//#####################################################################
// sjyi 2023.12.13 항상 스캔하는 모드 Jig A, Jig B 구분
//	BOOL	m_bUseAlwaysScan;
	BOOL	m_bUseAlwaysScanJigA;
	BOOL	m_bUseAlwaysScanJigB;
	//BOOL	m_bUseAlwaysScanRework; // sjyi 2024.05.28 재용접 스캔여부 추가
	BOOL	m_bUseAlwaysScanReworkJigA; // sjyi 2024.05.28 재용접 스캔여부 추가
	BOOL	m_bUseAlwaysScanReworkJigB; // sjyi 2024.05.28 재용접 스캔여부 추가
	BOOL	m_bUseAlwaysScanWeldJigA;	// sjyi 2024.05.29 용접 후, 스캔 여부 추가
	BOOL	m_bUseAlwaysScanWeldJigB;	// sjyi 2024.05.29 용접 후, 스캔 여부 추가

	BOOL	m_bUsePartialScan;
	CString m_strPartialScanMap;
	int		m_nScanSleepTime;
	BOOL	m_bSaveImage;

	// 2024.02.12 jh.kim 추가 - Image position scale, offset
	double  m_dPosScaleX, m_dPosScaleY;
//	int		m_nPosOffsetX, m_nPosOffsetY;
	double	m_dPosOffsetX, m_dPosOffsetY;

	VECMAP_BOOL m_vmapUseScan; // Vision Scan Map

	BOOL	m_bUseSpiralPtn;
	BOOL	m_bUseSandGlassPtn;
	BOOL	m_bUseSortieRepeat;

	BOOL	m_bSavePos;

// 2024.06.19 sjyi 고정좌표 검출기능 추가
//#####################################################################
//##[DistanceCheck] 고정좌표 검출기능
//#####################################################################
//##   UseCheckPos(JigA) : 고정좌표 검출 사용여부 - JigA (1 = 적용, 0 = 적용안함)
//##   UseCheckPos(JigB) : 고정좌표 검출 사용여부 - JigB (1 = 적용, 0 = 적용안함)
//##   PosFilePath(JigA) : 비교 고정좌표 파일 경로 - JigA
//##   PosFilePath(JigB) : 비교 고정좌표 파일 경로 - JigB
//##   NGRange(JigA) : 비교 기준, 이 값 이상 차이나면, 고정좌표로 용접 - JigA
//##   NGRange(JigB) : 비교 기준, 이 값 이상 차이나면, 고정좌표로 용접 - JigB
//#####################################################################
	BOOL m_bUseNGCheckJigA;		// 거리 NG Check 사용 여부(JigA)
	double m_dNGDistanceJigA;	// NG Check 기준 거리(JigA)
	CString m_strPosJigA;		// NG Check 기준 좌표(JigA)
	BOOL m_bUseNGCheckJigB;		// 거리 NG Check 사용 여부(JigA)
	double m_dNGDistanceJigB;	// NG Check 기준 거리(JigA)
	CString m_strPosJigB;		// NG Check 기준 좌표(JigA)
};


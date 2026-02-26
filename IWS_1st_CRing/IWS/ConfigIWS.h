#pragma once


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
	
	void BackUpConfig(); // sjyi 2024.06.14 추가, 멕시코 현장에서 수정 후, 바로 용접 진행(확인 없이 진행), 안전 및 수정사항 내용 확인을 위해 추가
	BOOL _CreateDirectory(CString sDirPath);

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

	//20251202 
	int m_nJumpDelay;

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

	// 2022.08.31 jh.kim 추가 - 분할 용접 사용 유무
	BOOL m_bDivisionWelding;

	// 2023.01.18 sjyi 추가 - 모델별 Matching 분리 여부
	BOOL m_bMatrixByModel;

	// 2023.02.14 sjyi 추가 - 시간동기화 사용 여부
	BOOL m_bTimeSync;

	// 2023.02.18 sjyi 추가 - PLC 통신 재연결 시도 여부
	BOOL m_bReconnectPLC;

	// 2023.06.12 sjyi Vision Scan 부분 스캔 설정
//#####################################################################
//##[VisionScan] Vision 스캔 사용 설정
//#####################################################################
//##   UseAlwaysScan(JigA) : 자동시, 용접 전 항상 스캔(Jig A)(1 = 적용, 0 = 적용안함)
//##   UseAlwaysScan(JigB) : 자동시, 용접 전 항상 스캔(Jig B)(1 = 적용, 0 = 적용안함)
//##   UseAlwaysScanRework(JigA) : 자동시, 재용접 전 항상 스캔(Jig A)(1 = 적용, 0 = 적용안함)
//##   UseAlwaysScanRework(JigB) : 자동시, 재용접 전 항상 스캔(Jig B)(1 = 적용, 0 = 적용안함)
//##   UseAlwaysScanWeld(JigA) : 자동시, 용접 후, 항상 스캔(Jig A)(1 = 적용, 0 = 적용안함)
//##   UseAlwaysScanWeld(JigB) : 자동시, 용접 후, 항상 스캔(Jig A)(1 = 적용, 0 = 적용안함)
//##   SaveImage : 스캔시 이미지 저장(1 = 적용, 0 = 적용안함)
//#####################################################################
	// sjyi 2024.06.11 추가 - 스캔이미지 저장여부
	BOOL m_bSaveImage;

	BOOL	m_bUseAlwaysScanJigA;
	BOOL	m_bUseAlwaysScanJigB;

	BOOL	m_bUseAlwaysScanReworkJigA; // sjyi 2024.05.28 재용접 스캔여부 추가
	BOOL	m_bUseAlwaysScanReworkJigB; // sjyi 2024.05.28 재용접 스캔여부 추가

	BOOL m_bUseAlwaysScanWeldJigA;	// sjyi 2024.05.29 용접 후, 스캔 여부 추가
	BOOL m_bUseAlwaysScanWeldJigB;	// sjyi 2024.05.29 용접 후, 스캔 여부 추가

	// 2024.02.12 jh.kim 추가 - Image position scale, offset
	double  m_dPosScaleX, m_dPosScaleY;
	double	m_dPosOffsetX, m_dPosOffsetY;

	BOOL	m_bUseSortieRepeat;

	// sjyi 2024.12.20 추가 패턴 사용 유무
	BOOL	m_bUseSpiralPtn;
	BOOL	m_bUseSandGlassPtn;

};


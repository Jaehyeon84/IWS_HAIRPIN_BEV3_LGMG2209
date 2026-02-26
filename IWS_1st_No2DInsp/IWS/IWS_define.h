#pragma once
#include <vector>
//#include "Job.h"
#include "CJobGroup.h"
#include "LaserPen.h"
//#include "Ini.h"

#define _SPI_LASER		100
#define _TRUMF_LASER	200
#define _IPG_LASER		300
#define _IPG_SINGLE		301
#define LASER_SOURCE	_TRUMF_LASER
#define INTELLISCAN
#define MAX_SCAN_CALC_VECTOR 100 // sjyi 2023.07.16 스캔 검증용 Ref 데이터 벡터 최대 크기

#define STR_PENFILE _T("penfile.dat")
#define STR_REFIMG_FILE _T("refImage.bmp")

#define STR_PROPERTY_FILE _T("Property.ini")
#define STR_CONFIG_FILE _T("configIWS.ini")

// 2022.09.07 jh.kim 추가 - PCC Matrix 기능 추가
#define STR_PCC_CONFIG_FILE _T("PCC.ini")

#define STR_SECTION_SYSTEM _T("System")
#define STR_SECTION_CALIBMETRIX _T("CalibMetrix")
#define STR_SECTION_CALIBMETRIX_JIGA _T("CalibMetrixJigA")		// sjyi 2022-07-08 JigA, JigB 구분을 위해 추가
#define STR_SECTION_CALIBMETRIX_JIGB _T("CalibMetrixJigB")		// sjyi 2022-07-08 JigA, JigB 구분을 위해 추가
#define STR_SECTION_CALIB_ONEBYONE_JIGA _T("CalibOnebyOneJigA")
#define STR_SECTION_CALIB_ONEBYONE_JIGB _T("CalibOnebyOneJigB")
#define STR_SECTION_PTRNLIMIT _T("PTRN_LIMIT")
#define STR_SECTION_WORKLIST _T("WorkList")
#define STR_KEY_WORKAREA_X _T("WorkAreaX")
#define STR_KEY_WORKAREA_Y _T("WorkAreaY")
#define STR_KEY_WORKAREA_CX _T("WorkAreaCenX")
#define STR_KEY_WORKAREA_CY _T("WorkAreaCenY")
#define STR_KEY_WORKAREA_IMGX _T("WorkAreaImgX")
#define STR_KEY_WORKAREA_IMGY _T("WorkAreaImgY")
#define STR_KEY_NCARD _T("NumOfCard")

#define STR_SECTION_CORRPARAM _T("CorrParam")
#define STR_KEY_CORR_FILE _T("Cor. File")
#define STR_KEY_CORR_SCALEFACTOR _T("Scale Factor")
#define STR_KEY_CORR_KX  _T("KX")
#define STR_KEY_CORR_KY  _T("KY")
#define STR_KEY_CORR_OFFSETX  _T("OffsetX")
#define STR_KEY_CORR_OFFSETY  _T("OffsetY")
#define STR_KEY_CORR_THETA  _T("Theta")
#define STR_KEY_CORR_BASESIZE _T("BaseSize")

#define STR_SECTION_HAIRPIN _T("Hairpin")
#define STR_SECTION_CENTER _T("CenterPos")
#define STR_LAYER_LEGEND _T("Legend")
#define STR_LAYER_WORKAREA_BK _T("WorkAreaBk")
#define STR_LAYER_IMAGE_BK _T("ScanImage")
#define STR_LAYER_0 _T("0")
#define STR_WORKAREA_RECT _T("WorkRect")
#define STR_LAYER_INFO _T("InfoLayer")

#define STR_SECTION_VISION _T("VISION")

#define IWS_VERSION _T("IWS [LGMG2209]_1st")
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
#define STR_SECTION_HEIGTCOMP	_T("HeightCompensation")
#define STR_KEY_HCOMP_USE		_T("UseComp")
#define STR_KEY_HCOMP_REF_H		_T("ReferenceHeight")
#define STR_KEY_HCOMP_CENTER_X	_T("Center(X)")
#define STR_KEY_HCOMP_CENTER_Y	_T("Center(Y)")
#define STR_KEY_HCOMP_WD		_T("Working Distance")
#define STR_KEY_HCOMP_USE_NGCHK   _T("UseDistanceNGCheck")
#define STR_KEY_HCOMP_NG_DISTANCE _T("NG Distance")
#define STR_KEY_HCOMP_TC_ANG_MAX	 _T("Telecentric Angle(MAX)")
#define STR_KEY_HCOMP_WORKING_RADIUS _T("Working Radius")
#define STR_KEY_HCOMP_SAVE_RESULT _T("Save Result")
#define STR_KEY_HCOMP_VIEW_MANUAL _T("View ManualSave")

// 2023.06.12 sjyi Vision Scan 부분 스캔 설정
//#####################################################################
//####[VisionScan] Vision 스캔 사용 설정
//#####################################################################
//##   UsePartialScan : 부분 스캔 사용 여부(1 = 적용, 0 = 적용안함)
//##   ScanMapFilePath : 부분 스캔 Map 파일 경로
//##   UseAlwaysScan : 자동시 항상 스캔(1 = 적용, 0 = 적용안함)
//##   ScanSleepTime : 스캐너 대기 시간(단위, msec)
//#####################################################################
#define STR_SECTION_VISIONSCAN		_T("VisionScan")
#define STR_KEY_VS_USE_PARTIAL		_T("UsePartialScan")
#define STR_KEY_VS_PARTIAL_MAP_PATH	_T("ScanMapFilePath")
//#define STR_KEY_VS_USE_ALWAYS_SCAN	_T("UseAlwaysScan")
#define STR_KEY_VS_USE_ALWAYS_SCAN_JIGA	_T("UseAlwaysScan(JigA)")
#define STR_KEY_VS_USE_ALWAYS_SCAN_JIGB	_T("UseAlwaysScan(JigB)")
//#define STR_KEY_SV_USE_ALWAYS_SCAN_REWORK _T("UseAlwaysScanRework")
#define STR_KEY_SV_USE_ALWAYS_SCAN_REWORK_A _T("UseAlwaysScanRework(JigA)")
#define STR_KEY_SV_USE_ALWAYS_SCAN_REWORK_B _T("UseAlwaysScanRework(JigB)")

#define STR_KEY_SV_USE_ALWAYS_SCAN_WELD_A _T("UseAlwaysScanWeld(JigA)")
#define STR_KEY_SV_USE_ALWAYS_SCAN_WELD_B _T("UseAlwaysScanWeld(JigB)")

#define STR_KEY_VS_SCAN_SLEEP		_T("ScanSleepTime")
#define STR_KEY_VS_SAVE_IMAGE		_T("SaveImage")

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
#define STR_SECTION_DISTANCE_CHECK		_T("DistanceCheck")
#define STR_KEY_DC_USE_CHECK_JIGA		_T("UseCheckPos(JigA)")
#define STR_KEY_DC_USE_CHECK_JIGB		_T("UseCheckPos(JigB)")
#define STR_KEY_DC_POS_PATH_JIGA		_T("PosFilePath(JigA)")
#define STR_KEY_DC_POS_PATH_JIGB		_T("PosFilePath(JigB)")
#define STR_KEY_DC_NG_RANGE_JIGA		_T("NGRange(JigA)")
#define STR_KEY_DC_NG_RANGE_JIGB		_T("NGRange(JigB)")


#define MAX_PENS		16
#define PEN_SIMULATION (MAX_PENS-2)


#define STRINGIFY(_s) #_s
#define DECLARE_USER_MESSAGE(name) \
		static const UINT name = ::RegisterWindowMessage(name##_MSG);

#define check_bit(data, loc) ((data)&(0x01<<(loc)))
#define set_bit(data, loc) ((data)|=(0x01 << (loc)))

#define WELDJOB_VER100 100
#define WELDJOB_VER101 101
#define WELDJOB_VER102 102
#define WELDJOB_VER103 103
#define WELDJOB_VER104 104
#define WELDJOB_VER105 105		// sjyi 2023.04.15 헤어핀 회전 추가 
#define WELDJOB_VERC	WELDJOB_VER105

#define WORK_VER100	   100
#define WORK_VER101	   101
#define WORK_VER102	   102
#define WORK_VER103	   103
#define	WORK_VER104	   104
#define	WORK_VER105	   105		// sjyi 2023.04.14 헤어핀 회전 추가
#define WORK_VER106	   106		// sjyi 2023.06.15 용접 타입 설정값 변경(In->Out, Out->In만 + 시작 슬롯, Step Offset 값 추가로 설정)
#define WORK_VER107	   107		// dhpark 2023.06.26 2D Inspection 추가
#define WORK_VER108	   108		// dhpark 2023.07.13 2D Offset 추가
#define WORK_VER109	   109		// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가
#define WORK_VER110	   110		// sjyi 2024.05.25 소티 반복, 스파이럴 패턴 추가
#define WORK_VERC	   WORK_VER110



#define PULSE_RESOLUTION 0.05 // msec
#define MAX_PULSE_SHAPE 200000
#define PULSE_DUTY_CYCLE 0.1
#define INTERPOLATE(x, x1, x2, y1, y2) (((y2)-(y1))/((x2)-(x1))*((x)-(x1))+y1)

// Vision Inspection
//#define PLC_READ_INSPECT_A    32000
//#define PLC_READ_INSPECT_B    42000
#define PLC_READ_INSPECT_A    31000		// sjyi 2022-07-08
#define PLC_READ_INSPECT_B    41000		// sjyi 2022-07-08
//#define PLC_INSP_MAX_BUFF     6144 //3072 // 48 x 8 x 16 /2

#define PLC_READ_REPAIR_A	  39000
#define PLC_READ_REPAIR_B	  49000
//#define PLC_REPAIR_MAX_BUFF	384

// PTRN
#define PATTERN_TYPE_NONE 0
#define PATTERN_TYPE_1ST  1
#define PATTERN_TYPE_2ND  2


typedef enum enIWS_1ST_PTN {
	IWS_1ST_ELLIPSE,		// 타원형
	IWS_1ST_LINE,			// 선형
	IWS_1ST_ZIGZAG,			// 지그재그
	IWS_1ST_OCTAGON,		// 옥타곤, 일부 사이트 미사용(ex, BEV3 남경, BEV3 멕시코 등)
	IWS_1ST_SPIRAL			// 나선형 추가, sjyi 2024.01.31
}IWS_1ST_PTN;

// sjyi 2022-08-28 2차패턴을 위해 enum 추가 -->
typedef enum enIWS_2ND_PTN {
	IWS_2ND_ELLIPSE,		// 타원형
	IWS_2ND_LINE,			// 선형
	IWS_2ND_ZIGZAG,			// 지그재그
	IWS_2ND_ZIGZAG2,		// 지그재그2 - 1차패턴과 같은 모양
	IWS_2ND_SANDGLASS,		// 모래시계
	IWS_2ND_SPIRAL			// 나선형 추가, sjyi 2024.01.31
}IWS_2ND_PTN;
// sjyi 2022-08-28 2차패턴을 위해 enum 추가 <--

// sjyi 2022-10-27 작업순서 타입 확인을 위한 enum 추가 -->
typedef enum enIWS_JOB_TYPE {
	IWS_JOB_DIV_INOUT,
	IWS_JOB_DIV_OUTIN,
	IWS_JOB_SPIRAL_INOUT,
	IWS_JOB_SPIRAL_OUTIN,
	IWS_JOB_NORMAL,
	IWS_JOB_SPIRAL2,
	IWS_JOB_CRING,
	IWS_JOB_CRING_DIVISION_BLOW
}IWS_JOB_TYPE;
// sjyi 2022-10-27 작업순서 타입 확인을 위한 enum 추가 <--

typedef enum enIWS_HAIRPIN_GEN_DIR {
	GEN_CCW = 0,
	GEN_CW
}IWS_HAIRPIN_GEN_DIR;

// sjyi 2024.02.13 Sortie 반복 방향 설정 Enum 추가
typedef enum enSORTIE_DIR {
	SORTIE_ONEWAY,	// 한 방향
	SORTIE_BIDIRECTION // 양방향
}SORTIE_DIR;

#define MAX_NUM_REPEAT_WELD 30
//
//#define LASER_MAX_POWER	5000
//#define WELD_REPEAT_MAX	30

typedef struct _ENTSPIRAL {
	int iTtype;
	double a, b;
	double cx, cy;
	int trun;
}ENTSPIRAL;

//#define MAX_HAIRPIN_LAYER 8
#define MAX_HAIRPIN_SLOT 96
#define MAX_HAIRPIN_LAYER 10




// 2022.08.15 jh.kim 변경 - Matrix 분할 ( 4분할 )
#define MATRIX_DIVISION 4

typedef struct _CALI_MATRIX
{
	double a11, a12, a13;
	double a21, a22, a23;

	_CALI_MATRIX()
	{
		a11 = 1.0;
		a12 = 0.0;
		a13 = 0.0;
		a21 = 0.0;
		a22 = 1.0;
		a23 = 0.0;
	};
} CALI_MATRIX;

typedef struct _IWS_CENTER_POS
{
	double dModel1_X;
	double dModel1_Y;
	double dModel2_X;
	double dModel2_Y;
	double dModel3_X;
	double dModel3_Y;
	double dModel4_X;
	double dModel4_Y;

	_IWS_CENTER_POS()
	{
		dModel1_X = 0;
		dModel1_Y = 0;
		dModel2_X = 0;
		dModel2_Y = 0;
		dModel3_X = 0;
		dModel3_Y = 0;
		dModel4_X = 0;
		dModel4_Y = 0;
	};
} IWS_CENTER_POS;

typedef struct _IWS_HAIRPIN_Z_V103
{
	double dblMin;
	double dblMax;
	int    nCnt;

	_IWS_HAIRPIN_Z_V103()
	{
		dblMin = 0;
		dblMax = 0;
		nCnt = 0;
	};

}IWS_HAIRPIN_Z_V103;

typedef struct _IWS_HAIRPIN_Z
{
	double dblMin;
	double dblMax;
	int    nCnt;
	double dblPower; // mspark 2022.04.18 향후에 미친 유저놈들이 요구하면 사용하면 됨. 현재는 고정

	_IWS_HAIRPIN_Z()
	{
		dblMin = 0;
		dblMax = 0;
		nCnt = 0;
		dblPower = 1.0;
	};

}IWS_HAIRPIN_Z;

typedef struct _IWS_HAIRPIN_Z_COMP_V103
{
	BOOL   bUse;
	double dblThreshold;
	int    nCnt;

	IWS_HAIRPIN_Z_V103 zHpArr[10];

	_IWS_HAIRPIN_Z_COMP_V103()
	{
		bUse = FALSE;
		dblThreshold = 0;
		nCnt = 0;
	};

}IWS_HAIRPIN_Z_COMP_V103;

typedef struct _IWS_HAIRPIN_Z_COMP
{
	BOOL   bUse;
	double dblThreshold;
	int    nCnt;
	double dblPower;// mspark 2022.04.20
	IWS_HAIRPIN_Z zHpArr[10];

	_IWS_HAIRPIN_Z_COMP()
	{
		bUse = FALSE;
		dblThreshold = 0;
		nCnt = 0;
		dblPower = 1.0;
	};

}IWS_HAIRPIN_Z_COMP;

typedef struct _IWS_HAIRPIN_OFFSET
{
	double ax, ay, bx, by;
	_IWS_HAIRPIN_OFFSET()
	{
		ax = 0.0;
		ay = 0.0; 
		bx = 0.0;
		by = 0.0;
	}
}IWS_HAIRPIN_OFFSET;


typedef struct _IWS_HAIRPIN_INFO {
	int nSlot;				// Slot 개수
	int nDrawSlot;			// 화면에 표시할 Slot 개수(실제 용접할 Slot)
	int nLayer;				// Layer 개수
	int nGenDir;			// 헤어핀 생성 방향(0 - CCW, 1 - CW
	double startAng;		// Slot 시작 각도
	double HpSize;			// Hairpin 1차패턴 길이
	double HpSizeW;			// Hairpin 1차패턴 폭
	double HpSize2nd;		// Hairpin 2차패턴 폭

	double LayerDia[MAX_HAIRPIN_LAYER]; // Layer 반지름
	double HpOverLen;		// 2차패턴 over length

	int nCntP1;				// 1차패턴 Count
	int nPenP1;				// 1차패턴 Pen
	int nWeldShapeP1;		// 1차패턴 Shape
	int nWeldZigZagFreqP1;  // 1차패턴 Zigzag Freq

	int nPenP2;				// 2차패턴 Pen
	int nCntP2;				// 2차패턴 Count
	int nWeldShapeP2;		// 2차패턴 Shape
	int nWeldZigZagFreqP2;  // 2차패턴 Zigzag Freq

	int nSwitchWelding;		// 스위치 웰딩 사용여부(Z보상에 대해서만 적용)
	int nWeldType;			// 용접 타입

	double dblPowerRatio;	// 용접 세기 비율
	double dblSpeedRatio;	// 용접 속도 비율

	int nModel;				// Bottom or Top(하부, 상부)

	// 2022-07-07 Matrix 관련 변수 추가--->
	//Layer 별 각도 Offset
	double dDegree12;
	double dDegree34;
	double dDegree56;
	double dDegree78;
	double dDegree90;
	// 2022-07-07 Matrix 관련 변수 추가<---

	IWS_HAIRPIN_OFFSET hpOffset[512]; // 헤어핀 Offset

	double m_dPenPower[10] = { 0 };
	double m_dPenSpeed[10] = { 0 };

	// mspark 2022.04.22
	BOOL bUseRptPowerProfile_1st;		// 반복횟수별 파워 조정 사용여부(1차 패턴)
	BOOL bUseRptPowerProfile_2nd;		// 반복횟수별 파워 조정 사용여부(2차 패턴)
	double dRptPowerProfile_1st[MAX_NUM_REPEAT_WELD] = { 1.0, };	// 반복횟수별 파워(1차 패턴) - 1.0이 100%임
	double dRptPowerProfile_2nd[MAX_NUM_REPEAT_WELD] = { 1.0, };	// 반복횟수별 파워(2차 패턴) - 1.0이 100%임

	BOOL bEnableSortiRpt;	// Sortie 반복 활설화 Flag
	int  nSortieCnt;		// Sortie 반복 횟수
	int  nSortieDir;		// Sortie 방향(0 - 한 방향, 1 - 양방향) // sjyi 2024.02.13 Sortie 반복 방향 설정 추가

	double HpSize2nd2;		// 모래시계(SandGlass) 패턴을 위한 변수(2차패턴 Width2)

	//// sjyi 2023.10.04
	//BOOL  bUseAutoModify;
	//double dNGRange;

	// sjyi 2023.04.14 패턴 회전각
	double dPtnRotationAngle;
	double dPtnRotationAngle2;

	// sjyi 2023.06.15 Weld Type관련 값 추가
	int nDivStartSlot;
	int nDivJumpOffset;

	// dhpark 2023.07.13 2D Offset 추가
	BOOL b3DUse;
	IWS_HAIRPIN_OFFSET hpOffset2D[512];

	// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전기능 사용여부 추가
	BOOL bReversal;

	// sjyi 2024.01.31 스파이럴 패턴 관련 내용 추가
	BOOL	bUseOutToInP1;	// Sprial Out to In 옵션(1st Pattern)
	BOOL	bUseOutToInP2;	// Sprial Out to In 옵션(2nd Pattern)
	BOOL	bCloseSpiralP1; // Sprial Closed 옵션(1st Pattern)
	BOOL	bCloseSpiralP2; // Sprial Closed 옵션(2nd Pattern)

	_IWS_HAIRPIN_INFO() {
		nSlot = 96;
		nDrawSlot = 96;
		nLayer = MAX_HAIRPIN_LAYER;
		startAng = 0;
		HpSize = 0.7;
		LayerDia[0] = 170;
		LayerDia[1] = 180;
		LayerDia[2] = 190;
		LayerDia[3] = 200;
		LayerDia[4] = 210;
		LayerDia[5] = 220;
		LayerDia[6] = 230;
		LayerDia[7] = 240;
		LayerDia[8] = 250;
		LayerDia[9] = 260;
		HpOverLen = 0.50;

		nCntP1 = 1;
		nPenP1 = 1;
		nWeldShapeP1 = 0;
		nWeldZigZagFreqP1 = 3;

		nCntP2 = 1;
		nPenP2 = 1;
		nWeldShapeP2 = 0;
		nWeldZigZagFreqP2 = 3;

		HpSize2nd = 0.4;
		HpSizeW = 0.1;

		nSwitchWelding = 0;
		nWeldType = 0;

		dblPowerRatio = 100.0;
		dblSpeedRatio = 100.0;

		// 2022-07-07 Matrix 관련 변수 추가--->
		dDegree12 = 0.0f;
		dDegree34 = 0.0f;
		dDegree56 = 0.0f;
		dDegree78 = 0.0f;
		dDegree90 = 0.0f;
		// 2022-07-07 Matrix 관련 변수 추가<---

		bUseRptPowerProfile_1st = FALSE;
		bUseRptPowerProfile_2nd = FALSE;

		ZeroMemory(dRptPowerProfile_1st, MAX_NUM_REPEAT_WELD);
		ZeroMemory(dRptPowerProfile_2nd, MAX_NUM_REPEAT_WELD);

		bEnableSortiRpt = FALSE;
		nSortieCnt = 0;
		nSortieDir = SORTIE_ONEWAY; // sjyi 2024.02.13 Sortie 반복 방향 설정 추가, 기본값 한방향

		nModel = 0;

		//bUseAutoModify = FALSE;
		//dNGRange = 10.0;

		// sjyi 2023.04.14 회전각
		dPtnRotationAngle = 0.0;
		dPtnRotationAngle2 = 0.0;

		// sjyi 2023.06.15 Weld Type Division 관련
		nDivStartSlot = 1;
		nDivJumpOffset = 1;

		// dhpark 2023.07.13 2D Offset 추가
		b3DUse = TRUE;

		// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전기능 사용여부 추가
		bReversal = FALSE;

		// sjyi 2024.01.31 스파이럴 패턴 관련 내용 추가
		bUseOutToInP1 = TRUE;
		bUseOutToInP2 = TRUE;
		bCloseSpiralP1 = TRUE;
		bCloseSpiralP2 = TRUE;

	};

}IWS_HAIRPIN_INFO;

typedef struct _IWS_HAIRPIN_INFO_V109 {
	int nSlot;
	int nDrawSlot;
	int nLayer;
	double startAng;
	double HpSize;
	double HpSizeW;
	double HpSize2nd;
	double LayerDia[MAX_HAIRPIN_LAYER];
	double HpOverLen;		// 2차패턴 over length

	int nCntP1;				// 1차패턴 Count
	int nPenP1;				// 1차패턴 Pen
	int nWeldShapeP1;		// 1차패턴 Shape
	int nWeldZigZagFreqP1;  // 1차패턴 Zigzag Freq

	int nPenP2;				// 2차패턴 Pen
	int nCntP2;				// 2차패턴 Count
	int nWeldShapeP2;		// 2차패턴 Shape
	int nWeldZigZagFreqP2;  // 2차패턴 Zigzag Freq

	int nSwitchWelding;
	int nWeldType;

	double dblPowerRatio;
	double dblSpeedRatio;

	// 2022-07-07 Matrix 관련 변수 추가--->
	//Layer 별 각도 Offset
	double dDegree12;
	double dDegree34;
	double dDegree56;
	double dDegree78;
	double dDegree90;
	// 2022-07-07 Matrix 관련 변수 추가<---

	IWS_HAIRPIN_OFFSET hpOffset[512];

	double m_dPenPower[10] = { 0 };
	double m_dPenSpeed[10] = { 0 };

	// mspark 2022.04.22
	BOOL bUseRptPowerProfile_1st;
	BOOL bUseRptPowerProfile_2nd;
	double dRptPowerProfile_1st[MAX_NUM_REPEAT_WELD] = { 1.0, };
	double dRptPowerProfile_2nd[MAX_NUM_REPEAT_WELD] = { 1.0, };

	// sjyi 2023.04.14 패턴 회전각
	double dPtnRotationAngle;
	double dPtnRotationAngle2;

	// sjyi 2023.06.15 Weld Type관련 값 추가
	int nDivStartSlot;
	int nDivJumpOffset;

	// dhpark 2023.07.13 2D Offset 추가
	BOOL b3DUse;
	IWS_HAIRPIN_OFFSET hpOffset2D[512];

	// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전기능 사용여부 추가
	BOOL bReversal;

	_IWS_HAIRPIN_INFO_V109() {
		nSlot = 96;
		nDrawSlot = 96;
		nLayer = MAX_HAIRPIN_LAYER;
		startAng = 0;
		HpSize = 0.7;
		LayerDia[0] = 170;
		LayerDia[1] = 180;
		LayerDia[2] = 190;
		LayerDia[3] = 200;
		LayerDia[4] = 210;
		LayerDia[5] = 220;
		LayerDia[6] = 230;
		LayerDia[7] = 240;
		LayerDia[8] = 250;
		LayerDia[9] = 260;
		HpOverLen = 0.50;

		nCntP1 = 1;
		nPenP1 = 1;
		nWeldShapeP1 = 0;
		nWeldZigZagFreqP1 = 3;

		nCntP2 = 1;
		nPenP2 = 1;
		nWeldShapeP2 = 0;
		nWeldZigZagFreqP2 = 3;

		HpSize2nd = 0.4;
		HpSizeW = 0.1;

		nSwitchWelding = 0;
		nWeldType = 0;

		dblPowerRatio = 100.0;
		dblSpeedRatio = 100.0;

		// 2022-07-07 Matrix 관련 변수 추가--->
		dDegree12 = 0.0f;
		dDegree34 = 0.0f;
		dDegree56 = 0.0f;
		dDegree78 = 0.0f;
		dDegree90 = 0.0f;
		// 2022-07-07 Matrix 관련 변수 추가<---

		bUseRptPowerProfile_1st = FALSE;
		bUseRptPowerProfile_2nd = FALSE;

		ZeroMemory(dRptPowerProfile_1st, MAX_NUM_REPEAT_WELD);
		ZeroMemory(dRptPowerProfile_2nd, MAX_NUM_REPEAT_WELD);

		// sjyi 2023.04.14 회전각
		dPtnRotationAngle = 0.0;
		dPtnRotationAngle2 = 0.0;

		// sjyi 2023.06.15 Weld Type Division 관련
		nDivStartSlot = 1;
		nDivJumpOffset = 1;

		// dhpark 2023.07.13 2D Offset 추가
		b3DUse = TRUE;

		// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전기능 사용여부 추가
		bReversal = FALSE;
	};

}IWS_HAIRPIN_INFO_V109;

typedef struct _IWS_HAIRPIN_INFO_V108 {
	int nSlot;
	int nDrawSlot;
	int nLayer;
	double startAng;
	double HpSize;
	double HpSizeW;
	double HpSize2nd;
	double LayerDia[MAX_HAIRPIN_LAYER];
	double HpOverLen;		// 2차패턴 over length

	int nCntP1;				// 1차패턴 Count
	int nPenP1;				// 1차패턴 Pen
	int nWeldShapeP1;		// 1차패턴 Shape
	int nWeldZigZagFreqP1;  // 1차패턴 Zigzag Freq

	int nPenP2;				// 2차패턴 Pen
	int nCntP2;				// 2차패턴 Count
	int nWeldShapeP2;		// 2차패턴 Shape
	int nWeldZigZagFreqP2;  // 2차패턴 Zigzag Freq

	int nSwitchWelding;
	int nWeldType;

	double dblPowerRatio;
	double dblSpeedRatio;

	// 2022-07-07 Matrix 관련 변수 추가--->
	//Layer 별 각도 Offset
	double dDegree12;
	double dDegree34;
	double dDegree56;
	double dDegree78;
	double dDegree90;
	// 2022-07-07 Matrix 관련 변수 추가<---

	IWS_HAIRPIN_OFFSET hpOffset[512];

	double m_dPenPower[10] = { 0 };
	double m_dPenSpeed[10] = { 0 };

	// mspark 2022.04.22
	BOOL bUseRptPowerProfile_1st;
	BOOL bUseRptPowerProfile_2nd;
	double dRptPowerProfile_1st[MAX_NUM_REPEAT_WELD] = { 1.0, };
	double dRptPowerProfile_2nd[MAX_NUM_REPEAT_WELD] = { 1.0, };

	// sjyi 2023.04.14 패턴 회전각
	double dPtnRotationAngle;
	double dPtnRotationAngle2;

	// sjyi 2023.06.15 Weld Type관련 값 추가
	int nDivStartSlot;
	int nDivJumpOffset;

	// dhpark 2023.07.13 2D Offset 추가
	BOOL b3DUse;
	IWS_HAIRPIN_OFFSET hpOffset2D[512];

	_IWS_HAIRPIN_INFO_V108() {
		nSlot = 96;
		nDrawSlot = 96;
		nLayer = MAX_HAIRPIN_LAYER;
		startAng = 0;
		HpSize = 0.7;
		LayerDia[0] = 170;
		LayerDia[1] = 180;
		LayerDia[2] = 190;
		LayerDia[3] = 200;
		LayerDia[4] = 210;
		LayerDia[5] = 220;
		LayerDia[6] = 230;
		LayerDia[7] = 240;
		LayerDia[8] = 250;
		LayerDia[9] = 260;
		HpOverLen = 0.50;

		nCntP1 = 1;
		nPenP1 = 1;
		nWeldShapeP1 = 0;
		nWeldZigZagFreqP1 = 3;

		nCntP2 = 1;
		nPenP2 = 1;
		nWeldShapeP2 = 0;
		nWeldZigZagFreqP2 = 3;

		HpSize2nd = 0.4;
		HpSizeW = 0.1;

		nSwitchWelding = 0;
		nWeldType = 0;

		dblPowerRatio = 100.0;
		dblSpeedRatio = 100.0;

		// 2022-07-07 Matrix 관련 변수 추가--->
		dDegree12 = 0.0f;
		dDegree34 = 0.0f;
		dDegree56 = 0.0f;
		dDegree78 = 0.0f;
		dDegree90 = 0.0f;
		// 2022-07-07 Matrix 관련 변수 추가<---

		bUseRptPowerProfile_1st = FALSE;
		bUseRptPowerProfile_2nd = FALSE;

		ZeroMemory(dRptPowerProfile_1st, MAX_NUM_REPEAT_WELD);
		ZeroMemory(dRptPowerProfile_2nd, MAX_NUM_REPEAT_WELD);

		// sjyi 2023.04.14 회전각
		dPtnRotationAngle = 0.0;
		dPtnRotationAngle2 = 0.0;

		// sjyi 2023.06.15 Weld Type Division 관련
		nDivStartSlot = 1;
		nDivJumpOffset = 1;

		// dhpark 2023.07.13 2D Offset 추가
		b3DUse = TRUE;
	};

}IWS_HAIRPIN_INFO_V108;

typedef struct _IWS_HAIRPIN_INFO_V106 {
	int nSlot;
	int nDrawSlot;
	int nLayer;
	double startAng;
	double HpSize;
	double HpSizeW;
	double HpSize2nd;
	double LayerDia[MAX_HAIRPIN_LAYER];
	double HpOverLen;		// 2차패턴 over length

	int nCntP1;				// 1차패턴 Count
	int nPenP1;				// 1차패턴 Pen
	int nWeldShapeP1;		// 1차패턴 Shape
	int nWeldZigZagFreqP1;  // 1차패턴 Zigzag Freq

	int nPenP2;				// 2차패턴 Pen
	int nCntP2;				// 2차패턴 Count
	int nWeldShapeP2;		// 2차패턴 Shape
	int nWeldZigZagFreqP2;  // 2차패턴 Zigzag Freq

	int nSwitchWelding;
	int nWeldType;

	double dblPowerRatio;
	double dblSpeedRatio;

	// 2022-07-07 Matrix 관련 변수 추가--->
	//Layer 별 각도 Offset
	double dDegree12;
	double dDegree34;
	double dDegree56;
	double dDegree78;
	double dDegree90;
	// 2022-07-07 Matrix 관련 변수 추가<---

	IWS_HAIRPIN_OFFSET hpOffset[512];

	double m_dPenPower[10] = { 0 };
	double m_dPenSpeed[10] = { 0 };

	// mspark 2022.04.22
	BOOL bUseRptPowerProfile_1st;
	BOOL bUseRptPowerProfile_2nd;
	double dRptPowerProfile_1st[MAX_NUM_REPEAT_WELD] = { 1.0, };
	double dRptPowerProfile_2nd[MAX_NUM_REPEAT_WELD] = { 1.0, };

	// sjyi 2023.04.14 패턴 회전각
	double dPtnRotationAngle;
	double dPtnRotationAngle2;

	// sjyi 2023.06.15 Weld Type관련 값 추가
	int nDivStartSlot;
	int nDivJumpOffset;

	_IWS_HAIRPIN_INFO_V106() {
		nSlot = 96;
		nDrawSlot = 96;
		nLayer = 8;
		startAng = 0;
		HpSize = 0.7;
		LayerDia[0] = 170;
		LayerDia[1] = 180;
		LayerDia[2] = 190;
		LayerDia[3] = 200;
		LayerDia[4] = 210;
		LayerDia[5] = 220;
		LayerDia[6] = 230;
		LayerDia[7] = 240;
		HpOverLen = 0.50;

		nCntP1 = 1;
		nPenP1 = 1;
		nWeldShapeP1 = 0;
		nWeldZigZagFreqP1 = 3;

		nCntP2 = 1;
		nPenP2 = 1;
		nWeldShapeP2 = 0;
		nWeldZigZagFreqP2 = 3;

		HpSize2nd = 0.4;
		HpSizeW = 0.1;

		nSwitchWelding = 0;
		nWeldType = 0;

		dblPowerRatio = 100.0;
		dblSpeedRatio = 100.0;

		// 2022-07-07 Matrix 관련 변수 추가--->
		dDegree12 = 0.0f;
		dDegree34 = 0.0f;
		dDegree56 = 0.0f;
		dDegree78 = 0.0f;
		dDegree90 = 0.0f;
		// 2022-07-07 Matrix 관련 변수 추가<---

		bUseRptPowerProfile_1st = FALSE;
		bUseRptPowerProfile_2nd = FALSE;

		ZeroMemory(dRptPowerProfile_1st, MAX_NUM_REPEAT_WELD);
		ZeroMemory(dRptPowerProfile_2nd, MAX_NUM_REPEAT_WELD);

		// sjyi 2023.04.14 회전각
		dPtnRotationAngle = 0.0;
		dPtnRotationAngle2 = 0.0;

		// sjyi 2023.06.15 Weld Type Division 관련
		nDivStartSlot = 1;
		nDivJumpOffset = 1;

	};

}IWS_HAIRPIN_INFO_V106;

typedef struct _IWS_HAIRPIN_INFO_V105 {
	int nSlot;
	int nDrawSlot;
	int nLayer;
	double startAng;
	double HpSize;
	double HpSizeW;
	double HpSize2nd;
	double LayerDia[MAX_HAIRPIN_LAYER];
	double HpOverLen;		// 2차패턴 over length

	int nCntP1;				// 1차패턴 Count
	int nPenP1;				// 1차패턴 Pen
	int nWeldShapeP1;		// 1차패턴 Shape
	int nWeldZigZagFreqP1;  // 1차패턴 Zigzag Freq
	
	int nPenP2;				// 2차패턴 Pen
	int nCntP2;				// 2차패턴 Count
	int nWeldShapeP2;		// 2차패턴 Shape
	int nWeldZigZagFreqP2;  // 2차패턴 Zigzag Freq

	int nSwitchWelding;
	int nWeldType;

	double dblPowerRatio;
	double dblSpeedRatio;

	// 2022-07-07 Matrix 관련 변수 추가--->
	//Layer 별 각도 Offset
	double dDegree12;
	double dDegree34;
	double dDegree56;
	double dDegree78;
	double dDegree90;
	// 2022-07-07 Matrix 관련 변수 추가<---

	IWS_HAIRPIN_OFFSET hpOffset[512];

	double m_dPenPower[10] = { 0 };
	double m_dPenSpeed[10] = { 0 };

	// mspark 2022.04.22
	BOOL bUseRptPowerProfile_1st;
	BOOL bUseRptPowerProfile_2nd;
	double dRptPowerProfile_1st[MAX_NUM_REPEAT_WELD] = { 1.0, };
	double dRptPowerProfile_2nd[MAX_NUM_REPEAT_WELD] = { 1.0, };

	// sjyi 2023.04.14 패턴 회전각
	double dPtnRotationAngle;
	double dPtnRotationAngle2;

	_IWS_HAIRPIN_INFO_V105() {
		nSlot = 96;
		nDrawSlot = 96;
		nLayer = MAX_HAIRPIN_LAYER;
		startAng = 0;
		HpSize = 0.7;
		LayerDia[0] = 170;
		LayerDia[1] = 180;
		LayerDia[2] = 190;
		LayerDia[3] = 200;
		LayerDia[4] = 210;
		LayerDia[5] = 220;
		LayerDia[6] = 230;
		LayerDia[7] = 240;
		LayerDia[8] = 250;
		LayerDia[9] = 260;
		HpOverLen = 0.50;

		nCntP1 = 1;
		nPenP1 = 1;
		nWeldShapeP1 = 0;
		nWeldZigZagFreqP1 = 3;

		nCntP2 = 1;
		nPenP2 = 1;
		nWeldShapeP2 = 0;
		nWeldZigZagFreqP2 = 3;

		HpSize2nd = 0.4;
		HpSizeW = 0.1;

		nSwitchWelding = 0;
		nWeldType = 0;

		dblPowerRatio = 100.0;
		dblSpeedRatio = 100.0;

		// 2022-07-07 Matrix 관련 변수 추가--->
		dDegree12 = 0.0f;
		dDegree34 = 0.0f;
		dDegree56 = 0.0f;
		dDegree78 = 0.0f;
		dDegree90 = 0.0f;
		// 2022-07-07 Matrix 관련 변수 추가<---

		bUseRptPowerProfile_1st = FALSE;
		bUseRptPowerProfile_2nd = FALSE;

		ZeroMemory(dRptPowerProfile_1st, MAX_NUM_REPEAT_WELD);
		ZeroMemory(dRptPowerProfile_2nd, MAX_NUM_REPEAT_WELD);

		// sjyi 2023.04.14 회전각
		dPtnRotationAngle = 0.0;
		dPtnRotationAngle2 = 0.0;
	};

}IWS_HAIRPIN_INFO_V105;

typedef struct _IWS_HAIRPIN_INFO_V104 {
	int nSlot;
	int nDrawSlot;
	int nLayer;
	double startAng;
	double HpSize;
	double HpSizeW;
	double HpSize2nd;
	double LayerDia[MAX_HAIRPIN_LAYER];
	double HpOverLen;		// 2차패턴 over length

	int nCntP1;				// 1차패턴 Count
	int nPenP1;				// 1차패턴 Pen
	int nWeldShapeP1;		// 1차패턴 Shape
	int nWeldZigZagFreqP1;  // 1차패턴 Zigzag Freq

	int nPenP2;				// 2차패턴 Pen
	int nCntP2;				// 2차패턴 Count
	int nWeldShapeP2;		// 2차패턴 Shape
	int nWeldZigZagFreqP2;  // 2차패턴 Zigzag Freq

	int nSwitchWelding;
	int nWeldType;

	double dblPowerRatio;
	double dblSpeedRatio;

	// 2022-07-07 Matrix 관련 변수 추가--->
	//Layer 별 각도 Offset
	double dDegree12;
	double dDegree34;
	double dDegree56;
	double dDegree78;
	double dDegree90;
	// 2022-07-07 Matrix 관련 변수 추가<---

	IWS_HAIRPIN_OFFSET hpOffset[512];

	double m_dPenPower[10] = { 0 };
	double m_dPenSpeed[10] = { 0 };

	// mspark 2022.04.22
	BOOL bUseRptPowerProfile_1st;
	BOOL bUseRptPowerProfile_2nd;
	double dRptPowerProfile_1st[MAX_NUM_REPEAT_WELD] = { 1.0, };
	double dRptPowerProfile_2nd[MAX_NUM_REPEAT_WELD] = { 1.0, };

	_IWS_HAIRPIN_INFO_V104() {
		nSlot = 96;
		nDrawSlot = 96;
		nLayer = MAX_HAIRPIN_LAYER;
		startAng = 0;
		HpSize = 0.7;
		LayerDia[0] = 170;
		LayerDia[1] = 180;
		LayerDia[2] = 190;
		LayerDia[3] = 200;
		LayerDia[4] = 210;
		LayerDia[5] = 220;
		LayerDia[6] = 230;
		LayerDia[7] = 240;
		LayerDia[8] = 250;
		LayerDia[9] = 260;
		HpOverLen = 0.50;

		nCntP1 = 1;
		nPenP1 = 1;
		nWeldShapeP1 = 0;
		nWeldZigZagFreqP1 = 3;

		nCntP2 = 1;
		nPenP2 = 1;
		nWeldShapeP2 = 0;
		nWeldZigZagFreqP2 = 3;

		HpSize2nd = 0.4;
		HpSizeW = 0.1;

		nSwitchWelding = 0;
		nWeldType = 0;

		dblPowerRatio = 100.0;
		dblSpeedRatio = 100.0;

		// 2022-07-07 Matrix 관련 변수 추가--->
		dDegree12 = 0.0f;
		dDegree34 = 0.0f;
		dDegree56 = 0.0f;
		dDegree78 = 0.0f;
		dDegree90 = 0.0f;
		// 2022-07-07 Matrix 관련 변수 추가<---

		bUseRptPowerProfile_1st = FALSE;
		bUseRptPowerProfile_2nd = FALSE;

		ZeroMemory(dRptPowerProfile_1st, MAX_NUM_REPEAT_WELD);
		ZeroMemory(dRptPowerProfile_2nd, MAX_NUM_REPEAT_WELD);
	};

}IWS_HAIRPIN_INFO_V104;

typedef struct _IWS_HAIRPIN_INFO_V103 {
	int nSlot;
	int nDrawSlot;
	int nLayer;
	double startAng;
	double HpSize;
	double HpSizeW;
	double HpSize2nd;
	double LayerDia[MAX_HAIRPIN_LAYER];
	double HpOverLen;		// 2nd over length

	int nCntP1;				// 1차패턴 Count
	int nPenP1;				// 1차패턴 Pen
	int nWeldShapeP1;		// 1차패턴 Shape
	int nWeldZigZagFreqP1;  // 1차패턴 Zigzag Freq

	int nPenP2;				// 2차패턴 Pen
	int nCntP2;				// 2차패턴 Count
	int nWeldShapeP2;		// 2차패턴 Shape
	int nWeldZigZagFreqP2;  // 2차패턴 Zigzag Freq

	int nSwitchWelding;
	int nWeldType;

	double dblPowerRatio;
	double dblSpeedRatio;

	IWS_HAIRPIN_OFFSET hpOffset[512];

	double m_dPenPower[10] = { 0 };
	double m_dPenSpeed[10] = { 0 };

	_IWS_HAIRPIN_INFO_V103() {
		nSlot = 96;
		nDrawSlot = 96;
		nLayer = MAX_HAIRPIN_LAYER;
		startAng = 0;
		HpSize = 0.7;
		LayerDia[0] = 170;
		LayerDia[1] = 180;
		LayerDia[2] = 190;
		LayerDia[3] = 200;
		LayerDia[4] = 210;
		LayerDia[5] = 220;
		LayerDia[6] = 230;
		LayerDia[7] = 240;
		LayerDia[8] = 250;
		LayerDia[9] = 260;
		HpOverLen = 0.50;

		nCntP1 = 1;
		nPenP1 = 1;
		nWeldShapeP1 = 0;
		nWeldZigZagFreqP1 = 3;

		nCntP2 = 1;
		nPenP2 = 1;
		nWeldShapeP2 = 0;
		nWeldZigZagFreqP2 = 3;

		HpSize2nd = 0.4;
		HpSizeW = 0.1;

		nSwitchWelding = 0;
		nWeldType = 0;

		dblPowerRatio = 100.0;
		dblSpeedRatio = 100.0;
	};

}IWS_HAIRPIN_INFO_V103;

enum 
{
	IPG_CW_YLR2000SM = 0,
	IPG_CW_YLR6000QCW,
	IPG_PULSE_YLR6000QCW,
	TRUMPF_TRUDISK
};

extern TCHAR g_dscLaserName[4][64];
extern COLORREF g_clrPen[16];

enum {
	IWS_PLC =0,
	IWS_LASER = 1,
	IWS_SCANNER,
	IWS_VSCAN,
	IWS_RECIPE,
	IWS_MODULE_END
};
typedef struct _IWS_STATUS
{
	UINT	scanDO;
	int		scanDI;

	WORD	plcRead;
	WORD	plcWrite;
	WORD	plcCmd;
	BOOL	bOk[IWS_MODULE_END];


	_IWS_STATUS() {
		scanDO = 0;
		scanDI = 0;
		plcRead = 0;
		plcWrite = 0;
		for (int i = 0; i < IWS_MODULE_END; i++) {
			bOk[i] = FALSE;
		}
	};

}IWS_STATUS;

//자동 좌표 보정 값 저장할 구조체
typedef struct _IWS_LOCATION
{
	double dX;
	double dY;
	double dZ;

	_IWS_LOCATION() {
		dX = 0.0f;
		dY = 0.0f;
		dZ = 0.0f;
	};

}IWS_LOCATION;

// sjyi 2022-06-02 Add 개별 반복에 대한 Power 조정 -->
typedef struct _IWS_HAIRPIN_REPEAT_INFO
{
	double dPower;

	_IWS_HAIRPIN_REPEAT_INFO() {
		dPower = 0.0f;
	};

}IWS_HAIRPIN_REPEAT_INFO;
// sjyi 2022-06-02 Add 개별 반복에 대한 Power 조정 <--

// 2022.08.15 jh.kim 변경 - Matrix 분할 ( 4분할 )_주석
// sjyi 2022-07-08 매트릭스 결과 저장을 위한 구조체 추가 -->
//typedef struct _IWS_MATRIX_RESULT
//{
//	double m_dCalibMtx_a11;
//	double m_dCalibMtx_a12;
//	double m_dCalibMtx_a13;
//	double m_dCalibMtx_a21;
//	double m_dCalibMtx_a22;
//	double m_dCalibMtx_a23;
//
//	_IWS_MATRIX_RESULT() {
//		m_dCalibMtx_a11	= 0.0;
//		m_dCalibMtx_a12	= 0.0;
//		m_dCalibMtx_a13	= 0.0;
//		m_dCalibMtx_a21	= 0.0;
//		m_dCalibMtx_a22	= 0.0;
//		m_dCalibMtx_a23	= 0.0;
//	};
//
//}IWS_MATRIX_RESULT;
// sjyi 2022-07-08 매트릭스 결과 저장을 위한 구조체 추가 <--

typedef std::vector<CJob*> IWS_VECTOR_JOB;
typedef std::vector<CJobGroup*> IWS_VECTOR_JOB_GROUP;
typedef std::vector<CLaserPen> IWS_LPEN;
typedef std::vector<int> vector_int;
typedef std::vector<double> vector_dbl;


class CMFCPropertyGridProperyDlb3 :public CMFCPropertyGridProperty
{
public:
	CMFCPropertyGridProperyDlb3(const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE):CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList)
	{		
	}

	// Simple property
	CMFCPropertyGridProperyDlb3(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL) :
		CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars){
	}
	
	virtual CString FormatProperty()
	{
		CString str;
		VARTYPE vtype = GetValue().vt;
		if (vtype == VT_R8 || vtype == VT_R4)
		{
			str.Format(_T("%.3f"),GetValue().dblVal); // Or whatever you want
		}
		else
		{
			str = CMFCPropertyGridProperty::FormatProperty();
		}

		return str;
	}
};

struct CorrParam {
	TCHAR	szName[MAX_PATH];
	TCHAR	szCorFile[MAX_PATH];
	TCHAR	szCcdFile[MAX_PATH];
	double  dblScaleFactor;
	double  dblKX;
	double  dblKY;
	double  dblOffsetX;
	double  dblOffsetY;
	double  dblTheta;
	double	dblWorkX, dblWorkY;
	double	dblImgCenX, dblImgCenY;
	double  dblImgScaleX, dblImgScaleY;
	double  dblLensPowerX;
	double  dblLensPowerY;
	double  dblVSsx, dblVSsy, dblVSex, dblVSey; // vision scan �������� ����
	BOOL	bUseCalib;				//DHChoi 20200624 : ���� ��� ���� ����
	int     iCalibX;				//DHChoi 20200624 : x���� ��� ��
	int     iCalibY;				//DHChoi 20200624 : y���� ��� ��
	int		iVSOverlap;				//DHChoi 20200624 : �������������� ����� Overlap�� (�ۼ�Ʈ)
	TCHAR	szCalibDir[MAX_PATH];	//DHChoi 20200624 : ���������� ����Ǿ��ִ� ���丮
	CorrParam() {
		szCorFile[0] = '\0';
		memset(szCcdFile, 0, sizeof(TCHAR)*MAX_PATH);
		memset(szName, 0, sizeof(TCHAR)*MAX_PATH);
		dblScaleFactor = 100;
		dblKX = dblKY = 1;
		dblOffsetX = dblOffsetY = 0;
		dblTheta = 0;
		dblWorkX = 0.0; dblWorkY = 0.0f;
		dblOffsetX = 0.0f;  dblImgCenY = 0.0f;
		dblImgScaleX = dblWorkX;
		dblImgScaleY = dblWorkY;
		dblLensPowerX = 1.0;
		dblLensPowerY = 1.0;
		bUseCalib = FALSE;
		iCalibX = 0;
		iCalibY = 0;
		iVSOverlap = 0;
		memset(szCalibDir, 0, sizeof(TCHAR)*MAX_PATH);
	};
	~CorrParam() {};
};


static BOOL PeekAndPump() {
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (GetMessage(&msg, NULL, NULL, NULL)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {  // if WM_QUIT
			return FALSE;
		}
	}
	return TRUE;
}

static void Pause(int ms)
{
	long time = clock();
	while (1) {
		if (clock() - time > ms)
			break;
		PeekAndPump();
	}
}
#pragma once
#include "IWSWork.h"

class CEntity;
class CIWSWorkHairpin :
	public CIWSWork
{
	typedef struct _HAIRPIN_DATA {
		double x;
		double y;
		double z;
	}HAIRPIN_DATA;
	typedef std::vector<HAIRPIN_DATA> HairPinData;
	typedef struct _lineEquation {
		double a, b, c;
		double start_x, start_y;         //The coordinate location of the start of the line.
		double end_x, end_y;           //The coordinate location of the end of the line.

	}LineEquation;
	typedef struct _circleEquation
	{
		double cx, cy, r;
	}CircleEquation;
	typedef std::vector<HANDLE> HairPinInfoEnt;
	enum {
		WORK_MODE_NONE=-1,
		WORK_MODE_VSCAN,
		WORK_MODE_INSPECTION,
		WORK_MODE_WELDING
	};
public:
	CIWSWorkHairpin();
	~CIWSWorkHairpin();
	void virtual clear();

	BOOL virtual IsWorkOn() {
		if (m_bRunVScan) return m_bRunVScan;
		return (m_pWorkThread ? TRUE : FALSE);
	}

 	void OffsetToCurrent();
 	void ModifyHairpinWeldRepeat(int nP1, int nP2, int st1 = 0, int nd2 = 0);
	void virtual SaveWork(CFile *pf, CIWSDoc *pDoc);
	BOOL virtual LoadWork(CFile *pf, CIWSDoc *pDoc);
	// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가
 	//void ModifyHairpin(int n1st = 0, int n2nd = 0, BOOL bUpdate = FALSE, double dRotateAngle = 0.0, double dRotationAngle2 = 0.0, BOOL bKeepRotation = FALSE);
	//void ModifyHairpin(int n1st = 0, int n2nd = 0, BOOL bUpdate = FALSE, double dRotateAngle = 0.0, double dRotationAngle2 = 0.0, BOOL bKeepRotation = FALSE, BOOL bReversal = FALSE);
	void ModifyHairpin(int n1st = 0, int n2nd = 0, BOOL bUpdate = FALSE, double dRotateAngle = 0.0, double dRotationAngle2 = 0.0, BOOL bKeepRotation = FALSE, BOOL bReversal = FALSE, BOOL bSlotChange = FALSE);

	void AlignHairpin(HANDLE handle = NULL);
	// sjyi 2023.04.13 헤어핀 회전 추가
	void RotateHairpin(CEntity* pEnt, double dRotateAngle);
	double Trunc360(double dDegree);


	int virtual DoWork();
	void virtual DoVScan();
	BOOL virtual DoInspection();
	static UINT StartVScanThread(LPVOID pParam);
	UINT StartVScanThreadBody();
	UINT StartInspectionBody();

	// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가
	//void GenHairPin(int nSlot, int nLayer, double startAng,
	//	double HpSize, double HpSizeW, double HpSize2nd,
	//	double *pLayerDia, int nCntP1, int nCntP2, int nPenP1, int nPenP2, int nModel, int nDrawSlot, double dOverLen,
	//	double dRotateAngle = 0.0
	//);

	//void GenHairPin(int nSlot, int nLayer, double startAng,
	//	double HpSize, double HpSizeW, double HpSize2nd,
	//	double *pLayerDia, int nCntP1, int nCntP2, int nPenP1, int nPenP2, int nModel, int nDrawSlot, double dOverLen,
	//	double dRotateAngle = 0.0, BOOL bReversal = FALSE
	//);
    // sjyi 2024.05.25 1차패턴 Spiral 추가, 2차패턴 모래시계, Sprial 추가
	void GenHairPin(int nSlot, int nLayer, double startAng,
		double HpSize, double HpSizeW, double HpSize2nd,
		double *pLayerDia, int nCntP1, int nCntP2, int nPenP1, int nPenP2, int nModel, int nDrawSlot, double dOverLen,
		double dRotateAngle = 0.0, BOOL bReversal = FALSE,
		double HpSize2nd_2 = 0.0, int nGenDir = GEN_CCW);



	HANDLE GenZigZagPatrn(HANDLE hBlock, double cx, double cy, double theta, double l, double w, int n);
	HANDLE GenPolygon(HANDLE hBlock, double cx, double cy, double dLength, double dWidth, double dtheta);
	HANDLE GenSandGlassPatrn(HANDLE hBlock, double cx, double cy, double dLength1, double dLength2, double dWidth1, double dWidth2, double dTheta, double dThetaCenter);

	void AlignEntityEllipse2nd(CEntity *pEnt, POINTF* points, int idx, int *penID, double dOverLen);
	//void AlignEntityEllipse1st(CEntity *pEnt, int *penID);
	void AlignEntityEllipse1st(CEntity *pEnt, int *penID, double dRotationAngle);

	void AlignEntityLine2nd(CEntity *pEnt, POINTF* points, int idx, int *penID, double dOverLen);
	//void AlignEntityLine1st(CEntity *pEnt, int *penID);
	void AlignEntityLine1st(CEntity *pEnt, int *penID, double dRotationAngle);

	void AlignEntityZigZag2nd(CEntity *pEnt, POINTF* points, int idx, int *penID, double d2ndWidth, double dOverLen);
	void AlignEntityZigZag2_2nd(CEntity* pEnt1st, CEntity *pEnt, int *penID, double dRotateAngle = 0.0); // sjyi 2022-08-19 2nd 패턴 zigzag2의 얼라인먼트를 위한 함수 수정

	// sjyi 2024.05.25 Octagon 추가
	//	void AlignEntityZigZag1st(CEntity *pEnt, int *penID, double dRotateAngle = 0.0);
	BOOL AlignEntityZigZag1st(CEntity *pEnt, int *penID, double dRotateAngle = 0.0);

	// sjyi 2024.05.25 2차 패턴 SandGlass 추가
	void AlignEntitySandGlass(CEntity *pEnt, POINTF* points, int idx, int *penID, double d2ndWidth, double dOverLen, double d2ndWidth2);

	// sjyi 2024.02.01 2차 패턴 Sprial 추가
	void AlignEntitySprial2nd(CEntity *pEnt, POINTF* points, int idx, int *penID);
	//void AlignEntitySpiral1st(CEntity *pEnt, int *penID);
	void AlignEntitySpiral1st(CEntity *pEnt, int *penID, double dRotationAngle);
	void GetHairpinCenter(double &cx, double &cy);

	void CalcHairpinInfo(); // 헤어핀의 레이어 중심 및 슬롯각을 피팅 하여서 계산한다. 
	void NewCalcHairpinInfo();
	void UpdateHairpinInfoLayer();

	BOOL GetSelectedJob();

	void ReadHairpinInfoV103(CFile * pf);
	void ReadHairpinZCompV103(CFile* pf);

	void ReadHairpinInfoV104(CFile * pf);

	void ReadHairpinInfoV105(CFile* pf);
	void ReadHairpinInfoV106(CFile* pf); // sjyi 2023.09.12 추가(아래 dhpark 내용)
	void ReadHairpinInfoV108(CFile* pf); // sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가로 파일 VersionUp
	void ReadHairpinInfoV109(CFile* pf); // sjyi 2024.05.25 Sortie 반복, 스파이럴 패턴, 모래시계 패턴 추가로 인해, Version Up
	void ReadHairpinInfoV110(CFile* pf); // sjyi 2024.10.17 Sortie 반복 맵 추가로 인해, Version Up

	void SetDivParameters(int &nWeldType, int &nJumpOffset, int &nStartSlot);

	//에너지 전달률
	//nPattern : 1 = 1차 패턴, 2 = 2차 패턴
	//nShape = Ellipse = 0, Line = 1, Zigzag = 2
	double GetWeldingEnergy(int nPattern, int nShape, double nWidth, double nLength, int nFreq, int Repeat, double dPower, double dSpeed);// kWh

	IWS_HAIRPIN_INFO   m_hp;
	IWS_HAIRPIN_Z_COMP m_zComp;

	// dhpark 2023.06.26 2D Inspection 기능 추가로 인한 2D 검사 레시피 경로 변수 추가
	int m_n2DPathLength;
	CString m_str2DRecipePath;
	BOOL m_b2DInspection;

	void SetCadEventEnable(BOOL bEnable);


public:
	CIWSWorkHairpin::LineEquation *m_pFitLine;
	CIWSWorkHairpin::CircleEquation *m_pFitCircle;
	CIWSWorkHairpin::HairPinInfoEnt m_entInfo;

	BOOL m_bRunVScan;
	int  m_nRefImgPixW;
	int  m_nRefImgPixH;
	float m_fRefImgSizeW;
	float m_fRefImgSizeH;

	int  m_nModel;	
protected:
	int m_iWorkMode;
};


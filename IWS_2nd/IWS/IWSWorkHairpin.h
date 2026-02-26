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
	//sjyi 2024.03.30 헤어핀 회전 관련 추가
// 	void ModifyHairpin(int st1 = 0, int nd2 = 0, BOOL bUpdate = FALSE);
	void ModifyHairpin(int n1st = 0, int n2nd = 0, BOOL bUpdate = FALSE, double dRotateAngle = 0.0, double dRotationAngle2 = 0.0, BOOL bKeepRotation = FALSE, BOOL bReversal = FALSE);
	void AlignHairpin(HANDLE handle=NULL);
	

	int virtual DoWork();
	void virtual DoVScan();
	BOOL virtual DoInspection();
	static UINT StartVScanThread(LPVOID pParam);
	UINT StartVScanThreadBody();
	UINT StartInspectionBody();

	void GenHairPin(int nSlot, int nLayer, double startAng,
		double HpSize, double HpSizeW, double HpSize2nd,
		double *pLayerDia, int nCntP1, int nCntP2, int nPenP1, int nPenP2, int nModel, int nDrawSlot, double dOverLen,
		double dRotateAngle = 0.0, BOOL bReversal = FALSE);
	HANDLE GenZigZagPatrn(HANDLE hBlock, double cx, double cy, double theta, double l, double w, int n);
	HANDLE GenPolygon(HANDLE hBlock, double cx, double cy, double dLength, double dWidth, double dtheta);

	void AlignEntityEllipse2nd(CEntity *pEnt, POINTF* points, int idx, int *penID, double dOverLen);
	void AlignEntityEllipse1st(CEntity *pEnt, int *penID);

	void AlignEntityLine2nd(CEntity *pEnt, POINTF* points, int idx, int *penID, double dOverLen);
	void AlignEntityLine1st(CEntity *pEnt, int *penID);

	void AlignEntityZigZag2nd(CEntity *pEnt, POINTF* points, int idx, int *penID, double d2ndWidth, double dOverLen);
	void AlignEntityZigZag2_2nd(CEntity* pEnt1st, CEntity *pEnt, int *penID, double dRotateAngle = 0.0); // sjyi 2022-08-19 2nd 패턴 zigzag2의 얼라인먼트를 위한 함수 수정
	void AlignEntityZigZag1st(CEntity *pEnt, int *penID, double dRotateAngle = 0.0);
	void GetHairpinCenter(double &cx, double &cy);

	void CalcHairpinInfo(); // 헤어핀의 레이어 중심 및 슬롯각을 피팅 하여서 계산한다. 
	void NewCalcHairpinInfo();
	void UpdateHairpinInfoLayer();

	BOOL GetSelectedJob();

	void ReadHairpinInfoV103(CFile * pf);
	void ReadHairpinZCompV103(CFile* pf);

	// sjyi 2024.03.30 헤어핀 회전 추가
	void ReadHairpinInfoV104(CFile * pf);
	void RotateHairpin(CEntity* pEnt, double dRotateAngle);
	double Trunc360(double dDegree);


	//에너지 전달률
	//nPattern : 1 = 1차 패턴, 2 = 2차 패턴
	//nShape = Ellipse = 0, Line = 1, Zigzag = 2
	double GetWeldingEnergy(int nPattern, int nShape, double nWidth, double nLength, int nFreq, int Repeat, double dPower, double dSpeed);// kWh

	IWS_HAIRPIN_INFO   m_hp;
	IWS_HAIRPIN_Z_COMP m_zComp;
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


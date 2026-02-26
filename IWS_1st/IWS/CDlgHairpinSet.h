#pragma once


// CDlgHairpinSet 대화 상자
#include "util/AutoEdit.h"

class CDlgHairpinSet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHairpinSet)

public:

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HAIPPIN_SET };
#endif

protected:
	/*CAutoEditDbl m_edit12degree;
	CAutoEditDbl m_edit34degree;
	CAutoEditDbl m_edit56degree;
	CAutoEditDbl m_edit78degree;*/

	CAutoEditDbl m_editZThreshold;
	CAutoEditInt m_editZnCnt;
	CAutoEditDbl m_editZPwr;

	CAutoEditDbl m_editZMin[10];
	CAutoEditDbl m_editZMax[10];
	CAutoEditInt m_editZCnt[10];

	CAutoEditDbl m_editPowerRatio;
	CAutoEditDbl m_editSpeedRatio;

	CAutoEditDbl m_editLegendAng;
	CAutoEditDbl m_editLegendDia;
	CAutoEditDbl m_editLegendDir;

	CAutoEditDbl m_editRcpScale[6];
	CAutoEditDbl m_editRcpAngle[6];
	CAutoEditDbl m_editRcpOffsetX[6];
	CAutoEditDbl m_editRcpOffsetY[6];

	CAutoEditDbl m_editHCompWD;
	CAutoEditDbl m_editHCompCenX;
	CAutoEditDbl m_editHCompCenY;
	CAutoEditDbl m_editHCompRef;
	CAutoEditDbl m_editHCompTeleAngle;
	CAutoEditDbl m_editHCompWorkRadius;


	CAutoEditInt m_editSortieCnt;	// sjyi 2022-08-28 Sortie Repeat Count

	CAutoEditDbl m_editNGDJigA;
	CAutoEditDbl m_editNGDJigB;

	CAutoEditInt m_editMatrixDivCnt;	// sjyi 2024.10.20 매트릭스 분할 개수 추가

public:
	/*double m_dDegree12;
	double m_dDegree34;
	double m_dDegree56;
	double m_dDegree78;*/

	BOOL   m_bZCompUse;
	double m_dblZThreshold;
	int    m_nZCtn;
	double m_dblZPwr;// mspark 2022.04.20
	IWS_HAIRPIN_Z m_zComp[10];

	double m_dPowerRatio;
	double m_dSpeedRatio;

	int m_nSwitchWelding;
	int m_nWeldType;

	double m_dLegendAng;
	double m_dLegendDia;
	double m_dLegendDir;

	double m_dRcpScale[6] = { 0 };
	double m_dRcpAngle[6] = { 0 };
	double m_dRcpOffsetX[6] = { 0 };
	double m_dRcpOffsetY[6] = { 0 };

	// 2023.07.12 dhpark 2D
	BOOL m_b3DUse;
	double m_d2DRcpScale[6] = { 0 };
	double m_d2DRcpAngle[6] = { 0 };
	double m_d2DRcpOffsetX[6] = { 0 };
	double m_d2DRcpOffsetY[6] = { 0 };


	double m_dHCompWD;
	double m_dHCompRef[4];
	double m_dHCompCenX;
	double m_dHCompCenY;
	double m_dHCompTCMaxAngle;
	double m_dHCompWorkRadius;

	int m_nZAxisCompMove;

	BOOL m_bSaveHResut;
	int m_nDivStartSlot;
	
	int m_nDivOffset;
	int m_nSlotCnt;

	int m_nModel;
	int m_nJig;

	BOOL m_bEnable;

	BOOL m_bSortieEnable;	// sjyi 2022-08-28 Sortie Repeat Enable Flag
	int m_nSortieDir;		// sjyi 2024.05.27 Sorite 반복 방향(0 : One-way, 1: Bi-direction)
	int m_nSortieCnt;		// sjyi 2024.05.27 Sortie 반복 횟수

	BOOL m_bUseAllwaysScanJigA;		// sjyi 2024.05.28 항상 스캔 사용 유무(JigA)
	BOOL m_bUseAllwaysScanJigB;		// sjyi 2024.05.28 항상 스캔 사용 유무(JigB)
	BOOL m_bUseAllwaysScanReweldA;	// sjyi 2024.05.28 항상 스캔 사용 유무(Reweld JigA)
	BOOL m_bUseAllwaysScanReweldB;	// sjyi 2024.05.28 항상 스캔 사용 유무(Reweld JigB)
	BOOL m_bUseAllwaysScanWeldA;	// sjyi 2024.05.28 항상 스캔 사용 유무(용접후 JigA)
	BOOL m_bUseAllwaysScanWeldB;	// sjyi 2024.05.28 항상 스캔 사용 유무(용접후 JigB)

	BOOL m_bUseSortieRepeat;	// sjyi 2024.05.28 Sortie 반복 사용 유무

	BOOL m_bUseNGChkJigA;
	BOOL m_bUseNGChkJigB;
	double m_dNGDistJigA;
	double m_dNGDistJigB;
	CString m_strPosPathJigA;
	CString m_strPosPathJigB;

	int m_nMatrixDivCnt;		// sjyi 2024.10.20 매트릭스 분할 개수 추가

	int m_nSortieRptMap[MAX_NUM_REPEAT_WELD];	// sjyi 2025.09.10 Sortie Rpt Map 추가

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadioZaxisComp();
	afx_msg void OnBnClickedCheckUseNgcheck();
	afx_msg void OnCbnSelchangeComboModel();
	afx_msg void OnCbnSelchangeComboJig();
	afx_msg void OnBnClickedBtnSetHref();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClickedRadioRcp3d();
	afx_msg void OnClickedRadioRcp2d();
	afx_msg void OnBnClickedButtonRcpEnable();
	afx_msg void OnClickedCheckSortie();
	afx_msg void OnKillfocusEditSotieCnt();
	afx_msg void OnKillfocusEditMatrixDivCnt();

	void CheckDivParams();
	void CheckZAxisCompMove();
//	void CheckZAxisCompNGCheck();
	void SetHCompRefHeight();
	void CheckSortieUI();

public:
	CDlgHairpinSet(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgHairpinSet();
	afx_msg void OnBnClickedBtnPosPathJiga();
	afx_msg void OnBnClickedBtnPosPathJigb();
	afx_msg void OnBnClickedBtnSortieMap();
};

#pragma once


// CDlgHairpinSet 대화 상자
#include "util/AutoEdit.h"

class CDlgHairpinSet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHairpinSet)

public:
	CDlgHairpinSet(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgHairpinSet();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HAIPPIN_SET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

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

	/*double m_dDegree12;
	double m_dDegree34;
	double m_dDegree56;
	double m_dDegree78;*/

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
	
	/*CAutoEditDbl m_edit12degree;
	CAutoEditDbl m_edit34degree;
	CAutoEditDbl m_edit56degree;
	CAutoEditDbl m_edit78degree;*/

	int m_nDivStartSlot;
	int m_nDivOffset;

	BOOL m_bUseAllwaysScanJigA;		// sjyi 2024.05.28 항상 스캔 사용 유무(JigA)
	BOOL m_bUseAllwaysScanJigB;		// sjyi 2024.05.28 항상 스캔 사용 유무(JigB)
	BOOL m_bUseAllwaysScanReweldA;	// sjyi 2024.05.28 항상 스캔 사용 유무(Reweld JigA)
	BOOL m_bUseAllwaysScanReweldB;	// sjyi 2024.05.28 항상 스캔 사용 유무(Reweld JigB)
	BOOL m_bUseAllwaysScanWeldA;	// sjyi 2024.05.28 항상 스캔 사용 유무(용접후 JigA)
	BOOL m_bUseAllwaysScanWeldB;	// sjyi 2024.05.28 항상 스캔 사용 유무(용접후 JigB)

	BOOL m_bUseSortieRepeat;	// sjyi 2024.05.28 Sortie 반복 사용 유무
	BOOL m_bSortieEnable;	// sjyi 2022-08-28 Sortie Repeat Enable Flag
	int m_nSortieDir;		// sjyi 2024.05.27 Sorite 반복 방향(0 : One-way, 1: Bi-direction)
	int m_nSortieCnt;		// sjyi 2024.05.27 Sortie 반복 횟수
	CAutoEditInt m_editSortieCnt;	// sjyi 2022-08-28 Sortie Repeat Count


};

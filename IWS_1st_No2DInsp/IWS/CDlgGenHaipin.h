#pragma once
#include "CGridLayerDia.h"
#include "util/AutoEdit.h"
// CDlgGenHaipin 대화 상자
class CDlgGenHaipin : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgGenHaipin)

public:
	CDlgGenHaipin(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgGenHaipin();

	void SetNumberOfLayer(int iLayer);
	int m_nSlot;
	int m_nDrawSlot;
	int m_nLayer;
	double m_dStartAng;
	double m_dHairpinSize;
	double m_dHairpinSizeW;
	double m_d2ndWeldSizeW;

	int m_nWeldRepeatP1;
	int m_nWeldPenP1;
	int m_nWeldShapeP1;
	int m_nWeldZigZagFreqP1;

	int m_nWeldRepeatP2;
	int m_nWeldPenP2;
	int m_nWeldShapeP2;
	int m_nWeldZigZagFreqP2;
	double m_dOverLen;

	int m_nModel;

	double m_dLayerDia[MAX_HAIRPIN_LAYER];

	double m_dPtnRotationAng;	// sjyi 2023.04.14 헤어핀 패턴 회전 각도

	BOOL m_bUseReversal; // sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HAIRPIN_GEN };
#endif
	CGridLayerDia m_gridLayerDiameter;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEditNumLayer();
	afx_msg void OnBnClickedRadio2ND(UINT uiD);	// sjyi 2022-05-19 2nd 패턴 확인용 함수 
	afx_msg void OnBnClickedRadio1ST(UINT uiD);	// sjyi 2024.05.25 1st 패턴 확인용 함수 
	//void UpdateStatic2nd(); // sjyi 2022-05-19 2nd 패턴에 따라, STATIC 텍스트 변경 

	void UpdateUI_P2(); // sjyi 2024.02.01 2nd 패턴에 따라, STATIC 텍스트 변경 
	void UpdateUI_P1(); // sjyi 2024.02.01 1st 패턴에 따라, STATIC 텍스트 변경 

	CAutoEditDbl m_editHairpinSize;
	CAutoEditDbl m_editHairpinStartAngle;
	CAutoEditInt m_editNumSlot;
	CAutoEditInt m_editDrawSlot;
	CAutoEditInt m_editNumLayer;	
	CAutoEditDbl m_editHairpinSizeW;
	CAutoEditDbl m_edit2ndweldSizeW;
	CAutoEditDbl m_edit2ndweldSizeW2;

	CAutoEditInt m_editWeldRepeatP1;
	CAutoEditInt m_editWeldPenP1;
	CAutoEditInt m_editWeldZigZagFreqP1;
	
	CAutoEditInt m_editWeldRepeatP2;
	CAutoEditInt m_editWeldPenP2;
	CAutoEditInt m_editWeldZigZagFreqP2;
	CAutoEditDbl m_editOverLen;
	CAutoEditDbl m_editPtnRotationAng; // sjyi 2023.04.14 헤어핀 패턴 회전 각도

	// sjyi 2024.05.25 Sprial 패턴 항목 추가
	BOOL m_bEnableOutToInP1;
	BOOL m_bEnableOutToInP2;
	BOOL m_bCloseSpiralP1;
	BOOL m_bCloseSpiralP2;

	// sjyi 2024.02.13 Sortie Repeat 내용 추가(헤어핀 설정 -> 헤어핀 생성 및 수정으로 위치 변경)
	BOOL m_bUseSortieRpt;
	CAutoEditInt m_editSortieCnt;
	int m_nSortieDir;
	int m_nSortieCnt;

	double m_d2ndWeldSizeW2;

	BOOL m_bUseSpiralPtn;
	BOOL m_bUseSandGlassPtn;
	BOOL m_bUseSortieRepeat;
};

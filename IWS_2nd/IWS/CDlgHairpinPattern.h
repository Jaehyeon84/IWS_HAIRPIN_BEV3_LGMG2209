#pragma once


// CDlgHairpinPattern 대화 상자
#include "util/AutoEdit.h"
#include "./ChartDirect/ChartViewer.h"
#include "UltimateGridLib/Include/UGCtrl.h"
#define  UWM_GRIDREPEATPOWERTBL_MSG _T("CGridJob")
#include "IWSDoc.h"

class CDlgHairpinPattern : public CDialogEx
{
	class CGridRepeatPowerTbl :public CUGCtrl {
	public:
		CGridRepeatPowerTbl();
		~CGridRepeatPowerTbl();

		virtual void OnSetup();
		virtual COLORREF OnGetDefBackColor(int section);
		virtual void OnCharDown(UINT *vcKey, BOOL processed);
		virtual int OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow);
		virtual int OnEditFinish(int col, long row, CWnd * edit, LPCTSTR string, BOOL cancelFlag);

		void SetMaxRow(int nRow);
		void SetChart(CChartViewer* pChart) { m_pChart = pChart; };
		void DrawChart();
		CChartViewer* m_pChart;
		double m_dPower[MAX_NUM_REPEAT_WELD];
		double m_dRepeat[MAX_NUM_REPEAT_WELD];
	};


	DECLARE_DYNAMIC(CDlgHairpinPattern)

public:
	CDlgHairpinPattern(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgHairpinPattern();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HAIRPIN_PATTERN };
#endif

protected:
	CChartViewer m_pChartProfile2nd;
	CChartViewer m_pChartProfile1st;
	CGridRepeatPowerTbl m_gridRptPower1st;
	CGridRepeatPowerTbl m_gridRptPower2nd;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	// sjyi 2022-05-19 ZigZag 패턴 체크 함수
	void Check2ndZigZag2();

	// sjyi 2022-05-20 에너지 계산을 위한 내부 변수 및 함수 추가 -->
	// 다이얼로그 내에서 값이 바뀔 경우, 다이얼로그 내에서만 계산 하고, OK 버튼을 눌러야 외부 출력이 변경되어야 함.
	// 이를 위해, 에디트 콘트롤 값을 가져와 저장하는 변수 추가
	// 본 변수 없이 직접 에디트 콘트롤 값을 가져와 계산해도 되나, 코딩이 보기 안좋음.
	double m_dHpSizeTemp;
	double m_dHpSizeWTemp;
	double m_dHpSize2ndWTemp;
	double m_dHpOverLenTemp;
	int m_iRptP1Temp;
	int m_iRptP2Temp;

	int m_nWeldPenP1Temp;
	int m_nWeldPenP2Temp;

	int m_nWeldZigZagFreqP1Temp;
	int m_nWeldZigZagFreqP2Temp;

	void UpdateEditCtrolTemp();
	void UpdateEnergyCalc();		// 에너지 계산 함수 추가
	// sjyi 2022-05-20 에너지 계산을 위한 내부 변수 및 함수 추가 <--

public:
	double m_dHpSize;
	double m_dHpSizeW;
	double m_dHpSize2ndW;
	double m_dHpOverLen;
	int m_iRptP1;
	int m_iRptP2;

	int m_n1ST;
	int m_n2ND;

	int m_nWeldPenP1;
	int m_nWeldPenP2;

	int m_nWeldShapeP1;
	int m_nWeldZigZagFreqP1;
	
	int m_nWeldShapeP2;
	int m_nWeldZigZagFreqP2;
	double m_dPower1st[MAX_NUM_REPEAT_WELD];
	double m_dPower2nd[MAX_NUM_REPEAT_WELD];
	BOOL m_bUseRptPowerProfile1st;
	BOOL m_bUseRptPowerProfile2nd;

	double m_dPtnRotation; // sjyi 2023.04.14 패턴 회전 속성 추가
	double m_dPtnRotation2; // sjyi 2023.04.14 패턴 회전 속성 추가 - 2차 패턴

	BOOL m_bUseReversal; // sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

public:
	CAutoEditDbl m_editPinSizeL;
	CAutoEditDbl m_editPinSizeW;
	CAutoEditDbl m_editPinSize2nd;
	CAutoEditInt m_editRepeatP1;
	CAutoEditInt m_editRepeatP2;

	CAutoEditInt m_editWeldPenP1;
	CAutoEditInt m_editWeldPenP2;
	CAutoEditDbl m_editOverLen;

	CAutoEditInt m_editWeldZigZagFreqP1;
	CAutoEditInt m_editWeldZigZagFreqP2;

	BOOL m_bUse1stPettern;
	BOOL m_bUse2ndPettern;

	CButton m_chkUseVariablePower1st;
	CButton m_chkUseVariablePower2nd;

	CAutoEditDbl m_edit1StWeldEnergy;
	CAutoEditDbl m_edit2ndWeldEnergy;
	CAutoEditDbl m_editTotalWeldEnergy;
	CAutoEditDbl m_editConfigWeldEnergy;

	CAutoEditDbl m_edit1StPower;
	CAutoEditDbl m_edit1StSpeed;
	CAutoEditDbl m_edit2ndPower;
	CAutoEditDbl m_edit2ndSpeed;

	CAutoEditDbl m_editPtnRotation; // sjyi 2023.04.14 패턴 회전 속성 추가
	CAutoEditDbl m_editPtnRotation2; // sjyi 2023.04.14 패턴 회전 속성 추가 2차 패턴

	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnKillfocusEditHpLength();
	afx_msg void OnEnKillfocusEditHpWidth();
	afx_msg void OnEnKillfocusEditHpRepeatP1();
	afx_msg void OnEnKillfocusEditWeldPenP1();
	afx_msg void OnEnKillfocusEditPreqP1();
	afx_msg void OnBnClickedRadio1ST(UINT uiD);
	afx_msg void OnBnClickedRadio2ND(UINT uiD);	
	afx_msg void OnBnClickedRadioLayerSelect1ST(UINT uiD);
	afx_msg void OnBnClickedRadioLayerSelect2ND(UINT uiD);
	afx_msg void OnEnKillfocusEditHp2ndWidth();
	afx_msg void OnEnKillfocusEditHpRepeatP2();
	afx_msg void OnEnKillfocusEditHpOverlen();
	afx_msg void OnEnKillfocusEditWeldPenP2();
	afx_msg void OnEnKillfocusEditPreqP2();		
	afx_msg void OnBnClickedCheckUse1stPettern();
	afx_msg void OnBnClickedCheckUse2ndPettern();
	afx_msg void OnBnClickedCheckUsePowerProfile1();
	afx_msg void OnBnClickedCheckUsePowerProfile2();
};

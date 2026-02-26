#pragma once
#include "BaseDialog.h"

// CDlgMatrix 대화 상자
class CChartViewer;
class CDlgMatrix : public CBaseDialog
{
	DECLARE_DYNAMIC(CDlgMatrix)

public:
	CDlgMatrix(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgMatrix();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MATRIX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CIWSDoc* m_pDoc;
	CChartViewer* m_pChart;
	int	m_nJigNo;		// sjyi 2022-07-08 JigA, JigB를 구분하기 위한 변수 추가(PLC Update를 통해 데이터 획득)
	
	//자동 좌표 보정 관련 행렬변수(Matrix)
	double m_dMatrix[6];
	double m_dMatrixDiv[MATRIX_DIVISION][6];
public:
	void DrawChart();

	//void StartOptimizer();
	void StartOptimizer(int inx);

	void PLCReadDataAdd(int nWordCnt);
	void IWSReadDataAdd();
	void FittedData();
	void FileSave();
	//double CalcError();
	double CalcError(int inx);

	void UpdatePlcJigNo();	// sjyi 2022-07-08 PLC로부터 JigNo 획득하기 위한 함수
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonPLCUpdate();
	afx_msg void OnBnClickedButtonIWSUpdate();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonCalc();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonSave();			
	CStatic m_stcPlcJig;
};

#pragma once

#include "BaseDialog.h"

#define PCC_POINTS 24

// CDlgMatrixPCC 대화 상자
class CChartViewer;
class CDlgMatrixPCC : public CBaseDialog
{
	DECLARE_DYNAMIC(CDlgMatrixPCC)

public:
	CDlgMatrixPCC(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgMatrixPCC();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MATRIX_PCC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CIWSDoc* m_pDoc;
	CChartViewer* m_pChart;
	int	m_nJigNo;
	
	//자동 좌표 보정 관련 행렬변수(Matrix)
	double m_dMatrix[6];
	CStatic m_stcPlcJig;

public:
	void DrawChart();
	void StartOptimizer();
	void PLCReadDataAdd(int nWordCnt, BOOL bPCC = FALSE);
	void FittedData();
	void FileSave();
	double CalcError();
	void UpdatePlcJigNo();

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonPLCUpdate();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonCalc();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonMatrixVscan();
	afx_msg void OnBnClickedButtonMatrixPcc();
};

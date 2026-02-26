#pragma once
#include "./util/AutoEdit.h"
#include "CChartPowerShape.h"
#include "CGridPulseShape.h"
#include "ColorComboBox.h"
// CDlgLaserPenCW 대화 상자

class CDlgLaserPenPulseShape : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLaserPenPulseShape)

public:
	CDlgLaserPenPulseShape(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgLaserPenPulseShape();
	CChartPowerShape *m_pChart;
	CGridPulseShape m_gridShape;
	IWS_LPEN *m_pPens;
	vector_point_dbl m_pulseShape;
	void UpdateChart(CLaserPen *pPen);
	void UpdateGrid(CLaserPen *pPen);
	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LASERPEN_PULSESHAPE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CAutoEditInt m_editPenID;
	CAutoEditDbl m_editLaserPower;
	CAutoEditDbl m_editFrequency;
	CAutoEditDbl m_editPulseWidth;
	CAutoEditDbl m_editJumpSpeed;
	CAutoEditDbl m_editWeldSpeed;
	CAutoEditInt m_editJumpDelay;
	CAutoEditInt m_editLaserOffDelay;
	CAutoEditInt m_editLaserOnDelay;
	CAutoEditInt m_editWeldDelay;
	CAutoEditInt m_editPolygonDelay;

	CColorComboBox m_cbLaserPens;
	afx_msg void OnCbnSelchangeComboboxexLaserpens();
	afx_msg void OnBnClickedMfcbuttonLaserpenApply();

	afx_msg void OnBnClickedCheckWobbel();
	CButton m_chkUseWobbel;
	CAutoEditDbl m_editWbFreq;
	CAutoEditDbl m_editWbLongitude;
	CAutoEditDbl m_editWbTranseverse;
protected:
	afx_msg LRESULT OnUwmCgridPowerShape(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedMfcbuttonAddRow();
	afx_msg void OnBnClickedMfcbuttonDelRow();
	afx_msg void OnBnClickedCheckPulseShape();
	CButton m_chkPulseShape;
	afx_msg void OnClickedCheckPulseShape();
//	afx_msg void OnEnChangeEditFrequency();
	CStatic m_stcPulseWidth;
	afx_msg void OnChangeEditPulsewidth();
};

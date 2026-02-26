#pragma once

#include "./util/AutoEdit.h"
#include "CChartPowerShape.h"
#include "CGridPowerShape.h"
#include "ColorComboBox.h"
#include <vector>
#include "LaserPen.h"
#include <atlconv.h>

// CDlgLaserPenCW 대화 상자

class CDlgLaserPenCW : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLaserPenCW)

public:
	CDlgLaserPenCW(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgLaserPenCW();
	CChartPowerShape *m_pChart;
	CGridPowerShape m_gridShape;
	IWS_LPEN *m_pPens;
	vector_point_dbl m_pwrShape;
	void UpdateChart(CLaserPen *pPen);
	void UpdateGrid(CLaserPen *pPen);
	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LASERPEN_CW };
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

	//22.05.03 에너지 전달률
	CAutoEditDbl m_edit1StLength;
	CAutoEditDbl m_edit1StWidth;
	CAutoEditInt m_edit1StRP;
	CAutoEditInt m_edit1StPen;
	CAutoEditDbl m_edit2ndWidth;
	CAutoEditInt m_edit2ndRP;
	CAutoEditDbl m_edit2ndLength;
	CAutoEditInt m_edit2ndPen;
	CAutoEditDbl m_edit1StEnergy;
	CAutoEditDbl m_edit2ndEnergy;
	CAutoEditDbl m_editTotalEnergy;
	CAutoEditDbl m_editConfigEnergy;
protected:
	afx_msg LRESULT OnUwmCgridPowerShape(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedMfcbuttonAddRow();
	afx_msg void OnBnClickedMfcbuttonDelRow();
	afx_msg void OnBnClickedCheckPowerShape();
	CButton m_chkPwrShape;

private:
	int m_nCopyPen;
	double m_dblCopyPower;
	double m_dblCopyFrequency;
	double m_dblCopyPulseWidth;
	double m_dblCopyJumpSpeed;
	double m_dblCopyMarkSpeed;
	int m_nCopyJumpDelay;
	int m_nCopyLaserOffDelay;
	int m_nCopyLaserOnDelay;
	int m_nCopyMarkDelay;
	int m_nCopyPolygonDelay;
public:
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnBnClickedButtonPaste();
	afx_msg void OnEnKillfocusEditWeldspeed();
	afx_msg void OnEnKillfocusEditJumpspeed();
	afx_msg void OnEnKillfocusEditLaserpower();
};

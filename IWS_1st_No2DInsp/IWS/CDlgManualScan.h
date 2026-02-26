#pragma once


// CDlgManualScan 대화 상자
#include "Scanner.h"
#include "./util/autoEdit.h"
class CIWSDoc;
class CDlgManualScan : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgManualScan)

public:
	CDlgManualScan(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgManualScan();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MANUAL_SCAN };
#endif
	void SetDocument(CIWSDoc *pDoc);
	
protected:
	CScanner *m_pScan;

	IWS_LPEN *m_pPens;
	CLaserPen m_pen;
	CLaserPen *m_pPenOld;


	CIWSDoc *m_pDoc;
	double CalcFrequency(double pulseWidth/*sec*/, double dutyCycle); // HZ;
	double CalcPulseWidth(double frequency/*Hz*/, double dutyCycle); // sec;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose() {
		if (m_pPenOld) {
			m_pScan->SetLaserPen(m_pPenOld);
			m_pScan->SetLaserPenParam();
		}
	};
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	CAutoEditDbl m_editFrequency;
	CAutoEditDbl m_editPulseWidth;
	CAutoEditDbl m_editPower;
	CAutoEditDbl m_editScanSpeed;
	CButton m_chkLaserOn;
	CAutoEditInt m_editLaserOnTime;
	CAutoEditDbl m_editGridSize;
	CAutoEditInt m_editGridCnt;
	CAutoEditDbl m_editMoveX;
	CAutoEditDbl m_editMoveY;
	afx_msg void OnBnClickedLaserEnable();
	CComboBox m_cbLaserPens;
	afx_msg void OnCbnSelchangeComboLaserpens();
	afx_msg void OnBnClickedScanMove();
	afx_msg void OnBnClickedButtionSavepen();
	afx_msg void OnBnClickedLaserEmission();
	afx_msg void OnBnClickedLaserOn();
	afx_msg void OnBnClickedGridMark();
	afx_msg void OnBnClickedDirMark();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CAutoEditDbl m_editRatioPwr;
	CAutoEditDbl m_editRatioSpd;
};

#pragma once
#include "BaseFormView.h"
#include "CDlgManualScan.h"
#include "HistoryEdit.h"
#include "./util/Led.h"
#include "CGridDIO.h"
#include "CGridPLC.h"
#include "./util/Label.h"
#include "./util/AutoEdit.h"

class CHomeView :
	public CBaseFormView
{
protected:
	
	DECLARE_DYNCREATE(CHomeView)
	CStringArray m_arrWorkList;
public:
	CHomeView() noexcept;
#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_FORMVIEW_HOME
	};
#endif
	CDlgManualScan *m_pDlgManualScan;
	CGridDIO m_gridDIO;
	CGridPLC m_gridPLC;
public:
	CIWSDoc* GetDocument() const;
public:
	virtual ~CHomeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void SetCurrentWorkFile(CString strWrk);
	void UpdateCycleTime();
	void RefreshWorklist();
	void AdjustLayoutUI();
	BOOL GetSubMonitorPos(RECT &rect);
	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ô´Ï´ï¿½.
public:
	void SetEnableUI(BOOL bEnable);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ï¿½ï¿½ï¿½ï¿½ï¿½Ô´Ï´ï¿½.
	virtual void OnInitialUpdate(); // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ Ã³ï¿½ï¿½ È£ï¿½ï¿½Ç¾ï¿½ï¿½ï¿½ï¿½Ï´ï¿?
	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Þ½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Ô¼ï¿½
protected:
	DECLARE_MESSAGE_MAP()

private:
	void UpdateButtonEnable(CCmdUI *pCmdUI);

public:
	afx_msg void OnButtonParamScanner();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnButtonManualScanner();
	afx_msg void OnDestroy();
	CLed m_ledLaserEmission;
	CLed m_ledLaserGuide;
	afx_msg void OnBnClickedButtonManualStart();
	afx_msg void OnBnClickedButtonOpenWork();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CHistoryEdit m_editLog;
	CLed m_lbStateLaser;
	CLed m_lbStateGuide;
	CLed m_lbStateSimulation;
	CLed m_lbStateDoWork;
	CLed m_lbStateLight;
	afx_msg void OnBnClickedButtonLaserSimulation();
	afx_msg void OnBnClickedButtonLaserGuide();
	afx_msg void OnBnClickedButtonLaserLaser();
	afx_msg void OnBnClickedButtonRefreshWorklist2();
	afx_msg void OnButtonCcdliive();
	LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);

	CLabel m_lbDIO;
	CLabel m_lbWorklist;
	CLabel m_lbSystemStatus;
	CLabel m_lbPLC;

	CComboBox m_cbWorkList;
	CMFCPropertyGridCtrl m_gridEntityParam;
	afx_msg void OnBnClickedButtonDetectStator();
	afx_msg void OnBnClickedButtonScanStator();
	afx_msg void OnBnClickedButtonResetDetection();
	CLed m_statusWorking;
	CLed m_statusPLC;
	CLed m_statusLaser;
	CLed m_statusScanner;
	CLed m_statusCCD;
	CLed m_statusInterface;

	afx_msg void OnBnClickedButtonClearLog();
	CStatic m_stcRcpNumber;
	CStatic m_stcRcpModel;
	CStatic m_stcLastJobNo;
	CStatic m_stcPlcJig;
	CStatic m_stcPlcWorkWeld;
	CStatic m_stcPlcWorkMode;

	CAutoEditDbl m_editDetectRange;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButtonOnline();
	afx_msg void OnBnClickedButtonOffline();

	// 2024.01.29 jh.kim º¯°æ - Draw È­¸é ÁÂÇ¥ÀÌµ¿À» À§ÇØ ¼öÁ¤
	void RibbonbarUseHaripin(BOOL bFlag);

	CLabel m_stcTactTime;
	afx_msg void OnBnClickedButtonLight();
	afx_msg void OnButtonScanRng();
	afx_msg void OnBnClickedButtonScanInspection();

	afx_msg void OnUpdateButtonLogInOut(CCmdUI *pCmdUI);
	afx_msg void OnButtonUserLogInOut();
	
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	//afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonSnap();
	afx_msg void OnUpdateButtonParamScanner(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonManualScanner(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonCcdliive(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonScanRng(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedRcpCheck();
	afx_msg void OnBnClickedButton5();
};

#ifndef _DEBUG  // MFCApplication6View.cppï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿?ï¿½ï¿½ï¿½ï¿½
inline CIWSDoc* CHomeView::GetDocument() const
{
	return reinterpret_cast<CIWSDoc*>(m_pDocument);
}
#endif
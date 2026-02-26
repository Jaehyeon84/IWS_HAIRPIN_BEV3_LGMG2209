#pragma once


// CDlgScanner 대화 상자
#include "Scanner.h"
#include "./util/AutoEdit.h"
class CIWSDoc;
class CDlgScanner : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgScanner)

public:
	CDlgScanner(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgScanner();
	void SetDocument(CIWSDoc *pDoc);
	void SetCorrParam(CorrParam *pParam) {
		m_pCorrParam = pParam;
	};

	void UpdateControl();
	void SetConfigPath(CString strCfgPath) { m_strConfigPath = strCfgPath; };
	BOOL IsChanged() { return m_bUpdate; };

	static BOOL m_bUseScaleFactor;
	static BOOL m_bUseScaleRatio;
	static BOOL m_bUseAutoScale;

	
	static double  m_dBaseSize;
	static double  m_dRealSizeX;
	static double  m_dRealSizeY;

	
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SCANNER };
#endif

protected:
	int m_iCurrentLens;
	CIWSDoc *m_pDoc;
	BOOL m_bUpdate;
	CString m_strConfigPath;
	CorrParam *m_pCorrParam;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CMFCEditBrowseCtrl m_editCorrFile;
	virtual BOOL OnInitDialog();
	// 스케일 계수
	CAutoEditDbl m_editScaleFactor;
	CAutoEditDbl m_editScaleKX;
	CAutoEditDbl m_editScaleKY;
	CAutoEditDbl m_editRotation;
	CAutoEditDbl m_editOffsetX;
	CAutoEditDbl m_editOffsetY;
	afx_msg void OnBnClickedScaleFactorChange();
	CButton m_chkUseScalFactor;
	CButton m_chkUseScaleRatio;
	afx_msg void OnBnClickedScaleRateChange();
	afx_msg void OnBnClickedBaseSizeChange();
	CAutoEditDbl m_editBaseSize;
	CAutoEditDbl m_editRealSizeX;
	CAutoEditDbl m_editRealSizeY;
	CButton m_chkUseAutoScale;
	afx_msg void OnBnClickedAutoChange();
	afx_msg void OnBnClickedCorrFileFind();
	afx_msg void OnBnClickedMfcbuttonScannerApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnSelchangeComboLensdb();
	CMFCEditBrowseCtrl m_editCCDCorrFile;
	CAutoEditDbl m_editWorkX;
	CAutoEditDbl m_editWorkY;
	CComboBox m_cbLensDB;
	CAutoEditDbl m_editCcdCalStep;
	CAutoEditDbl m_editLensPowerX;
	CAutoEditDbl m_editLensPowerY;

	CAutoEditDbl m_editImageScaleX;
	CAutoEditDbl m_editImageScaleY;
};

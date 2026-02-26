#pragma once


// CDlgVScanCalib 대화 상자
#include ".\VRSaux\WorkImg.h"
#include "CImageMerging.h"


class CDlgVScanCalib_Scan;
class CDlgVScanCalib_Learn;
class CDlgVScanCalib_Stiching;
class CIWSDoc;
class CDlgVScanCalib : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVScanCalib)

public:
	CDlgVScanCalib(CIWSDoc* m_pDoc, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgVScanCalib();

	friend CDlgVScanCalib_Scan;
	friend CDlgVScanCalib_Learn;
	friend CDlgVScanCalib_Stiching;

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_VSCANING_CALIB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	
	CIWSDoc* m_pDoc;

	BOOL m_bSetCenterPos;
	CImageMerging* m_pMergeCtrl;
	CDlgVScanCalib_Scan*		m_pDlgScan;
	CDlgVScanCalib_Learn*		m_pDlgLearning;
	CDlgVScanCalib_Stiching*	m_pDlgStiching;

	DECLARE_MESSAGE_MAP()
public:

	CButton m_chkSelBlockMode;

	virtual BOOL OnInitDialog();

	CornerData m_pOrgGridCorner;

	BOOL m_bInitGridParam;
	BOOL m_bFindDarkObj;
	float m_fEdgeMinThreshold;
	int   m_iGridUnitSize_px;
	float m_fGridUnitSize_mm;
	//float m_fResolution_cntPmm;
	float m_fLensPower;
	float m_fLearnStrengthMin;
	float m_fLearnStrengthMax;

	int   m_iImgW;
	int   m_iImgH;

	double m_dOldLensPowerX;
	double m_dOldLensPowerY;
	double m_dOldScanSx;
	double m_dOldScanSy;
	double m_dOldScanEx;
	double m_dOldScanEy;
	int    m_iOldBlockX;
	int    m_iOldBlockY;
	int    m_iOldOverlap;
	BOOL   m_bOldUseCalibration;




	Rect  m_rCopyArea;
	Point m_szOverlap_px;
	Image*** m_pScanImage_org;
	PointFloat **m_pGrabPos;
	int   m_nBlockX;
	int   m_nBlockY;

	double m_dScanXmin;
	double m_dScanXmax;
	double m_dScanYmin;
	double m_dScanYmax;
	UINT   m_iOverlap;
	DWORD m_dwScanTimeS;


	afx_msg void OnDestroy();

	


	afx_msg void OnBnClickedSelBlockMode();





	


	
	

	Image* m_pDarkImg;



	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	CString m_sCalibPath;
	afx_msg void OnBnClickedButtonDoSaveLearn();
	afx_msg void OnBnClickedButtonDoLoadLearn();


	BOOL	m_bLoadedLearnImg;


	afx_msg void OnBnClickedButtonChangeCenterPos();


	afx_msg void OnBnClickedButtonMoveScaling();

	afx_msg void OnBnClickedButtonSaveData();
	virtual BOOL PreTranslateMessage(MSG* pMsg);



	int m_rdoCalibStep;
	afx_msg void OnBnClickedRadioInitNScan();
	afx_msg void OnBnClickedRadioLearning();
	afx_msg void OnBnClickedRadioStiching();
	afx_msg void OnBnClickedRadioSave();


};

#pragma once
#include "VRS.h"
#include "./FreeSrc/SxGroupBox.h"
#include "../CGridWeldCal.h"

// CDispImgDlg 대화 상자입니다.
class CWorkImg;
class CIWSDoc;
class CGrabImgDlg;
class CDispImgDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDispImgDlg)

public:
	CDispImgDlg(CIWSDoc *pDoc=NULL, CWnd* pParent=NULL);   // 표준 생성자입니다.
	virtual ~CDispImgDlg();

	BOOL 	m_bShowLive;
	Rect	m_rtBlink;

	UINT	m_nDispWd;
	UINT	m_nDispHt;
	UINT	m_nDispCh;
	float	m_fResolX;
	float	m_fResolY;

	unsigned char* m_pDisplayimgData;
	Image*  m_pDispNiImg;
	Image*	m_pResizeImg;
	int		m_iDispNiWindow;

	CGridWeldCal m_gridCal;

	CWorkImg* m_pAttach;
	float	m_fAvgFR;
	HISTO_INFO	m_histoInfo;

	Point	m_ptMDown;
	Point	m_ptMMove;
	BOOL	m_bMouseDown;
	BOOL	m_bClearOverlay;
	Tool	m_iCurTool;
public:
	void SetDocuments(CIWSDoc *pDoc) { m_pDoc = pDoc; };
	void MovePosScaner(double x, double y, BOOL bRel = TRUE);

	//
	void StopDisplay();
	void StartDisplay();
	void MoveNResizeCtrl(int cx, int cy);
	void DrawHistoGraph(CDC* pDC, CRect rc, Image* pImg);
	void DrawHistoGraph_Sobel(CDC* pDC, CRect rc, Image* pImg);
	void DisplayTextOnDC(int size, BOOL bthick, CRect rc, COLORREF txtclr, CString str, BOOL bpos, CDC *pDC);

	//
	BOOL	m_bDisplayThredAlive;
	HANDLE	m_hEventDisplayThreadEnd;
	static UINT DisplayImageThread(LPVOID pArgs);
	void DisplayImageThreadFunc();
	
	// for auxiilary functionality
	void OnMouseMove();
	void OnLButtonDown();
	void OnLButtonUp();

	// Convert pixel distance to real distance
	float ConvertPxlToRealDist(int dX, int dY);
	void CalcHistogram(Image* pImg, HISTO_INFO* pHisto, int nBin = 256);
	void CalcHistogram_sobel(Image* pImg, HISTO_INFO* pHisto, long* lSobelVal, int nBin = 256);
	HENHMETAFILE Histogram(Image* pImg, BOOL bLoaded, int min_thresh, int max_thresh);
	//
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERA_VIEW_DLG };
#endif

protected:
	double m_calibX[5][5];
	double m_calibY[5][5];
	double m_dCalibStep;
	CIWSDoc *m_pDoc;


	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int GetWeldMark(int axis, double x, double y);

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	afx_msg LRESULT OnDisplay(WPARAM, LPARAM);
	afx_msg LRESULT OnDisplayHisto(WPARAM, LPARAM);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_pictHisto;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnLineGauge();
	CStatic m_stcDistance;
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnClose();
	CSxGroupBox  m_gb1;
	CSxGroupBox  m_gb2;
	afx_msg void OnBnClickedButtonScanMoveYp();
	afx_msg void OnBnClickedButtonScanMoveYn();
	afx_msg void OnBnClickedButtonScanMoveXp();
	afx_msg void OnBnClickedButtonScanMoveXn();
	afx_msg void OnBnClickedButtonScanMoveOrg();
	CEdit m_editScanStep;
	afx_msg void OnBnClickedButtonScanMoveXp2();
	//afx_msg void OnBnClickedBtnAcqWeldpt();
	CStatic m_stScanCurrPos;
	afx_msg void OnBnClickedBtnCalibModify();
	afx_msg void OnBnClickedBtnCalibSave();
	afx_msg void OnBnClickedBtnCalibLoad();
	afx_msg void OnBnClickedCheckWeldpointCalib();
	CButton m_chkWeldPointCalib;
	afx_msg LRESULT OnUwmGridWeldCalib(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWorkImgEvent(WPARAM wParam, LPARAM lParam);
};

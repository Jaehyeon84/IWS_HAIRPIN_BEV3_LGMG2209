#pragma once
#include "VRS.h"
#include "./FreeSrc/SxGroupBox.h"
#include "../CGridWeldCal.h"
#include "../util/AutoEdit.h"

// CDispImgDlg 대화 상자입니다.
class CWorkImg;
class CIWSDoc;
//class CGrabImgRunner;
class CDispImgDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDispImgDlg)

public:
	CDispImgDlg(CIWSDoc *pDoc=NULL, CWnd* pParent=NULL);   // 표준 생성자입니다.
	virtual ~CDispImgDlg();

	/* main CCD */
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
	CWinThread* m_pDispThread;

	CGridWeldCal m_gridCal;

	CWorkImg* m_pAttach;
	float	m_fAvgFR;
	HISTO_INFO	m_histoInfo;

	Point	m_ptMDown;
	Point	m_ptMMove;
	BOOL	m_bMouseDown;
	BOOL	m_bClearOverlay;
	Tool	m_iCurTool;

	/* 2nd CCD for just viwewing */
	BOOL 	m_bShowLive2nd;

	UINT	m_nDispWd2nd;
	UINT	m_nDispHt2nd;
	UINT	m_nDispCh2nd;
	float	m_fResolX2nd;
	float	m_fResolY2nd;

	UINT	m_nImgSizeWd2nd;
	UINT	m_nImgSizeHt2nd;

	unsigned char* m_pDisplayimgData2nd;
	Image*  m_pDispNiImg2nd;
	int		m_iDispNiWindow2nd;

	unsigned char* m_pDpData2nd;

	CWorkImg* m_pAttach2nd;
	CWinThread* m_pDispThread2nd;
	float	m_fAvgFR2nd;

	// sjyi 2024.04.06 추가, 패턴 2개 사용여부 및 Offset 사용 여부
	BOOL m_bUseCCD2ndPtnDisplay;
	BOOL m_bUseCCDPtnOffset;

public:
	/* main CCD */
	void SetDocuments(CIWSDoc *pDoc) { m_pDoc = pDoc; };
	void MovePosScaner(double x, double y, BOOL bRel = TRUE);

	//
	//void StopDisplay();
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

	/* 2nd CCD for just viewing */
	//void StopDisplay2nd();
	void StartDisplay2nd();

	BOOL	m_bDisplayThredAlive2nd;
	HANDLE	m_hEventDisplayThreadEnd2nd;
	static UINT DisplayImageThread2nd(LPVOID pArgs);
	void DisplayImageThreadFunc2nd();

	// sjyi 2024.03.29 추가(CCD Live 용접포인트 표시 관련 추가)
	void MoveCCDOffset(double dX, double dY); // Offset값 이동
	void UpdateCCDOffset();// Offset값 적용
	void LoadCCDOffset(); // Offset값 Load(from Config File)
	void UpdateCCDOffsetBtn(); // Offset 버튼 SHOW/HIDE 설정

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERA_VIEW_DLG };
#endif

protected:
	double m_calibX[5][5];
	double m_calibY[5][5];
	double m_dCalibStep;
	CIWSDoc *m_pDoc;
	CAutoEditDbl m_edtiWeldPOffsetX;
	CAutoEditDbl m_edtiWeldPOffsetY;


	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int GetWeldMark(int axis, double x, double y);

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//afx_msg LRESULT OnDisplay(WPARAM, LPARAM);
	afx_msg LRESULT OnDisplayHisto(WPARAM, LPARAM);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_pictHisto;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnLineGauge();
	CStatic m_stcDistance;
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnClose();
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
	afx_msg void OnBnClickedSnapImg();

	CButton m_chkWeldPointOffset;
	afx_msg void OnBnClickedCheckWeldpointOffset();
	double m_dWeldPOffsetX;
	double m_dWeldPOffsetY;
	afx_msg void OnBnClickedBtnSetWeldpOffset();


};

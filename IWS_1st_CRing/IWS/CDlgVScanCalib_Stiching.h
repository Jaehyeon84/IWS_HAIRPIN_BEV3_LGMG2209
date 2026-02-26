#pragma once


#include ".\VRSaux\WorkImg.h"
#include "CImageMerging.h"
// CDlgVScanCalib_Stiching 대화 상자

class CDlgVScanCalib;
class CIWSDoc;
class CImageMerging;
class CDlgVScanCalib_Stiching : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVScanCalib_Stiching)
	typedef enum enShowTarget
	{
		SHOW_IMG_NONE,
		SHOW_IMG_ORG,
		SHOW_IMG_LEARN,
		SHOW_IMG_CALIB,
		SHOW_IMG_MERGE
	}ShowTarget;
public:
	CDlgVScanCalib_Stiching(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgVScanCalib_Stiching();

	CIWSDoc* m_pDoc;
	CDlgVScanCalib* m_pDlgCalib;
	CImageMerging* m_pMergeCtrl;



// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_VSCANING_CALIB_STICHING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	void InitDlgData();



	Image* m_pLearningImg;
	CWorkImg* m_pImgWnd;
	BOOL m_bInitDlg;


	afx_msg void OnBnClickedButtonZoomP();
	afx_msg void OnBnClickedButtonZoomFit();
	afx_msg void OnBnClickedButtonZoomN();
	float m_fZoomScale;
	float m_fZoomScaleUnit;
	Point m_fZoomCen;
	Point m_fZoomCenMerge;
	float m_fZoomScaleMerge;

	CEdit m_eLearnAreaSX;
	CEdit m_eLearnAreaSY;
	CEdit m_eLearnAreaEX;
	CEdit m_eLearnAreaEY;

	CEdit m_eBlockX;
	CEdit m_eBlockY;
	CSpinButtonCtrl m_spinBlockX;
	CSpinButtonCtrl m_spinBlockY;
	CStatic m_stBlockCntX;
	CStatic m_stBlockCntY;

	CEdit m_eMoveBlockUnit_px;
	CButton m_chkShowGridLine;

	//CButton m_chkShowOrgin;
	//CButton m_chkShowLearn;
	//CButton m_chkShowCalib;
	//CButton m_chkShowMerge;

	afx_msg void OnBnClickedButtonDoCalibMerge();
	afx_msg void OnBnClickedButtonSetCenterBlock();

	//afx_msg void OnBnClickedCheckShowOrigin();
	//afx_msg void OnBnClickedCheckShowLearn();
	//afx_msg void OnBnClickedCheckShowCalib();
	//afx_msg void OnBnClickedCheckShowMerged();

	afx_msg void OnDeltaposSpinBlockX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinBlockY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSelBlockUp();
	afx_msg void OnBnClickedButtonSelBlockLeft();
	afx_msg void OnBnClickedButtonSelBlockDown();
	afx_msg void OnBnClickedButtonSelBlockRight();


	afx_msg void OnBnClickedButtonBmoveYN();
	afx_msg void OnBnClickedButtonBmoveYP();
	afx_msg void OnBnClickedButtonBmoveXN();
	afx_msg void OnBnClickedButtonBmoveXP();

	afx_msg void OnBnClickedCheckShowGridLine();

	afx_msg LRESULT OnWorkImgEvent(WPARAM wParam, LPARAM lParam);
	BOOL m_bMouseDown;
	Point m_pMDN;
	Point m_pMUP;

	Point m_pSelBlock;
	void SelectImageBlock(Point pt);
	void ShiftBlock(Point StartBlock, int iShiftX, int iShiftY);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void SelBlockArrowKey(int iXDiff, int iYDiff);
	void ShowBtnUICheck(ShowTarget pTarget);
	ShowTarget m_enShowTarget;
	int m_iDisTarget;
	afx_msg void OnBnClickedRadioShowOrg();
	afx_msg void OnBnClickedRadioShowLearn();
	afx_msg void OnBnClickedRadioShowCalib();
	afx_msg void OnBnClickedRadioShowMerged();
	afx_msg void OnBnClickedCheckShowCopyArea();
	CButton m_chkShowCopyArea;
};

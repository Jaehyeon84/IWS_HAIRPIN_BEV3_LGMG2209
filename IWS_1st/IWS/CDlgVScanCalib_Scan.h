#pragma once

#include ".\VRSaux\WorkImg.h"
#include "CImageMerging.h"
// CDlgVScanCalib_Scan 대화 상자

class CDlgVScanCalib;
class CIWSDoc;
class CDlgVScanCalib_Scan : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVScanCalib_Scan)

public:
	CDlgVScanCalib_Scan(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgVScanCalib_Scan();

	CIWSDoc* m_pDoc;
	CDlgVScanCalib* m_pDlgCalib;
	CImageMerging* m_pMergeCtrl;



// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_VSCANING_CALIB_SCAN };
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


	Image* m_pSnapImg;
	CWorkImg* m_pImgWnd;
	BOOL m_bInitDlg;


	CEdit m_eStrength;
	CEdit m_eGridXCnt;
	CEdit m_eGridYCnt;
	CEdit m_eGridUnitSize_mm;

	CButton m_chkFindDarkObj;

	CEdit m_eScanXmin;
	CEdit m_eScanXmax;
	CEdit m_eScanYmin;
	CEdit m_eScanYmax;
	CEdit m_eOverlap_p;
	CEdit m_eOverlapX_mm;
	CEdit m_eOverlapY_mm;

	afx_msg void OnBnClickedButtonMoveToOrigin();
	afx_msg void OnBnClickedButtonSnapImage();
	void MovePosScaner(double x, double y, BOOL bRel);
	afx_msg void OnBnClickedButtonSetStrength();
	void CalcGridDimInfo();



	void FindCornerNDraw(Image* pImg, CornerData* pCorner, float fMinThreshold, int iGridUnitSize);
	afx_msg void OnBnClickedButtonSetGridInfo();
	afx_msg void OnBnClickedButtonSetGridParam();



	afx_msg void OnBnClickedButtonDoScan();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBnClickedButtonLoadLearnImg();
	afx_msg void OnEnChangeEditScanOverlapP();


	CStatic m_stGridUnitSize_px;
	CStatic m_stLensPower;



	afx_msg void OnBnClickedButtonZoomP();
	afx_msg void OnBnClickedButtonZoomFit();
	afx_msg void OnBnClickedButtonZoomN();
	float m_fZoomScale;
	float m_fZoomScaleUnit;
	Point m_fZoomCen;
	afx_msg void OnBnClickedCheckUseVirtualMode();
	afx_msg void OnDestroy();
};

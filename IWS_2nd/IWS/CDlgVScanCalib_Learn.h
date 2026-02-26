#pragma once


#include ".\VRSaux\WorkImg.h"
#include "CImageMerging.h"

#include "./UltimateGridLib/include/UGCtrl.h"
// CDlgVScanCalib_Learn 대화 상자

class CDlgVScanCalib;
class CIWSDoc;
class CImageMerging;
class CDlgVScanCalib_Learn : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVScanCalib_Learn)

	class CScanMap :
		public CUGCtrl
	{
	public:
		CScanMap();
		~CScanMap();
		CDlgVScanCalib_Learn* m_pLearningDlg;
		virtual void OnSetup();
		virtual COLORREF OnGetDefBackColor(int section);
		virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
		virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
		virtual void OnKeyDown(UINT *vcKey, int processed);
		void SetBlockSize(int nX, int nY);
		void UpdateStatus(CImageMerging* pMergeCtrl);
		void UpdateResult(CImageMerging* pMergeCtrl);
		DECLARE_MESSAGE_MAP()
		afx_msg void OnSize(UINT nType, int cx, int cy);

	protected:
		int m_nCntX;
		int m_nCntY;
		
	};






public:
	CDlgVScanCalib_Learn(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgVScanCalib_Learn();

	CIWSDoc* m_pDoc;
	CDlgVScanCalib* m_pDlgCalib;
	CImageMerging* m_pMergeCtrl;


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_VSCANING_CALIB_LEARN };
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


	CScanMap m_LearningMap;
	void SelBlock(int x, int y);
	Point m_pSelBlock;


	CEdit m_eStrengthMin;
	CEdit m_eStrengthMax;
	CEdit m_eLearnAreaW;
	CEdit m_eLearnAreaH;
	afx_msg void OnBnClickedButtonDoLearn();



	afx_msg void OnBnClickedButtonZoomP();
	afx_msg void OnBnClickedButtonZoomFit();
	afx_msg void OnBnClickedButtonZoomN();
	float m_fZoomScale;
	float m_fZoomScaleUnit;
	Point m_fZoomCen;
	afx_msg void OnBnClickedRadioShowOrg();
	afx_msg void OnBnClickedRadioConfLgrid();
	afx_msg void OnBnClickedRadioDoLearn();
	afx_msg void OnBnClickedRadioConfSgrid();
	afx_msg void OnBnClickedRadioStretching();
	afx_msg void OnBnClickedRadioConfCgrid();
	afx_msg void OnBnClickedButtonInitParam();
	int m_iManualLearningStep;

	static UINT StartThreadLearning(LPVOID pParam);
	int ThreadLearning();
	CWinThread* m_pThreadLearning;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonCalcStichingParam();
};

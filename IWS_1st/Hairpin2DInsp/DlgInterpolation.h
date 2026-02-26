#pragma once

class CHairpin2DInspDlg;
class CDetectionAlgorithm;

// CDlgInterpolation 대화 상자

class CDlgInterpolation : public CDialog
{
	DECLARE_DYNAMIC(CDlgInterpolation)

public:
	CDlgInterpolation(CHairpin2DInspDlg* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgInterpolation();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_INTERPOLATION };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	CHairpin2DInspDlg* m_pParent;
	CImageDisplay* m_pImageDisplay;
	CDetectionAlgorithm* m_pDetectionAlgorithm;

	CScrollBar m_scrollbarThreshold;
	CScrollBar m_scrollbarThreshold_High;
	CComboBox m_cmbThreshold;
	CButton m_checkThresholdReverse;
	long m_nHighThresholdRegionNoiseFilteringSize;
	long m_nMinBlobSize;
	long m_nMaxBlobCount;
	CButton m_checkBlobMerge;
	long m_nBlobMergeDist;
	
	CListCtrl m_listBlobInfo;
	CButton m_checkInterpolationX;
	CButton m_checkInterpolationY;

	void UpdateUI();
	void UpdateParameters();
	void GetProcData(short* pProc, long procSize, short* pProcParams, long procParamsSize);

	afx_msg void OnBnClickedBtnUpdatebloblist();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnInspection();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckThrresholdReverse();
	afx_msg void OnBnClickedCheckUsemergeblob();
	afx_msg void OnBnClickedBtnOriginalImage();
	afx_msg void OnBnClickedBtnProceededImage();
	afx_msg void OnLvnItemchangedListBloblist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnImageProcManager();
	afx_msg void OnBnClickedBtnThresholdImage();
	afx_msg void OnBnClickedBtnUnrolledImage();
	afx_msg void OnBnClickedCheckInterpolationX();
	afx_msg void OnBnClickedCheckInterpolationY();
	afx_msg void OnCbnSelchangeComboThreshold();
};

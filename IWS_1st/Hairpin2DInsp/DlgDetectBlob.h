#pragma once

class CHairpin2DInspDlg;
class CDetectionAlgorithm;

// CDlgDetectBlob 대화 상자

class CDlgDetectBlob : public CDialog
{
	DECLARE_DYNAMIC(CDlgDetectBlob)

public:
	CDlgDetectBlob(CHairpin2DInspDlg* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgDetectBlob();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DETECT_BLOB };
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
	CButton m_checkThresholdReverse;
	long m_nMinBlobSize;
	long m_nMaxBlobCount;
	CButton m_checkBlobMerge;
	long m_nBlobMergeDist;
	std::vector<HUBSDK::BlobInfo> m_vecBlobInfoList;

	CListCtrl m_listBlobInfo;

	void UpdateUI();
	void UpdateParameters();

	afx_msg void OnBnClickedBtnUpdatebloblist();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnInspection();
	afx_msg void OnBnClickedCheckUsemergeblob();
	afx_msg void OnBnClickedBtnOriginalImage();
	afx_msg void OnBnClickedBtnProceededImage();
	afx_msg void OnLvnItemchangedListBloblist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnThresholdImage();
	afx_msg void OnBnClickedBtnUnrolledImage();
};

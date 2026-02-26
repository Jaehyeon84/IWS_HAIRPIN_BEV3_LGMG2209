#pragma once


// CDlgResultViewer 대화 상자

class CDetectionAlgorithm;

class CDlgResultViewer : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgResultViewer)

public:
	CDlgResultViewer(CDetectionAlgorithm* pDetectionAlgorithm, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgResultViewer();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_RESULT_VIEWER };
//#endif

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CImageDisplay* m_pImageDisplay;
	CDetectionAlgorithm* m_pDetectionAlgorithm;
	CListCtrl m_listResult;
	CLabel m_lblJig;
	BOOL m_bResult;
	CLabel m_lblResult;
	CLabel m_lblH12;
	CLabel m_lblH3;
	CLabel m_lblC;
	CLabel m_lblT;
	CLabel m_lblJ;

	void ShowResult();
	void UpdateResult();
	void SetCriteriaSpec(CLabel &lbl, const CString strName, const CString strmm, const float max);
	void SetCriteriaSpec(CLabel &lbl, const CString strName, const CString strmm, const float min, const float max);
	void SetJigName(const CString strJig);
	void SetResult(const CString strResult);
	void ShowReviewOverlay(const BOOL bRemoveOverlay, const BOOL bShowOverlay, const long slot = 0, const long layer = 0, const BOOL bSelect = FALSE);
	void SetOverlay(const long slot, const long layer, const BOOL bDBClick);

	afx_msg void OnLvnItemchangedListResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnWholeImage();
};

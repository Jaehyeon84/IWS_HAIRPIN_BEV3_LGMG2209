#pragma once


class CHairpin2DInspDlg;
class CDetectionAlgorithm;

struct SBatchInfo
{
	CString m_strFilePath;
	CString m_strTotalResult;

	void Init()
	{
		m_strFilePath = _T("");
		m_strTotalResult = _T("");
	}

	void Copy(SBatchInfo source)
	{
		m_strFilePath = source.m_strFilePath;
		m_strTotalResult = source.m_strTotalResult;
	}
};

// CDlgBatchInspection 대화 상자

class CDlgBatchInspection : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBatchInspection)

public:
	CDlgBatchInspection(CHairpin2DInspDlg* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgBatchInspection();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_BATCH_INSPECTION };
//#endif

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CHairpin2DInspDlg* m_pParent;
	CDetectionAlgorithm* m_pDetectionAlgorithm;

	CListCtrl m_listData;
	std::vector<SBatchInfo> m_vecsBatchInfo;
	long m_nSeletedIndex;
	long m_nInspIndex;

	void SetData();
	void InitListControl();
	void SetListControl();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnAddFiles();
	afx_msg void OnBnClickedBtnDelFiles();
	afx_msg void OnBnClickedBtnClearFileList();
	afx_msg void OnBnClickedBtnAllCheck();
	afx_msg void OnBnClickedBtnAllUnCheck();
	afx_msg void OnBnClickedBtnAutoInspection();
	afx_msg void OnBnClickedBtnAutoInspectionStop();
	afx_msg void OnBnClickedBtnInspection();
	afx_msg void OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult);
};

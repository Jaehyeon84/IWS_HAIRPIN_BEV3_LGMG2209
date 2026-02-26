#pragma once

class CHairpin2DInspDlg;
class CDetectionAlgorithm;

// CDlgHairpin 대화 상자

class CDlgHairpin : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHairpin)

public:
	CDlgHairpin(CHairpin2DInspDlg* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgHairpin();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_HAIRPIN_CENTER };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	CHairpin2DInspDlg* m_pParent;
	CImageDisplay* m_pImageDisplay;
	CDetectionAlgorithm* m_pDetectionAlgorithm;
	
	long m_nWidthSize_1BlobMinSize;
	long m_nWidthSize_2BlobMinSize;
	long m_nWidthSize_2MoreBlobMinSize;
	long m_nComplementHeightSize;
	long m_nComplementHeightLimit;
	CButton m_checkUseComplementCenter;
	long m_nMinHairpinWidth;
	long m_nMinHairpinHeight;
	long m_nMaxHairpinWidth;
	long m_nMaxHairpinHeight;
	long m_nMinHairpinWidthfor1Blob;
	CButton m_checkUseCalCenterOffset_replace3D;
	CButton m_checkUseCorrectionOffset_replace3D;
	CListCtrl m_listAlgorithmLog;

	void UpdateUI();
	void UpdateParameters();
	void UpdateLog();

	afx_msg void OnBnClickedBtnOriginalImage();
	afx_msg void OnBnClickedBtnUnrolledImage();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnInspection2();
	float m_f2D3D_Potion_Limit_X;
	float m_f2D3D_Potion_Limit_Y;
	CButton m_checkUseLayerPairReplace;
};

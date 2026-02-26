#pragma once

class CHairpin2DInspDlg;
class CDetectionAlgorithm;

// CDlgROI 대화 상자

class CDlgROI : public CDialog
{
	DECLARE_DYNAMIC(CDlgROI)

public:
	CDlgROI(CHairpin2DInspDlg* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgROI();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_ROI };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CHairpin2DInspDlg* m_pParent;
	CDetectionAlgorithm* m_pDetectionAlgorithm;

	long m_nSlotCount;
	long m_nLayerCount;
	long m_nLayerWidth;
	long m_nSlotHeight;
	long m_nSlotTopOffset;
	long m_nSlotBottomOffset;
	long m_nHairpinAreaROICenterToLeftOffset;
	long m_nHairpinAreaROICenterToTopOffset;
	long m_nHairpinAreaROICenterToRightOffset;
	long m_nHairpinAreaROICenterToBottomOffset;
	long m_nOffsetX;
	CGridCtrl m_gridEachROIOffsetX;
	long m_nEachRoiOffsetXStartSlot;
	long m_nEachRoiOffsetXEndSlot;
	long m_nEachRoiOffsetXValue;
	long m_nSearchROIWidthIncreaseRatio;
	long m_nAutoROIProgressCount;
	CButton m_checkUseAutoROI;
	long m_nStartSlotIndex;

	void UpdateUI();
	void UpdateParameters();

	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnOriginalImage();
	afx_msg void OnBnClickedBtnInspection();
	afx_msg void OnBnClickedBtnInspection4();
	afx_msg void OnGridEditEnd(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnBnClickedBtnEachRoiOffsetXSet();
	afx_msg void OnBnClickedCheckUseAutoRoi();
};


// Hairpin2DInspDlg.h: 헤더 파일
//

#pragma once

#include "resource.h"

class CDlgBatchInspection;
class CDlgMakeUnrolled;
class CDlgROI;
class CDlgRemoveNoise;
class CDlgInterpolation;
class CDlgDetectBlob;
class CDlgCriteria;
class CDetectionAlgorithm;
class CDlgHairpin;

// CHairpin2DInspDlg 대화 상자
class CHairpin2DInspDlg : public CDialog
{
// 생성입니다.
public:
	CHairpin2DInspDlg(CDetectionAlgorithm* pDetectionAlgorithm, CWnd* pParent = nullptr);	// 표준 생성자입니다.
	~CHairpin2DInspDlg();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HAIRPIN2DINSP_DIALOG };
//#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CImageDisplay* m_pImageDisplay;
	CDlgBatchInspection* m_pDlgBatchInspection;
	CDlgMakeUnrolled* m_pDlgMakeUnrolled;
	CDlgROI* m_pDlgROI;
	CDlgRemoveNoise* m_pDlgRemoveNoise;
	CDlgInterpolation* m_pDlgInterpolation;
	CDlgDetectBlob* m_pDlgDetectBlob;
	CDlgCriteria* m_pDlgCriteria;
	CDetectionAlgorithm* m_pDetectionAlgorithm;
	CDlgHairpin* m_pDlgHairpin;

	BYTE* m_pbyImage; // 원본 영상
	long m_nOriginalImageWidth;
	long m_nOriginalImageHeight;
	long m_nOriginalImageSize;
	long m_nUnrolledImageWidth;
	long m_nUnrolledImageHeight;
	long m_nUnrolledImageSize;
	BOOL m_bDetectionAlgorithminDlg;
	CTabCtrl m_tabDetection;
	CLabel m_lblResult;
	BOOL m_bResult;
	CString m_strRecipeFileName;

	BOOL ImageOpen2D(const CString strPath);
	void UpdateUI();
	BOOL LoadRecipe(const LPCTSTR szPathName);
	BOOL SaveRecipe(const LPCTSTR szPathName);
	void ShowOriginal(const BOOL bRemoveOverlay = TRUE, const BOOL bShowOverlay = FALSE);
	void ShowMakeUnrolled(const BOOL bRemoveOverlay = TRUE);
	void ShowROI(const BOOL bRemoveOverlay = TRUE, const BOOL bShowOverlay = FALSE);
	void ShowRemoveNoise(const IMAGE image, const BOOL bRemoveOverlay = TRUE, const BOOL bShowOverlay = FALSE);
	void ShowInterpolation(const IMAGE image, const BOOL bRemoveOverlay = TRUE, const BOOL bShowOverlay = FALSE);
	void ShowDetectBlob(const BOOL bRemoveOverlay = TRUE, const BOOL bShowOverlay = FALSE);
	void ShowHairpinOverlay_Unrolled(const BOOL bRemoveOverlay = TRUE, const BOOL bShowOverlay = FALSE);
	void ShowHairpinOverlay_Original(const BOOL bRemoveOverlay = TRUE, const BOOL bShowOverlay = FALSE);
	void ShowCriteriaOverlay_Unrolled(const BOOL bRemoveOverlay = TRUE, const BOOL bShowOverlay = FALSE, const long slot = 0, const long layer = 0, const BOOL bSelect = FALSE);
	void ShowCriteriaOverlay_Original(const BOOL bRemoveOverlay = TRUE, const BOOL bShowOverlay = FALSE, const long slot = 0, const long layer = 0, const BOOL bSelect = FALSE);
	BOOL UpdateRemoveNoiseThresholdImage(const BOOL bDisplay);
	BOOL UpdateRemoveNoiseProcImage(const BOOL bDisplay);
	BOOL UpdateInterThresholdImage(const BOOL bDisplay);
	BOOL UpdateInterProcImage(const BOOL bDisplay);
	void SetResult(const CString strResult);
	BOOL CreateFolder(CString sDirPath);
	long FileAllsavefordebugging(const CString sZipFilePathName);

	afx_msg void OnTcnSelchangeTabDetection(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileRecipeopen();
	afx_msg void OnFileRecipesaveas();
	afx_msg void OnImageImageopen();
	afx_msg void OnInspectionInspection();
	afx_msg void OnInspectionBatchinspection();
	afx_msg void OnBnClickedBtnInspection();
	afx_msg void OnFileDatasave();
	afx_msg void OnFileAllsavefordebugging();
	afx_msg void OnFileRecipemanage();
};

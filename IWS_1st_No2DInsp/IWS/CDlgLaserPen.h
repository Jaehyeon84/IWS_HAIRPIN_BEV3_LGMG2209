#pragma once
#include "./util/AutoEdit.h"
// CDlgLaserPen 대화 상자

class CDlgLaserPen : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLaserPen)

public:
	CDlgLaserPen(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgLaserPen();

	IWS_LPEN *m_pPens;
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LASERPEN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CAutoEditInt m_editPenID;
	CAutoEditDbl m_editLaserPower;
	CAutoEditDbl m_editFrequency;
	CAutoEditDbl m_editPulseWidth;
	CAutoEditDbl m_editJumpSpeed;
	CAutoEditDbl m_editWeldSpeed;
	CAutoEditInt m_editJumpDelay;
	CAutoEditInt m_editLaserOffDelay;
	CAutoEditInt m_editLaserOnDelay;
	CAutoEditInt m_editWeldDelay;
	CAutoEditInt m_editPolygonDelay;

	CComboBox m_cbLaserPens;
	afx_msg void OnCbnSelchangeComboboxexLaserpens();
	afx_msg void OnBnClickedMfcbuttonLaserpenApply();
	
	afx_msg void OnBnClickedCheckWobbel();
	CButton m_chkUseWobbel;
	CAutoEditDbl m_editWbFreq;
	CAutoEditDbl m_editWbLongitude;
	CAutoEditDbl m_editWbTranseverse;
};

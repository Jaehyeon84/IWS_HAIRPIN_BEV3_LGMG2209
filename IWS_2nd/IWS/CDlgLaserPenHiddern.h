#pragma once
#include "./util/AutoEdit.h"
#include "ColorComboBox.h"
#include <vector>

// CDlgLaserPenHiddern 대화 상자

class CDlgLaserPenHiddern : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLaserPenHiddern)

public:
	CDlgLaserPenHiddern(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgLaserPenHiddern();

	double m_dPenPower[10] = { 0 };
	double m_dPenSpeed[10] = { 0 };
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LASERPEN_HIDDERN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CAutoEditDbl m_editLaserPower;
	CAutoEditDbl m_editLaserSpeed;

	CColorComboBox m_cbLaserPens;
	afx_msg void OnCbnSelchangeComboboxexLaserpens();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedMfcbuttonLaserpenApply();
	afx_msg void OnBnClickedOk();
};

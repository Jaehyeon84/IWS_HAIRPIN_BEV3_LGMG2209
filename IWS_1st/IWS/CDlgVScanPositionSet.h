#pragma once
#include "util/AutoEdit.h"
#include "VRSaux/VRS.h"
#include "CEMiniSerial.h"

// CDlgVScanPositionSet 대화 상자
	
class CDlgVScanPositionSet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVScanPositionSet)

public:
	CDlgVScanPositionSet(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgVScanPositionSet();

	double m_dblCenImgX, m_dblCenImgY;
	double m_sx, m_sy, m_ex, m_ey;
	double m_dModel1_X, m_dModel1_Y, m_dModel2_X, m_dModel2_Y, m_dModel3_X, m_dModel3_Y, m_dModel4_X, m_dModel4_Y;
	int m_nLightPower;
	CEMiniSerial* m_pCtrl;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_VSCANIMG_POS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CAutoEditDbl m_editModel1X;
	CAutoEditDbl m_editModel1Y;
	CAutoEditDbl m_editModel2X;
	CAutoEditDbl m_editModel2Y;
	CAutoEditDbl m_editModel3X;
	CAutoEditDbl m_editModel3Y;
	CAutoEditDbl m_editModel4X;
	CAutoEditDbl m_editModel4Y;

	CAutoEditDbl m_editSx;
	CAutoEditDbl m_editSy;
	CAutoEditDbl m_editEx;
	CAutoEditDbl m_editEy;

	CAutoEditDbl m_editImgCenX;
	CAutoEditDbl m_editImgCenY;

	CEdit m_EditImgX;
	CEdit m_EditImgY;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

public:
	afx_msg void OnBnClickedBtnLightPower();
	CAutoEditInt m_editLightPower;
	afx_msg void OnBnClickedButtonScanCalibration();
};

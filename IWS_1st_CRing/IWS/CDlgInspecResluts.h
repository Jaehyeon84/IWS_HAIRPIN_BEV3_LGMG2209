#pragma once

#include "UltimateGridLib/Include/UGCtrl.h"

// CDlgInspecResluts 대화 상자
class CEthernetPLC;
class CIWSDoc;
class CDlgInspecResluts : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgInspecResluts)
	typedef struct _HAIRPIN_DATA {
		double x;
		double y;
		double z;
	}HAIRPIN_DATA;
	typedef std::vector<HAIRPIN_DATA> HairPinData;
	typedef struct _circleEquation
	{
		double cx, cy, r;
	}CircleEquation;
public:
	CDlgInspecResluts(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgInspecResluts();

	CUGCtrl m_grid;
	CUGCtrl m_gridCalc;

	CEthernetPLC *m_pPLC;
	CIWSDoc* m_pDoc;
	int m_nSlot;
	int m_nLayer;
	void InitGrid();
	void UpdateGrid();
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INSPECt_RESULTS };
#endif

protected:	
	HairPinData m_hairpin;
	void CalcFittedHairpin();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonUpdate();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonResSave();
	afx_msg void OnBnClickedButtonResImport();
};

#pragma once
#include "IWS_define.h"

// CDlgReworkJob 대화 상자

typedef struct _ST_REWORK_JOB
{
	int nSlotNo;
	int nLayerNo;
	int nPtnNo;
	_ST_REWORK_JOB() {
		nSlotNo = 0;
		nLayerNo = 0;
		nPtnNo = 0;
	};
}ST_REWORK_JOB;

typedef std::vector<ST_REWORK_JOB> IWS_VECTOR_REWORK_JOB;


class CDlgReworkJob : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgReworkJob)

public:
	CDlgReworkJob(IWS_HAIRPIN_INFO hpInfo, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgReworkJob();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REWORK_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDelete();

// 멤버 변수
protected:
	IWS_HAIRPIN_INFO m_hpInfo;

public:
	CListCtrl m_ctrlJobList;
	IWS_VECTOR_REWORK_JOB m_vecReworkJob;

// 멤버 함수
protected:
	void InitCtrlJobList();
	void InitComboBox();

	void AddReworkJob(int nSlotNo = 0, int nLayerNo = 0, int nPtnNo = 0);
	void DeleteReworkJob(int nSlotNo = 0, int nLayerNo = 0, int nPtnNo = 0);
	void DeleteSelectedReworkJob(int nSlotNo, int nLayerNo, int nPtnNo);

	void UpdateReworkJobListCtrl(BOOL bSort = FALSE);


public:
	virtual BOOL OnInitDialog();

	void GetReworkJob(IWS_VECTOR_REWORK_JOB &vecReworkJob);
	CComboBox m_ctrlSlotNo;
	CComboBox m_ctrlLayerNo;
	afx_msg void OnBnClickedButtonDelete2();

	BOOL IsExistReworkJob(int nSlotNo, int nLayerNo, int nPtnNo);

	CComboBox m_ctrlPtnNo;
};

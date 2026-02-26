#pragma once


// CDlgRecipeManage 대화 상자

class CDlgRecipeManage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRecipeManage)

public:
	CDlgRecipeManage(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgRecipeManage();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_RECIPE_MANAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CRecipeManager* m_pRecipeSource;
	CRecipeManager* m_pRecipeTarget;

	CString m_strRefRecipePath;
	std::vector<CString> m_vecstrTargetRecipesPath;
	CListBox m_listTargetRecipe;
	CButton m_checkMakeUnrolled;
	CButton m_checkROI;
	CButton m_checkRemoveNoise;
	CButton m_checkInterpolation;
	CButton m_checkDetectBlob;
	CButton m_checkHairpinCenter;
	CButton m_checkCriteria;
	CButton m_checkPublic;

	void UpdateApplyList();

	afx_msg void OnBnClickedBtnOpenRefRecipe();
	afx_msg void OnBnClickedBtnOpenTargetRecipes();
	afx_msg void OnBnClickedBtnChange();
	afx_msg void OnLbnDblclkListTargetRecipe();
};

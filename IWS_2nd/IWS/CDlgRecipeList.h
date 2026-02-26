#pragma once


// CDlgRecipeList 대화 상자

class CDlgRecipeList : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRecipeList)

public:
	CDlgRecipeList(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgRecipeList();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RECIPELIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CStringArray m_arrWorkFiles;
	CStringArray m_arrSelectedList;

	CListBox m_listRecipeFiles;
	CListBox m_listSelectRecipe;
	afx_msg void OnBnClickedButtonSelectRecipe();
	afx_msg void OnBnClickedButtonUnselectRecipe();
	afx_msg void OnBnClickedButtonUnselectAll();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSelectDown();
	afx_msg void OnBnClickedButtonSelectUp();
};

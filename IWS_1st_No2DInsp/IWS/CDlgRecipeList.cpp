// CDlgRecipeList.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgRecipeList.h"
#include "afxdialogex.h"


// CDlgRecipeList 대화 상자

IMPLEMENT_DYNAMIC(CDlgRecipeList, CDialogEx)

CDlgRecipeList::CDlgRecipeList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_RECIPELIST, pParent)
{

}

CDlgRecipeList::~CDlgRecipeList()
{
}

void CDlgRecipeList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RECIPE_IN_FOLDER, m_listRecipeFiles);
	DDX_Control(pDX, IDC_LIST_SELECTED_RECIPE, m_listSelectRecipe);
}


BEGIN_MESSAGE_MAP(CDlgRecipeList, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_RECIPE, &CDlgRecipeList::OnBnClickedButtonSelectRecipe)
	ON_BN_CLICKED(IDC_BUTTON_UNSELECT_RECIPE, &CDlgRecipeList::OnBnClickedButtonUnselectRecipe)
	ON_BN_CLICKED(IDC_BUTTON_UNSELECT_ALL, &CDlgRecipeList::OnBnClickedButtonUnselectAll)
	ON_BN_CLICKED(IDOK, &CDlgRecipeList::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_DOWN, &CDlgRecipeList::OnBnClickedButtonSelectDown)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_UP, &CDlgRecipeList::OnBnClickedButtonSelectUp)
END_MESSAGE_MAP()


// CDlgRecipeList 메시지 처리기


void CDlgRecipeList::OnBnClickedButtonSelectRecipe()
{
	int iSel = m_listRecipeFiles.GetCurSel();
	if (iSel < 0) return;
	int iPlace = m_listSelectRecipe.GetCurSel();
	if (iPlace < 0 || iPlace >= m_listSelectRecipe.GetCount())
		m_listSelectRecipe.AddString(m_arrWorkFiles[iSel]);
	else
		m_listSelectRecipe.InsertString(iPlace, m_arrWorkFiles[iSel]);
	m_listSelectRecipe.SetCurSel(iPlace);
	m_listRecipeFiles.SetCurSel(-1);
}


void CDlgRecipeList::OnBnClickedButtonUnselectRecipe()
{
	int iSel  = m_listSelectRecipe.GetCurSel();
	if (iSel < 0) return;
	m_listSelectRecipe.DeleteString(iSel);
}


void CDlgRecipeList::OnBnClickedButtonUnselectAll()
{
	m_listSelectRecipe.ResetContent();
}


BOOL CDlgRecipeList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	INT_PTR nWorkfiles = m_arrWorkFiles.GetSize();

	for (INT_PTR i = 0; i < nWorkfiles; i++) {
		m_listRecipeFiles.AddString(m_arrWorkFiles[i]);
	}

	INT_PTR nRecipe = m_arrSelectedList.GetSize();
	for (INT_PTR i = 0; i < nRecipe; i++) {
		m_listSelectRecipe.AddString(m_arrSelectedList[i]);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgRecipeList::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelected = m_listSelectRecipe.GetCount();
	if (nSelected <= 0) return;
	m_arrSelectedList.RemoveAll();

	for (int i = 0; i < nSelected; i++) {
		CString str;
		m_listSelectRecipe.GetText(i, str);
		m_arrSelectedList.Add(str);
	}
	CDialogEx::OnOK();
}




void CDlgRecipeList::OnBnClickedButtonSelectDown()
{
	int iSel = m_listSelectRecipe.GetCurSel();
	if (iSel < 0) return;
	CString strSelected, strTarget;
	int iTarget = iSel + 1;
	if (iTarget >= m_listSelectRecipe.GetCount()) return;
	m_listSelectRecipe.GetText(iSel, strSelected);
	m_listSelectRecipe.GetText(iTarget, strTarget);

	m_listSelectRecipe.DeleteString(iSel);
	m_listSelectRecipe.DeleteString(iSel);
	m_listSelectRecipe.InsertString(iSel, strSelected);
	m_listSelectRecipe.InsertString(iSel, strTarget);
	m_listSelectRecipe.SetCurSel(iTarget);
}


void CDlgRecipeList::OnBnClickedButtonSelectUp()
{
	int iSel = m_listSelectRecipe.GetCurSel();
	if (iSel <= 0) return;
	CString strSelected, strTarget;
	int iTarget = iSel - 1;

	m_listSelectRecipe.GetText(iSel, strSelected);
	m_listSelectRecipe.GetText(iTarget, strTarget);

	m_listSelectRecipe.DeleteString(iTarget);
	m_listSelectRecipe.DeleteString(iTarget);
	m_listSelectRecipe.InsertString(iTarget, strTarget);
	m_listSelectRecipe.InsertString(iTarget, strSelected);
	m_listSelectRecipe.SetCurSel(iTarget);
}

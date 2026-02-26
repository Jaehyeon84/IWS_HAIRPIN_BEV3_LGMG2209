// DlgRecipeManage.cpp: 구현 파일
//

#include "pch.h"
#include "Hairpin2DInsp.h"
#include "DlgRecipeManage.h"
#include "afxdialogex.h"


// CDlgRecipeManage 대화 상자

IMPLEMENT_DYNAMIC(CDlgRecipeManage, CDialogEx)

CDlgRecipeManage::CDlgRecipeManage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_RECIPE_MANAGE, pParent)
	, m_pRecipeSource(nullptr)
	, m_pRecipeTarget(nullptr)
	, m_strRefRecipePath(_T(""))
{

}

CDlgRecipeManage::~CDlgRecipeManage()
{
	if(m_pRecipeSource != nullptr)
	{
		delete m_pRecipeSource;
	}

	if(m_pRecipeTarget != nullptr)
	{
		delete m_pRecipeTarget;
	}
}

void CDlgRecipeManage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_MAKE_UNROLLED, m_checkMakeUnrolled);
	DDX_Control(pDX, IDC_CHECK_ROI, m_checkROI);
	DDX_Control(pDX, IDC_CHECK_REMOVE_NOISE, m_checkRemoveNoise);
	DDX_Control(pDX, IDC_CHECK_INTERPOLATION, m_checkInterpolation);
	DDX_Control(pDX, IDC_CHECK_DETECT_BLOB, m_checkDetectBlob);
	DDX_Control(pDX, IDC_CHECK_HAIRPIN_CENTER, m_checkHairpinCenter);
	DDX_Control(pDX, IDC_CHECK_CRITERIA, m_checkCriteria);
	DDX_Control(pDX, IDC_CHECK_PUBLIC, m_checkPublic);
	DDX_Text(pDX, IDC_EDIT_REF_RECIPE_PATH, m_strRefRecipePath);
	DDX_Control(pDX, IDC_LIST_TARGET_RECIPE, m_listTargetRecipe);
}


BEGIN_MESSAGE_MAP(CDlgRecipeManage, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_OPEN_REF_RECIPE, &CDlgRecipeManage::OnBnClickedBtnOpenRefRecipe)
	ON_BN_CLICKED(IDC_BTN_OPEN_TARGET_RECIPES, &CDlgRecipeManage::OnBnClickedBtnOpenTargetRecipes)
	ON_BN_CLICKED(IDC_BTN_CHANGE, &CDlgRecipeManage::OnBnClickedBtnChange)
	ON_LBN_DBLCLK(IDC_LIST_TARGET_RECIPE, &CDlgRecipeManage::OnLbnDblclkListTargetRecipe)
END_MESSAGE_MAP()


// CDlgRecipeManage 메시지 처리기


void CDlgRecipeManage::OnBnClickedBtnOpenRefRecipe()
{
	CFileDialog Dlg(TRUE, _T("recipe"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, _T("Recipe File (*.recipe)|*.recipe||"), NULL, NULL, FALSE);

	if(Dlg.DoModal() == IDOK)
	{
		UpdateData(TRUE);

		m_strRefRecipePath = Dlg.GetPathName();

		if(m_pRecipeSource != nullptr)
		{
			delete m_pRecipeSource;
		}

		m_pRecipeSource = new CRecipeManager;
		m_pRecipeSource->InitParameters();
		m_pRecipeSource->LoadRecipeParameters(m_strRefRecipePath);

		UpdateData(FALSE);
	}
}


void CDlgRecipeManage::OnBnClickedBtnOpenTargetRecipes()
{
	CFileDialog Dlg(TRUE, _T("recipe"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, _T("Recipe File (*.recipe)|*.recipe||"), NULL, NULL, FALSE);

	int nMaxFileName = 4096000;
	TCHAR *pszFileNames = new TCHAR[nMaxFileName];
	ZeroMemory(pszFileNames, nMaxFileName);

	Dlg.m_ofn.lpstrFile = pszFileNames;
	Dlg.m_ofn.nMaxFile = nMaxFileName - 1;

	if(Dlg.DoModal() == IDOK)
	{
		POSITION pos = Dlg.GetStartPosition();

		while(pos)
		{
			m_vecstrTargetRecipesPath.push_back(Dlg.GetNextPathName(pos));
		}

		UpdateApplyList();
	}

	delete[] pszFileNames;

	Invalidate(FALSE);
}

void CDlgRecipeManage::OnLbnDblclkListTargetRecipe()
{
	long nCurSel = m_listTargetRecipe.GetCurSel();

	if(nCurSel < 0)
	{
		return;
	}

	m_vecstrTargetRecipesPath.erase(m_vecstrTargetRecipesPath.begin() + nCurSel);

	UpdateApplyList();
}

void CDlgRecipeManage::UpdateApplyList()
{
	m_listTargetRecipe.ResetContent();

	for(long idx = 0; idx < (long)m_vecstrTargetRecipesPath.size(); idx++)
	{
		m_listTargetRecipe.AddString(m_vecstrTargetRecipesPath[idx]);
	}
}


void CDlgRecipeManage::OnBnClickedBtnChange()
{
	BOOL bMakeUnrolled = m_checkMakeUnrolled.GetCheck();
	BOOL bROI = m_checkROI.GetCheck();
	BOOL bRemoveNoise = m_checkRemoveNoise.GetCheck();
	BOOL bInterpolation = m_checkInterpolation.GetCheck();
	BOOL bDetectBlob = m_checkDetectBlob.GetCheck();
	BOOL bHairpinCenter = m_checkHairpinCenter.GetCheck();
	BOOL bCriteria = m_checkCriteria.GetCheck();
	BOOL bPublic = m_checkPublic.GetCheck();

	if(bMakeUnrolled == FALSE && bROI == FALSE && bRemoveNoise == FALSE && bInterpolation == FALSE &&
		bDetectBlob == FALSE && bHairpinCenter == FALSE && bCriteria == FALSE && bPublic == FALSE)
	{
		::AfxMessageBox(_T("Please Select CheckBox"));
		return;
	}

	for(long idx = 0; idx < (long)m_vecstrTargetRecipesPath.size(); idx++)
	{
		m_pRecipeTarget = new CRecipeManager;
		m_pRecipeTarget->InitParameters();
		m_pRecipeTarget->LoadRecipeParameters(m_vecstrTargetRecipesPath[idx]);

		if(bMakeUnrolled == TRUE)
		{
			m_pRecipeTarget->m_sUnrolledParams = m_pRecipeSource->m_sUnrolledParams;
		}

		if(bROI == TRUE)
		{
			m_pRecipeTarget->m_sROIParams = m_pRecipeSource->m_sROIParams;
		}

		if(bRemoveNoise == TRUE)
		{
			m_pRecipeTarget->m_sRemoveNoiseParams = m_pRecipeSource->m_sRemoveNoiseParams;
		}

		if(bInterpolation == TRUE)
		{
			m_pRecipeTarget->m_sInterHairpinParams = m_pRecipeSource->m_sInterHairpinParams;
		}

		if(bDetectBlob == TRUE)
		{
			m_pRecipeTarget->m_sDetectBlobParams = m_pRecipeSource->m_sDetectBlobParams;
		}

		if(bHairpinCenter == TRUE)
		{
			m_pRecipeTarget->m_sHairpinCenterParams = m_pRecipeSource->m_sHairpinCenterParams;
		}

		if(bCriteria == TRUE)
		{
			m_pRecipeTarget->m_sCriteriaParams = m_pRecipeSource->m_sCriteriaParams;
		}

		if(bPublic == TRUE)
		{
			m_pRecipeTarget->m_sPublicParams = m_pRecipeSource->m_sPublicParams;
		}

		m_pRecipeTarget->SaveRecipeParameters(m_vecstrTargetRecipesPath[idx]);

		delete m_pRecipeTarget;
		m_pRecipeTarget = nullptr;
	}

	::AfxMessageBox(_T("Complete!!"));
}



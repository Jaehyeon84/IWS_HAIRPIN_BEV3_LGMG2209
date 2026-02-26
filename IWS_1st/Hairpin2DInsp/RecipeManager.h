#pragma once

class CRecipeManager
{
public:
	CRecipeManager();
	~CRecipeManager();

	SUnrolledParams m_sUnrolledParams;
	SROIParams m_sROIParams;
	SRemoveNoiseParams m_sRemoveNoiseParams;
	SInterHairpinParams m_sInterHairpinParams;
	SDetectBlobParams m_sDetectBlobParams;
	SHairpinCenterParams m_sHairpinCenterParams;
	SCriteriaParams m_sCriteriaParams;
	SPublicParams m_sPublicParams;

	void InitParameters();
	BOOL LoadRecipeParameters(const CString strRecipePathName);
	BOOL SaveRecipeParameters(const CString strRecipePathName);
};

extern CRecipeManager g_RecipeManager;

#include "stdafx.h"
#include "IWS.h"
#include "EntyEllipse.h"


CEntyEllipse::CEntyEllipse()
{
	m_strName.Format(L"Ellipse_%d", m_iEntCount);
	iDir = false;
	//m_hCadEntity = NULL;
}


CEntyEllipse::~CEntyEllipse()
{
}

void CEntyEllipse::UpdatePath(HANDLE hEntity)
{
	int nPlines = lcExpEntity(hEntity, LC_EXP_OUTLINE, false);
	if (nPlines > 0) {
		m_path.clear();
		for (int i = 0; i < nPlines; ++i) {

			int nVers = lcExpGetPline(0);
			for (int iVer = 0; iVer < nVers; ++iVer) {
				Point3Dbl pts;
				lcExpGetVertex(&pts.x, &pts.y);
				// add a point to indicate the vertex
				pts.z = 0.0;
				m_path.push_back(pts);
			}
		}
	}
	if (iDir)
		std::reverse(m_path.begin(), m_path.end());
	UpdateCenter();
}

BOOL CEntyEllipse::Save(CFile *pf, int iver)
{
	CStringA strName(m_strName);
	int nStr = strName.GetLength();
	pf->Write(&nStr, sizeof(int));
	pf->Write(strName.GetBuffer(), sizeof(char)*nStr);
	pf->Write(&center, sizeof(Point3Dbl));
	pf->Write(&Rmajor, sizeof(double));
	pf->Write(&Rminor, sizeof(double));
	pf->Write(&RotAngle, sizeof(double));
	pf->Write(&StartAngle, sizeof(double));
	pf->Write(&ArcAngle, sizeof(double));
	pf->Write(&iDir, sizeof(bool));
	return TRUE;
}

BOOL CEntyEllipse::Load(CFile *pf, int iver)
{
	int nStr;
	char tmp[100] = "";
	pf->Read(&nStr, sizeof(int));
	pf->Read(&tmp, sizeof(char)*nStr);
	CString str(tmp);
	m_strName = str;
	pf->Read(&center, sizeof(Point3Dbl));
	pf->Read(&Rmajor, sizeof(double));
	pf->Read(&Rminor, sizeof(double));
	pf->Read(&RotAngle, sizeof(double));
	pf->Read(&StartAngle, sizeof(double));
	pf->Read(&ArcAngle, sizeof(double));
	if (iver < WELDJOB_VER102)
		iDir = false;
	else
		pf->Read(&iDir, sizeof(bool));
	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = AddCurrEntityToBlock(hBlock);
	SetDrawEntity(hEnt);
	lcEventsEnable(TRUE);
	return TRUE;
}

HANDLE CEntyEllipse::AddCurrEntityToBlock(HANDLE hBlock)
{
	HANDLE handle;
	if(m_hCadEntity)
		handle = lcBlockAddClone(hBlock, m_hCadEntity);
	else {
		handle = lcBlockAddEllipse(hBlock, center.x, center.y,
			Rmajor, Rminor, RotAngle*LC_DEG_TO_RAD, StartAngle*LC_DEG_TO_RAD, ArcAngle*LC_DEG_TO_RAD);
	}
	lcEntMove(handle, m_offset.x, m_offset.y);
	return handle;
}

void CEntyEllipse::SetDrawEntity(HANDLE hEntity)
{
	CEntity::SetDrawEntity(hEntity);
	

	center.x = lcPropGetFloat(hEntity, LC_PROP_ELL_X);
	center.y = lcPropGetFloat(hEntity, LC_PROP_ELL_Y);
	Rmajor = lcPropGetFloat(hEntity, LC_PROP_ELL_R1);
	Rminor = lcPropGetFloat(hEntity, LC_PROP_ELL_R2);
	StartAngle = lcPropGetFloat(hEntity, LC_PROP_ELL_ANG0)* LC_RAD_TO_DEG;;
	ArcAngle = lcPropGetFloat(hEntity, LC_PROP_ELL_ANGARC)* LC_RAD_TO_DEG;;
	RotAngle = lcPropGetFloat(hEntity, LC_PROP_ELL_ANGLE)* LC_RAD_TO_DEG;;
	if (m_pColorID)
		lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);
	UpdatePath(hEntity);
	TRACE("SetDrawEntity Pts = %d\n", m_path.size());

}

void CEntyEllipse::DisplayEntityProperties(CMFCPropertyGridCtrl *pProp)
{
	CEntity::DisplayEntityProperties(pProp);
	CMFCPropertyGridProperty* jobInfo = new CMFCPropertyGridProperty(_T("Entity Parameter"));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("center x"), (_variant_t)center.x, L"center x", (DWORD_PTR)&center.x));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("center y"), (_variant_t)center.y, L"center y", (DWORD_PTR)&center.y));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Rmajor"), (_variant_t)Rmajor, L"Rmajor", (DWORD_PTR)&Rmajor));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Rminor"), (_variant_t)Rminor, L"Rminor", (DWORD_PTR)&Rminor));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("RotAngle"), (_variant_t)RotAngle, L"RotAngle", (DWORD_PTR)&RotAngle));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("StartAngle"), (_variant_t)StartAngle, L"StartAngle", (DWORD_PTR)&StartAngle));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("ArcAngle"), (_variant_t)ArcAngle, L"ArcAngle", (DWORD_PTR)&ArcAngle));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("CW(0)/CCW(1)"), (_variant_t)iDir, L"direction", (DWORD_PTR)&iDir));


	pProp->AddProperty(jobInfo);
}

void CEntyEllipse::UpdateEntity(DWORD_PTR dwData, COleVariant value)
{
	lcEventsEnable(FALSE);
	lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);

	lcPropPutFloat(m_hCadEntity, LC_PROP_ELL_X, center.x);
	lcPropPutFloat(m_hCadEntity, LC_PROP_ELL_Y, center.y);
	lcPropPutFloat(m_hCadEntity, LC_PROP_ELL_R1, Rmajor);
	lcPropPutFloat(m_hCadEntity, LC_PROP_ELL_R2, Rminor);
	lcPropPutFloat(m_hCadEntity, LC_PROP_ELL_ANG0, StartAngle*LC_DEG_TO_RAD);
	lcPropPutFloat(m_hCadEntity, LC_PROP_ELL_ANGARC, ArcAngle*LC_DEG_TO_RAD);
	lcPropPutFloat(m_hCadEntity, LC_PROP_ELL_ANGLE, RotAngle*LC_DEG_TO_RAD);
	lcEntMove(m_hCadEntity, m_offset.x, m_offset.y);
	lcEntUpdate(m_hCadEntity);
	lcWndRedraw(g_hLcWnd);
	UpdatePath(m_hCadEntity);
	lcEventsEnable(TRUE);
}

void CEntyEllipse::ChangedEntity(HANDLE hEntity)
{
	ASSERT(hEntity == m_hCadEntity);
	center.x = lcPropGetFloat(hEntity, LC_PROP_ELL_X);
	center.y = lcPropGetFloat(hEntity, LC_PROP_ELL_Y);
	Rmajor = lcPropGetFloat(hEntity, LC_PROP_ELL_R1);
	Rminor = lcPropGetFloat(hEntity, LC_PROP_ELL_R2);
	StartAngle = lcPropGetFloat(hEntity, LC_PROP_ELL_ANG0)* LC_RAD_TO_DEG;;
	ArcAngle = lcPropGetFloat(hEntity, LC_PROP_ELL_ANGARC)* LC_RAD_TO_DEG;;
	RotAngle = lcPropGetFloat(hEntity, LC_PROP_ELL_ANGLE)* LC_RAD_TO_DEG;;
	m_offset.x = 0;
	m_offset.y = 0;
	UpdatePath(hEntity);
}

void CEntyEllipse::UpdateCenter()
{
	m_cen = center;
}

CEntyEllipse CEntyEllipse::operator=(CEntyEllipse& rhs)
{
	CEntity::operator=(rhs);
	center = rhs.center;
	Rmajor = rhs.Rmajor;
	Rminor = rhs.Rminor;
	RotAngle = rhs.RotAngle;
	StartAngle = rhs.StartAngle;
	ArcAngle = rhs.ArcAngle;
	return *this;
}

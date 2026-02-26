#include "stdafx.h"
#include "IWS.h"
#include "EntyArc.h"


CEntyArc::CEntyArc()
{
	radius = 0.0;
	angle_st = 0.0;
	angle_arc = 0.0;
	m_strName.Format(L"Arc_%d", m_iEntCount);
	iDir = false;
}


CEntyArc::~CEntyArc()
{
}

void CEntyArc::UpdatePath(HANDLE hEntity)
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


BOOL CEntyArc::Save(CFile *pf, int iver)
{
	CStringA strName(m_strName);
	int nStr = strName.GetLength();
	pf->Write(&nStr, sizeof(int));
	pf->Write(strName.GetBuffer(), sizeof(char)*nStr);
	pf->Write(&center, sizeof(Point3Dbl));
	pf->Write(&angle_arc, sizeof(double));
	pf->Write(&angle_st, sizeof(double));
	pf->Write(&radius, sizeof(double));
	pf->Write(&iDir, sizeof(bool));
	return TRUE;
}

BOOL CEntyArc::Load(CFile *pf, int iver)
{
	int nStr;
	char tmp[100] = "";
	pf->Read(&nStr, sizeof(int));
	pf->Read(&tmp, sizeof(char)*nStr);
	CString str(tmp);
	m_strName = str;
	pf->Read(&center, sizeof(Point3Dbl));
	pf->Read(&angle_arc, sizeof(double));
	pf->Read(&angle_st, sizeof(double));
	pf->Read(&radius, sizeof(double));
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

HANDLE CEntyArc::AddCurrEntityToBlock(HANDLE hBlock)
{
	HANDLE handle;
	if(m_hCadEntity)
		handle = lcBlockAddClone(hBlock, m_hCadEntity);
	else
		handle = lcBlockAddArc(hBlock, center.x, center.y, radius, angle_st*LC_DEG_TO_RAD, angle_arc*LC_DEG_TO_RAD);

	lcEntMove(handle, m_offset.x, m_offset.y);
	return handle;
}

void CEntyArc::SetDrawEntity(HANDLE hEntity)
{
	CEntity::SetDrawEntity(hEntity);
	
	
	center.x = lcPropGetFloat(hEntity, LC_PROP_ARC_X);
	center.y = lcPropGetFloat(hEntity, LC_PROP_ARC_Y);
	radius = lcPropGetFloat(hEntity, LC_PROP_ARC_RAD);
	angle_st = lcPropGetFloat(hEntity, LC_PROP_ARC_ANG0) * LC_RAD_TO_DEG;
	angle_arc = lcPropGetFloat(hEntity, LC_PROP_ARC_ANGARC)* LC_RAD_TO_DEG;


	UpdatePath(hEntity);
	if (m_pColorID)
		lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);
	TRACE("Polyline Pts = %d\n", m_path.size());

}

void CEntyArc::DisplayEntityProperties(CMFCPropertyGridCtrl *pProp)
{
	CEntity::DisplayEntityProperties(pProp);
	CMFCPropertyGridProperty* jobInfo = new CMFCPropertyGridProperty(_T("Entity Parameter"));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("start angel"), (_variant_t)angle_st, L"start angle", (DWORD_PTR)&angle_st));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("arc angel"), (_variant_t)angle_arc, L"arc angle", (DWORD_PTR)&angle_arc));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("center x"), (_variant_t)center.x, L"center x", (DWORD_PTR)&center.x));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("center y"), (_variant_t)center.y, L"center y", (DWORD_PTR)&center.y));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("radius"), (_variant_t)radius, L"radius", (DWORD_PTR)&radius));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("CW(0)/CCW(1)"), (_variant_t)iDir, L"direction", (DWORD_PTR)&iDir));

	pProp->AddProperty(jobInfo);

}

void CEntyArc::UpdateEntity(DWORD_PTR dwData, COleVariant value)
{
	lcEventsEnable(FALSE);
	lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);

	lcPropPutFloat(m_hCadEntity, LC_PROP_ARC_X, center.x);
	lcPropPutFloat(m_hCadEntity, LC_PROP_ARC_Y, center.y);
	lcPropPutFloat(m_hCadEntity, LC_PROP_ARC_RAD, radius);
	lcPropPutFloat(m_hCadEntity, LC_PROP_ARC_ANG0, angle_st*LC_DEG_TO_RAD);
	lcPropPutFloat(m_hCadEntity, LC_PROP_ARC_ANGARC, angle_arc*LC_DEG_TO_RAD);
	lcEntUpdate(m_hCadEntity);
	UpdatePath(m_hCadEntity);
	lcWndRedraw(g_hLcWnd);
	lcEventsEnable(TRUE);

}

void CEntyArc::ChangedEntity(HANDLE hEntity)
{
	ASSERT(hEntity == m_hCadEntity);
	center.x = lcPropGetFloat(m_hCadEntity, LC_PROP_ARC_X);
	center.y = lcPropGetFloat(m_hCadEntity, LC_PROP_ARC_Y);
	radius = lcPropGetFloat(m_hCadEntity, LC_PROP_ARC_RAD);
	angle_st = lcPropGetFloat(m_hCadEntity, LC_PROP_ARC_ANG0)*LC_RAD_TO_DEG;
	angle_arc = lcPropGetFloat(m_hCadEntity, LC_PROP_ARC_ANGARC)*LC_RAD_TO_DEG;
	m_offset.x = 0;
	m_offset.y = 0;
	UpdatePath(hEntity);
}

void CEntyArc::UpdateCenter()
{
	m_cen = center;
}

CEntyArc CEntyArc::operator=(CEntyArc& rhs)
{
	CEntity::operator=(rhs);
	this->radius = rhs.radius;
	this->angle_st = rhs.angle_st;
	this->angle_arc = rhs.angle_arc;
	this->center = rhs.center;
	return *this;
}

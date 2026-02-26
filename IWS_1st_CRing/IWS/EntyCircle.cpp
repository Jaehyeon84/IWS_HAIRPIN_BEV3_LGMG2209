#include "stdafx.h"
#include "IWS.h"
#include "EntyCircle.h"


CEntyCircle::CEntyCircle()
{
	m_strName.Format(L"Circle_%d", m_iEntCount);
	overpath = 0.0;
}


CEntyCircle::~CEntyCircle()
{
}

CEntyCircle CEntyCircle::operator=(CEntyCircle& rhs)
{
	CEntity::operator=(rhs);
	return *this;
}

void CEntyCircle::UpdatePath(HANDLE hEntity)
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
	if (overpath > 0.1) {
		int nOver = int(double(m_path.size())*overpath /100.0);
		for (int i = 0; i < nOver; i++) {
			m_path.push_back(m_path[i]);
		}
	}
	UpdateCenter();
	TRACE("UpdatePath Pts = %d\n", m_path.size());
}

BOOL CEntyCircle::Save(CFile *pf, int iver)
{
	CStringA strName(m_strName);
	int nStr = strName.GetLength();
	pf->Write(&nStr, sizeof(int));
	pf->Write(strName.GetBuffer(), sizeof(char)*nStr);
	pf->Write(&center, sizeof(Point3Dbl));
	pf->Write(&radius, sizeof(double));
	pf->Write(&iDir, sizeof(bool));
	pf->Write(&StartAngle, sizeof(double));
	pf->Write(&overpath, sizeof(double)); // WELDJOB_VER100

	return TRUE;
}

BOOL CEntyCircle::Load(CFile *pf, int iver)
{
	int nStr;
	char tmp[100] = "";
	pf->Read(&nStr, sizeof(int));
	pf->Read(&tmp, sizeof(char)*nStr);
	CString str(tmp);
	m_strName = str;
	pf->Read(&center, sizeof(Point3Dbl));
	pf->Read(&radius, sizeof(double));
	pf->Read(&iDir, sizeof(bool));
	pf->Read(&StartAngle, sizeof(double));
	if (iver >= WELDJOB_VER100) {
		pf->Read(&overpath, sizeof(double));
	}
	else
		overpath = 0.0;
	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = AddCurrEntityToBlock(hBlock);
	SetDrawEntity(hEnt);
	lcEventsEnable(TRUE);
	return TRUE;
}

HANDLE CEntyCircle::AddCurrEntityToBlock(HANDLE hBlock)
{
	HANDLE handle;
	if(m_hCadEntity)
		handle = lcBlockAddClone(hBlock, m_hCadEntity);
	else {
		handle = lcBlockAddCircle(hBlock, center.x, center.y, radius, FALSE);
		lcPropPutFloat(handle, LC_PROP_CIRCLE_ANG0, StartAngle * LC_DEG_TO_RAD);
		lcPropPutBool(handle, LC_PROP_CIRCLE_DIRCW, iDir);
	}
	lcEntMove(handle, m_offset.x, m_offset.y);
	return handle;
}

void CEntyCircle::SetDrawEntity(HANDLE hEntity)
{
	CEntity::SetDrawEntity(hEntity);	
	if (m_pColorID)
		lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);
	ChangedEntity(hEntity);
}

void CEntyCircle::DisplayEntityProperties(CMFCPropertyGridCtrl *pProp)
{
	CEntity::DisplayEntityProperties(pProp);
	CMFCPropertyGridProperty* jobInfo = new CMFCPropertyGridProperty(_T("Entity Parameter"));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("center x"), (_variant_t)center.x, L"center x", (DWORD_PTR)&center.x));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("center y"), (_variant_t)center.y, L"center y", (DWORD_PTR)&center.y));
	//jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("radius"), (_variant_t)radius, L"radius", (DWORD_PTR)&radius));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("diameter"), (_variant_t)diameter, L"diameter", (DWORD_PTR)&diameter));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("CW(0)/CCW(1)"), (_variant_t)iDir, L"direction", (DWORD_PTR)&iDir));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("start angle"), (_variant_t)StartAngle, L"start angle", (DWORD_PTR)&StartAngle));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("over path(%)"), (_variant_t)overpath, L"over path(%)", (DWORD_PTR)&overpath));

	pProp->AddProperty(jobInfo);
}

void CEntyCircle::UpdateEntity(DWORD_PTR dwData, COleVariant value)
{
	lcEventsEnable(FALSE);
	lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);

	lcPropPutFloat(m_hCadEntity, LC_PROP_CIRCLE_X, center.x);
	lcPropPutFloat(m_hCadEntity, LC_PROP_CIRCLE_Y, center.y);
	//lcPropPutFloat(m_hCadEntity, LC_PROP_CIRCLE_RAD, radius);
	radius = diameter / 2.0;
	lcPropPutFloat(m_hCadEntity, LC_PROP_CIRCLE_DIAM, diameter);
	lcPropPutFloat(m_hCadEntity, LC_PROP_CIRCLE_ANG0, StartAngle * LC_DEG_TO_RAD);
	lcPropPutBool(m_hCadEntity, LC_PROP_CIRCLE_DIRCW, iDir);

	lcEntMove(m_hCadEntity, m_offset.x, m_offset.y);
	lcEntUpdate(m_hCadEntity);
	UpdatePath(m_hCadEntity);
	lcWndRedraw(g_hLcWnd);
	lcEventsEnable(TRUE);

}

void CEntyCircle::ChangedEntity(HANDLE hEntity)
{
	ASSERT(hEntity == m_hCadEntity);
	center.x = lcPropGetFloat(m_hCadEntity, LC_PROP_CIRCLE_X);
	center.y = lcPropGetFloat(m_hCadEntity, LC_PROP_CIRCLE_Y);
	//radius = lcPropGetFloat(m_hCadEntity, LC_PROP_CIRCLE_RAD);
	diameter = lcPropGetFloat(m_hCadEntity, LC_PROP_CIRCLE_DIAM);
	radius = diameter / 2.0;
	StartAngle = lcPropGetFloat(m_hCadEntity, LC_PROP_CIRCLE_ANG0) * LC_RAD_TO_DEG;;
	iDir = lcPropGetBool(m_hCadEntity, LC_PROP_CIRCLE_DIRCW);


	m_offset.x = 0;
	m_offset.y = 0;
	UpdatePath(hEntity);
}

void CEntyCircle::UpdateCenter()
{
	m_cen = center;
}

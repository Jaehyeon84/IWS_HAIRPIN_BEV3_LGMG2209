#include "stdafx.h"
#include "IWS.h"
#include "EntyPoint.h"


CEntyPoint::CEntyPoint()
{
	m_strName.Format(L"Point_%d", m_iEntCount);
	delay = 100;
}


CEntyPoint::~CEntyPoint()
{
}

BOOL CEntyPoint::Save(CFile *pf, int iver)
{
	CStringA strName(m_strName);
	int nStr = strName.GetLength();
	pf->Write(&nStr, sizeof(int));
	pf->Write(strName.GetBuffer(), sizeof(char)*nStr);
	pf->Write(&point0, sizeof(Point3Dbl));
	if (iver >= WELDJOB_VER100) {
		pf->Write(&delay, sizeof(int));
	}
	return TRUE;
}

BOOL CEntyPoint::Load(CFile *pf, int iver)
{
	int nStr;
	char tmp[100] = "";
	pf->Read(&nStr, sizeof(int));
	pf->Read(&tmp, sizeof(char)*nStr);
	CString str(tmp);
	m_strName = str;
	pf->Read(&point0, sizeof(Point3Dbl));
	if (iver >= WELDJOB_VER100) {
		pf->Read(&delay, sizeof(int));
	}
	else
		delay = (int)100.0;
	lcEventsEnable(FALSE);

	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = AddCurrEntityToBlock(hBlock);
	SetDrawEntity(hEnt);
	lcEventsEnable(TRUE);

	return TRUE;
}

CEntyPoint CEntyPoint::operator=(CEntyPoint& rhs)
{
	CEntity::operator=(rhs);
	return *this;
}

HANDLE CEntyPoint::AddCurrEntityToBlock(HANDLE hBlock)
{
	HANDLE handle;
	if(m_hCadEntity)
		handle = lcBlockAddClone(hBlock, m_hCadEntity);
	else {
		handle = lcBlockAddPoint(hBlock, point0.x, point0.y);
	}
	lcEntMove(handle, m_offset.x, m_offset.y);
	return handle;
}

void CEntyPoint::SetDrawEntity(HANDLE hEntity)
{
	CEntity::SetDrawEntity(hEntity);
	if (m_pColorID)
		lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);
	ChangedEntity(hEntity);
}

void CEntyPoint::DisplayEntityProperties(CMFCPropertyGridCtrl *pProp) 
{
	CEntity::DisplayEntityProperties(pProp);
	CMFCPropertyGridProperty* jobInfo = new CMFCPropertyGridProperty(_T("Entity Parameter"));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("point x"), (_variant_t)point0.x, L"point x", (DWORD_PTR)&point0.x));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("point y"), (_variant_t)point0.y, L"point y", (DWORD_PTR)&point0.y));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("delay(ms)"), (_variant_t)delay, L"delay time", (DWORD_PTR)&delay));

	pProp->AddProperty(jobInfo);
}

void CEntyPoint::UpdateEntity(DWORD_PTR dwData, COleVariant value)
{
	lcEventsEnable(FALSE);
	lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);

	lcPropPutFloat(m_hCadEntity, LC_PROP_POINT_X, point0.x);
	lcPropPutFloat(m_hCadEntity, LC_PROP_POINT_Y, point0.y);
	lcEntMove(m_hCadEntity, m_offset.x, m_offset.y);
	lcEntUpdate(m_hCadEntity);
	lcWndRedraw(g_hLcWnd);
	UpdatePath(m_hCadEntity);
	lcEventsEnable(TRUE);

}

void CEntyPoint::ChangedEntity(HANDLE hEntity)
{
	ASSERT(hEntity == m_hCadEntity);
	point0.x = lcPropGetFloat(m_hCadEntity, LC_PROP_POINT_X);
	point0.y = lcPropGetFloat(m_hCadEntity, LC_PROP_POINT_Y);
	m_offset.x = 0;
	m_offset.y = 0;
	UpdatePath(hEntity);
}

void CEntyPoint::UpdatePath(HANDLE hEntity)
{
	m_path.clear();
	m_path.push_back(point0);
	m_path.push_back(point0);
	UpdateCenter();
}

void CEntyPoint::UpdateCenter()
{
	m_cen = point0;
}


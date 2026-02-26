#include "stdafx.h"
#include "IWS.h"
#include "EntityRect.h"


CEntityRect::CEntityRect()
{
	m_strName.Format(L"Rect_%d", m_iEntCount);
	radius = 0;
}


CEntityRect::~CEntityRect()
{
}

BOOL CEntityRect::Save(CFile *pf, int iver)
{
	CStringA strName(m_strName);
	int nStr = strName.GetLength();
	pf->Write(&nStr, sizeof(int));
	pf->Write(strName.GetBuffer(), sizeof(char)*nStr);
	pf->Write(&center, sizeof(Point3Dbl));	
	pf->Write(&width, sizeof(double));
	pf->Write(&height, sizeof(double));
	pf->Write(&angle, sizeof(double));
	pf->Write(&iDir, sizeof(bool));
	pf->Write(&radius, sizeof(double));
	return TRUE;
}

BOOL CEntityRect::Load(CFile *pf, int iver)
{
	int nStr;
	char tmp[100] = "";
	pf->Read(&nStr, sizeof(int));
	pf->Read(&tmp, sizeof(char)*nStr);
	CString str(tmp);
	m_strName = str;
	pf->Read(&center, sizeof(Point3Dbl));
	pf->Read(&width, sizeof(double));
	pf->Read(&height, sizeof(double));
	pf->Read(&angle, sizeof(double));
	if (iver < WELDJOB_VER102) {
		radius = 0;
		iDir = false;
	}
	else {
		pf->Read(&iDir, sizeof(bool));
		pf->Read(&radius, sizeof(double));
	}
	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = AddCurrEntityToBlock(hBlock);
	SetDrawEntity(hEnt);
	lcEventsEnable(TRUE);

	return TRUE;
}

HANDLE CEntityRect::AddCurrEntityToBlock(HANDLE hBlock)
{
	HANDLE handle;
	if(m_hCadEntity)
		handle = lcBlockAddClone(hBlock, m_hCadEntity);
	else
		handle = lcBlockAddRect2(hBlock, center.x - width * 0.5, center.y - height * 0.5, width, height, radius, FALSE);
		//handle = lcBlockAddRect(hBlock, center.x, center.y, width, height, angle, FALSE);
	lcEntMove(handle, m_offset.x, m_offset.y);
	return handle;
}

void CEntityRect::SetDrawEntity(HANDLE hEntity)
{
	CEntity::SetDrawEntity(hEntity);
	if (m_pColorID)
		lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);
	ChangedEntity(hEntity);
}

void CEntityRect::DisplayEntityProperties(CMFCPropertyGridCtrl *pProp)
{
	CEntity::DisplayEntityProperties(pProp);
	CMFCPropertyGridProperty* jobInfo = new CMFCPropertyGridProperty(_T("Entity Parameter"));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Center x"), (_variant_t)center.x, L"Center x", (DWORD_PTR)&center.x));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Center y"), (_variant_t)center.y, L"Center y", (DWORD_PTR)&center.y));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Width"), (_variant_t)width, L"Width", (DWORD_PTR)&width));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Height"), (_variant_t)height, L"Height", (DWORD_PTR)&height));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Angle"), (_variant_t)angle, L"Angle", (DWORD_PTR)&angle));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Corners radius"), (_variant_t)radius, L"Corners radius", (DWORD_PTR)&radius));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("CW(0)/CCW(1)"), (_variant_t)iDir, L"direction", (DWORD_PTR)&iDir));

	pProp->AddProperty(jobInfo);
}

void CEntityRect::UpdateEntity(DWORD_PTR dwData, COleVariant value)
{
	lcEventsEnable(FALSE);
	lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);
	lcPropPutFloat(m_hCadEntity, LC_PROP_RECT_X, center.x);
	lcPropPutFloat(m_hCadEntity, LC_PROP_RECT_Y, center.y);
	lcPropPutFloat(m_hCadEntity, LC_PROP_RECT_W, width);
	lcPropPutFloat(m_hCadEntity, LC_PROP_RECT_H, height);
	lcPropPutFloat(m_hCadEntity, LC_PROP_RECT_ANGLE, angle*LC_DEG_TO_RAD);
	lcPropPutFloat(m_hCadEntity, LC_PROP_RECT_RAD, radius);
	lcEntMove(m_hCadEntity, m_offset.x, m_offset.y);
	lcEntUpdate(m_hCadEntity);
	lcWndRedraw(g_hLcWnd);
	UpdatePath(m_hCadEntity);
	lcEventsEnable(TRUE);
}

void CEntityRect::ChangedEntity(HANDLE hEntity)
{
	ASSERT(hEntity == m_hCadEntity);
	center.x = lcPropGetFloat(m_hCadEntity, LC_PROP_RECT_X);
	center.y = lcPropGetFloat(m_hCadEntity, LC_PROP_RECT_Y);
	width = lcPropGetFloat(m_hCadEntity, LC_PROP_RECT_W);
	height = lcPropGetFloat(m_hCadEntity, LC_PROP_RECT_H);
	angle = lcPropGetFloat(m_hCadEntity, LC_PROP_RECT_ANGLE)*LC_RAD_TO_DEG;
	radius = lcPropGetFloat(m_hCadEntity, LC_PROP_RECT_RAD);

	m_offset.x = 0;
	m_offset.y = 0;
	UpdatePath(hEntity);
}

void CEntityRect::UpdatePath(HANDLE hEntity)
{
	m_path.clear();
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
	TRACE("UpdatePath Pts = %d\n", m_path.size());
}

void CEntityRect::UpdateCenter()
{
	m_cen = center;
}

CEntityRect CEntityRect::operator=(CEntityRect& rhs)
{
	this->center = rhs.center;
	this->width = rhs.width;
	this->height= rhs.height;
	this->angle= rhs.angle;
	return *this;
}

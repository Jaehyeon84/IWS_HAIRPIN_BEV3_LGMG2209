#include "stdafx.h"
#include "IWS.h"
#include "EntyLine.h"


CEntyLine::CEntyLine()
{
	m_strName.Format(L"Line_%d", m_iEntCount);
	iDir = false;
}


CEntyLine::~CEntyLine()
{
}

CEntyLine CEntyLine::operator=(CEntyLine& rhs)
{
	return *this;
}

BOOL CEntyLine::Save(CFile *pf, int iver)
{
	CStringA strName(m_strName);
	int nStr = strName.GetLength();
	pf->Write(&nStr, sizeof(int));
	pf->Write(strName.GetBuffer(), sizeof(char)*nStr);
	pf->Write(&point0, sizeof(Point3Dbl));
	pf->Write(&point1, sizeof(Point3Dbl));
	pf->Write(&length, sizeof(double));
	pf->Write(&iDir, sizeof(bool));
	return TRUE;
}

BOOL CEntyLine::Load(CFile *pf, int iver)
{
	int nStr;
	char tmp[100]="";
	pf->Read(&nStr, sizeof(int));
	pf->Read(&tmp, sizeof(char)*nStr);
	CString str(tmp);
	m_strName = str;
	pf->Read(&point0, sizeof(Point3Dbl));
	pf->Read(&point1, sizeof(Point3Dbl));
	pf->Read(&length, sizeof(double));
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

HANDLE CEntyLine::AddCurrEntityToBlock(HANDLE hBlock)
{
	HANDLE handle;
	if (m_hCadEntity)
		handle = lcBlockAddClone(hBlock, m_hCadEntity);
	else {
		handle = lcBlockAddLine(hBlock, point0.x, point0.y, point1.x, point1.y);
	}
	lcEntMove(handle, m_offset.x, m_offset.y);
	return handle;
}

void CEntyLine::SetDrawEntity(HANDLE hEntity)
{
	CEntity::SetDrawEntity(hEntity);

	if(m_pColorID) 
		lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);
	ChangedEntity(hEntity);
	//TRACE("Polyline Pts = %d\n", m_path.size());
}

void CEntyLine::DisplayEntityProperties(CMFCPropertyGridCtrl *pProp)
{
	CEntity::DisplayEntityProperties(pProp);
	CMFCPropertyGridProperty* jobInfo = new CMFCPropertyGridProperty(_T("Entity Parameter"));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("start x"), (_variant_t)point0.x, L"start x", (DWORD_PTR)&point0.x));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("start y"), (_variant_t)point0.y, L"start y", (DWORD_PTR)&point0.y));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("end x"), (_variant_t)point1.x, L"end x", (DWORD_PTR)&point1.x));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("end y"), (_variant_t)point1.y, L"end y", (DWORD_PTR)&point1.y));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("length"), (_variant_t)length, L"length", (DWORD_PTR)&length));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("CW(0)/CCW(1)"), (_variant_t)iDir, L"direction", (DWORD_PTR)&iDir));

	pProp->AddProperty(jobInfo);
}

void CEntyLine::UpdateEntity(DWORD_PTR dwData, COleVariant value)
{
	lcEventsEnable(FALSE);
	lcPropPutFloat(m_hCadEntity, LC_PROP_LINE_X0,point0.x);
	lcPropPutFloat(m_hCadEntity, LC_PROP_LINE_Y0,point0.y);
	lcPropPutFloat(m_hCadEntity, LC_PROP_LINE_X1, point1.x);
	lcPropPutFloat(m_hCadEntity, LC_PROP_LINE_Y1, point1.y);
	lcPropPutFloat(m_hCadEntity, LC_PROP_LINE_LEN, length);
	lcEntMove(m_hCadEntity, m_offset.x, m_offset.y);
// 	lcPropPutBool(m_hCadEntity, LC_PROP_ENT_COLORT, TRUE);
	lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);
	lcEntUpdate(m_hCadEntity);
	lcWndRedraw(g_hLcWnd);
	UpdatePath(m_hCadEntity);
	lcEventsEnable(TRUE);
}

void CEntyLine::ChangedEntity(HANDLE hEntity)
{
	ASSERT(hEntity == m_hCadEntity);

	point0.x = lcPropGetFloat(hEntity, LC_PROP_LINE_X0);
	point0.y = lcPropGetFloat(hEntity, LC_PROP_LINE_Y0);
	point1.x = lcPropGetFloat(hEntity, LC_PROP_LINE_X1);
	point1.y = lcPropGetFloat(hEntity, LC_PROP_LINE_Y1);
	m_offset.x = 0;
	m_offset.y = 0;
	length = lcPropGetFloat(hEntity, LC_PROP_LINE_LEN); 	
	UpdatePath(hEntity);
}

void CEntyLine::UpdatePath(HANDLE hEntity)
{
	m_path.clear();
	if (iDir) {
		m_path.push_back(point1);
		m_path.push_back(point0);
	}
	else {
		m_path.push_back(point0);
		m_path.push_back(point1);
	}
	UpdateCenter();
}

void CEntyLine::UpdateCenter()
{
	m_cen.x = point0.x + point1.x;
	m_cen.x = m_cen.x * 0.5;
	m_cen.y = point0.y + point1.y;
	m_cen.y = m_cen.y * 0.5;
}

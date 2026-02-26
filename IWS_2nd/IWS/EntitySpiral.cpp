#include "stdafx.h"
#include "IWS.h"
#include "EntitySpiral.h"
#include <math.h>
#define M_PI 3.14159265358979323846 
#define RES_SPDEG 5 // 5�� �������� �����̷� ����

CEntitySpiral::CEntitySpiral()
{
	
	m_radiusX = 10.0;
	m_radiusY = 10.0;
	m_resolution = 128;
	m_rotAng = 0.0;
	m_center.x = m_center.y = m_center.z = 0.0;
	m_turn = 10;
	m_bClose = FALSE;
	m_dirOut2In = false;
	m_strName.Format(L"Spiral_%d", m_iEntCount);;
}


CEntitySpiral::~CEntitySpiral()
{
}

CEntitySpiral CEntitySpiral::operator=(CEntitySpiral& rhs)
{
	m_center = rhs.m_center;
	m_radiusX = rhs.m_radiusX;
	m_radiusY = rhs.m_radiusY;
	m_turn = rhs.m_turn;
	m_resolution = rhs.m_resolution;
	m_rotAng = rhs.m_rotAng;
	m_bClose = rhs.m_bClose;
	m_dirOut2In = rhs.m_dirOut2In;
	return *this;
}
void CEntitySpiral::UpdatePath(HANDLE hEntity)
{
	int nPlines = lcExpEntity(hEntity, LC_EXP_ALL, false);
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
	if (m_dirOut2In)
		std::reverse(m_path.begin(), m_path.end());

	// for debug 
// 	int idx = 0;
// 	for(auto p:m_path)
// 		TRACE("i, Pts = %f,%f\n", idx++, p.x, p.y);

	TRACE("Extrated Polyline Pts = %d\n", m_path.size());

}

BOOL CEntitySpiral::Save(CFile *pf, int iver)
{
	CStringA strName(m_strName);
	int nStr = strName.GetLength();
	pf->Write(&nStr, sizeof(int));
	pf->Write(strName.GetBuffer(), sizeof(char)*nStr);
	pf->Write(&m_center, sizeof(Point3Dbl));
	pf->Write(&m_radiusX, sizeof(double));
	pf->Write(&m_radiusY, sizeof(double));
	pf->Write(&m_turn, sizeof(double));
	pf->Write(&m_rotAng, sizeof(double));
	pf->Write(&m_resolution, sizeof(double));
	pf->Write(&m_bClose, sizeof(bool));
	pf->Write(&m_dirOut2In, sizeof(bool));
	return TRUE;
}

BOOL CEntitySpiral::Load(CFile *pf, int iver)
{
	int nStr;
	char tmp[100] = "";
	pf->Read(&nStr, sizeof(int));
	pf->Read(&tmp, sizeof(char)*nStr);
	CString str(tmp);
	m_strName = str;
	pf->Read(&m_center, sizeof(Point3Dbl));
	pf->Read(&m_radiusX, sizeof(double));
	pf->Read(&m_radiusY, sizeof(double));
	pf->Read(&m_turn, sizeof(double));
	pf->Read(&m_rotAng, sizeof(double));
	pf->Read(&m_resolution, sizeof(double));
	pf->Read(&m_bClose, sizeof(bool));
	if (iver >= WELDJOB_VER101)
		pf->Read(&m_dirOut2In, sizeof(bool));
	else
		m_dirOut2In = false;

	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hPline = AddCurrEntityToBlock(hBlock);
	SetDrawEntity(hPline);
	lcEventsEnable(TRUE);
	return TRUE;
}

HANDLE CEntitySpiral::AddCurrEntityToBlock(HANDLE hBlock)
{
	HANDLE handle;
	if(m_hCadEntity)
		handle = lcBlockAddClone(hBlock, m_hCadEntity);
	else {
		handle = lcBlockAddSpiral(hBlock, m_center.x, m_center.y, m_radiusX, m_turn, TRUE, m_bClose);
		lcPropPutFloat(handle, LC_PROP_SPIR_R2, m_radiusY);
	}
	lcEntMove(handle, m_offset.x, m_offset.y);
	return handle;
}

void CEntitySpiral::SetDrawEntity(HANDLE hEntity)
{
	CEntity::SetDrawEntity(hEntity);

	m_center.x = lcPropGetFloat(m_hCadEntity, LC_PROP_SPIR_X);
	m_center.y = lcPropGetFloat(m_hCadEntity, LC_PROP_SPIR_Y);
	m_center.z = 0;
	m_radiusX = lcPropGetFloat(m_hCadEntity, LC_PROP_SPIR_R);
	m_radiusY = lcPropGetFloat(m_hCadEntity, LC_PROP_SPIR_R2);
	m_rotAng = lcPropGetFloat(m_hCadEntity, LC_PROP_SPIR_ANGLE);;
	m_resolution = lcPropGetInt(m_hCadEntity, LC_PROP_SPIR_RESOL);;
	m_turn = lcPropGetFloat(m_hCadEntity, LC_PROP_SPIR_TURNS);;
	m_bClose = lcPropGetBool(m_hCadEntity, LC_PROP_SPIR_CLOSED);
	UpdatePath(hEntity);
	lcEntUpdate(m_hCadEntity);
	lcWndRedraw(g_hLcWnd);
	if (m_pColorID)
		lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);
}

void CEntitySpiral::DisplayEntityProperties(CMFCPropertyGridCtrl *pProp)
{
	CEntity::DisplayEntityProperties(pProp);
	CMFCPropertyGridProperty* jobInfo = new CMFCPropertyGridProperty(_T("Entity Parameter"));

	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Center x"), (_variant_t)m_center.x, L"Center x", (DWORD_PTR)&m_center.x));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Center y"), (_variant_t)m_center.y, L"Center y", (DWORD_PTR)&m_center.y));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Radius"), (_variant_t)m_radiusX, L"Radius X", (DWORD_PTR)&m_radiusX));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Radius1"), (_variant_t)m_radiusY, L"Radius Y", (DWORD_PTR)&m_radiusY));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Num. turn"), (_variant_t)m_turn, L"Num. turn", (DWORD_PTR)&m_turn));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Rotation Ang."), (_variant_t)m_rotAng, L"Rotation Ang.", (DWORD_PTR)&m_rotAng));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Resolution"), (_variant_t)m_resolution, L"Resolution", (DWORD_PTR)&m_resolution));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Outter Circle"), (_variant_t)m_bClose, L"Outter Circle", (DWORD_PTR)&m_bClose));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Outside to Inside"), (_variant_t)m_dirOut2In, L"Weld direction", (DWORD_PTR)&m_dirOut2In));

	pProp->AddProperty(jobInfo);
}

void CEntitySpiral::UpdateEntity(DWORD_PTR dwData, COleVariant value)
{
	lcEventsEnable(FALSE);
	lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);

	lcPropPutFloat(m_hCadEntity, LC_PROP_SPIR_X, m_center.x);
	lcPropPutFloat(m_hCadEntity, LC_PROP_SPIR_Y, m_center.y);
	lcPropPutFloat(m_hCadEntity, LC_PROP_SPIR_R, m_radiusX);
	lcPropPutFloat(m_hCadEntity, LC_PROP_SPIR_R2, m_radiusY);
	lcPropPutFloat(m_hCadEntity, LC_PROP_SPIR_ANGLE, m_rotAng);
	lcPropPutInt(m_hCadEntity, LC_PROP_SPIR_RESOL, m_resolution);
	lcPropPutFloat(m_hCadEntity, LC_PROP_SPIR_TURNS, m_turn);
	lcPropPutBool(m_hCadEntity, LC_PROP_SPIR_CLOSED, m_bClose);
	lcEntMove(m_hCadEntity, m_offset.x, m_offset.y);
	lcEntUpdate(m_hCadEntity);
	UpdatePath(m_hCadEntity);
	lcWndRedraw(g_hLcWnd);	
	lcEventsEnable(TRUE);
}

void CEntitySpiral::ChangedEntity(HANDLE hEntity)
{
	ASSERT(hEntity == m_hCadEntity);
	
	m_center.x = lcPropGetFloat(m_hCadEntity, LC_PROP_SPIR_X);
	m_center.y = lcPropGetFloat(m_hCadEntity, LC_PROP_SPIR_Y);
	m_center.z = 0;
	m_radiusX = lcPropGetFloat(m_hCadEntity, LC_PROP_SPIR_R);
	m_radiusY = lcPropGetFloat(m_hCadEntity, LC_PROP_SPIR_R2);
	m_rotAng = lcPropGetFloat(m_hCadEntity, LC_PROP_SPIR_ANGLE);
	m_resolution = lcPropGetInt(m_hCadEntity, LC_PROP_SPIR_RESOL);
	m_turn = lcPropGetFloat(m_hCadEntity, LC_PROP_SPIR_TURNS);
	m_bClose = lcPropGetBool(m_hCadEntity, LC_PROP_SPIR_CLOSED);
	m_offset.x = 0;
	m_offset.y = 0;
	UpdatePath(hEntity);
}

void CEntitySpiral::UpdateCenter()
{
	m_cen = m_center;
}

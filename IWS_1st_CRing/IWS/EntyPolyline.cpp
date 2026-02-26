#include "stdafx.h"
#include "IWS.h"
#include "EntyPolyline.h"
#include <afxtempl.h>
/*CMap<int, int, CString, CString&> g_dicPLFitType;*/

// g_dicPLFitType.SetAt(LC_PLFIT_NONE, CString("NONE"));
// g_dicPLFitType.SetAt(LC_PLFIT_QUAD, , _T("QUAD"));
// g_dicPLFitType.SetAt(LC_PLFIT_CUBIC, _T("CUBIC"));
// g_dicPLFitType.SetAt(LC_PLFIT_SPLINE, _T("SPLINE"));
// g_dicPLFitType.SetAt(LC_PLFIT_ROUND, _T("ROUND"));
// g_dicPLFitType.SetAt(LC_PLFIT_LINQUAD, _T("LINQUAD"));
/*g_dicPLFitType[1] = _T("df");*/
CEntyPolyline::CEntyPolyline(BOOL bCustom)
{
	m_FitType[0] = LC_PLFIT_NONE;
	m_FitType[1] = LC_PLFIT_QUAD;
	m_FitType[2] = LC_PLFIT_CUBIC;
	m_FitType[3] = LC_PLFIT_SPLINE;
	m_FitType[4] = LC_PLFIT_ROUND;
	m_FitType[5] = LC_PLFIT_LINQUAD;

	m_strFitType[0] = (_T("NONE"));
	m_strFitType[1] = (_T("QUAD"));
	m_strFitType[2] = (_T("CUBIC"));
	m_strFitType[3] = (_T("SPLINE"));
	m_strFitType[4] = (_T("ROUND"));
	m_strFitType[5] = (_T("LINQUAD"));
	m_strName.Format(L"Polyline", m_iEntCount);

	iDir = false;

	m_bCustomPolyLine = bCustom;
}


CEntyPolyline::~CEntyPolyline()
{
}

BOOL CEntyPolyline::Save(CFile *pf, int iver)
{
	CStringA strName(m_strName);
	int nStr = strName.GetLength();
	pf->Write(&nStr, sizeof(int));
	pf->Write(strName.GetBuffer(), sizeof(char)*nStr);
	pf->Write(&m_iFitType, sizeof(int));
	int nSize = (int)m_points.size();
	pf->Write(&nSize, sizeof(int));
	for (auto p : m_points) {
		pf->Write(&p, sizeof(PointPoly));
	}
	pf->Write(&iDir, sizeof(bool));
	return TRUE;
}

BOOL CEntyPolyline::Load(CFile *pf, int iver)
{
	int nStr;
	char tmp[100] = "";
	pf->Read(&nStr, sizeof(int));
	pf->Read(&tmp, sizeof(char)*nStr);
	CString str(tmp);
	m_strName = str;
	pf->Read(&m_iFitType, sizeof(int));
	int nSize = 0;
	pf->Read(&nSize, sizeof(int));
	if (iver < WELDJOB_VER103) {
		for (int i = 0; i < nSize;i++) {
			Point3Dbl p;
			pf->Read(&p, sizeof(Point3Dbl));
			PointPoly pt;
			pt.x = p.x;
			pt.y = p.y;
			pt.z = p.z;

			m_points.push_back(pt);
		}
	}
	else {
		for (int i = 0; i < nSize; i++) {
			PointPoly p;
			pf->Read(&p, sizeof(PointPoly));
			m_points.push_back(p);
		}
	}
	if (iver < WELDJOB_VER102)
		iDir = false;
	else
		pf->Read(&iDir, sizeof(bool));
	lcEventsEnable(FALSE);

	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hPline = AddCurrEntityToBlock(hBlock);
	SetDrawEntity(hPline);
	lcEventsEnable(TRUE);

	return TRUE;
}

CEntyPolyline CEntyPolyline::operator=(CEntyPolyline& rhs)
{
	
	memcpy(this->m_FitType, rhs.m_FitType, sizeof(int) * 6);
	for (int i = 0; i < MAX_PLINEFIT_TYPE; i++) {
		this->m_strFitType[i] = rhs.m_strFitType[i];
	}
	return *this;
}

HANDLE CEntyPolyline::AddCurrEntityToBlock(HANDLE hBlock)
{
	HANDLE handle;
	HANDLE hVer;
	if(m_hCadEntity)
		handle = lcBlockAddClone(hBlock, m_hCadEntity);
	else {
		handle = lcBlockAddPolyline(hBlock, 0, FALSE, FALSE);
		for (auto p : m_points) {
			hVer=lcPlineAddVer(handle, 0, p.x, p.y);
		}
		lcPlineEnd(handle);

		lcPropPutInt(handle, LC_PROP_PLINE_FIT, m_iFitType);		

	hVer = lcPlineGetFirstVer(handle);
		int idx = 0;
		while (hVer != 0) {
			lcPropPutFloat(hVer, LC_PROP_VER_SEGARCRAD, m_points[idx].arc_r);
			lcPropPutFloat(hVer, LC_PROP_VER_SEGARCANG, m_points[idx].arc_ang);
			hVer = lcPlineGetNextVer(handle, hVer);
			idx++;
		}
	}
	lcEntMove(handle, m_offset.x, m_offset.y);

	return handle;
}

void CEntyPolyline::SetDrawEntity(HANDLE hEntity)
{
	CEntity::SetDrawEntity(hEntity);

	if (m_pColorID)
		lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);
	ChangedEntity(hEntity);
	//TRACE("Polyline Pts = %d\n", m_path.size());
}

void CEntyPolyline::DisplayEntityProperties(CMFCPropertyGridCtrl *pProp)
{
	CEntity::DisplayEntityProperties(pProp);
	int idx = -1;
	for (int i = 0; i < MAX_PLINEFIT_TYPE; i++) {
		if (m_FitType[i] == m_iFitType) {
			idx = i;
			break;
		}
	}
	if (idx < 0)return;
	CMFCPropertyGridProperty* jobInfo = new CMFCPropertyGridProperty(_T("Entity Parameter"));
	CMFCPropertyGridProperty *pPropFit = new CMFCPropertyGridProperty(_T("Chage Fit Curve"), (_variant_t)m_strFitType[idx], L"Fit type(0,1)", (DWORD_PTR)&m_iFitType);
	for (int i = 0; i < MAX_PLINEFIT_TYPE; i++) pPropFit->AddOption(m_strFitType[i]);
	jobInfo->AddSubItem(pPropFit);
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("CW(0)/CCW(1)"), (_variant_t)iDir, L"direction", (DWORD_PTR)&iDir));

	pProp->AddProperty(jobInfo);
}

void CEntyPolyline::UpdateEntity(DWORD_PTR dwData, COleVariant value)
{
	lcEventsEnable(FALSE);
	lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);

	if (dwData == (DWORD_PTR)&m_iFitType && value.vt == VT_BSTR) {

		CString str = value;
		int idx = -1;
		for (int i = 0; i < MAX_PLINEFIT_TYPE; i++) {
			if (m_strFitType[i] == str) {
				idx = i;
				break;
			}
		}
		if (idx < 0) return;
		m_iFitType = m_FitType[idx];
		if (m_iFitType != lcPropGetInt(m_hCadEntity, LC_PROP_PLINE_FIT)) {
			lcPropPutInt(m_hCadEntity, LC_PROP_PLINE_FIT, m_iFitType);
			lcEntMove(m_hCadEntity, m_offset.x, m_offset.y);
			lcEntUpdate(m_hCadEntity);
			lcWndRedraw(g_hLcWnd);
			UpdatePath(m_hCadEntity);
		}
	}
	else {
		lcEntMove(m_hCadEntity, m_offset.x, m_offset.y);
		UpdatePath(m_hCadEntity);
	}
	
	lcEventsEnable(TRUE);

}

void CEntyPolyline::ChangedEntity(HANDLE hEntity)
{
	ASSERT(hEntity == m_hCadEntity);
	m_iFitType = lcPropGetInt(m_hCadEntity, LC_PROP_PLINE_FIT);
	HANDLE hVer;

	hVer = lcPlineGetFirstVer(m_hCadEntity);
	m_points.clear();

	while (hVer != 0) {
		PointPoly pt;		
		pt.x = lcPropGetFloat(hVer, LC_PROP_VER_X);
		pt.y = lcPropGetFloat(hVer, LC_PROP_VER_Y);
		pt.arc_ang= lcPropGetFloat(hVer, LC_PROP_VER_SEGARCANG);
		pt.arc_r = lcPropGetFloat(hVer, LC_PROP_VER_SEGARCRAD);

		m_points.push_back(pt);
		hVer = lcPlineGetNextVer(m_hCadEntity, hVer);
	}
	UpdatePath(hEntity);
}

void CEntyPolyline::UpdateCenter()
{
	double dsumx = 0;
	double dsumy = 0;
	int nPath = 0;

	// sjyi 2022-08-23 중심점 계산 오류 수정 --->
	// 멀티라인이면서 도형(육각형, 사각형 등)일 경우, 
	// 처음점과 끝점이 같은점이면서, 두번 계산되어, 중심 계산 오류 발생
	// 처음점 두번 계산 제외하기 위해 아래 내용 추가
	double dFirstX = m_path[0].x;
	double dFirstY = m_path[0].y;
	BOOL bFirst = TRUE;
	// sjyi 2022-08-23 중심점 계산 오류 수정 <---

	for (auto p : m_path) {

		// sjyi 2022-08-23 중심점 계산 오류 수정 --->
		// 처음점 두번 계산 제외하기 위해 아래 내용 추가
		if (dFirstX == p.x && dFirstY == p.y)
		{
			if (bFirst == FALSE)
				continue;
			else
				bFirst = FALSE;
		}
		// sjyi 2022-08-23 중심점 계산 오류 수정 <---

		dsumx = dsumx + p.x;
		dsumy = dsumy + p.y;
		nPath++;

	}
	m_cen.x = dsumx / ((double)nPath);
	m_cen.y = dsumy / ((double)nPath);
}

void CEntyPolyline::UpdatePath(HANDLE hEntity)
{
	lcEventsEnable(FALSE);
	m_iFitType = lcPropGetInt(hEntity, LC_PROP_PLINE_FIT);
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
	lcEventsEnable(TRUE);
	if (iDir)
		std::reverse(m_path.begin(), m_path.end());
	UpdateCenter();

// 	HANDLE hVer;
// 	hVer = lcPlineGetFirstVer(hEntity);
// 	double r, ang;
// 	while (hVer != 0) {
// 		PointPoly pt;
// 		ang = lcPropGetFloat(hVer, LC_PROP_VER_SEGARCANG);
// 		r = lcPropGetFloat(hVer, LC_PROP_VER_SEGARCRAD);
// 		hVer = lcPlineGetNextVer(hEntity, hVer);
// 	}
//	TRACE("Extrated Polyline Pts = %d\n", m_path.size());
}

int	 CEntyPolyline::GetEntityType()
{
	if (m_bCustomPolyLine == FALSE)
	{
		return LC_ENT_POLYLINE;
	}
	else
	{
		return (LC_ENT_POLYLINE + LC_ENT_CUSTOM);
	}
}


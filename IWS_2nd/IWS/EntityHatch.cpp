#include "stdafx.h"
#include "IWS.h"
#include "EntityHatch.h"


CEntityHatch::CEntityHatch()
{
	m_strName.Format(L"Hatch_%d", m_iEntCount);
	biDir = true;
}


CEntityHatch::~CEntityHatch()
{
}

BOOL CEntityHatch::Save(CFile *pf, int iVer /*= 0*/)
{
	CStringA strName(m_strName);
	int nStr = strName.GetLength();
	pf->Write(&nStr, sizeof(int));
	pf->Write(strName.GetBuffer(), sizeof(char)*nStr);
	pf->Write(&ptrnScale, sizeof(double));
	pf->Write(&ptrnAngle, sizeof(double));
	pf->Write(&ptrnIdx, sizeof(int));
	int nPath = (int)hatchLoops.size();
	pf->Write(&nPath, sizeof(int));
	int nLoop;
	for (auto p : hatchLoops) {
		nLoop = (int)p.size();
		pf->Write(&nLoop, sizeof(int));
		for (auto pp : p) {
			pf->Write(&pp, sizeof(Point3Dbl));
		}
	}
	pf->Write(&biDir, sizeof(bool));
	return TRUE;
}

BOOL CEntityHatch::Load(CFile *pf, int iver)
{
	int nStr;
	char tmp[100] = "";
	pf->Read(&nStr, sizeof(int));
	pf->Read(&tmp, sizeof(char)*nStr);
	CString str(tmp);
	m_strName = str;
	pf->Read(&ptrnScale, sizeof(double));
	pf->Read(&ptrnAngle, sizeof(double));
	pf->Read(&ptrnIdx, sizeof(int));

	int nPath = 0;
	pf->Read(&nPath, sizeof(int));
	int nLoop;
	hatchLoops.clear();
	for (int i = 0; i < nPath; i++) {
		pf->Read(&nLoop, sizeof(int));
		vector_point3dbl loop;
		for (int j = 0; j < nLoop; j++) {
			Point3Dbl p;
			pf->Read(&p, sizeof(Point3Dbl));
			loop.push_back(p);
		}
		hatchLoops.push_back(loop);
	}
	if (iver >= WELDJOB_VER101)
		pf->Read(&biDir, sizeof(bool));
	else
		biDir = true;

	lcEventsEnable(FALSE);

	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hPline = AddCurrEntityToBlock(hBlock);
	SetDrawEntity(hPline);
	lcEventsEnable(TRUE);
	return TRUE;
}

CEntityHatch CEntityHatch::operator=(CEntityHatch& rhs)
{
	return *this;
}

HANDLE CEntityHatch::AddCurrEntityToBlock(HANDLE hBlock)
{
	HANDLE handle;
	if (m_hCadEntity)
		handle = lcBlockAddClone(hBlock, m_hCadEntity);
	else {
		handle = lcBlockAddHatch(hBlock, L"", L"0, 0,0, 0,1", ptrnScale, ptrnAngle*LC_DEG_TO_RAD);
		lcHatchBoundStart(handle);

		for (auto p : hatchLoops) {
			for (auto pp : p) {
				lcHatchBoundPoint(handle, pp.x, pp.y);
			}
			lcHatchBoundEndLoop(handle);
		}
		lcHatchBoundEnd(handle);

	}
	lcEntMove(handle, m_offset.x, m_offset.y);
	return handle;
}

void CEntityHatch::SetDrawEntity(HANDLE hEntity)
{
	CEntity::SetDrawEntity(hEntity);
	if (m_pColorID)
		lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);
	ChangedEntity(hEntity);
}

void CEntityHatch::DisplayEntityProperties(CMFCPropertyGridCtrl *pProp)
{
	CEntity::DisplayEntityProperties(pProp);
	CMFCPropertyGridProperty* jobInfo = new CMFCPropertyGridProperty(_T("Entity Parameter"));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Angle"), (_variant_t)ptrnAngle, L"Angle", (DWORD_PTR)&ptrnAngle));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Distance(mm)"), (_variant_t)ptrnScale, L"Distance(mm)", (DWORD_PTR)&ptrnScale));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Bi-Direction"), (_variant_t)biDir, L"Hatch Path Direction", (DWORD_PTR)&biDir));
	pProp->AddProperty(jobInfo);
}

void CEntityHatch::UpdateEntity(DWORD_PTR dwData, COleVariant value)
{
	lcEventsEnable(FALSE);
	lcPropPutInt(m_hCadEntity, LC_PROP_ENT_COLOR, g_clrPen[*m_pColorID]);

	lcPropPutFloat(m_hCadEntity, LC_PROP_HATCH_ANGLE, ptrnAngle*LC_DEG_TO_RAD);
	lcPropPutFloat(m_hCadEntity, LC_PROP_HATCH_SCALE, ptrnScale);
	lcEntMove(m_hCadEntity, m_offset.x, m_offset.y);
	lcEntUpdate(m_hCadEntity);
	UpdatePath(m_hCadEntity);
	lcWndRedraw(g_hLcWnd);
	lcEventsEnable(TRUE);
}

void CEntityHatch::ChangedEntity(HANDLE hEntity)
{
	ASSERT(hEntity == m_hCadEntity);
	ptrnAngle = lcPropGetFloat(m_hCadEntity, LC_PROP_HATCH_ANGLE)*LC_RAD_TO_DEG;
	ptrnScale = lcPropGetFloat(m_hCadEntity, LC_PROP_HATCH_SCALE);

	m_offset.x = 0;
	m_offset.y = 0;

	// save hatch loop
	hatchLoops.clear();
	int k = 0;
	int nPaths = lcPropGetInt(hEntity, LC_PROP_HATCH_NLOOP);
	Point3Dbl pt;
	for (int i = 0; i < nPaths; ++i) {
		int nLoop= lcHatchGetLoopSize(hEntity, i);
		vector_point3dbl loop;
		for (int j = 0; j < nLoop; ++j) {
			Point3Dbl pt;
			lcHatchGetPoint(hEntity, k, &(pt.x), &(pt.y));
			++k;
			loop.push_back(pt);
		}
		hatchLoops.push_back(loop);
	}

	TRACE("======== Hatch loop size : %d\n", hatchLoops.size());
	for (auto p: hatchLoops) {
		TRACE("  loop %d\n", p.size());
	}
	UpdatePath(hEntity);
}

void CEntityHatch::UpdateCenter()
{

}

void CEntityHatch::UpdatePath(HANDLE hEntity)
{
	double X, Y, Xprev, Yprev, Dist, Len;
	int iPline;
	int nPlines = lcExpEntity(hEntity, LC_EXP_ALL, false);
	int nVers;
	int nPnts = 0;
	int iVer = 0;
	iPline = 0;
	Xprev = 0;
	Yprev = 0;
	Dist = 0;
	BOOL   bRet, bPenDown;
	m_path.clear();
	while (1) {
		nVers = lcExpGetPline(0.0);
		if (nVers > 0) {
			iPline++;
			iVer = 0;
			while (1) {
				bRet = lcExpGetVertex(&X, &Y);
				if (bRet == FALSE) {
					// exit vertices cycle
					bPenDown = false;
					break;
				}
				// here X,Y has vertex value
				if (X != Xprev || Y != Yprev) {
					Len = _hypot(Xprev - X, Yprev - Y);  // distance from previous position
					Dist += Len;
					Point3Dbl pt;
					pt.x = X; pt.y = Y;
					m_path.push_back(pt);
					Xprev = X;
					Yprev = Y;
					nPnts++;
				}
				iVer++;
				bPenDown = true;
			}
		}
		else {
			// exit polylines cycle
			break;
		}
	}

	hatchPath[0].clear();
	hatchPath[1].clear();
	Point3Dbl pt;
	size_t nSize = m_path.size();
	for (size_t i = 0; i < nSize; i++) {
		pt = m_path[i];
		hatchPath[i % 2].push_back(pt);
	}

	if (hatchPath[0].size() != hatchPath[1].size()) {
		AfxMessageBox(L"Hatch Path error");
		return;
	}
		
	if (biDir) {
		nSize = hatchPath[0].size();

		for (size_t i = 0; i < nSize; i++) {
			if (i % 2)
			{
				pt = hatchPath[0][i];
				hatchPath[0][i] = hatchPath[1][i];
				hatchPath[1][i] = pt;
			}
		}
	}
	
	// for debug 
// 	nSize = hatchPath[0].size();
// 	for (size_t i = 0; i < nSize; i++) {
// 		TRACE("%d, stp = %f,%f\n", i, hatchPath[0][i].x, hatchPath[0][i].y);
// 		TRACE("%d, edp = %f,%f\n", i, hatchPath[1][i].x, hatchPath[1][i].y);
// 	}
// 	int idx = 0;
// 	for(auto p:m_path)
// 		TRACE("%d, Pts = %f,%f\n", idx++, p.x, p.y);
}

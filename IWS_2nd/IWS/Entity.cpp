#include "stdafx.h"
#include "IWS.h"
#include "Entity.h"

int CEntity::m_iEntCount = -1;

void CEntity::SetPen(int *pClr)
{
	m_pColorID = pClr;
	if (m_hCadEntity) {
		HANDLE hData = lcXDataBegin();
		lcPropPutInt(hData, LC_PROP_XDATA_INT, *m_pColorID);
		int nSize = lcXDataEnd(hData);

		if (lcEntXData(m_hCadEntity, 1, 1, nSize)) {
			// write temporary buffer (hData) into the entity
			lcPropPutHandle(m_hCadEntity, LC_PROP_ENT_XDATA, hData);
		}
		lcXDataClear(hData);
	}	
}

int CEntity::GetPen()
{
	int Id = lcPropGetInt(m_hCadEntity, LC_PROP_ENT_XDATAID);
	if (!Id) return -1;
	int flag = lcPropGetInt(m_hCadEntity, LC_PROP_ENT_XDATAFLAGS);
	int nSize = lcPropGetInt(m_hCadEntity, LC_PROP_ENT_XDATASIZE);
	HANDLE hData = lcPropGetHandle(m_hCadEntity, LC_PROP_ENT_XDATA);
	lcXDataSet(hData);
	// get data
	int iPen = lcPropGetInt(hData, LC_PROP_XDATA_INT);
	return iPen;
}

void CEntity::SetOffset(double x, double y)
{
	m_offset.x = x;
	m_offset.y = y;
	UpdateEntity(NULL, COleVariant::COleVariant(0.0));
}

void CEntity::SetDrawEntity(HANDLE hEntity)
{
	m_hCadEntity = hEntity;
}

void CEntity::DisplayEntityProperties(CMFCPropertyGridCtrl *pProp)
{
	CMFCPropertyGridProperty* jobInfo = new CMFCPropertyGridProperty(_T("Common Offset"));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Offset x"), (_variant_t)m_offset.x, L"Offset x", (DWORD_PTR)&m_offset.x));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Offset y"), (_variant_t)m_offset.y, L"Offset y", (DWORD_PTR)&m_offset.y));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Center x"), (_variant_t)m_cen.x, L"Center x", (DWORD_PTR)&m_cen.x));
	jobInfo->AddSubItem(new CMFCPropertyGridProperyDlb3(_T("Center y"), (_variant_t)m_cen.y, L"Center y", (DWORD_PTR)&m_cen.y));

	pProp->AddProperty(jobInfo);
}


CEntity::CEntity()
{
	m_hCadEntity = NULL;
	m_iEntCount++;
	m_pColorID = NULL;
}


CEntity::~CEntity()
{
}
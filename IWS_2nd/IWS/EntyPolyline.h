#pragma once
#include "Entity.h"


#define MAX_PLINEFIT_TYPE 6
class CEntyPolyline :
	public CEntity
{
public:
	CEntyPolyline();
	~CEntyPolyline();

	std::vector<PointPoly> m_points;
	int m_iFitType;
	bool iDir;
public:
	virtual BOOL Save(CFile *pf, int iVer = 0);
	virtual BOOL Load(CFile *pf, int iver);
	virtual CEntyPolyline operator=(CEntyPolyline& rhs);
	virtual HANDLE AddCurrEntityToBlock(HANDLE hBlock);
	virtual void SetDrawEntity(HANDLE hEntity);
	virtual void DisplayEntityProperties(CMFCPropertyGridCtrl *pProp) ;
	virtual void UpdateEntity(DWORD_PTR dwData, COleVariant value);
	virtual void ChangedEntity(HANDLE hEntity);
	virtual int	 GetEntityType() { return LC_ENT_POLYLINE; };
	virtual void UpdateCenter();
private:
	int m_FitType[MAX_PLINEFIT_TYPE];
	CString m_strFitType[MAX_PLINEFIT_TYPE];
	void UpdatePath(HANDLE hEntity);
};


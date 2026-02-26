#pragma once
#include "Entity.h"
class CEntyPoint :
	public CEntity
{
public:
	CEntyPoint();
	~CEntyPoint();

	Point3Dbl point0;
	int delay;
	virtual BOOL Save(CFile *pf, int iVer = 0);
	virtual BOOL Load(CFile *pf, int iver);
	virtual CEntyPoint operator=(CEntyPoint& rhs);
	virtual HANDLE AddCurrEntityToBlock(HANDLE hBlock);
	virtual void SetDrawEntity(HANDLE hEntity);
	virtual void DisplayEntityProperties(CMFCPropertyGridCtrl *pProp) ;
	virtual void UpdateEntity(DWORD_PTR dwData, COleVariant value);
	virtual void ChangedEntity(HANDLE hEntity);
	virtual int	 GetEntityType() { return LC_ENT_POINT; };
	virtual void UpdatePath(HANDLE hEntity);
	virtual void UpdateCenter();
};


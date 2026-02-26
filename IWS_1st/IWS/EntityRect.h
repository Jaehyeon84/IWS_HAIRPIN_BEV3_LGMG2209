#pragma once
#include "Entity.h"
class CEntityRect :
	public CEntity
{
public:
	CEntityRect();
	virtual ~CEntityRect();
	Point3Dbl center;
	double width, height, angle;
	double radius;
	bool iDir;
public:
	virtual BOOL Save(CFile *pf, int iVer = 0);
	virtual BOOL Load(CFile *pf, int iver);
	virtual CEntityRect operator=(CEntityRect& rhs);
	virtual HANDLE AddCurrEntityToBlock(HANDLE hBlock);
	virtual void SetDrawEntity(HANDLE hEntity);
	virtual void DisplayEntityProperties(CMFCPropertyGridCtrl *pProp);
	virtual void UpdateEntity(DWORD_PTR dwData, COleVariant value);
	virtual void ChangedEntity(HANDLE hEntity);
	virtual void UpdatePath(HANDLE hEntity);
	virtual int	 GetEntityType() { return LC_ENT_RECT; };
	virtual void UpdateCenter();
};


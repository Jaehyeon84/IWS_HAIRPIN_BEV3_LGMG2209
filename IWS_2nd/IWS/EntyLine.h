#pragma once
#include "Entity.h"
class CEntyLine :
	public CEntity
{
public:
	CEntyLine();
	~CEntyLine();

	Point3Dbl point0, point1;
	double length;
	bool iDir;
	virtual BOOL Save(CFile *pf, int iVer = 0);
	virtual BOOL Load(CFile *pf, int iver);
	virtual CEntyLine operator=(CEntyLine& rhs);
	virtual HANDLE AddCurrEntityToBlock(HANDLE hBlock);
	virtual void SetDrawEntity(HANDLE hEntity);
	virtual void DisplayEntityProperties(CMFCPropertyGridCtrl *pProp) ;
	virtual void UpdateEntity(DWORD_PTR dwData, COleVariant value);
	virtual void ChangedEntity(HANDLE hEntity);
	virtual int	 GetEntityType() { return LC_ENT_LINE; };
	virtual void UpdatePath(HANDLE hEntity) ;
	virtual void UpdateCenter();
};


#pragma once
#include "Entity.h"
class CEntityHatch :
	public CEntity
{
public:
	CEntityHatch();
	~CEntityHatch();
	virtual BOOL Save(CFile *pf, int iVer = 0);
	virtual BOOL Load(CFile *pf, int iver);
	virtual CEntityHatch operator=(CEntityHatch& rhs);
	virtual HANDLE AddCurrEntityToBlock(HANDLE hBlock);
	virtual void SetDrawEntity(HANDLE hEntity);
	virtual void DisplayEntityProperties(CMFCPropertyGridCtrl *pProp);
	virtual void UpdateEntity(DWORD_PTR dwData, COleVariant value);
	virtual void ChangedEntity(HANDLE hEntity);
	virtual int	 GetEntityType() { return LC_ENT_HATCH; };
	virtual void UpdateCenter();
	void UpdatePath(HANDLE hEntity);

public:
	vector2_point3dbl hatchLoops;
	vector_point3dbl hatchPath[2];
	double ptrnScale;
	double ptrnAngle;
	int		ptrnIdx;
	bool	biDir;
};


#pragma once
#include "Entity.h"
class CEntyCircle :
	public CEntity
{
public:
	CEntyCircle();
	~CEntyCircle();
	
	Point3Dbl center;
	double diameter;
	double radius;
	bool iDir;
	double StartAngle;
	double overpath;
	void UpdatePath(HANDLE hEntity);
public:

	virtual BOOL Save(CFile *pf, int iVer = 0);
	virtual BOOL Load(CFile *pf, int iver);
	virtual CEntyCircle operator=(CEntyCircle& rhs);
	virtual HANDLE AddCurrEntityToBlock(HANDLE hBlock);
	virtual void SetDrawEntity(HANDLE hEntity);
	virtual void DisplayEntityProperties(CMFCPropertyGridCtrl *pProp);
	virtual void UpdateEntity(DWORD_PTR dwData, COleVariant value);
	virtual void ChangedEntity(HANDLE hEntity);
	virtual int	 GetEntityType() { return LC_ENT_CIRCLE; };
	virtual void UpdateCenter();
};


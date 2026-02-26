#pragma once
#include "Entity.h"
class CEntyArc :
	public CEntity
{
public:
	CEntyArc();
	~CEntyArc();
	Point3Dbl center;
	double radius;
	double angle_st, angle_arc;
	bool iDir;
	void UpdatePath(HANDLE hEntity);
public:
	virtual BOOL Save(CFile *pf, int iVer = 0);
	virtual BOOL Load(CFile *pf, int iver);
	virtual CEntyArc operator=(CEntyArc& rhs);
	virtual HANDLE AddCurrEntityToBlock(HANDLE hBlock);
	virtual void SetDrawEntity(HANDLE hEntity);
	virtual void DisplayEntityProperties(CMFCPropertyGridCtrl *pProp);
	virtual void UpdateEntity(DWORD_PTR dwData, COleVariant value);
	virtual void ChangedEntity(HANDLE hEntity);
	virtual int	 GetEntityType() { return LC_ENT_ARC; };
	virtual void UpdateCenter();
};


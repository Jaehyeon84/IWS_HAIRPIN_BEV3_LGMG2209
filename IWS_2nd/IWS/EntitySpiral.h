#pragma once
#include "Entity.h"
class CEntitySpiral :
	public CEntity
{
	
public:
	CEntitySpiral();
	~CEntitySpiral();
	Point3Dbl m_center;
	double m_radiusX, m_radiusY;
	double m_turn;
	int m_resolution;
	double m_rotAng;
	bool m_bClose;
	bool m_dirOut2In;
	void UpdatePath(HANDLE hEntity);
public:
	virtual BOOL Save(CFile *pf, int iVer = 0);
	virtual BOOL Load(CFile *pf, int iver);
	virtual CEntitySpiral operator=(CEntitySpiral& rhs);
	virtual HANDLE AddCurrEntityToBlock(HANDLE hBlock);
	virtual void SetDrawEntity(HANDLE hEntity);
	virtual void DisplayEntityProperties(CMFCPropertyGridCtrl *pProp);
	virtual void UpdateEntity(DWORD_PTR dwData, COleVariant value);
	virtual void ChangedEntity(HANDLE hEntity);
	virtual int	 GetEntityType() { return LC_ENT_SPIRAL; };
	virtual void UpdateCenter();
};


#pragma once
#include "Entity.h"
class CEntyEllipse :
	public CEntity
{
public:
	CEntyEllipse();
	~CEntyEllipse();

	Point3Dbl center;
	double Rmajor, Rminor;
	double RotAngle, StartAngle, ArcAngle;
	bool iDir;
	void UpdatePath(HANDLE hEntity);

	virtual BOOL Save(CFile *pf, int iVer = 0);
	virtual BOOL Load(CFile *pf, int iver);
	virtual CEntyEllipse operator=(CEntyEllipse& rhs);
	virtual HANDLE AddCurrEntityToBlock(HANDLE hBlock);
	virtual void SetDrawEntity(HANDLE hEntity);
	virtual void DisplayEntityProperties(CMFCPropertyGridCtrl *pProp) ;
	virtual void UpdateEntity(DWORD_PTR dwData, COleVariant value);
	virtual void ChangedEntity(HANDLE hEntity);
	virtual int	 GetEntityType() { return LC_ENT_ELLIPSE; };
	virtual void UpdateCenter();
};


#pragma once

#include <vector>

typedef struct _Point3Dbl {
	_Point3Dbl() :x(0.0), y(0.0), z(0.0) {};
	_Point3Dbl(double _x, double  _y) :x(_x), y(_y), z(0.0) {};
	double x, y, z;
}Point3Dbl;
typedef struct _PointPoly {
	_PointPoly() :x(0.0), y(0.0), z(0.0), arc_ang(0.0), arc_r(0.0){};
	_PointPoly(double _x, double  _y) :x(_x), y(_y), z(0.0) {};
	_PointPoly(double _x, double  _y, double _ang, double _r) :x(_x), y(_y), z(0.0) , arc_ang(_ang), arc_r(_r){};
	double x, y, z;
	double arc_ang;
	double arc_r;
}PointPoly;
static inline Point3Dbl operator+(const Point3Dbl& p0, const Point3Dbl& p1) { return Point3Dbl(p0.x + p1.x, p0.y + p1.y); }
static inline Point3Dbl operator-(const Point3Dbl& p0, const Point3Dbl& p1) { return Point3Dbl(p0.x - p1.x, p0.y - p1.y); }
static inline Point3Dbl operator*(const Point3Dbl& p0, const double i) { return Point3Dbl(p0.y*i, p0.y*i); }
static inline Point3Dbl operator/(const Point3Dbl& p0, const double i) { return Point3Dbl(p0.x / i, p0.x / i); }
static inline bool shorterThen(const Point3Dbl& p0, double len)
{
	if (p0.x > len || p0.x < -len)
		return false;
	if (p0.y > len || p0.y < -len)
		return false;
	return _hypot(p0.x, p0.y) <= len ;
}

typedef std::vector<Point3Dbl> vector_point3dbl;
typedef std::vector<vector_point3dbl> vector2_point3dbl;
class CEntity
{
public:
	CEntity();
	virtual ~CEntity();
	
	CString m_strName;
	HANDLE m_hCadEntity;
	HANDLE m_hHatch;
	int m_iEntityType;
	static int m_iEntCount;
	vector_point3dbl m_path;
	Point3Dbl m_offset;
	Point3Dbl m_cen;
	int *m_pColorID;
public:
	void SetPen(int *pClr);
	int  GetPen();
	virtual void SetOffset(double x, double y);
	virtual void UpdateCenter() = 0;
	virtual BOOL Save(CFile *pf, int iVer = 0) = 0;
	virtual BOOL Load(CFile *pf, int iVer) = 0;
	virtual HANDLE AddCurrEntityToBlock(HANDLE hBlock) = 0;
	virtual void SetDrawEntity(HANDLE hEntity);
	virtual void DisplayEntityProperties(CMFCPropertyGridCtrl *pProp);
	virtual void UpdateEntity(DWORD_PTR dwData, COleVariant value) = 0;
	virtual void ChangedEntity(HANDLE hEntity) = 0;
	virtual void UpdatePath(HANDLE hEntity) = 0;
	virtual int	 GetEntityType() = 0;
};

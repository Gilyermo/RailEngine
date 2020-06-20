/****************************************************
*		плоскость в трехмерном пространстве:		*
*				ax + by + cz + d = 0				*
*				{a, b, c} - нормаль					*
*  d - расстояние от плоскости до начала координат;	*
*		плоскость должна быть нормализована			*
****************************************************/
#pragma once

#include "Math.h"
#include "Vector3.h"
#include "Vector4.h"


namespace RGE
{
namespace Math
{

enum PlaneSide{
	Front,
	Back,
	Inside
};

class KERNEL_API Plane{
public:
	Plane(){}
	Plane(float _a, float _b, float _c, float _d = 0.0f){
		Set(_a, _b, _c, _d);
	}
	Plane(const Vector3& _n, float _d = 0.0f){
		Set(_n, _d);
	}
	Plane(const Vector4& abcd){
		Set(abcd);
	}
	Plane(const Vector3& pt1, const Vector3& pt2, const Vector3& pt3){
		Set(pt1, pt2, pt3);
	}
	Plane(const Vector3& normal, const Vector3& point){
		Set(normal, point);
	}
	Plane(const Plane& p){
		n = p.n;	d = p.d;
	}
	

	void Set(float _a, float _b, float _c, float _d = 0.0f){
		n.Set(_a, _b, _c);		d = _d;
		Normalize();
	}
	void Set(const Vector3& _n, float _d = 0.0f){
		n = _n;		d = _d;
		Normalize();
	}
	void Set(const Vector4& abcd){
		n.Set(abcd.x, abcd.y, abcd.z);	d = abcd.w;	
		Normalize();
	}
	void Set(const Vector3& pt1, const Vector3& pt2, const Vector3& pt3){
		n = (pt2-pt1).Cross(pt3-pt1);	d = -n.Dot(pt1);
		Normalize();
	}
	void Set(const Vector3& normal, const Vector3& point){
		n = normal;		d = -normal.Dot(point);
		Normalize();
	}

	
	void Normalize();

// проекция вектора на плоскость
	Vector3 Project(const Vector3& vec) const;

// проекция вектора вдоль некоторого направления на плоскость
	Vector3 ProjectAlongDirection(const Vector3& vec, const Vector3& dir) const;

	float AngleWith(const Vector3& vec) const;		// угол между плоскостью и вектором
	float AngleWith(const Plane& p) const;			// угол между плосокстями

//NOTE: если расстояние положительное - точка спереди плоскости, иначе - позади 
	float DistanceTo(const Vector3& pt) const;		// расстояние от плоскости до точки вдоль нормали
	float DistanceTo(const Plane& p) const;			// расстояние между плоскостями

	PlaneSide SideOf(const Vector3& pt) const;		// с какой стороны находится точка
	bool IsPointInside(const Vector3& pt) const;

	bool IsParallelTo(const Vector3& vec) const;	// проверка на параллельность с вектором
	bool IsParallelTo(const Plane& p) const;		// проверка на параллельность с плоскостью


	Plane& operator=(const Plane& p);

public:
	Vector3 n;
	float d;
};

}
}
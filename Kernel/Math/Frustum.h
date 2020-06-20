/********************************************
*	класс для работы с усеченной пирамидой	*	
********************************************/
#pragma once

#include "Math.h"
#include "Plane.h"


namespace RGE
{
namespace Math
{

class KERNEL_API Frustum{
public:
	enum FrustumPlane{
		Near = 0,
		Far,
		Lft,
		Rgt,
		Top,
		Btm,
		PlaneCount
	};

public:
	Frustum(){}
	Frustum(const Vector3& ntl, const Vector3& ntr, const Vector3& nbr, const Vector3& nbl,
			const Vector3& ftl, const Vector3& ftr, const Vector3& fbr, const Vector3& fbl)
	{
		Set(ntl, ntr, nbr, nbl, ftl, ftr, fbr, fbl);
	}
	Frustum(const Matrix44& viewProj){
		Set(viewProj);
	}
	Frustum(const Frustum& f){
		for(int i=0; i<PlaneCount; i++)
			plane[i] = f.plane[i];
	}

// фрустум по 8 точкам (ntl - near_top_left, ntr - near_top_right, etc.)
	void Set(const Vector3& ntl, const Vector3& ntr, const Vector3& nbr, const Vector3& nbl,
			 const Vector3& ftl, const Vector3& ftr, const Vector3& fbr, const Vector3& fbl);
	void Set(const Matrix44& viewProj);

	bool IsPointInside(const Vector3& pt) const;

	Frustum& operator=(const Frustum& f){
		if(this != &f){
			for(int i=0; i<PlaneCount; i++)
				plane[i] = f.plane[i];
		}
		return *this;
	} 

public:
	Plane plane[PlaneCount];
};

}
}
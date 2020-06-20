#pragma once

#include "Math.h"
#include "Vector3.h"


namespace RGE
{
namespace Math
{

class Sphere{
public:
	Sphere(){}
	Sphere(const Vector3& _center, float _radius){
		Set(_center, _radius);
	}
	Sphere(const Sphere& s){
		center = s.center;	
		radius = s.radius;
	}

	void Set(const Vector3& _center, float _radius){
		center = _center;		
		radius = _radius;
	}

	bool IsPointInside(const Vector3& pt) const{
		return (pt - center).Norm_sq() <= radius*radius;
	}

	Sphere& operator=(const Sphere& s){
		if(this != &s){
			center = s.center;	
			radius = s.radius;
		}
		return *this;
	}

public:
	Vector3 center;
	float	radius;
};

}
}
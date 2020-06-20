/************************************
*	Ray(t) = origin + t*dir, t>=0	*
************************************/
#pragma once

#include "Math.h"
#include "Line.h"


namespace RGE
{
namespace Math
{

class Ray : public Line{
public:
	Ray(){}
	Ray(const Vector3& _origin, const Vector3& _dir) : Line(_origin, _dir){}
	Ray(const Ray& ray){
		origin	=	ray.origin;
		dir		=	ray.dir;
	}
	~Ray(){}

	Ray& operator=(const Ray& ray){
		if(this != &ray){
			origin	=	ray.origin;
			dir		=	ray.dir;
		}
		return *this;
	}
};

}
}
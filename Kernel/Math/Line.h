/************************************************
*	Line(t) = origin + t*dir, -inf < t < +inf	*
************************************************/
#pragma once

#include "Math.h"
#include "Vector3.h"


namespace RGE
{
namespace Math
{

class Line{
public:
	Line(){}
	Line(const Vector3& _origin, const Vector3& _dir){
		Set(_origin, _dir);
	}
	Line(const Line& line){
		origin	=	line.origin;
		dir		=	line.dir;
	}
	virtual ~Line(){}

	void Set(const Vector3& _origin, const Vector3& _dir){
		origin	=	_origin;
		dir		=	_dir;
	}

	Vector3 GetPoint(float t) const{
		return origin + t*dir;
	}

	Line& operator=(const Line& line){
		if(this != &line){
			origin	=	line.origin;
			dir		=	line.dir;
		}
		return *this;
	}

public:
	Vector3 origin;
	Vector3 dir;
};

}
}
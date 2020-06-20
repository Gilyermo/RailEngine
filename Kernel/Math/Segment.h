/************************************************
*	Segment(t) = origin + t*dir, 0 <= t <= 1	*
************************************************/
#pragma once

#include "Math.h"
#include "Line.h"


namespace RGE
{
namespace Math
{

class Segment : public Line{
public:
	Segment(){}
	Segment(const Vector3& pt_from, const Vector3& pt_to){
		Set(pt_from, pt_to);
	}
	Segment(const Line& line, float t1=0.0f, float t2=1.0f){
		Set(line, t1, t2);
	}
	Segment(const Segment& s){
		origin	=	s.origin;
		dir		=	s.dir;
	}

	void Set(const Vector3& pt_from, const Vector3& pt_to){
		origin	= pt_from;
		dir		= pt_to - pt_from;
	}

	void Set(const Line& line, float t1=0.0f, float t2=1.0f){
		origin	= line.GetPoint(t1);
		dir		= line.GetPoint(t2) - origin;
	}


	Vector3 GetA() const{
		return origin;
	}

	Vector3 GetB() const{
		return origin + dir;
	}


	Segment& operator=(const Segment& s){
		if(this != &s){
			origin	=	s.origin;
			dir		=	s.dir;
		}
		return *this;
	}
};

}
}
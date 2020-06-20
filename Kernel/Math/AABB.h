/********************************************
*	Ѕокс, выровненный по координатным ос€м	*
********************************************/
#pragma once

#include "Math.h"
#include "Vector3.h"


namespace RGE
{
namespace Math
{

class AABB{
public:
	AABB(){}
	AABB(const Vector3& _min, const Vector3& _max){
		Set(_min, _max);
	}
	AABB(const AABB& aabb){
		Set(aabb.min, aabb.max);
	}

	void Set(const Vector3& _min, const Vector3& _max){
		min = _min;			max = _max;
	}

	
	bool IsPointInside(const Vector3& pt) const{
		if(min<=pt && pt<=max)	return true;
		return false;
	}


	Vector3 GetCenter() const{
		return 0.5f * (min + max);
	}

	Vector3 GetExtent() const{
		return 0.5f * (max - min);
	}


	AABB& operator=(const AABB& aabb){
		if(this != &aabb){
			min = aabb.min;
			max = aabb.max;
		}
		return *this;
	}

public:
	Vector3 min;
	Vector3 max;
};

}
}
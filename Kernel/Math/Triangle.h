/********************************************
*	треугольник в трехмерном пространстве	*
********************************************/
#pragma once

#include "Math.h"
#include "Vector3.h"


namespace RGE
{
namespace Math
{

class KERNEL_API Triangle{
public:
	Triangle(){}
	Triangle(const Vector3& _v1, const Vector3& _v2, const Vector3& _v3){	
		Set(_v1, _v2, _v3);
	}
	Triangle(const Vector3& _v1, const Vector3& _v2, const Vector3& _v3, const Vector3& _n){
		Set(_v1, _v2, _v3, _n);
	}
	Triangle(const Vector3* vertices, const Vector3& _n){
		Set(vertices, _n);
	}
	Triangle(const Vector3* vertices){
		Set(vertices);
	}
	Triangle(const Triangle& t){
		v1 = t.v1;	v2 = t.v2;	v3 = t.v3;
		n  = t.n;
	}

// вершины перечисл€ютс€ по часовой стрелке
	void Set(const Vector3& _v1, const Vector3& _v2, const Vector3& _v3){
		v1 = _v1;	v2 = _v2;	v3 = _v3;
		n = (v2 - v1).Cross(v3 - v1).Normalized();
	}
	void Set(const Vector3& _v1, const Vector3& _v2, const Vector3& _v3, const Vector3& _n){
		v1 = _v1;	v2 = _v2;	v3 = _v3;
		n  = _n;
	}
	void Set(const Vector3* vertices, const Vector3& _n){
		Set(vertices[0], vertices[1], vertices[2], _n);
	}
	void Set(const Vector3* vertices){
		Set(vertices[0], vertices[1], vertices[2]);
	}

	float Square() const;
	bool IsPointInside(const Vector3& pt) const;

	Vector3 GetA() const{
		return v1;
	}
	Vector3 GetB() const{
		return v2;
	}
	Vector3 GetC() const{
		return v3;
	}

	Triangle& operator=(const Triangle& t){
		if(this != &t){
			v1 = t.v1;	v2 = t.v2;	v3 = t.v3;
			n  = t.n;
		}
		return *this;
	}

public:
	Vector3	v1, v2, v3;
	Vector3 n;
};

}
}
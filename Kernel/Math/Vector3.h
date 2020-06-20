/************************************
*		3-компонентый вектор в		*
*	левосторонней системе координат	*
************************************/
#pragma once

#include "Math.h"


namespace RGE
{
namespace Math
{

class KERNEL_API Vector3{
public:
	Vector3(){
		x = 0.0f;	y = 0.0f;	z = 0.0f;
	}
	explicit Vector3(float _x, float _y = 0.0f, float _z = 0.0f){
		x = _x;		y = _y;		z = _z;
	}
	Vector3(const Vector3& vec){
		x = vec.x;	y = vec.y;	z = vec.z;
	}
	Vector3(const Vector2& vec, float _z=0.0f);
	explicit Vector3(const Vector4& vec);

	void Set(float _x, float _y, float _z){
		x = _x;		y = _y;		z = _z;
	}
	void Set(const Vector2& vec, float _z=0.0f);
	void Set(const Vector4& vec);


	float		Norm() const;
	float		Norm_sq() const;
	float		Normalize();								// нормализовать этот вектор; возвращает норму исходного вектора
	Vector3		Normalized() const;							// получить вектор, равный нормализованному данному вектору

	float		Dot(const Vector3& vec) const;				
	Vector3		Cross(const Vector3& vec) const;

	Vector3&	ProjectOn(const Vector3& vec);				// проекция на вектор in-place
	Vector3		ProjectionOn(const Vector3& vec) const;		// проекция на вектор (результат помещается в новый объект)
	Vector3&	Reflect(const Plane& plane);				// отраженный вектор in-place
	Vector3		Reflection(const Plane& plane) const;		// отраженный вектор (результат помещается в новый объект)

	bool		IsParallelTo(const Vector3& vec) const;

	Vector3&	operator=(const Vector3& vec);

	Vector3&	operator+=(const Vector3& vec);
	Vector3&	operator-=(const Vector3& vec);
	Vector3&	operator*=(const float a);
	Vector3&	operator/=(const float a);

	Vector3		operator+(const Vector3& vec) const;
	Vector3		operator-(const Vector3& vec) const;
	Vector3		operator*(const float a) const;
	Vector3		operator/(const float a) const;

	Vector3		operator+() const;
	Vector3		operator-() const;

	float		operator()(int indx) const;
	float&		operator()(int indx);

	bool operator==(const Vector3& vec) const;
	bool operator!=(const Vector3& vec) const;
	bool operator<=(const Vector3& vec) const;
	bool operator>=(const Vector3& vec) const;
	bool operator<(const Vector3& vec) const;
	bool operator>(const Vector3& vec) const;

	friend Vector3 operator*(const float a, const Vector3& vec){
		return Vector3(vec.x*a, vec.y*a, vec.z*a);	
	}

public:
	float x, y, z;
};

}
}
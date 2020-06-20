/************************************
*		4-компонентый вектор в		*
*	левосторонней системе координат	*
************************************/
#pragma once

#include "Math.h"


namespace RGE
{
namespace Math
{

class KERNEL_API Vector4{
public:
	Vector4(){
		x = 0.0f;	y = 0.0f;	z = 0.0f;	w = 0.0f;
	}
	explicit Vector4(float _x, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f){
		Set(_x, _y, _z, _w);
	}
	Vector4(const Vector4& vec){
		Set(vec.x, vec.y, vec.z, vec.w);
	}
	Vector4(const Vector2& vec, float _z=0.0f, float _w=0.0f);
	Vector4(const Vector3& vec, float _w=0.0f);


	void Set(float _x, float _y, float _z, float _w){
		x = _x;		y = _y;		z = _z;		w = _w;
	}
	void Set(const Vector2& vec, float _z=0.0f, float _w=0.0f);
	void Set(const Vector3& vec, float _w=0.0f);


	float		Norm() const;
	float		Norm_sq() const;
	float		Normalize();								// нормализовать этот вектор; возвращает норму исходного вектора
	Vector4		Normalized() const;							// получить вектор, равный нормализованному данному вектору

	float		Dot(const Vector4& vec) const;				
	Vector4		Cross(const Vector4& vec) const;

	Vector4&	ProjectOn(const Vector4& vec);				// проекция на вектор in-place
	Vector4		ProjectionOn(const Vector4& vec) const;		// проекция на вектор (результат помещается в новый объект)
	Vector4&	Reflect(const Plane& plane);				// отраженный вектор in-place
	Vector4		Reflection(const Plane& plane) const;		// отраженный вектор (результат помещается в новый объект

	bool		IsParallelTo(const Vector4& vec) const;

	Vector4&	operator=(const Vector4& vec);

	Vector4&	operator+=(const Vector4& vec);
	Vector4&	operator-=(const Vector4& vec);
	Vector4&	operator*=(const float a);
	Vector4&	operator/=(const float a);

	Vector4		operator+(const Vector4& vec) const;
	Vector4		operator-(const Vector4& vec) const;
	Vector4		operator*(const float a) const;
	Vector4		operator/(const float a) const;

	Vector4		operator+() const;
	Vector4		operator-() const;

	float		operator()(int indx) const;
	float&		operator()(int indx);

	bool		operator==(const Vector4& vec) const;
	bool		operator!=(const Vector4& vec) const;
	bool		operator<=(const Vector4& vec) const;
	bool		operator>=(const Vector4& vec) const;
	bool		operator<(const Vector4& vec) const;
	bool		operator>(const Vector4& vec) const;

	friend Vector4 operator*(const float a, const Vector4& vec){
		return Vector4(vec.x*a, vec.y*a, vec.z*a, vec.w*a);	
	}

public:
	float x, y, z, w;
};

}
}
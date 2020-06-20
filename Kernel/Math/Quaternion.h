/********************************************************
*	������������� ���������� ������� ����� �����������;	*
*		q = {v, w} = {axis * sin(a/2), cos(a/2)}		*
*		axis - ��� ��������, a - ���� ��������			*
********************************************************/
#pragma once

#include "Math.h"
#include "Vector3.h"
#include "Vector4.h"


namespace RGE
{
namespace Math
{

class KERNEL_API Quaternion{
public:
	Quaternion(){}
	Quaternion(float _x, float _y, float _z, float _w){
		Set(_x, _y, _z, _w);
	}
	Quaternion(const Vector4& vec){
		Set(vec);
	}
	Quaternion(const Vector3& _v, float _w){
		Set(_v, _w);
	}
	Quaternion(const Vector3& axis, float cs, float sn){
		Set(axis, cs, sn);
	}
	Quaternion(const Quaternion& q){
		v = q.v;
		w = q.w; 
	}
	explicit Quaternion(const Vector3& vec){
		v = vec;	w = 0.0f;
	}

	void Set(float _x, float _y, float _z, float _w){
		v.Set(_x, _y, _z);	
		w = _w;
	}
	void Set(const Vector4& vec){
		v.Set(vec.x, vec.y, vec.z);
		w = vec.w;
	}
	void Set(const Vector3& _v, float _w){
		v = _v;
		w = _w;
	}
// cs = cos(0.5*a); sn = sin(0.5*a) 
	void Set(const Vector3& axis, float cs, float sn){
		v = axis * sn;
		w = cs;
	}

	Quaternion& Identify();

	Quaternion& Conjugate();
	Quaternion  Conjugated() const;

	Quaternion& Inverse();
	Quaternion  Inversed() const;
	
	Vector3		Rotate(const Vector3& vec) const;

	float		Dot(const Quaternion& q) const;

	float		Norm() const;						// ����� ����������� - ����� ��������� ���������
	float		Magnitude() const;					// ������ (�����) ����������� - ������ �� �����

	Quaternion& Normalize();				
	Quaternion  Normalized() const;

	Quaternion& FromEulerAngles(const EulerAngles& euler_angles);
	EulerAngles ToEulerAngles() const;

	Quaternion&	FromAxisAngle(const Vector3& axis, float angle);	// ���������� ����������� �� ������� � ���� �������� ������ ����
	void		ToAxisAngle(Vector3& axis, float& angle) const;		// �������������� ����������� � ������ � ���� �������� ������ ����

// ����������� �������� ������������ ����� ����� �������������; t �� [0; 1]
// q_from � q_to - ��������� �����������
	static Quaternion Slerp(const Quaternion& q_from, const Quaternion& q_to, float t);

	Quaternion& operator=(const Quaternion& q);

	Quaternion& operator+=(const Quaternion& q);
	Quaternion& operator-=(const Quaternion& q);
	Quaternion& operator*=(const Quaternion& q);
	Quaternion& operator*=(const float s);
	Quaternion& operator/=(const float s);

	Quaternion	operator+(const Quaternion& q) const;
	Quaternion	operator-(const Quaternion& q) const;
	Quaternion	operator*(const Quaternion& q) const;
	Quaternion	operator*(const float s) const;
	Quaternion	operator/(const float s) const; 

	Quaternion	operator+() const;
	Quaternion	operator-() const;

	bool operator==(const Quaternion& q) const;
	bool operator!=(const Quaternion& q) const;

	friend Quaternion operator*(const float a, const Quaternion& q){
		return Quaternion(q.v.x*a, q.v.y*a, q.v.z*a, q.w*a);	
	}

public:
	Vector3 v;
	float   w;
};

}
}
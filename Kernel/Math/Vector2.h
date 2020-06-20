/************************************
*		2-����������� ������ �		*
*	������������� ������� ���������	*
************************************/
#pragma once

#include "Math.h"


namespace RGE
{
namespace Math
{

class KERNEL_API Vector2{
public:
	Vector2(){
		x = 0.0f;	y = 0.0f;
	}
	explicit Vector2(float _x, float _y = 0.0f){
		x = _x;		y = _y;
	}
	Vector2(const Vector2& vec){
		x = vec.x;	y = vec.y;
	}

	explicit Vector2(const Vector3& vec);
	explicit Vector2(const Vector4& vec);

	void Set(float _x, float _y){
		x = _x;		y = _y;
	}
	void Set(const Vector3& vec);
	void Set(const Vector4& vec);


	float		Norm() const;
	float		Norm_sq() const;
	float		Normalize();								// ������������� ���� ������; ���������� ����� ��������� �������
	Vector2		Normalized() const;							// �������� ������, ������ ���������������� ������� �������

	float		Dot(const Vector2& vec) const;				
	Vector2		Perp() const;

	Vector2&	ProjectOn(const Vector2& vec);				// �������� �� ������ in-place
	Vector2		ProjectionOn(const Vector2& vec) const;		// �������� �� ������ (��������� ���������� � ����� ������)

	bool		IsParallelTo(const Vector2& vec) const;

	Vector2&	operator=(const Vector2& vec);

	Vector2&	operator+=(const Vector2& vec);
	Vector2&	operator-=(const Vector2& vec);
	Vector2&	operator*=(const float a);
	Vector2&	operator/=(const float a);

	Vector2		operator+(const Vector2& vec) const;
	Vector2		operator-(const Vector2& vec) const;
	Vector2		operator*(const float a) const;
	Vector2		operator/(const float a) const;

	Vector2		operator+() const;
	Vector2		operator-() const;

	float		operator()(int indx) const;
	float&		operator()(int indx);

	bool operator==(const Vector2& vec) const;
	bool operator!=(const Vector2& vec) const;
	bool operator<=(const Vector2& vec) const;
	bool operator>=(const Vector2& vec) const;
	bool operator<(const Vector2& vec) const;
	bool operator>(const Vector2& vec) const;

	friend Vector2 operator*(const float a, const Vector2& vec){
		return Vector2(vec.x*a, vec.y*a);	
	}

public:
	float x, y;
};

}
}
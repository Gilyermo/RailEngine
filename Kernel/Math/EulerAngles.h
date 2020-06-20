/************************************************************
*		представление поворота через углы эйлера;			*
*	задается тремя углами поворота вокруг каждой из осей;	*
*				углы задаются в градусах					*
************************************************************/
#pragma once

#include "Math.h"


namespace RGE
{
namespace Math
{

class KERNEL_API EulerAngles{
public:
	EulerAngles(){
		x = 0.0f;	y = 0.0f;	z = 0.0f;
	}
	EulerAngles(float _x, float _y, float _z){
		x = _x;		y = _y;		z = _z;
	}
	EulerAngles(const EulerAngles& euler_angles){
		x = euler_angles.x;
		y = euler_angles.y;
		z = euler_angles.z;
	}

	void Set(float _x, float _y, float _z){
		x = _x;		y = _y;		z = _z;
	}


	EulerAngles&	operator=(const EulerAngles& vec);

	EulerAngles&	operator+=(const EulerAngles& vec);
	EulerAngles&	operator-=(const EulerAngles& vec);
	EulerAngles&	operator*=(const float a);
	EulerAngles&	operator/=(const float a);

	EulerAngles		operator+(const EulerAngles& vec) const;
	EulerAngles		operator-(const EulerAngles& vec) const;
	EulerAngles		operator*(const float a) const;
	EulerAngles		operator/(const float a) const;

	EulerAngles		operator+() const;
	EulerAngles		operator-() const;

	bool operator==(const EulerAngles& vec) const;
	bool operator!=(const EulerAngles& vec) const;

public:
	float x, y, z;
};

}
}
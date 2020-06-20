#include "EulerAngles.h"

namespace RGE
{
namespace Math
{

EulerAngles& EulerAngles::operator=(const EulerAngles& e_ang){
	if(this != &e_ang){
		x = e_ang.x;	y = e_ang.y;	z = e_ang.z;
	}
	return *this;
}


EulerAngles& EulerAngles::operator+=(const EulerAngles& e_ang){
	x += e_ang.x;		y += e_ang.y;		z += e_ang.z;
	return *this;
}

EulerAngles& EulerAngles::operator-=(const EulerAngles& e_ang){
	x -= e_ang.x;		y -= e_ang.y;		z -= e_ang.z;
	return *this;
}

EulerAngles& EulerAngles::operator*=(const float a){
	x *= a;		y *= a;		z *= a;
	return *this;
}

EulerAngles& EulerAngles::operator/=(const float a){
	x /= a;		y /= a;		z /= a;
	return *this;
}


EulerAngles EulerAngles::operator+(const EulerAngles& e_ang) const{
	return EulerAngles(x+e_ang.x, y+e_ang.y, z+e_ang.z);
}

EulerAngles EulerAngles::operator-(const EulerAngles& e_ang) const{
	return EulerAngles(x-e_ang.x, y-e_ang.y, z-e_ang.z);
}

EulerAngles EulerAngles::operator*(const float a) const{
	return EulerAngles(x*a, y*a, z*a);
}

EulerAngles EulerAngles::operator/(const float a) const{
	return EulerAngles(x/a, y/a, z/a);
}


EulerAngles EulerAngles::operator+() const{
	return EulerAngles(x, y, z);
}

EulerAngles EulerAngles::operator-() const{
	return EulerAngles(-x, -y, -z);
}


bool EulerAngles::operator==(const EulerAngles& e_ang) const{
	return fabs(x-e_ang.x)<EPS  &&  fabs(y-e_ang.y)<EPS  &&  fabs(z-e_ang.z)<EPS;
}

bool EulerAngles::operator!=(const EulerAngles& e_ang) const{
	return fabs(x-e_ang.x)>EPS  ||  fabs(y-e_ang.y)>EPS  ||  fabs(z-e_ang.z)>EPS;
}

}
}
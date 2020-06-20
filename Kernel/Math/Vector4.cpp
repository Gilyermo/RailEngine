#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Plane.h"


namespace RGE
{
namespace Math
{

Vector4::Vector4(const Vector2& vec, float _z, float _w){
	Set(vec, _z, _w);
}

Vector4::Vector4(const Vector3& vec, float _w){
	Set(vec, _w);
}


void Vector4::Set(const Vector2& vec, float _z, float _w){
	x = vec.x;	y = vec.y;	z = _z;		w = _w;
}

void Vector4::Set(const Vector3& vec, float _w){
	x = vec.x;	y = vec.y;	z = vec.z;	w = _w;
}


float Vector4::Norm() const{
	return sqrtf(x*x + y*y + z*z + w*w);
}

float Vector4::Norm_sq() const{
	return x*x + y*y + z*z + w*w;
}

float Vector4::Normalize(){
	float norm = Norm();
	x /= norm;		y /= norm;		z /= norm;		w /= norm;
	return norm;
}

Vector4 Vector4::Normalized() const{
	float norm = Norm();
	return Vector4(x/norm, y/norm, z/norm, w/norm);
}


float Vector4::Dot(const Vector4& vec) const{
	return x*vec.x + y*vec.y + z*vec.z + w*vec.w;
}

Vector4 Vector4::Cross(const Vector4& vec) const{
	Vector3 v1(*this), v2(vec);
	return v1.Cross(v2);
}


Vector4& Vector4::ProjectOn(const Vector4& vec){
	float coef = this->Dot(vec) / Norm_sq();
	x *= coef;	y *= coef;	z *= coef;	w *= coef;
	return *this;
}

Vector4 Vector4::ProjectionOn(const Vector4& vec) const{
	float coef = this->Dot(vec) / Norm_sq();
	return Vector4(x*coef, y*coef, z*coef, w*coef);
}

Vector4& Vector4::Reflect(const Plane& plane){
	Vector4 n(plane.n);
	*this -= 2.0f * this->Dot(n) * n;
	return *this;
}

Vector4 Vector4::Reflection(const Plane& plane) const{
	Vector4 n(plane.n);
	return *this - 2.0f * this->Dot(n) * n;
}

bool Vector4::IsParallelTo(const Vector4& vec) const{
	return this->Cross(vec).Norm_sq() <= EPS;
}

Vector4& Vector4::operator=(const Vector4& vec){
	if(this != &vec){
		x = vec.x;	y = vec.y;	z = vec.z;	w = vec.w;
	}
	return *this;
}


Vector4& Vector4::operator+=(const Vector4& vec){
	x += vec.x;		y += vec.y;		z += vec.z;		w += vec.w;
	return *this;
}

Vector4& Vector4::operator-=(const Vector4& vec){
	x -= vec.x;		y -= vec.y;		z -= vec.z;		w -= vec.w;
	return *this;
}

Vector4& Vector4::operator*=(const float a){
	x *= a;		y *= a;		z *= a;		w *= a;
	return *this;
}

Vector4& Vector4::operator/=(const float a){
	x /= a;		y /= a;		z /= a;		w /= a;
	return *this;
}


Vector4 Vector4::operator+(const Vector4& vec) const{
	return Vector4(x+vec.x, y+vec.y, z+vec.z, w+vec.w);
}

Vector4 Vector4::operator-(const Vector4& vec) const{
	return Vector4(x-vec.x, y-vec.y, z-vec.z, w-vec.w);
}

Vector4 Vector4::operator*(const float a) const{
	return Vector4(x*a, y*a, z*a, w*a);
}

Vector4 Vector4::operator/(const float a) const{
	return Vector4(x/a, y/a, z/a, w/a);
}


Vector4 Vector4::operator+() const{
	return Vector4(x, y, z, w);
}

Vector4 Vector4::operator-() const{
	return Vector4(-x, -y, -z, -w);
}


float Vector4::operator()(int indx) const{
	return (&x)[indx];
}

float& Vector4::operator()(int indx){
	return (&x)[indx];
}


bool Vector4::operator==(const Vector4& vec) const{
	return fabs(x-vec.x)<EPS  &&  fabs(y-vec.y)<EPS  &&  fabs(z-vec.z)<EPS  &&  fabs(w-vec.w)<EPS;
}

bool Vector4::operator!=(const Vector4& vec) const{
	return fabs(x-vec.x)>EPS  ||  fabs(y-vec.y)>EPS  ||  fabs(z-vec.z)>EPS	||	fabs(w-vec.w)>EPS;
}

bool Vector4::operator<=(const Vector4& vec) const{
	return Vector3(*this) <= Vector3(vec);
}

bool Vector4::operator>=(const Vector4& vec) const{
	return Vector3(*this) >= Vector3(vec);
}

bool Vector4::operator<(const Vector4& vec) const{
	return Vector3(*this) < Vector3(vec);
}

bool Vector4::operator>(const Vector4& vec) const{
	return Vector3(*this) > Vector3(vec);
}

}
}
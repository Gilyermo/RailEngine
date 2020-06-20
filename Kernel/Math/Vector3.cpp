#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Plane.h"


namespace RGE
{
namespace Math
{

Vector3::Vector3(const Vector2& vec, float _z){
	Set(vec, _z);
}

Vector3::Vector3(const Vector4& vec){
	Set(vec);
}

void Vector3::Set(const Vector2& vec, float _z){
	x = vec.x;		y = vec.y;		z = _z;
}

void Vector3::Set(const Vector4& vec){
	float rw;
	if(fabs(vec.w)<EPS)	rw = 1.0f;
	else				rw = 1.0f/vec.w;
	x = vec.x * rw;
	y = vec.y * rw;
	z = vec.z * rw;
}

float Vector3::Norm() const{
	return sqrtf(x*x + y*y + z*z);
}

float Vector3::Norm_sq() const{
	return x*x + y*y +z*z;
}

float Vector3::Normalize(){
	float norm = Norm();
	x /= norm;	y /= norm;	z /= norm;
	return norm;
}

Vector3 Vector3::Normalized() const{
	float norm = Norm();
	return Vector3(x/norm, y/norm, z/norm);
}


float Vector3::Dot(const Vector3& vec) const{
	return x*vec.x + y*vec.y + z*vec.z;
}

Vector3 Vector3::Cross(const Vector3& vec) const{
	return Vector3(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
}


Vector3& Vector3::ProjectOn(const Vector3& vec){
	float coef = this->Dot(vec) / Norm();
	x *= coef;	y *= coef;	z *= coef;
	return *this;
}

Vector3 Vector3::ProjectionOn(const Vector3& vec) const{
	float coef = this->Dot(vec) / Norm();
	return Vector3(x*coef, y*coef, z*coef);
}

Vector3& Vector3::Reflect(const Plane& plane){
	const Vector3& n = plane.n;
	*this -= 2.0f * this->Dot(n) * n;
	return *this;
}

Vector3 Vector3::Reflection(const Plane& plane) const{
	const Vector3& n = plane.n;
	return *this - 2.0f * this->Dot(n) * n;
}

bool Vector3::IsParallelTo(const Vector3& vec) const{
	return this->Cross(vec).Norm_sq() <= EPS;
}

Vector3& Vector3::operator=(const Vector3& vec){
	if(this != &vec){
		x = vec.x;	y = vec.y;	z = vec.z;
	}
	return *this;
}


Vector3& Vector3::operator+=(const Vector3& vec){
	x += vec.x;		y += vec.y;		z += vec.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& vec){
	x -= vec.x;		y -= vec.y;		z -= vec.z;
	return *this;
}

Vector3& Vector3::operator*=(const float a){
	x *= a;		y *= a;		z *= a;
	return *this;
}

Vector3& Vector3::operator/=(const float a){
	x /= a;		y /= a;		z /= a;
	return *this;
}


Vector3 Vector3::operator+(const Vector3& vec) const{
	return Vector3(x+vec.x, y+vec.y, z+vec.z);
}

Vector3 Vector3::operator-(const Vector3& vec) const{
	return Vector3(x-vec.x, y-vec.y, z-vec.z);
}

Vector3 Vector3::operator*(const float a) const{
	return Vector3(x*a, y*a, z*a);
}

Vector3 Vector3::operator/(const float a) const{
	return Vector3(x/a, y/a, z/a);
}


Vector3 Vector3::operator+() const{
	return Vector3(x, y, z);
}

Vector3 Vector3::operator-() const{
	return Vector3(-x, -y, -z);
}


float Vector3::operator()(int indx) const{
	return (&x)[indx];
}

float& Vector3::operator()(int indx){
	return (&x)[indx];
}


bool Vector3::operator==(const Vector3& vec) const{
	return fabs(x-vec.x)<EPS  &&  fabs(y-vec.y)<EPS  &&  fabs(z-vec.z)<EPS;
}

bool Vector3::operator!=(const Vector3& vec) const{
	return fabs(x-vec.x)>EPS  ||  fabs(y-vec.y)>EPS  ||  fabs(z-vec.z)>EPS;
}

bool Vector3::operator<=(const Vector3& vec) const{
	return x<=vec.x && y<=vec.y && z<=vec.z;
}

bool Vector3::operator>=(const Vector3& vec) const{
	return x>=vec.x && y>=vec.y && z>=vec.z;
}

bool Vector3::operator<(const Vector3& vec) const{
	return x<vec.x && y<vec.y && z<vec.z;
}

bool Vector3::operator>(const Vector3& vec) const{
	return x>vec.x && y>vec.y && z>vec.z;
}

}
}
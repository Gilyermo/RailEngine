#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Plane.h"


namespace RGE
{
namespace Math
{

Vector2::Vector2(const Vector3& vec){
	Set(vec);
}

Vector2::Vector2(const Vector4& vec){
	Set(vec);
}

void Vector2::Set(const Vector3& vec){
	x = vec.x;	y = vec.y;
}

void Vector2::Set(const Vector4& vec){
	Vector3 v3(vec);
	Set(v3);
}


float Vector2::Norm() const{
	return sqrtf(x*x + y*y);
}

float Vector2::Norm_sq() const{
	return x*x + y*y;
}

float Vector2::Normalize(){
	float norm = Norm();
	x /= norm;	y /= norm;
	return norm;
}

Vector2 Vector2::Normalized() const{
	float norm = Norm();
	return Vector2(x/norm, y/norm);
}


float Vector2::Dot(const Vector2& vec) const{
	return x*vec.x + y*vec.y;
}

Vector2 Vector2::Perp() const{
	return Vector2(y, -x);
}


Vector2& Vector2::ProjectOn(const Vector2& vec){
	float coef = this->Dot(vec) / Norm();
	x *= coef;	y *= coef;
	return *this;
}

Vector2 Vector2::ProjectionOn(const Vector2& vec) const{
	float coef = this->Dot(vec) / Norm();
	return Vector2(x*coef, y*coef);
}

bool Vector2::IsParallelTo(const Vector2& vec) const{
	float dotResult = this->Dot(vec);
	return fabs( 1.0f - fabs(dotResult) ) <= EPS;
}

Vector2& Vector2::operator=(const Vector2& vec){
	if(this != &vec){
		x = vec.x;	y = vec.y;
	}
	return *this;
}


Vector2& Vector2::operator+=(const Vector2& vec){
	x += vec.x;		y += vec.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& vec){
	x -= vec.x;		y -= vec.y;
	return *this;
}

Vector2& Vector2::operator*=(const float a){
	x *= a;		y *= a;
	return *this;
}

Vector2& Vector2::operator/=(const float a){
	x /= a;		y /= a;
	return *this;
}


Vector2 Vector2::operator+(const Vector2& vec) const{
	return Vector2(x+vec.x, y+vec.y);
}

Vector2 Vector2::operator-(const Vector2& vec) const{
	return Vector2(x-vec.x, y-vec.y);
}

Vector2 Vector2::operator*(const float a) const{
	return Vector2(x*a, y*a);
}

Vector2 Vector2::operator/(const float a) const{
	return Vector2(x/a, y/a);
}


Vector2 Vector2::operator+() const{
	return Vector2(x, y);
}

Vector2 Vector2::operator-() const{
	return Vector2(-x, -y);
}


float Vector2::operator()(int indx) const{
	return (&x)[indx];
}

float& Vector2::operator()(int indx){
	return (&x)[indx];
}


bool Vector2::operator==(const Vector2& vec) const{
	return fabs(x-vec.x)<EPS  &&  fabs(y-vec.y)<EPS;
}

bool Vector2::operator!=(const Vector2& vec) const{
	return fabs(x-vec.x)>EPS  ||  fabs(y-vec.y)>EPS;
}

bool Vector2::operator<=(const Vector2& vec) const{
	return x<=vec.x && y<=vec.y;
}

bool Vector2::operator>=(const Vector2& vec) const{
	return x>=vec.x && y>=vec.y;
}

bool Vector2::operator<(const Vector2& vec) const{
	return x<vec.x && y<vec.y;
}

bool Vector2::operator>(const Vector2& vec) const{
	return x>vec.x && y>vec.y;
}

}
}
#include "Quaternion.h"
#include "Matrix44.h"
#include "EulerAngles.h"


namespace RGE
{
namespace Math
{

Quaternion& Quaternion::Identify(){
	v.Set(0.0f, 0.0f, 0.0f);
	w = 1.0f;
	return *this;
}


Quaternion& Quaternion::Conjugate(){
	v = -v;
	return *this;
}

Quaternion Quaternion::Conjugated() const{
	return Quaternion(-v, w);
}


Quaternion& Quaternion::Inverse(){
	float q_norm = Norm();
	v = v / (-q_norm);	w /= q_norm;
	return *this;
}

Quaternion Quaternion::Inversed() const{
	float q_norm = Norm();
	return this->Conjugated() / q_norm;
}

Vector3 Quaternion::Rotate(const Vector3& vec) const{
	Quaternion v_quat(vec);
	return ( (*this) * v_quat * this->Inversed() ).v;
}

float Quaternion::Dot(const Quaternion& q) const{
	return v.Dot(q.v) + w*q.w;
}


float Quaternion::Norm() const{
	return v.Norm_sq() + w*w;
}

float Quaternion::Magnitude() const{
	return sqrtf(v.Norm_sq() + w*w);
}


Quaternion& Quaternion::Normalize(){
	float len = Magnitude();
	v /= len;	w /= len;
	return *this;
}

Quaternion Quaternion::Normalized() const{
	float len = Magnitude();
	return *this / len;
}


Quaternion& Quaternion::FromEulerAngles(const EulerAngles& eulerAngles){
	Quaternion y_rot, x_rot;
	this->FromAxisAngle(Vector3(0.0f, 0.0f, 1.0f), DegreeToRad(eulerAngles.z));
	y_rot.FromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), DegreeToRad(eulerAngles.y));
	x_rot.FromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), DegreeToRad(eulerAngles.x));

	*this *= y_rot * x_rot;
	return *this;
}

EulerAngles Quaternion::ToEulerAngles() const{
	Matrix44 rotation;
	rotation.FromQuaternion(*this);
	return rotation.ToEulerAngles();
}

Quaternion& Quaternion::FromAxisAngle(const Vector3& axis, float angle){
	v = axis * sinf(angle*0.5f);	
	w = cosf(angle*0.5f);
	return *this;
}

void Quaternion::ToAxisAngle(Vector3& axis, float& angle) const{
	angle = 2.0f * acosf(w);
	axis  = v / sinf(angle*0.5f);
}


// сферическая линейная интерполяция между двумя кватернионами; t из [0; 1]
// q_from и q_to - единичные кватернионы
Quaternion Quaternion::Slerp(const Quaternion& q_from, const Quaternion& q_to, float t){
	float angle = acosf( q_from.Dot(q_to) );	// угол между кватрнионами
	float sn_angle = sinf(angle);

// если кватернионы близки используем линейную интерполяцию
	if(fabs(sn_angle) < EPS)
		return Lerp(q_from, q_to, t);
	else
		return ( q_from*sinf((1-t)*angle) + q_to*sinf(t*angle) ) / sn_angle;
}


Quaternion& Quaternion::operator=(const Quaternion& q){
	if(this != &q){
		v = q.v;	w = q.w;
	}
	return *this;
}

Quaternion& Quaternion::operator+=(const Quaternion& q){
	v += q.v;	w += q.w;
	return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& q){
	v -= q.v;	w -= q.w;
	return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& q){
	Set( v.Cross(q.v) + w*q.v + q.w*v, w*q.w - v.Dot(q.v) );
	return *this;
}

Quaternion& Quaternion::operator*=(const float s){
	v *= s;		w *= s;
	return *this;
}

Quaternion& Quaternion::operator/=(const float s){
	v /= s;		w /= s;
	return *this;
}


Quaternion Quaternion::operator+(const Quaternion& q) const{
	return Quaternion(v + q.v, w + q.w);
}

Quaternion Quaternion::operator-(const Quaternion& q) const{
	return Quaternion(v - q.v, w - q.w);
}

Quaternion Quaternion::operator*(const Quaternion& q) const{
	return Quaternion( v.Cross(q.v) + w*q.v + q.w*v, w*q.w - v.Dot(q.v) );
}

Quaternion Quaternion::operator*(const float s) const{
	return Quaternion(v*s, w*s);
}

Quaternion Quaternion::operator/(const float s) const{
	return Quaternion(v/s, w/s);
}


Quaternion Quaternion::operator+() const{
	return Quaternion(v, w);
}

Quaternion Quaternion::operator-() const{
	return Quaternion(-v, -w);
}


bool Quaternion::operator==(const Quaternion& q) const{
	return v == q.v && fabs(w-q.w) < EPS;
}

bool Quaternion::operator!=(const Quaternion& q) const{
	return v != q.v || fabs(w-q.w) > EPS;
}

}
}
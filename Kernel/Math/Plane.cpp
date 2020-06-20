#include "Plane.h"
#include "Vector3.h"


namespace RGE
{
namespace Math
{

void Plane::Normalize(){
	float Norm = n.Norm();
	n.Normalize();		d /= Norm;
}


Vector3 Plane::Project(const Vector3& vec) const{
	float dist = DistanceTo(vec);
	return vec + dist * n;
}


float Plane::AngleWith(const Vector3& vec) const{
	return static_cast<float>( M_PI*0.5f - acosf(n.Dot(vec.Normalized())) );
}

float Plane::AngleWith(const Plane& p) const{
	return acosf( n.Dot(-p.n) );
}


float Plane::DistanceTo(const Vector3& pt) const{
	return d + n.x*pt.x + n.y*pt.y + n.z*pt.z;
}

float Plane::DistanceTo(const Plane& p) const{
	if(!IsParallelTo(p))	return 0.0f;
	return p.d - d;
}


PlaneSide Plane::SideOf(const Vector3& pt) const{
	float dist = DistanceTo(pt);

	if(dist < -EPS)	return Back;
	if(dist > EPS)	return Front;
	return Inside;
}

bool Plane::IsPointInside(const Vector3& pt) const{
	float dist = DistanceTo(pt);
	return fabs(dist) < EPS;
}

bool Plane::IsParallelTo(const Vector3& vec) const{
	return fabsf( n.Dot(vec.Normalized()) ) < EPS;
}

bool Plane::IsParallelTo(const Plane& p) const{
	return fabsf( n.Dot(p.n) + 1.0f ) < EPS;
}

Plane& Plane::operator=(const Plane& p){
	if(this != &p){
		n = p.n;	d = p.d;
	}
	return *this;
}

}
}
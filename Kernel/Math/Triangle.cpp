#include "Triangle.h"


namespace RGE
{
namespace Math
{

float Triangle::Square() const{
	Vector3 a = v2 - v1;
	Vector3 b = v3 - v1;
	float a_dot_b = a.Dot(b);
	return 0.5f * sqrtf( a.Norm_sq()*b.Norm_sq() - a_dot_b*a_dot_b );
}

bool Triangle::IsPointInside(const Vector3& pt) const{
	Vector3 a = v2 - v1, b = v3 - v1, d = pt - v1;
	float d11 = a.Dot(a);
	float d12 = a.Dot(b);
	float d22 = b.Dot(b);

	float denom = d11*d22 - d12*d12;
	if(fabs(denom)<EPS)					// треугольник вырожденный
		return false;
		
	float d13 = a.Dot(d);
	float d23 = b.Dot(d);

	float u = (d13*d22 - d23*d12) / denom;
	float v = (d11*d23 - d12*d13) / denom;
	float w = 1.0f - v - u;

	return 0.0f <= u && u <= 1.0f && 
			0.0f <= v && v <= 1.0f &&
			0.0f <= w && w <= 1.0f;
}

}
}
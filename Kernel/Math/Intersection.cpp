#include "Intersection.h"
#include "AABB.h"
#include "Sphere.h"
#include "Frustum.h"
#include "Plane.h"
#include "Rect.h"
#include "Triangle.h"
#include "Line.h"
#include "Ray.h"
#include "Segment.h"

#include <algorithm>


namespace RGE
{
namespace Math
{

#pragma region Line-AABB
// проверка на пересечение бокса и линии, ограниченной t_min и t_max, методом Смитса;
// в случае успеха точки входа и выхода линии из бокса возвращается в t_min и t_max
bool AABB_vs_Line_Test(const Line& line, const AABB& aabb, float& t_min, float& t_max){
	for(int i=0; i<3; i++){
		if(fabs(line.dir(i)) < EPS){
			if( line.origin(i)<aabb.min(i) || line.origin(i)>aabb.max(i) )
				return false;
		}
		else{
			float inv_dir = 1.0f / line.dir(i);
			float t1 = (aabb.min(i) - line.origin(i)) * inv_dir;
			float t2 = (aabb.max(i) - line.origin(i)) * inv_dir;
		
			if(t1>t2)			std::swap(t1, t2);
			if(t1>t_min)		t_min = t1;
			if(t2<t_max)		t_max = t2;
			if(t_max < t_min)	return false;
		}
	}

	return true;
}

bool Intersect(const Line& line, const AABB& aabb, float* t1, float* t2){
	float t_min = -FLT_MAX, t_max = FLT_MAX;

	if(AABB_vs_Line_Test(line, aabb, t_min, t_max)){
		if(t1)	*t1 = t_min;
		if(t2)	*t2 = t_max;
		return true;
	}

	return false;
}
#pragma endregion

#pragma region Ray-AABB
bool Intersect(const Ray& ray, const AABB& aabb, float* t1, float* t2){
	float t_min = 0.0f, t_max = FLT_MAX;

	if(AABB_vs_Line_Test(ray, aabb, t_min, t_max)){
		if(t1)	*t1 = t_min;
		if(t2)	*t2 = t_max;
		return true;
	}

	return false;
}
#pragma endregion

#pragma region Segment-AABB
bool Intersect(const Segment& s, const AABB& aabb, float* t1, float* t2){
	float t_min = 0.0f, t_max = 1.0f;

	if(AABB_vs_Line_Test(s, aabb, t_min, t_max)){
		if(t1)	*t1 = t_min;
		if(t2)	*t2 = t_max;
		return true;
	}

	return false;
}
#pragma endregion


#pragma region Line-Sphere
bool Intersect(const Line& line, const Sphere& s, float* t1, float* t2){
	Vector3 m			=	line.origin - s.center;
	float dir_dot_m		=	line.dir.Dot(m);
	float dir_norm_sq	=	line.dir.Norm_sq();
	float discriminant	=	dir_dot_m*dir_dot_m - dir_norm_sq*(m.Norm_sq() - s.radius*s.radius);		// четверть дискриминанта
	
	if(discriminant < 0.0f)		
		return false;
	
	if(t1!=nullptr || t2!=nullptr){
		if(fabs(discriminant) <= EPS){
			if(t1)	*t1 = -dir_dot_m / dir_norm_sq;
			if(t2)	*t2 = *t1;
		}
		else{
			float sqrt_D = sqrtf(discriminant);
			if(t1)	*t1 = (-dir_dot_m - sqrt_D) / dir_norm_sq;
			if(t2)	*t2 = (-dir_dot_m + sqrt_D) / dir_norm_sq;
		}
	}

	return true;
}
#pragma endregion

#pragma region Ray-Sphere
bool Intersect(const Ray& ray, const Sphere& s, float* _t1, float* _t2){
	float t1, t2;

	if(Intersect(static_cast<const Line&>(ray), s, &t1, &t2)){
		if(t2 < 0.0f)	return false;
		if(t1 < 0.0f)	t1 = t2;

		if(_t1)		*_t1 = t1;
		if(_t2)		*_t2 = t2;
		return true;
	}

	return false;
}
#pragma endregion

#pragma region Segment-Sphere
bool Intersect(const Segment& seg, const Sphere& s, float* _t1, float* _t2){
	float t1, t2;

	if(Intersect(static_cast<const Line&>(seg), s, &t1, &t2)){
// если сегмент не пересекает границы сферы, но находится внутри нее 
		if( t1<0.0f && s.IsPointInside(seg.GetA()) )	t1 = 0.0f;
		if( t2>1.0f && s.IsPointInside(seg.GetB()) )	t2 = 1.0f;

		if(t1<0.0f || t2>1.0f)	return false;

		if(_t1)		*_t1 = t1;
		if(_t2)		*_t2 = t2;

		return true;
	}

	return false;
}
#pragma endregion


#pragma region Line-Plane
bool Intersect(const Line& line, const Plane& p, float* t){
	if(p.IsParallelTo(line.dir))
		return false;

	if(t)	
		*t = -(p.d + line.origin.Dot(p.n)) / line.dir.Dot(p.n);
	return true;
}
#pragma endregion

#pragma region Ray-Plane
bool Intersect(const Ray& r, const Plane& p, float* _t){
	if(p.IsParallelTo(r.dir))
		return false;

	float t = -(p.d + r.origin.Dot(p.n)) / r.dir.Dot(p.n);
	if(t < 0.0f)	return false;
	if(_t)			*_t = t;
	return true;
}
#pragma endregion

#pragma region Segment-Plane
bool Intersect(const Segment& s, const Plane& p, float* _t){
	if(p.IsParallelTo(s.dir))
		return false;

	float t = -(p.d + s.origin.Dot(p.n)) / s.dir.Dot(p.n);
	if(t<0.0f || t>1.0f)	return false;
	if(_t)					*_t = t;
	return true;
}
#pragma endregion


#pragma region Line-Triangle
bool Intersect(const Line& line, const Triangle& triangle, float* _u, float* _v, float* _t){
	Vector3 a = triangle.v1 - triangle.v2, 
			b = triangle.v1 - triangle.v3, 
			d = triangle.v1 - line.origin;
	Vector3 e = line.dir.Cross(d),
			n = a.Cross(b);

	float denom = line.dir.Dot(n);
	if(fabs(denom)<EPS)		// линия параллельна плоскости треугольника
		return false;

	denom = 1.0f / denom;
	float t = d.Dot(n) * denom;
	float u = e.Dot(b) * denom;
	float v = -e.Dot(a) * denom;
	float w = 1.0f - u - v;

	if(u<0.0f || u>1.0f || v<0.0f || v>1.0f || w<0.0f || w>1.0f)
		return false;

	if(_u)	*_u = u;
	if(_v)	*_v = v;
	if(_t)	*_t = t;

	return true;
}
#pragma endregion

#pragma region Ray-Triangle
bool Intersect(const Ray& r, const Triangle& triangle, float* _u, float* _v, float* _t){
	float u, v, t;

	if(Intersect(static_cast<const Line&>(r), triangle, &u, &v, &t)){
		if(t<0.0f)	return false;

		if(_u)	*_u = u;
		if(_v)	*_v = v;
		if(_t)	*_t = t;
		return true;
	}
	return false;
}
#pragma endregion

#pragma region Segment-Triangle
bool Intersect(const Segment& s, const Triangle& triangle, float* _u, float* _v, float* _t){
	float u, v, t;

	if(Intersect(static_cast<const Line&>(s), triangle, &u, &v, &t)){
		if(t<0.0f || t>1.0f)	
			return false;

		if(_u)	*_u = u;
		if(_v)	*_v = v;
		if(_t)	*_t = t;
		return true;
	}
	return false;
}
#pragma endregion


#pragma region AABB-AABB
bool Intersect(const AABB& aabb1, const AABB& aabb2, AABB* aabb){
	float max_x = std::min(aabb1.max.x, aabb2.max.x);
	float min_x = std::max(aabb1.min.x, aabb2.min.x);
	if(max_x < min_x)
		return false;

	float max_y = std::min(aabb1.max.y, aabb2.max.y);
	float min_y = std::max(aabb1.min.y, aabb2.min.y);
	if(max_y < min_y)
		return false;

	float max_z = std::min(aabb1.max.z, aabb2.max.z);
	float min_z = std::max(aabb1.min.z, aabb2.min.z);
	if(max_z < min_z)
		return false;

	if(aabb)
		aabb->Set(Vector3(min_x, min_y, min_z), Vector3(max_x, max_y, max_z));
	return true;
}
#pragma endregion


#pragma region AABB-Sphere
bool Intersect(const AABB& aabb, const Sphere& s){
	float distance = 0.0f;
	
	for(int i=0; i<3; i++){
		float d = s.center(i) - aabb.min(i);
		if(d < 0.0f){
			distance += d*d;
			continue;
		}

		d = s.center(i) - aabb.max(i);
		if(d > 0.0f)
			distance += d*d;
	}

	return distance < s.radius * s.radius;
}
#pragma endregion


#pragma region Sphere-Sphere
bool Intersect(const Sphere& s1, const Sphere& s2){
	return (s1.center - s2.center).Norm_sq() <= s1.radius*s1.radius + s2.radius*s2.radius;
}
#pragma endregion


#pragma region Frustum-AABB
bool Intersect(const Frustum& f, const AABB& aabb){
	Vector3 aabbCenter = aabb.GetCenter();
	Vector3 aabbExtent = aabb.GetExtent();

	for(int i=0; i<Frustum::PlaneCount; i++){
		const Plane& p = f.plane[i];

		float d = p.DistanceTo(aabbCenter);
		float r = aabbExtent.x * fabs(p.n.x) + 
				  aabbExtent.y * fabs(p.n.y) + 
				  aabbExtent.z * fabs(p.n.z);

		if(d+r < 0.0f)	return false;
	}

	return true;
}
#pragma endregion


#pragma region Frustum-Sphere
bool Intersect(const Frustum& f, const Sphere& s){
	for(int i=0; i<Frustum::PlaneCount; i++){
		if( f.plane[i].DistanceTo( s.center ) < -s.radius )
			return false;
	}
	return true;
}
#pragma endregion


#pragma region Frustum-Triangle
bool Intersect(const Frustum& f, const Triangle& t){
	for(int i=0; i<Frustum::PlaneCount; i++){
		const Plane& p = f.plane[i];
		if(p.DistanceTo(t.v1)<=0.0f && p.DistanceTo(t.v2)<=0.0f && p.DistanceTo(t.v3)<=0.0f)
			return false;
	}
	return true;
}
#pragma endregion


#pragma region Plane-Plane
bool Intersect(const Plane& p1, const Plane& p2, Line* line){
	if(p1.IsParallelTo(p2))	
		return false;

	if(line){
		Vector3 n3 = p1.n.Cross(p2.n);
		line->origin = (p2.d*p1.n - p1.d*p2.n).Cross(n3);

		float n3_norm = n3.Normalize();
		line->origin /= n3_norm * n3_norm;
		line->dir = n3;
	}
	return true;
}
#pragma endregion


#pragma region Plane-Triangle
bool Intersect(const Plane& p, const Triangle& t, Segment* seg){
	PlaneSide side[3] = { p.SideOf(t.v1), p.SideOf(t.v2), p.SideOf(t.v3) };
	if(side[1] == side[0] && side[2] == side[0])
		return side[0] == Inside;

	if(seg){
		Segment s1, s2;
		if(side[0] != side[1]){
			s1.Set(t.v1, t.v2);
			
			if(side[0] != side[2])
				s2.Set(t.v1, t.v3);
			else
				s2.Set(t.v2, t.v3);
		}
		else{
			s1.Set(t.v1, t.v3);
			s2.Set(t.v2, t.v3);
		}

		float t1, t2;
		Intersect(p, s1, &t1);
		Intersect(p, s2, &t2);
		seg->Set(s1.GetPoint(t1), s2.GetPoint(t2));
	}

	return true;
}
#pragma endregion


#pragma region Rect-Rect
bool Intersect(const Rect& rt1, const Rect& rt2, Rect* rect){
	float max_x = std::min(rt1.rgt_top.x, rt2.rgt_top.x);
	float min_x = std::max(rt1.lft_btm.x, rt2.lft_btm.x);
	if(max_x < min_x)	
		return false;
	
	float max_y = std::min(rt1.rgt_top.y, rt2.rgt_top.y);
	float min_y = std::max(rt1.lft_btm.y, rt2.lft_btm.y);
	if(max_y < min_y)
		return false;

	if(rect){
		rect->lft_btm.Set(min_x, min_y);
		rect->rgt_top.Set(max_x, max_y);
	}

	return true;
}
#pragma endregion


#pragma region Rect-Triangle
bool Intersect(const Rect& rect, const Triangle& triangle){
// вершина треугольника попала внуть прямоугольника
	if(rect.IsPointInside( Vector2(triangle.v1) ))	return true;
	if(rect.IsPointInside( Vector2(triangle.v2) ))	return true;
	if(rect.IsPointInside( Vector2(triangle.v3) ))	return true;

// вершина прямоугольника внутри треугольника
	if( triangle.IsPointInside(rect.GetA()) )		return true;
	if( triangle.IsPointInside(rect.GetB()) )		return true;
	if( triangle.IsPointInside(rect.GetC()) )		return true;
	if( triangle.IsPointInside(rect.GetD()) )		return true;

// проверка на пересечение линий треугольника и прямоугольника
	Segment e1( Vector2(triangle.v1), Vector2(triangle.v2) ),
			e2( Vector2(triangle.v2), Vector2(triangle.v3) ),
			e3( Vector2(triangle.v3), Vector2(triangle.v1) );

	Segment f1( rect.GetA(), rect.GetB() );
	if(Intersect(e1, f1) || Intersect(e2, f1) || Intersect(e3, f1))	
		return true;

	Segment f2( rect.GetB(), rect.GetC() );
	if(Intersect(e1, f2) || Intersect(e2, f2) || Intersect(e3, f2))	
		return true;

	Segment f3( rect.GetC(), rect.GetD() );
	if(Intersect(e1, f3) || Intersect(e2, f3) || Intersect(e3, f3))	
		return true;

	Segment f4( rect.GetD(), rect.GetA() );
	if(Intersect(e1, f4) || Intersect(e2, f4) || Intersect(e3, f4))	
		return true;

	return false;
}
#pragma endregion


#pragma region Rect-Point
bool Intersect(const Rect& rt, const Vector2& pt, float* u, float* v){
	if(pt>=rt.lft_btm && pt<=rt.rgt_top){
		if(u)	*u = (pt.x - rt.lft_btm.x) / (rt.rgt_top.x - rt.lft_btm.x);
		if(v)	*v = (pt.y - rt.lft_btm.y) / (rt.rgt_top.y - rt.lft_btm.y);
		return true;
	}

	return false;
}
#pragma endregion


#pragma region Triangle-Triangle
bool TrianglesDirectIntersection(const Triangle& t1, const Triangle& t2){
	AABB aabb1, aabb2;

	aabb1.min.Set(std::min(t1.v1.x, std::min(t1.v2.x, t1.v3.x)),	
				  std::min(t1.v1.y, std::min(t1.v2.y, t1.v3.y)),
				  std::min(t1.v1.z, std::min(t1.v2.z, t1.v3.z)));
	aabb1.max.Set(std::max(t1.v1.x, std::max(t1.v2.x, t1.v3.x)),	
				  std::max(t1.v1.y, std::max(t1.v2.y, t1.v3.y)),
				  std::max(t1.v1.z, std::max(t1.v2.z, t1.v3.z)));

	aabb2.min.Set(std::min(t2.v1.x, std::min(t2.v2.x, t2.v3.x)),	
				  std::min(t2.v1.y, std::min(t2.v2.y, t2.v3.y)),
				  std::min(t2.v1.z, std::min(t2.v2.z, t2.v3.z)));
	aabb2.max.Set(std::max(t2.v1.x, std::max(t2.v2.x, t2.v3.x)),	
				  std::max(t2.v1.y, std::max(t2.v2.y, t2.v3.y)),
				  std::max(t2.v1.z, std::max(t2.v2.z, t2.v3.z)));

	if(!Intersect(aabb1, aabb2))	
		return false;
	if(t1.IsPointInside(t2.v1) || t1.IsPointInside(t2.v2) || t1.IsPointInside(t2.v3))
		return true;
	if(t2.IsPointInside(t1.v1) || t2.IsPointInside(t1.v2) || t2.IsPointInside(t1.v3))
		return true;

	Segment s1, s2;
	s1.Set(t1.v1, t1.v2);

	s2.Set(t2.v1, t2.v2);
	if(Intersect(s1, s2))	return true;
	s2.Set(t2.v2, t2.v3);
	if(Intersect(s1, s2))	return true;
	s2.Set(t2.v3, t2.v1);
	if(Intersect(s1, s2))	return true;

	s1.Set(t1.v2, t1.v3);
	if(Intersect(s1, s2))	return true;
	s2.Set(t2.v1, t2.v3);
	if(Intersect(s1, s2))	return true;
	s2.Set(t2.v2, t2.v3);
	if(Intersect(s1, s2))	return true;

	s1.Set(t1.v3, t1.v1);
	if(Intersect(s1, s2))	return true;
	s2.Set(t2.v3, t2.v1);
	if(Intersect(s1, s2))	return true;
	s2.Set(t2.v1, t2.v2);
	if(Intersect(s1, s2))	return true;

	return false;
} 

bool Intersect(const Triangle& tri1, const Triangle& tri2, Segment* seg){
	Plane p1(tri1.n, tri1.v1), p2(tri2.n, tri2.v1);

	Segment s1, s2;
	if(!Intersect(tri1, p2, &s1) || !Intersect(tri2, p1, &s2))	
		return false;
	
// возможно наложение треугольников
	if(s1.dir.Norm_sq() < EPS)
		return TrianglesDirectIntersection(tri1, tri2);

	float t1, t2;
	if(!Intersect(s1, s2, &t1, &t2))
		return false;

	if(seg)
		seg->Set(s1.GetPoint(t1), s2.GetPoint(t2));
	return true;
}
#pragma endregion


#pragma region Triangle-Point
bool Intersect(const Triangle& t, const Vector3& pt, float* _u, float* _v){
	Vector3 a = t.v2 - t.v1, b = t.v3 - t.v1, d = pt - t.v1;
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
	float w = 1.0f - u - v;

	if(u<0.0f || u>1.0f || v<0.0f || v>1.0f || w<0.0f || w>1.0f)
		return false;

	if(_u)	*_u = u;
	if(_v)	*_v = v;
	return true;
}
#pragma endregion


#pragma region Line-Line
bool Intersect(const Line& line1, const Line& line2, float* _t1, float* _t2){
	Vector3 delta_a		=	line2.origin - line1.origin;
	Vector3 d1_x_d2		=	line1.dir.Cross( line2.dir );
	float d1_x_d2_norm	=	d1_x_d2.Norm_sq();
	float t1, t2;

// линии не параллельны
	if(d1_x_d2_norm > EPS){
		t1 = delta_a.Cross( line2.dir ).Dot( d1_x_d2 ) / d1_x_d2_norm;		// (a2-a1) x d2 * (d1 x d2) / || d1 x d2 || 
		t2 = delta_a.Cross( line1.dir ).Dot( d1_x_d2 ) / d1_x_d2_norm;		// (a2-a1) x d1 * (d1 x d2) / || d1 x d2 ||
	}
	else
		return false;

	if(_t1)		*_t1 = t1;
	if(_t2)		*_t2 = t2;
	return true;
}
#pragma endregion

#pragma region Ray-Ray
bool Intersect(const Ray& ray1, const Ray& ray2, float* _t1, float* _t2){
	float t1, t2;

	if(Intersect(static_cast<const Line&>(ray1), static_cast<const Line&>(ray2), &t1, &t2)){
		if(t1<0.0f || t2<0.0f)
			return false;

		if(_t1)		*_t1 = t1;
		if(_t2)		*_t2 = t2;
		return true;
	}

	return false;
} 
#pragma endregion

#pragma region Segment-Segment
bool SegmentIntersection_1D(float o1, float d1, float o2, float d2, float* _t1, float* _t2){
	float a1 = o1, b1 = o1 + d1;
	float a2 = o2, b2 = o2 + d2;

	if(a1>b1)	std::swap(a1, b1);
	if(a2>b2)	std::swap(a2, b2);

	if(b1<a2 || a1>b2)
		return false;

// концы отрезка на пересечении
	float p1 = std::max(a1, a2);
	float p2 = std::min(b1, b2);
	
	*_t1 = (p1 - o1) / d1;
	*_t2 = (p2 - o2) / d2;
	return true;
}

bool Intersect(const Segment& s1, const Segment& s2, float* _t1, float* _t2){
	Vector3 delta_a		=	s2.origin - s1.origin;
	Vector3 d1_x_d2		=	s1.dir.Cross( s2.dir );
	float d1_x_d2_norm	=	d1_x_d2.Norm_sq();
	float t1, t2;

// отрезки не параллельны
	if(d1_x_d2_norm > EPS){
		t1 = delta_a.Cross( s2.dir ).Dot( d1_x_d2 ) / d1_x_d2_norm;		// (a2-a1) x d2 * (d1 x d2) / || d1 x d2 ||^2 
		t2 = delta_a.Cross( s1.dir ).Dot( d1_x_d2 ) / d1_x_d2_norm;		// (a2-a1) x d1 * (d1 x d2) / || d1 x d2 ||^2

		if(t1<0.0f || t1>1.0f || t2<0.0f || t2>1.0f)
			return false;
	}
	else{
// возможно наложение отрезков
		if(delta_a.IsParallelTo(s1.dir)){	
			int maxDirIndx = 0;
			if(fabs(s1.dir.y) > fabs(s1.dir.x))				maxDirIndx = 1;
			if(fabs(s1.dir.z) > fabs(s1.dir(maxDirIndx)))	maxDirIndx = 2;

			if(!SegmentIntersection_1D(s1.origin(maxDirIndx), s1.dir(maxDirIndx),
									   s2.origin(maxDirIndx), s2.dir(maxDirIndx),
									   &t1, &t2))
			{
				return false;
			}
		}
		else
			return false;
	}

	if(_t1)		*_t1 = t1;
	if(_t2)		*_t2 = t2;
	return true;
}
#pragma endregion

}
}
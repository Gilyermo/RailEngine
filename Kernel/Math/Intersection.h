/********************************************
*	реализация функций проверки пересечений	*
********************************************/
#pragma once

#include "Math.h"


namespace RGE
{
namespace Math
{

#pragma region Line-AABB

KERNEL_API bool Intersect(const Line& line, const AABB& aabb, float* t1=nullptr, float* t2=nullptr);
inline bool Intersect(const AABB& aabb, const Line& line, float* t1=nullptr, float* t2=nullptr){
	return Intersect(line, aabb, t1, t2); 
}

#pragma endregion

#pragma region Ray-AABB

KERNEL_API bool Intersect(const Ray& ray, const AABB& aabb, float* t1=nullptr, float* t2=nullptr);
inline bool Intersect(const AABB& aabb, const Ray& ray, float* t1=nullptr, float* t2=nullptr){
	return Intersect(ray, aabb, t1, t2); 
}

#pragma endregion

#pragma region Segment-AABB

KERNEL_API bool Intersect(const Segment& s, const AABB& aabb, float* t1=nullptr, float* t2=nullptr);
inline bool Intersect(const AABB& aabb, const Segment& s, float* t1=nullptr, float* t2=nullptr){
	return Intersect(s, aabb, t1, t2); 
}

#pragma endregion


#pragma region Line-Sphere

KERNEL_API bool Intersect(const Line& line, const Sphere& s, float* t1=nullptr, float* t2=nullptr);
inline bool Intersect(const Sphere& s, const Line& line, float* t1=nullptr, float* t2=nullptr){
	return Intersect(line, s, t1, t2);
}

#pragma endregion

#pragma region Ray-Sphere

KERNEL_API bool Intersect(const Ray& ray, const Sphere& s, float* t1=nullptr, float* t2=nullptr);
inline bool Intersect(const Sphere& s, const Ray& ray, float* t1=nullptr, float* t2=nullptr){
	return Intersect(ray, s, t1, t2);
}

#pragma endregion

#pragma region Segment-Sphere

KERNEL_API bool Intersect(const Segment& seg, const Sphere& s, float* t1=nullptr, float* t2=nullptr);
inline bool Intersect(const Sphere& s, const Segment& seg, float* t1=nullptr, float* t2=nullptr){
	return Intersect(seg, s, t1, t2);
}

#pragma endregion


#pragma region Line-Plane

KERNEL_API bool Intersect(const Line& line, const Plane& p, float* t=nullptr);
inline bool Intersect(const Plane& p, const Line& line, float* t=nullptr){
	return Intersect(line, p, t);
}

#pragma endregion

#pragma region Ray-Plane

KERNEL_API bool Intersect(const Ray& r, const Plane& p, float* t=nullptr);
inline bool Intersect(const Plane& p, const Ray& r, float* t=nullptr){
	return Intersect(r, p, t);
}

#pragma endregion

#pragma region Segment-Plane

KERNEL_API bool Intersect(const Segment& s, const Plane& p, float* t=nullptr);
inline bool Intersect(const Plane& p, const Segment& s, float* t=nullptr){
	return Intersect(s, p, t);
}

#pragma endregion


#pragma region Line-Triangle

//Pt = (1-u-v)*T.v1 + u*T.v2 + v*T.v3 = line.GetPoint(t)
KERNEL_API bool Intersect(const Line& line, const Triangle& triangle, float* u=nullptr, float* v=nullptr, float* t=nullptr);
inline bool Intersect(const Triangle& triangle, const Line& line, float* u=nullptr, float* v=nullptr, float* t=nullptr){
	return Intersect(line, triangle, u, v, t);
}

#pragma endregion

#pragma region Ray-Triangle

//Pt = (1-u-v)*T.v1 + u*T.v2 + v*T.v3 = ray.GetPoint(t)
KERNEL_API bool Intersect(const Ray& r, const Triangle& triangle, float* u=nullptr, float* v=nullptr, float* t=nullptr);
inline bool Intersect(const Triangle& triangle, const Ray& ray, float* u=nullptr, float* v=nullptr, float* t=nullptr){
	return Intersect(ray, triangle, u, v, t);
}

#pragma endregion

#pragma region Segment-Triangle

//Pt = (1-u-v)*T.v1 + u*T.v2 + v*T.v3 = s.GetPoint(t)
KERNEL_API bool Intersect(const Segment& s, const Triangle& triangle, float* u=nullptr, float* v=nullptr, float* t=nullptr);
inline bool Intersect(const Triangle& triangle, const Segment& s, float* u=nullptr, float* v=nullptr, float* t=nullptr){
	return Intersect(s, triangle, u, v, t);
}

#pragma endregion


#pragma region AABB-AABB
KERNEL_API bool Intersect(const AABB& aabb1, const AABB& aabb2, AABB* aabb = nullptr);
#pragma endregion


#pragma region AABB-Sphere

KERNEL_API bool Intersect(const AABB& aabb, const Sphere& s);
inline bool Intersect(const Sphere& s, const AABB& aabb){
	return Intersect(aabb, s);
}

#pragma endregion


#pragma region Sphere-Sphere
KERNEL_API bool Intersect(const Sphere& s1, const Sphere& s2);
#pragma endregion


#pragma region Frustum-AABB

KERNEL_API bool Intersect(const Frustum& f, const AABB& aabb);
inline bool Intersect(const AABB& aabb, const Frustum& f){
	return Intersect(f, aabb);
}

#pragma endregion


#pragma region Frustum-Sphere
 
KERNEL_API bool Intersect(const Frustum& f, const Sphere& s);
inline bool Intersect(const Sphere& s, const Frustum& f){
	return Intersect(f, s);
}

#pragma endregion


#pragma region Frustum-Triangle

KERNEL_API bool Intersect(const Frustum& f, const Triangle& t);
inline bool Intersect(const Triangle& t, const Frustum& f){
	return Intersect(f, t);
}

#pragma endregion


#pragma region Plane-Plane
KERNEL_API bool Intersect(const Plane& p1, const Plane& p2, Line* line=nullptr);
#pragma endregion


#pragma region Plane-Triangle

//NOTE: если треугольник лежит на плоскости, то содержимое data не изменяется
KERNEL_API bool Intersect(const Plane& p, const Triangle& t, Segment* seg=nullptr);
inline bool Intersect(const Triangle& t, const Plane& p, Segment* seg=nullptr){
	return Intersect(p, t, seg);
}

#pragma endregion


#pragma region Rect-Rect
KERNEL_API bool Intersect(const Rect& rt1, const Rect& rt2, Rect* rect=nullptr);
#pragma endregion


#pragma region Rect-Triangle

// проверка выполняется с проекцией треугольника на плоскость XY. Никаких данных не возвращается
KERNEL_API bool Intersect(const Rect& rect, const Triangle& triangle);
inline bool Intersect(const Triangle& triangle, const Rect& rect){
	return Intersect(rect, triangle);
}

#pragma endregion


#pragma region Rect-Point

KERNEL_API bool Intersect(const Rect& rt, const Vector2& pt, float* u=nullptr, float* v=nullptr);
inline bool Intersect(const Vector2& pt, const Rect& rt, float* u=nullptr, float* v=nullptr){
	return Intersect(rt, pt, u, v);
}

#pragma endregion


#pragma region Triangle-Triangle
KERNEL_API bool Intersect(const Triangle& t1, const Triangle& t2, Segment* seg=nullptr);
#pragma endregion


#pragma region Triangle-Point
//NOTE: для треугольников, близких к вырожденным, результат может быть неверным
// Pt = (1-u-v)*Triangle.v1 + u*Triangle.v2 + v*Triangle.v3
KERNEL_API bool Intersect(const Triangle& t, const Vector3& pt, float* u=nullptr, float* v=nullptr);
inline bool Intersect(const Vector3& pt, const Triangle& t, float* u=nullptr, float* v=nullptr){
	return Intersect(t, pt, u, v);
}

#pragma endregion


#pragma region Line-Line
KERNEL_API bool Intersect(const Line& line1, const Line& line2, float* t1=nullptr, float* t2=nullptr);
#pragma endregion

#pragma region Ray-Ray
KERNEL_API bool Intersect(const Ray& ray1, const Ray& ray2, float* t1=nullptr, float* t2=nullptr);
#pragma endregion

#pragma region Segment-Segment
//NOTE: в случае наложения отрезков s1(t1) - один конец пересечения, а s2(t2) - другой
KERNEL_API bool Intersect(const Segment& s1, const Segment& s2, float* t1=nullptr, float* t2=nullptr);
#pragma endregion

}
}
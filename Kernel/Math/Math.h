/********************************************
*	объ€вление всех математических классов,	*
*			констант и функций				*
********************************************/
#pragma once

#define _USE_MATH_DEFINES
#include "../KernelDLL.h"
#include <math.h>
#include <cstdint>
#include <memory.h>


namespace RGE
{
namespace Math
{
// Constants
 	extern KERNEL_API float EPS /*= 1E-4f*/;

//==============
// Spatial
//==============
	class Vector2;
	class Vector3;
	class Vector4;
	class Matrix33;
	class Matrix44;
	class Quaternion;
	class EulerAngles;

//==============
// Geometry
//==============
	class AABB;
	class Sphere;
	class Frustum;
	class Plane;
	class Rect;
	class Triangle;
	class Line;
	class Ray;
	class Segment;

//==============
// Functions
//==============
	inline float DegreeToRad(float degree){
		return static_cast<float> (degree * M_PI/180.0f);
	}

	inline float RadToDegree(float rad){
		return static_cast<float> (rad * 180.0f/M_PI);
	}

// ceil( (float)a / (float)b );
	template <typename Int> Int Ceil(Int a, Int b){
		return (a + b - 1) / b;
	}

// линейна€ интерпол€ци€ от a до b; t из [0, 1]
	template<typename T>
	T Lerp(const T& a, const T& b, float t){
		return (1.0f-t)*a + t*b;
	}


// определение старшего выставленного бита
	KERNEL_API uint8_t HighestOneBit(uint64_t val);
	KERNEL_API uint8_t HighestOneBit(uint32_t val);
	KERNEL_API uint8_t HighestOneBit(uint16_t val);
	KERNEL_API uint8_t HighestOneBit(uint8_t val);
}
}
/********************************************************
*	пр€моугольник в плоскости OXY, выровненный по ос€м	*
********************************************************/
#pragma once

#include "Math.h"
#include "Vector2.h"


namespace RGE
{
namespace Math
{

class Rect{
public:
	Rect(){}
	Rect(float x_min, float y_min, float x_max, float y_max){
		Set(x_min, y_min, x_max, y_max);
	}
	Rect(const Vector2& _lft_btm, const Vector2& _rgt_top){
		Set(_lft_btm, _rgt_top);
	}
	Rect(const Rect& rt){
		Set(rt.lft_btm, rt.rgt_top);
	}

	void Set(float x_min, float y_min, float x_max, float y_max){
		lft_btm.Set(x_min, y_min);
		rgt_top.Set(x_max, y_max);
	}

	void Set(const Vector2& _lft_btm, const Vector2& _rgt_top){
		lft_btm = _lft_btm;
		rgt_top = _rgt_top;
	}

	bool IsPointInside(const Vector2& pt) const{
		return lft_btm<=pt && pt<=rgt_top;
	}


	Vector2 GetA() const{
		return lft_btm;
	}

	Vector2 GetB() const{
		return Vector2(lft_btm.x, rgt_top.y);
	}

	Vector2 GetC() const{
		return rgt_top;
	}

	Vector2 GetD() const{
		return Vector2(rgt_top.x, lft_btm.y);
	}


	Rect& operator=(const Rect& rt){
		if(this != &rt){
			lft_btm = rt.lft_btm;
			rgt_top = rt.rgt_top;
		}
		return *this;
	}

public:
	Vector2 lft_btm;
	Vector2 rgt_top;
};

}
}
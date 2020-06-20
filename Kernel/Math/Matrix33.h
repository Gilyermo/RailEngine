/************************************
*	двумерный аналог матрицы 4x4;	*
************************************/
#pragma once

#include "Math.h"


namespace RGE
{
namespace Math
{

class KERNEL_API Matrix33{
public:
	Matrix33(){}
	Matrix33(float _11,		 float _12=0.0f, float _13=0.0f,
			 float _21=0.0f, float _22=0.0f, float _23=0.0f,
			 float _31=0.0f, float _32=0.0f, float _33=0.0f)
	{
		Set(_11, _12, _13,
			_21, _22, _23,
			_31, _32, _33);
	}
	Matrix33(const float _m[9]){
		Set(_m);
	}
	Matrix33(const Vector3& row1, const Vector3& row2, const Vector3& row3){
		Set(row1, row2, row3);
	}
	Matrix33(const Matrix33& mat){
		memcpy(m, mat.m, 9*sizeof(float));
	}

	void Set(float _11, float _12, float _13,
			 float _21, float _22, float _23,
			 float _31, float _32, float _33)
	{
		m[0] = _11;		m[1] = _12;		m[2] = _13;
		m[3] = _21;		m[4] = _22;		m[5] = _23;
		m[6] = _31;		m[7] = _32;		m[8]= _33; 
	}

	void Set(const float _m[9]){
		memcpy(m, _m, 9*sizeof(float));
	}

	void		Set(const Vector3& row1, const Vector3& row2, const Vector3& row3);

	void		Fill(float val);

	void		SetRow(int rowIndx, const Vector3& row);
	Vector3		GetRow(int rowIndx) const;

	void		SetCol(int colIndx, const Vector3& col);
	Vector3		GetCol(int colIndx) const;

	float		Determinant() const;

	Vector2		TransformPoint(const Vector2& pt) const;
	Vector2		TransformVector(const Vector2& vec) const;
	Vector3		Transform(const Vector3& vec) const;

	Matrix33&	Identify();
	Matrix33&	Transpose();
	Matrix33	Transposed() const;
	Matrix33&	Inverse(float* det = nullptr);
	Matrix33	Inversed(float* det = nullptr) const;

//NOTE: положительный угол поворота дает поворот против часовой стрелки
	Matrix33&	Rotation(float angle);
	Matrix33&	RotationAroundPoint(const Vector2& pt, float angle);

	Matrix33&	Translation(float px, float py);
	Matrix33&	Translation(const Vector2& pos);
	Matrix33&	Scaling(float sx, float sy);
	Matrix33&	Scaling(const Vector2& scale);


	Matrix33&	operator=(const Matrix33& mat);

	Matrix33&	operator+=(const Matrix33& mat);
	Matrix33&	operator-=(const Matrix33& mat);
	Matrix33&	operator*=(const Matrix33& mat);
	Matrix33&	operator*=(const float a);
	Matrix33&	operator/=(const float a);

	Matrix33	operator+(const Matrix33& mat) const;
	Matrix33	operator-(const Matrix33& mat) const;
	Matrix33	operator*(const Matrix33& mat) const;
	Matrix33	operator*(const float a) const;
	Matrix33	operator/(const float a) const;  

	Matrix33	operator-() const;
	Matrix33	operator+() const;

	bool	operator==(const Matrix33& mat) const;
	bool	operator!=(const Matrix33& mat) const;


	float operator()(int i, int j) const{
		return m[i*3 + j];
	}
	float& operator()(int i, int j){
		return m[i*3 + j];
	}

	float operator[](int elem) const{
		return m[elem];
	}
	float& operator[](int elem){
		return m[elem];
	}

	friend Matrix33 operator*(const float a, const Matrix33& mat){
		Matrix33 res(mat);
		for(int i=0; i<9; i++)
			res.m[i] *= a;
		return res;
	}

private:
	float Minor2(int i1, int i2, int j1, int j2) const{
		return m[i1*3 + j1] * m[i2*3 + j2] - m[i1*3 + j2] * m[i2*3 + j1];
	}

public:
	float m[9];
};


Matrix33 KERNEL_API		Matrix33Rotation(float angle);
Matrix33 KERNEL_API		Matrix33RotationAroundPoint(const Vector2& pt, float angle);
Matrix33 KERNEL_API		Matrix33Translation(float px, float py);
Matrix33 KERNEL_API		Matrix33Translation(const Vector2& pos);
Matrix33 KERNEL_API		Matrix33Scaling(float sx, float sy);
Matrix33 KERNEL_API		Matrix33Scaling(const Vector2& scale);

}
}
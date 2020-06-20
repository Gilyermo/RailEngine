/****************************************************
*	матрица 4x4 в левосторонней системе координат	*
****************************************************/
#pragma once

#include "Math.h"


namespace RGE
{
namespace Math
{

class KERNEL_API Matrix44{
public:
	Matrix44(){}
	Matrix44(float _11,		 float _12=0.0f, float _13=0.0f, float _14=0.0f,
			 float _21=0.0f, float _22=0.0f, float _23=0.0f, float _24=0.0f,
			 float _31=0.0f, float _32=0.0f, float _33=0.0f, float _34=0.0f,
			 float _41=0.0f, float _42=0.0f, float _43=0.0f, float _44=0.0f)
	{
		Set(_11, _12, _13, _14,
			_21, _22, _23, _24,
			_31, _32, _33, _34,
			_41, _42, _43, _44);
	}
	Matrix44(const float _m[16]){
		Set(_m);
	}
	Matrix44(const Vector4& row1, const Vector4& row2, const Vector4& row3, const Vector4& row4){
		Set(row1, row2, row3, row4);
	}
	Matrix44(const Matrix44& mat){
		memcpy(m, mat.m, 16*sizeof(float));
	}

	void Set(float _11, float _12, float _13, float _14,
			 float _21, float _22, float _23, float _24,
			 float _31, float _32, float _33, float _34,
			 float _41, float _42, float _43, float _44)
	{
		m[0] = _11;		m[1] = _12;		m[2] = _13;		m[3] = _14;
		m[4] = _21;		m[5] = _22;		m[6] = _23;		m[7] = _24;
		m[8] = _31;		m[9] = _32;		m[10]= _33;		m[11]= _34;
		m[12]= _41;		m[13]= _42;		m[14]= _43;		m[15]= _44; 
	}
	void Set(const float _m[16]){
		memcpy(m, _m, 16*sizeof(float));
	}
	void Set(const Vector4& row1, const Vector4& row2, const Vector4& row3, const Vector4& row4);

	void		Fill(float val);
	
	void		SetRow(int rowIndx, const Vector4& row);
	Vector4		GetRow(int rowIndx) const;
	
	void		SetCol(int colIndx, const Vector4& col);
	Vector4		GetCol(int colIndx) const;

	float		Determinant() const;

	Vector3		TransformPoint(const Vector3& pt) const;
	Vector3		TransformVector(const Vector3& vec) const;
	Vector4		Transform(const Vector4& vec) const;

	Matrix44&	Identify();
	Matrix44&	Transpose();
	Matrix44	Transposed() const;
	Matrix44&	Inverse(float* det = nullptr);
	Matrix44	Inversed(float* det = nullptr) const;

//NOTE: положительный угол поворота дает поворот против часовой стрелки, если смотерть вдоль оси поворота
	Matrix44&	RotationX(float angle);
	Matrix44&	RotationY(float angle);
	Matrix44&	RotationZ(float angle);
	Matrix44&	Rotation(float x_angle, float y_angle, float z_angle);	// последовательность поворотов (X -> Y -> Z)
	Matrix44&	RotationAroundAxis(const Vector3& axis, float angle);
	Matrix44&	RotationAroundAxis(const Vector3& axis_origin, const Vector3& axis, float angle);

	Matrix44&	Translation(float px, float py, float pz);
	Matrix44&	Translation(const Vector3& pos);
	Matrix44&	Scaling(float sx, float sy, float sz);
	Matrix44&	Scaling(const Vector3& scale);

/**
*	Матрица видового преобразования
*	right, up, look - орты по соответствующим направляениям
*	pos - точка, из которой они выходят
**/
	Matrix44&	View(const Vector3& right, const Vector3& up, const Vector3& look, const Vector3& eye);
	void		ExtractFromView(Vector3* right, Vector3* up, Vector3* look, Vector3* eye) const;

/**
*	Матрица перспективной проекции
*	fovY		-	угол обзора по Y, в радианах
*	aspectRatio -	соотношение сторон экрана
*	zNear, zFar -	ближняя и дальняя плоскости по Z, в мировых координатах
**/
	Matrix44&	PerspectiveFovProjection(float fovY, float aspectRatio, float zNear, float zFar);
	void		ExtractFromPerspectiveFov(float* fovY, float* aspectRatio, float* zNear, float* zFar) const;

/**
*	Матрица перспективной проекции
*	vWidth, vHeight - ширина и высота ближней плоскости, в мировых координатах
*	zNear, zFar		- ближняя и дальняя плоскость по Z, в мировых координатах
**/
	Matrix44&	PerspectiveOffCenterProjection(float vWidth, float vHeight, float zNear, float zFar);
	void		ExtractFromPerspectiveOffCenter(float* vWidth, float* vHeight, float* zNear, float* zFar) const;

/**
*	Матрица ортографической проекции
*	vWidth, vHeight - ширина и высота ближней плоскости, в мировых координатах
*	zNear, zFar		- ближняя и дальняя плоскость по Z, в мировых координатах
**/
	Matrix44&	OrthoProjection(float vWidth, float vHeight, float zNear, float zFar);
	void		ExtractFromOrtho(float* vWidth, float* vHeight, float* zNear, float* zFar) const;
	

// функции конвертирования
	Matrix44&	FromEulerAngles(const EulerAngles& euler_angles);
	EulerAngles ToEulerAngles() const;								// матрица должна содержать только поворот

	Matrix44&	FromQuaternion(const Quaternion& q);				// преобразование из единичного кватерниона
	Quaternion	ToQuaternion() const;								// матрица должна содержать только поворот


	Matrix44&	operator=(const Matrix44& mat);

	Matrix44&	operator+=(const Matrix44& mat);
	Matrix44&	operator-=(const Matrix44& mat);
	Matrix44&	operator*=(const Matrix44& mat);
	Matrix44&	operator*=(const float a);
	Matrix44&	operator/=(const float a);

	Matrix44	operator+(const Matrix44& mat) const;
	Matrix44	operator-(const Matrix44& mat) const;
	Matrix44	operator*(const Matrix44& mat) const;
	Matrix44	operator*(const float a) const;
	Matrix44	operator/(const float a) const;  

	Matrix44	operator-() const;
	Matrix44	operator+() const;

	bool	operator==(const Matrix44& mat) const;
	bool	operator!=(const Matrix44& mat) const;


	float operator()(int i, int j) const{
		return m[(i<<2) + j];
	}
	float& operator()(int i, int j){
		return m[(i<<2) + j];
	}

	float operator[](int elem) const{
		return m[elem];
	}
	float& operator[](int elem){
		return m[elem];
	}

	friend Matrix44 operator*(const float a, const Matrix44& mat){
		Matrix44 res(mat);
		for(int i=0; i<16; i++)
			res.m[i] *= a;
		return res;
	}

private:
	float Minor2(int i1, int i2, int j1, int j2) const{
		return m[(i1<<2) + j1] * m[(i2<<2) + j2] - m[(i1<<2) + j2] * m[(i2<<2) + j1];
	}

public:
	float m[16];
};


Matrix44 KERNEL_API		Matrix44RotationX(float angle);
Matrix44 KERNEL_API		Matrix44RotationY(float angle);
Matrix44 KERNEL_API		Matrix44RotationZ(float angle);
Matrix44 KERNEL_API		Matrix44Rotation(float x_angle, float y_angle, float z_angle);	// последовательность поворотов (X -> Y -> Z)
Matrix44 KERNEL_API		Matrix44RotationAroundAxis(const Vector3& axis, float angle);
Matrix44 KERNEL_API		Matrix44RotationAroundAxis(const Vector3& axis_origin, const Vector3& axis, float angle);
Matrix44 KERNEL_API		Matrix44Translation(float px, float py, float pz);
Matrix44 KERNEL_API		Matrix44Translation(const Vector3& pos);
Matrix44 KERNEL_API		Matrix44Scaling(float sx, float sy, float sz);
Matrix44 KERNEL_API		Matrix44Scaling(const Vector3& scale);
Matrix44 KERNEL_API		Matrix44View(const Vector3& right, const Vector3& up, const Vector3& look, const Vector3& eye);
Matrix44 KERNEL_API		Matrix44PerspectiveFovProjection(float fovY, float aspectRatio, float zNear, float zFar);
Matrix44 KERNEL_API		Matrix44PerspectiveOffCenterProjection(float vWidth, float vHeight, float zNear, float zFar);
Matrix44 KERNEL_API		Matrix44OrthoProjection(float vWidth, float vHeight, float zNear, float zFar);

}
}
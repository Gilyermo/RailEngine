#include "Matrix33.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Plane.h"


namespace RGE
{
namespace Math
{

void Matrix33::Set(const Vector3& row1, const Vector3& row2, const Vector3& row3){
	m[0] = row1.x;		m[1] = row1.y;		m[2] = row1.z;
	m[3] = row2.x;		m[4] = row2.y;		m[5] = row2.z;
	m[6] = row3.x;		m[7] = row3.y;		m[8] = row3.z;
}

void Matrix33::Fill(float val){
	for(int i=0; i<9; i++)	
		m[i] = val;
}

void Matrix33::SetRow(int rowIndx, const Vector3& row){
	int base = 3*rowIndx;
	m[base]		=	row.x;	
	m[base+1]	=	row.y;	
	m[base+2]	=	row.z;
}

Vector3 Matrix33::GetRow(int rowIndx) const{
	int base = rowIndx << 2;
	return Vector3(m[base], m[base+1], m[base+3]);
}

void Matrix33::SetCol(int colIndx, const Vector3& col){
	m[colIndx]		=	col.x;	
	m[colIndx+3]	=	col.y;
	m[colIndx+6]	=	col.z;
}

Vector3 Matrix33::GetCol(int colIndx) const{
	return Vector3(m[colIndx], m[colIndx+3], m[colIndx+6]);
}


float Matrix33::Determinant() const{
	return m[0] * Minor2(1, 2, 1, 2) + 
		   m[1] * Minor2(1, 2, 0, 2) + 
		   m[2] * Minor2(1, 2, 0, 1);
}


Vector2 Matrix33::TransformPoint(const Vector2& pt) const{
	Vector2 res;
	float inv_z = 1.0f / (m[2] + m[5] + m[8]);

	res.x = (pt.x*m[0] + pt.y*m[3] + m[6]) * inv_z;
	res.y = (pt.x*m[1] + pt.y*m[4] + m[7]) * inv_z;

	return res;
}

Vector2 Matrix33::TransformVector(const Vector2& vec) const{
	return Vector2(vec.x*m[0] + vec.y*m[3],
				   vec.x*m[1] + vec.y*m[4]);
}

Vector3 Matrix33::Transform(const Vector3& vec) const{
	Vector3 res;

	res.x = vec.x*m[0] + vec.y*m[3] + vec.z*m[6];
	res.y = vec.x*m[1] + vec.y*m[4] + vec.z*m[7];
	res.z = vec.x*m[2] + vec.y*m[5] + vec.z*m[8];

	return res;
}


Matrix33& Matrix33::Identify(){
	memset(m, 0, 9*sizeof(float));
	m[0] = 1.0f;	m[4] = 1.0f;	m[8] = 1.0f;
	return *this;
}

Matrix33& Matrix33::Transpose(){
	for(int i=0; i<3; i++)
		for(int j=i+1; j<3; j++){
			float tmp = m[i*3 + j];
			m[i*3 + j] = m[j*3 + i];
			m[j*3 + i] = tmp;
		}
	return *this;
}

Matrix33 Matrix33::Transposed() const{
	Matrix33 mat;
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++){
			mat(i,j) = m[j*3 + i];
		}
	return mat;

}

Matrix33& Matrix33::Inverse(float* det){
	float m_1[9];			// det * m^(-1)
	float inv_det;

	m_1[0] =  Minor2(1, 2, 1, 2);
	m_1[1] = -Minor2(0, 2, 1, 2);
	m_1[2] =  Minor2(0, 1, 1, 2);
	
	m_1[3] = -Minor2(1, 2, 0, 2);
	m_1[4] =  Minor2(0, 2, 0, 2);
	m_1[5] = -Minor2(0, 1, 0, 2);

	m_1[6] =  Minor2(1, 2, 0, 1);
	m_1[7] = -Minor2(0, 2, 0, 1);
	m_1[8] =  Minor2(0, 1, 0, 1);

	inv_det = m[0]*m_1[0] + m[1]*m_1[3] + m[2]*m_1[6];
	if(det)					
		*det = inv_det;
	inv_det = 1.0f/inv_det;

	for(int k=0; k<9; k++)
		m[k] = inv_det*m_1[k];

	return *this;
}

Matrix33 Matrix33::Inversed(float* det) const{
	Matrix33 tmp(*this);
	return tmp.Inverse(det);
}


Matrix33& Matrix33::Rotation(float angle){
	float cs = cosf(angle),
		  sn = sinf(angle);

	m[0] = cs;		m[1] = sn;		m[2] = 0.0f;
	m[3] = -sn;		m[4] = cs;		m[5] = 0.0f;
	m[6] = 0.0f;	m[7] = 0.0f;	m[8] = 1.0f;

	return *this;
}

Matrix33& Matrix33::RotationAroundPoint(const Vector2& pt, float angle){
	float cs = cosf(angle), sn = sinf(angle);
	float px = pt.x, py = pt.y;

// Translation(-px, -py) * Rotation(angle) * Translation(px, py)
	m[0] = cs;						m[1] = sn;						m[2] = 0.0f;
	m[3] = -sn;						m[4] = cs;						m[5] = 0.0f;
	m[6] = px*(1.0f-cs) + py*sn;	m[7] = -px*sn + py*(1.0f-cs);	m[8] = 1.0f;

	return *this;
}


Matrix33& Matrix33::Translation(float px, float py){
	m[0] = 1.0f;	m[1] = 0.0f;	m[2] = 0.0f;
	m[3] = 0.0f;	m[4] = 1.0f;	m[5] = 0.0f;
	m[6] = px;		m[7] = py;		m[8] = 1.0f;
	return *this;
}

Matrix33& Matrix33::Translation(const Vector2& pos){
	return Translation(pos.x, pos.y);
}

Matrix33& Matrix33::Scaling(float sx, float sy){
	m[0] = sx;		m[1] = 0.0f;	m[2] = 0.0f;
	m[3] = 0.0f;	m[4] = sy;		m[5] = 0.0f;
	m[6] = 0.0f;	m[7] = 0.0f;	m[8]= 1.0f;
	return *this;
}

Matrix33& Matrix33::Scaling(const Vector2& scale){
	return Scaling(scale.x, scale.y);
}


Matrix33& Matrix33::operator=(const Matrix33& mat){
	if(this != &mat)
		memcpy(m, mat.m, 9 * sizeof(float));
	return *this;
}


Matrix33& Matrix33::operator+=(const Matrix33& mat){
	for(int i=0; i<9; i++)
		m[i] += mat.m[i];
	return *this;
}

Matrix33& Matrix33::operator-=(const Matrix33& mat){
	for(int i=0; i<9; i++)
		m[i] -= mat.m[i];
	return *this;
}

Matrix33& Matrix33::operator*=(const Matrix33& mat){
	float tmp[9];
	const float* m2 = mat.m;

	memcpy(tmp, m, 9*sizeof(float));

	for(int i=0; i<3; i++){
		int base = i*3;

		m[base]		=	tmp[base]*m2[0]	+ tmp[base+1]*m2[3] + tmp[base+2]*m2[6];
		m[base+1]	=	tmp[base]*m2[1] + tmp[base+1]*m2[4] + tmp[base+2]*m2[7];
		m[base+2]	=	tmp[base]*m2[2] + tmp[base+1]*m2[5] + tmp[base+2]*m2[8];
	}

	return *this;
}

Matrix33& Matrix33::operator*=(const float a){
	for(int i=0; i<9; i++)
		m[i] *= a;
	return *this;
}

Matrix33& Matrix33::operator/=(const float a){
	for(int i=0; i<9; i++)
		m[i] /= a;
	return *this;
}


Matrix33 Matrix33::operator+(const Matrix33& mat) const{
	Matrix33 res;
	for(int i=0; i<9; i++)
		res.m[i] = m[i] + mat.m[i];
	return res;
}

Matrix33 Matrix33::operator-(const Matrix33& mat) const{
	Matrix33 res;
	for(int i=0; i<9; i++)
		res.m[i] = m[i] - mat.m[i];
	return res;
}

Matrix33 Matrix33::operator*(const Matrix33& mat) const{
	float res[9];
	const float* m2 = mat.m;

	for(int i=0; i<3; i++){
		int base = i*3;

		res[base]	= m[base]*m2[0]	+ m[base+1]*m2[3] + m[base+2]*m2[6];
		res[base+1] = m[base]*m2[1]	+ m[base+1]*m2[4] + m[base+2]*m2[7];
		res[base+2] = m[base]*m2[2]	+ m[base+1]*m2[5] + m[base+2]*m2[8];
	}

	return Matrix33(res);
}

Matrix33 Matrix33::operator*(const float a) const{
	Matrix33 res;
	for(int i=0; i<9; i++)
		res.m[i] = m[i] * a;
	return res;
}

Matrix33 Matrix33::operator/(const float a) const{
	Matrix33 res;
	for(int i=0; i<9; i++)
		res.m[i] = m[i] / a;
	return res;
}


Matrix33 Matrix33::operator-() const{
	Matrix33 res;
	for(int i=0; i<9; i++)
		res.m[i] = -m[i];
	return res;
}

Matrix33 Matrix33::operator+() const{
	Matrix33 res(m);
	return res;
}


bool Matrix33::operator==(const Matrix33& mat) const{
	for(int i=0; i<9; i++)
		if( fabs(m[i] - mat.m[i]) > EPS )
			return false;
	return true;
}

bool Matrix33::operator!=(const Matrix33& mat) const{
	for(int i=0; i<9; i++)
		if( fabs(m[i] - mat.m[i]) > EPS )
			return true;
	return true;
}


Matrix33 Matrix33Rotation(float angle){
	Matrix33 m;
	return m.Rotation(angle);
}

Matrix33 Matrix33RotationAroundPoint(const Vector2& pt, float angle){
	Matrix33 m;
	return m.RotationAroundPoint(pt, angle);
}

Matrix33 Matrix33Translation(float px, float py){
	Matrix33 m;
	return m.Translation(px, py);
}

Matrix33 Matrix33Translation(const Vector2& pos){
	Matrix33 m;
	return m.Translation(pos.x, pos.y);
}

Matrix33 Matrix33Scaling(float sx, float sy){
	Matrix33 m;
	return m.Scaling(sx, sy);
}

Matrix33 Matrix33Scaling(const Vector2& scale){
	Matrix33 m;
	return m.Scaling(scale.x, scale.y);
}


}
}
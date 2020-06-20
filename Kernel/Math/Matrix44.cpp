#include "Matrix44.h"
#include "Vector4.h"
#include "Vector3.h"
#include "EulerAngles.h"
#include "Quaternion.h"
#include "Plane.h"


namespace RGE
{
namespace Math
{

void Matrix44::Set(const Vector4& row1, const Vector4& row2, const Vector4& row3, const Vector4& row4){
	m[0] = row1.x;		m[1] = row1.y;		m[2] = row1.z;		m[3] = row1.w;
	m[4] = row2.x;		m[5] = row2.y;		m[6] = row2.z;		m[7] = row2.w;
	m[8] = row3.x;		m[9] = row3.y;		m[10]= row3.z;		m[11]= row3.w;
	m[12]= row4.x;		m[13]= row4.y;		m[14]= row4.z;		m[15]= row4.w;
}

void Matrix44::Fill(float val){
	for(int i=0; i<16; i++)	
		m[i] = val;
}

void Matrix44::SetRow(int rowIndx, const Vector4& row){
	int base = 4*rowIndx;
	m[base]		=	row.x;	
	m[base+1]	=	row.y;	
	m[base+2]	=	row.z;	
	m[base+3]	=	row.w;
}

Vector4 Matrix44::GetRow(int rowIndx) const{
	int base = rowIndx << 2;
	return Vector4(m[base], m[base+1], m[base+2], m[base+3]);
}

void Matrix44::SetCol(int colIndx, const Vector4& col){
	m[colIndx]		=	col.x;	
	m[colIndx+4]	=	col.y;
	m[colIndx+8]	=	col.z;
	m[colIndx+12]	=	col.w;
}

Vector4 Matrix44::GetCol(int colIndx) const{
	return Vector4(m[colIndx], m[colIndx+4], m[colIndx+8], m[colIndx+12]);
}


float Matrix44::Determinant() const{
	return Minor2(2, 3, 2, 3) * (m[0]*m[5] - m[1]*m[4]) +
		   Minor2(2, 3, 1, 3) * (m[2]*m[4] - m[0]*m[6]) +
		   Minor2(2, 3, 1, 2) * (m[0]*m[7] - m[3]*m[4]) +
		   Minor2(2, 3, 0, 3) * (m[1]*m[6] - m[2]*m[5]) + 
		   Minor2(2, 3, 0, 2) * (m[3]*m[5] - m[1]*m[7]) +
		   Minor2(2, 3, 0, 1) * (m[2]*m[7] - m[3]*m[6]);
}


Vector3 Matrix44::TransformPoint(const Vector3& pt) const{
	Vector3 res;
	float inv_w = 1.0f / (m[3] + m[7] + m[11] + m[15]);

	res.x = (pt.x*m[0] + pt.y*m[4] + pt.z*m[8] + m[12]) * inv_w;
	res.y = (pt.x*m[1] + pt.y*m[5] + pt.z*m[9] + m[13]) * inv_w;
	res.z = (pt.x*m[2] + pt.y*m[6] + pt.z*m[10]+ m[14]) * inv_w;

	return res;
}

Vector3 Matrix44::TransformVector(const Vector3& vec) const{
	Vector3 res;

	res.x = vec.x*m[0] + vec.y*m[4] + vec.z*m[8];
	res.y = vec.x*m[1] + vec.y*m[5] + vec.z*m[9];
	res.z = vec.x*m[2] + vec.y*m[6] + vec.z*m[10];

	return res;
}

Vector4 Matrix44::Transform(const Vector4& vec) const{
	Vector4 res;

	res.x = vec.x*m[0] + vec.y*m[4] + vec.z*m[8] + vec.w*m[12];
	res.y = vec.x*m[1] + vec.y*m[5] + vec.z*m[9] + vec.w*m[13];
	res.z = vec.x*m[2] + vec.y*m[6] + vec.z*m[10]+ vec.w*m[14];
	res.w = vec.x*m[3] + vec.y*m[7] + vec.z*m[11]+ vec.w*m[15];

	return res;
}


Matrix44& Matrix44::Identify(){
	memset(m, 0, 16*sizeof(float));
	m[0] = 1.0f;	m[5] = 1.0f;	m[10] = 1.0f;	m[15] = 1.0f;
	return *this;
}

Matrix44& Matrix44::Transpose(){
	for(int i=0; i<4; i++)
		for(int j=i+1; j<4; j++){
			float tmp = m[(i<<2) + j];
			m[(i<<2) + j] = m[(j<<2) + i];
			m[(j<<2) + i] = tmp;
		}
	return *this;
}

Matrix44 Matrix44::Transposed() const{
	Matrix44 mat;
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++){
			mat(i,j) = m[(j<<2) + i];
		}
	return mat;
}

Matrix44& Matrix44::Inverse(float* det){
	float m_1[16];			// det * m^(-1)
	float a01, a02, a03;	// миноры второго порядка по соответствующим столбцам
	float a12, a13, a23;
	
	for(int i=0; i<4; i+=2){
		int i1 = i+1;

		a01 = Minor2(3-i1, 3-i, 0, 1);
		a02 = Minor2(3-i1, 3-i, 0, 2);
		a03 = Minor2(3-i1, 3-i, 0, 3);
		a12 = Minor2(3-i1, 3-i, 1, 2);
		a13 = Minor2(3-i1, 3-i, 1, 3);
		a23 = Minor2(3-i1, 3-i, 2, 3);

		m_1[0*4+i] =  (m[i1*4+1]*a23 - m[i1*4+2]*a13 + m[i1*4+3]*a12);		//A[i][0]
		m_1[1*4+i] = -(m[i1*4+0]*a23 - m[i1*4+2]*a03 + m[i1*4+3]*a02);		//A[i][1]
		m_1[2*4+i] =  (m[i1*4+0]*a13 - m[i1*4+1]*a03 + m[i1*4+3]*a01);		//A[i][2]
		m_1[3*4+i] = -(m[i1*4+0]*a12 - m[i1*4+1]*a02 + m[i1*4+2]*a01);		//A[i][3]

		m_1[0*4+i1] = -(m[i*4+1]*a23 - m[i*4+2]*a13 + m[i*4+3]*a12);			//A[i1][0]
		m_1[1*4+i1] =  (m[i*4+0]*a23 - m[i*4+2]*a03 + m[i*4+3]*a02);			//A[i1][1]
		m_1[2*4+i1] = -(m[i*4+0]*a13 - m[i*4+1]*a03 + m[i*4+3]*a01);			//A[i1][2]
		m_1[3*4+i1] =  (m[i*4+0]*a12 - m[i*4+1]*a02 + m[i*4+2]*a01);			//A[i1][3]
	}

	float inv_det = m[0]*m_1[0] + m[1]*m_1[4] + m[2]*m_1[8] + m[3]*m_1[12];

	if(det)					
		*det = inv_det;

	inv_det = 1.0f/inv_det;

	for(int k=0; k<16; k++)
		m[k] = inv_det*m_1[k];

	return *this;
}

Matrix44 Matrix44::Inversed(float* det) const{
	Matrix44 tmp(*this);
	return tmp.Inverse(det);
}


Matrix44& Matrix44::RotationX(float angle){
	float cs = cosf(angle),
		  sn = sinf(angle);

	m[0] = 1.0f;	m[1] = 0.0f;	m[2] = 0.0f;	m[3] = 0.0f;
	m[4] = 0.0f;	m[5] = cs;		m[6] = sn;		m[7] = 0.0f;
	m[8] = 0.0f;	m[9] = -sn;		m[10]= cs;		m[11]= 0.0f;
	m[12]= 0.0f;	m[13]= 0.0f;	m[14]= 0.0f;	m[15]= 1.0f;

	return *this;
}

Matrix44& Matrix44::RotationY(float angle){
	float cs = cosf(angle),
		  sn = sinf(angle);

	m[0] = cs;		m[1] = 0.0f;	m[2] = -sn;		m[3] = 0.0f;
	m[4] = 0.0f;	m[5] = 1.0f;	m[6] = 0.0f;	m[7] = 0.0f;
	m[8] = sn;		m[9] = 0.0f;	m[10]= cs;		m[11]= 0.0f;
	m[12]= 0.0f;	m[13]= 0.0f;	m[14]= 0.0f;	m[15]= 1.0f;

	return *this;
}

Matrix44& Matrix44::RotationZ(float angle){
	float cs = cosf(angle),
		  sn = sinf(angle);

	m[0] = cs;		m[1] = sn;		m[2] = 0.0f;	m[3] = 0.0f;
	m[4] = -sn;		m[5] = cs;		m[6] = 0.0f;	m[7] = 0.0f;
	m[8] = 0.0f;	m[9] = 0.0f;	m[10]= 1.0f;	m[11]= 0.0f;
	m[12]= 0.0f;	m[13]= 0.0f;	m[14]= 0.0f;	m[15]= 1.0f;

	return *this;
}

Matrix44& Matrix44::Rotation(float x_angle, float y_angle, float z_angle){
	float cs_x = cosf(x_angle), sn_x = sinf(x_angle);
	float cs_y = cosf(y_angle), sn_y = sinf(y_angle);
	float cs_z = cosf(z_angle), sn_z = sinf(z_angle);

	m[0] = cs_y * cs_z;						m[1] = cs_y * sn_z;						m[2] = -sn_y;		m[3] = 0.0f;
	m[4] = cs_z*sn_x*sn_y - cs_x*sn_z;		m[5] = cs_x*cs_z + sn_x*sn_y*sn_z;		m[6] = cs_y*sn_x;	m[7] = 0.0f;
	m[8] = sn_x*sn_z + cs_x*cs_z*sn_y;		m[9] = cs_x*sn_y*sn_z - cs_z*sn_x;		m[10]= cs_x*cs_y;	m[11]= 0.0f;
	m[12]= 0.0f;							m[13]= 0.0f;							m[14]= 0.0f;		m[15]= 1.0f;

	return *this;
}

Matrix44& Matrix44::RotationAroundAxis(const Vector3& axis, float angle){
	float a = axis.x, 
		  b = axis.y, 
		  c = axis.z;
	float d = sqrtf(b*b + c*c);
	float cs = cosf(angle),
		  sn = sinf(angle);
		
	if(d<EPS){
		m[0] = 1.0f;	m[1] = 0.0f;	m[2] = 0.0f;	m[3] = 0.0f;
		m[4] = 0.0f;	m[5] = cs;		m[6] = sn;		m[7] = 0.0f;
		m[8] = 0.0f;	m[9] = -sn;		m[10]= cs;		m[11]= 0.0f;
		m[12]= 0.0f;	m[13]= 0.0f;	m[14]= 0.0f;	m[15]= 1.0f;

		return *this;
	}
		
	//RotationX*RotationY
	m[0] = d;			m[1] = 0.0f;	m[2] = a;		m[3] = 0.0f;
	m[4] = -a*b/d;		m[5] = c/d;		m[6] = b;		m[7] = 0.0f;
	m[8] = -a*c/d;		m[9] = -b/d;	m[10]= c;		m[11]= 0.0f;
	m[12] = 0.0f;		m[13] = 0.0f;	m[14] = 0.0f;	m[15] = 1.0f;
	
	//RotationZ*RotationY^(-1)*RotationX^(-1)
	Matrix44 tmp(
		 cs*d, (c*sn - a*b*cs)/d,	-(a*c*cs + sn*b)/d,	0.0f,
		-d*sn, (c*cs + a*b*sn)/d,	(a*c*sn - b*cs)/d,	0.0f,
		   a,  	     b,					 c,				0.0f,
		  0.0f,		0.0f,				0.0f,			1.0f);

	*this *= tmp;
	return *this;
}

Matrix44& Matrix44::RotationAroundAxis(const Vector3& axis_orig, const Vector3& axis, float angle){
	float a = axis.x, 
		  b = axis.y, 
		  c = axis.z;
	float d = sqrtf(b*b + c*c);
	float px = axis_orig.x, 
		  py = axis_orig.y, 
		  pz = axis_orig.z;
	float cs = cosf(angle),
		  sn = sinf(angle);
		
	if(d<EPS){
		m[0] = 1.0f;	m[1] = 0.0f;	m[2] = 0.0f;	m[3] = 0.0f;
		m[4] = 0.0f;	m[5] = cs;		m[6] = sn;		m[7] = 0.0f;
		m[8] = 0.0f;	m[9] = -sn;		m[10]= cs;		m[11]= 0.0f;
		m[12]= 0.0f;
		m[13]= -py*(cs-1) + pz*sn;	
		m[14]= pz*(cs-1) - py*sn;
		m[15]= 1.0f;

		return *this;
	}
		
	//Translate*RoatationX*RotationY
	m[0] = d;			m[1] = 0.0f;	m[2] = a;	m[3] = 0.0f;
	m[4] = -a*b/d;		m[5] = c/d;		m[6] = b;	m[7] = 0.0f;
	m[8] = -a*c/d;		m[9] = -b/d;	m[10]= c;	m[11]= 0.0f;
		
	m[12] = -d*px + a/d*(b*py + c*pz);
	m[13] = (-c*py + b*pz)/d;
	m[14] = -b*py - c*pz - a*px;
	m[15] = 1.0f;
	
	//RotationZ*RotationY^(-1)*RotationX^(-1)*Translate^(-1)
	Matrix44 tmp(
		 cs*d, (c*sn - a*b*cs)/d,-(a*c*cs + sn*b)/d,	0.0f,
		-d*sn, (c*cs + a*b*sn)/d, (a*c*sn - b*cs)/d,	0.0f,
			a,   	     b,				c,				0.0f,
		   px,			py,			   pz,				1.0f);

	*this *= tmp;
	return *this;
}


Matrix44& Matrix44::Translation(float px, float py, float pz){
	m[0] = 1.0f;	m[1] = 0.0f;	m[2] = 0.0f;	m[3] = 0.0f;
	m[4] = 0.0f;	m[5] = 1.0f;	m[6] = 0.0f;	m[7] = 0.0f;
	m[8] = 0.0f;	m[9] = 0.0f;	m[10]= 1.0f;	m[11]= 0.0f;
	m[12]= px;		m[13]= py;		m[14]= pz;		m[15]= 1.0f;

	return *this;
}

Matrix44& Matrix44::Translation(const Vector3& pos){
	return Translation(pos.x, pos.y, pos.z);
}

Matrix44& Matrix44::Scaling(float sx, float sy, float sz){
	m[0] = sx;		m[1] = 0.0f;	m[2] = 0.0f;	m[3] = 0.0f;
	m[4] = 0.0f;	m[5] = sy;		m[6] = 0.0f;	m[7] = 0.0f;
	m[8] = 0.0f;	m[9] = 0.0f;	m[10]= sz;		m[11]= 0.0f;
	m[12]= 0.0f;	m[13]= 0.0f;	m[14]= 0.0f;	m[15]= 1.0f;

	return *this;
}

Matrix44& Matrix44::Scaling(const Vector3& scale){
	return Scaling(scale.x, scale.y, scale.z);
}


Matrix44& Matrix44::View(const Vector3& right, const Vector3& up, const Vector3& look, const Vector3& eye){
	m[0] = right.x;			m[1] = up.x;		m[2] = look.x;			m[3] = 0.0f;
	m[4] = right.y;			m[5] = up.y;		m[6] = look.y;			m[7] = 0.0f;
	m[8] = right.z;			m[9] = up.z;		m[10]= look.z;			m[11]= 0.0f;
	m[12]= -eye.Dot(right); m[13]= -eye.Dot(up);m[14]= -eye.Dot(look);	m[15]= 1.0f;

	return *this;
}

void Matrix44::ExtractFromView(Vector3* right, Vector3* up, Vector3* look, Vector3* eye) const{
	right->Set(m[0], m[4], m[8]);
	up->Set(m[1], m[5], m[9]);
	look->Set(m[2], m[6], m[10]);

	Vector3 z(-m[12], -m[13], -m[14]);
	Vector3 v = z.Cross(*up);
	float denom = 1.0f / right->Dot( up->Cross(*look) );

	eye->Set(v.Dot(*look) * denom,
			 right->Dot( z.Cross(*look) ) * denom,
			 -right->Dot(v) * denom);
}


Matrix44& Matrix44::PerspectiveFovProjection(float fovY, float aspectRatio, float zNear, float zFar){
	float scaleY = 1.0f / tanf(fovY / 2.0f);
	float scaleX = scaleY / aspectRatio;

	m[0] = scaleX;	m[1] = 0.0f;	m[2] = 0.0f;						m[3] = 0.0f;
	m[4] = 0.0f;	m[5] = scaleY;	m[6] = 0.0f;						m[7] = 0.0f;
	m[8] = 0.0f;	m[9] = 0.0f;	m[10]= zFar / (zFar-zNear);			m[11]= 1.0f;
	m[12]= 0.0f;	m[13]= 0.0f;	m[14]= -zNear*zFar / (zFar-zNear);	m[15]= 0.0f;

	return *this;
}

void Matrix44::ExtractFromPerspectiveFov(float* fovY, float* aspectRatio, float* zNear, float* zFar) const{
	*fovY			=	2.0f * atanf(1.0f/m[5]);
	*aspectRatio	=	m[5] / m[0];
	*zNear			=	-m[14] / m[10];
	*zFar			=	-m[14] / (m[10] - 1.0f);
}


Matrix44& Matrix44::PerspectiveOffCenterProjection(float vWidth, float vHeight, float zNear, float zFar){
	float scaleX = 2.0f*zNear / vWidth;
	float scaleY = 2.0f*zNear / vHeight;

	m[0] = scaleX;		m[1] = 0.0f;	m[2] = 0.0f;						m[3] = 0.0f;
	m[4] = 0.0f;		m[5] = scaleY;	m[6] = 0.0f;						m[7] = 0.0f;
	m[8] = 0.0f;		m[9] = 0.0f;	m[10]= zFar / (zFar-zNear);			m[11]= 1.0f;
	m[12]= 0.0f;		m[13]= 0.0;		m[14]= -zFar*zNear / (zFar-zNear);	m[15]= 0.0f;

	return *this;
}

void Matrix44::ExtractFromPerspectiveOffCenter(float* vWidth, float* vHeight, float* zNear, float* zFar) const{
	*zNear		=	-m[14] / m[10];
	*zFar		=	-m[14] / (m[10] - 1.0f);
	*vWidth		=	2.0f*(*zNear) / m[0];
	*vHeight	=	2.0f*(*zNear) / m[5];
}


Matrix44& Matrix44::OrthoProjection(float vWidth, float vHeight, float zNear, float zFar){
	m[0] = 2.0f / vWidth;	m[1] = 0.0f;			m[2] = 0.0f;					m[3] = 0.0f;
	m[4] = 0.0f;			m[5] = 2.0f / vHeight;	m[6] = 0.0f;					m[7] = 0.0f;
	m[8] = 0.0f;			m[9] = 0.0f;			m[10]= 1.0f / (zFar-zNear);		m[11]= 0.0f;
	m[12]= 0.0f;			m[13]= 0.0;				m[14]= -zNear / (zFar-zNear);	m[15]= 1.0f;

	return *this;
}

void Matrix44::ExtractFromOrtho(float* vWidth, float *vHeight, float* zNear, float* zFar) const{
	*vWidth		=	2.0f / m[0];
	*vHeight	=	2.0f / m[5];
	*zNear		=	-m[14] / m[10];
	*zFar		=	1.0f/m[10] + *zNear;
}


Matrix44& Matrix44::FromEulerAngles(const EulerAngles& euler_angles){
	return Rotation(DegreeToRad(euler_angles.x), 
					DegreeToRad(euler_angles.y), 
					DegreeToRad(euler_angles.z));
}

// матрица должна содержать только поворот
EulerAngles Matrix44::ToEulerAngles() const{
	float alf_x = 0.0f;
	float alf_y = asinf(-m[2]);
	float alf_z = 0.0f;

	float cos_alf_y = cosf(alf_y);
	if(fabs(cos_alf_y)>EPS)
	{
		alf_x = asinf(m[1*4 + 2] / cos_alf_y);
		alf_z = asinf(m[1] / cos_alf_y);
	}
	else
	{
		float bet = 0.0f;
		float gam = 0.0f;

		if(m[2]>0.0f)	// alf_y == M_PI/2.0
		{
			bet = -asinf(m[2*4 + 1] - 2.0f * m[1*4 + 0]);
			gam = asinf(-m[2*4 + 1]);
		}
		else				// alf_y == -M_PI/2.0
		{
			bet = asinf(-m[2*4 + 1]);
			gam = -asinf(m[2*4 + 1] + 2.0f*m[1*4 + 0]);
		}

		alf_x = 0.5f * (bet + gam);
		alf_z = 0.5f * (bet - gam);
	}

	return EulerAngles(RadToDegree(alf_x), RadToDegree(alf_y), RadToDegree(alf_z));
}

// преобразование из единичного кватерниона
Matrix44& Matrix44::FromQuaternion(const Quaternion& q){
	float x2 = 2.0f * q.v.x * q.v.x,	y2 = 2.0f * q.v.y * q.v.y,		z2 = 2.0f * q.v.z * q.v.z;
	float xy = 2.0f * q.v.x * q.v.y,	xz = 2.0f * q.v.x * q.v.z,		xw = 2.0f * q.v.x * q.w;
	float yz = 2.0f * q.v.y * q.v.z,	yw = 2.0f * q.v.y * q.w;
	float zw = 2.0f * q.v.z * q.w;

	m[0] = 1.0f - y2-z2;	m[1] = xy - zw;			m[2] = xz - yw;			m[3] = 0.0f;
	m[4] = xy + zw;			m[5] = 1.0f - x2-z2;	m[6] = yz + xw;			m[7] = 0.0f;
	m[8] = xz + yw;			m[9] = yz - xw;			m[10]= 1.0f - x2-y2;	m[11]= 0.0f;
	m[12]= 0.0f;			m[13]= 0.0f;			m[14]= 0.0f;			m[15]= 1.0f;

	return *this;
}

// матрица должна содержать только поворот
Quaternion Matrix44::ToQuaternion() const{
	float trace = m[0] + m[5] + m[10] + 1.0f;

// делаем преобразование сразу
	if(trace>0.0f){
		float w = 0.5f * sqrtf(trace);
		float s = 0.25f / w;
		return Quaternion( (m[6]-m[9])*s, (m[8]-m[2])*s, (m[4]-m[1])*s, w );
	}

// находим максимальный элемент на диагонали и в зависимости от него вычисляем кватернион
	if(m[0]>m[5] && m[0]>m[10]){
		float x = 0.5f * sqrtf(1.0f + m[0] - m[5] - m[10]);
		float s = 0.25f / x;
		return Quaternion( x, (m[4]+m[2])*s, (m[2]+m[8])*s, (m[6]-m[9])*s ); 
	}

	if(m[5]>m[0] && m[5]>m[10]){
		float y = 0.5f * sqrtf(1.0f + m[5] - m[0] - m[10]);
		float s = 0.25f / y;
		return Quaternion( (m[1]+m[4])*s, y, (m[6]+m[9])*s, (m[8]-m[2])*s );
	}

	if(m[10]>m[0] && m[10]>m[5]){
		float z = 0.5f * sqrtf(1.0f + m[10] - m[0] - m[5]);
		float s = 0.25f / z;
		return Quaternion( (m[2]+m[8])*s, (m[9]+m[6])*s, z, (m[4]-m[1])*s);
	}

	return Quaternion();
}


Matrix44& Matrix44::operator=(const Matrix44& mat){
	if(this != &mat)
		memcpy(m, mat.m, 16 * sizeof(float));
	return *this;
}


Matrix44& Matrix44::operator+=(const Matrix44& mat){
	for(int i=0; i<16; i++)
		m[i] += mat.m[i];
	return *this;
}

Matrix44& Matrix44::operator-=(const Matrix44& mat){
	for(int i=0; i<16; i++)
		m[i] -= mat.m[i];
	return *this;
}

Matrix44& Matrix44::operator*=(const Matrix44& mat){
	float tmp[16];
	const float* m2 = mat.m;

	memcpy(tmp, m, 16*sizeof(float));

	for(int i=0; i<4; i++){
		int base = i<<2;

		m[base]		=	tmp[base]*m2[0] + tmp[base+1]*m2[0+4] +
	 					tmp[base+2]*m2[0+8] + tmp[base+3]*m2[0+12];
	
		m[base+1]	=	tmp[base]*m2[1] + tmp[base+1]*m2[1+4] +
	 					tmp[base+2]*m2[1+8] + tmp[base+3]*m2[1+12];

		m[base+2]	=	tmp[base]*m2[2] + tmp[base+1]*m2[2+4] +
	 					tmp[base+2]*m2[2+8] + tmp[base+3]*m2[2+12];

		m[base+3]	=	tmp[base]*m2[3] + tmp[base+1]*m2[3+4] +
	 					tmp[base+2]*m2[3+8] + tmp[base+3]*m2[3+12];
	}

	return *this;
}

Matrix44& Matrix44::operator*=(const float a){
	for(int i=0; i<16; i++)
		m[i] *= a;
	return *this;
}

Matrix44& Matrix44::operator/=(const float a){
	for(int i=0; i<16; i++)
		m[i] /= a;
	return *this;
}


Matrix44 Matrix44::operator+(const Matrix44& mat) const{
	Matrix44 res;
	for(int i=0; i<16; i++)
		res.m[i] = m[i] + mat.m[i];
	return res;
}

Matrix44 Matrix44::operator-(const Matrix44& mat) const{
	Matrix44 res;
	for(int i=0; i<16; i++)
		res.m[i] = m[i] - mat.m[i];
	return res;
}

Matrix44 Matrix44::operator*(const Matrix44& mat) const{
	float res[16];
	const float* m2 = mat.m;

	for(int i=0; i<4; i++){
		int base = i<<2;

		res[base]	= m[base]*m2[0]	  + m[base+1]*m2[4] +
					  m[base+2]*m2[8] + m[base+3]*m2[12];

		res[base+1] = m[base]*m2[1]		+ m[base+1]*m2[1+4] +
					  m[base+2]*m2[1+8] + m[base+3]*m2[1+12];

		res[base+2] = m[base]*m2[2]		+ m[base+1]*m2[2+4] +
					  m[base+2]*m2[2+8] + m[base+3]*m2[2+12];

		res[base+3] = m[base]*m2[3]		+ m[base+1]*m2[3+4] +
					  m[base+2]*m2[3+8] + m[base+3]*m2[3+12];
	}

	return Matrix44(res);
}

Matrix44 Matrix44::operator*(const float a) const{
	Matrix44 res;
	for(int i=0; i<16; i++)
		res.m[i] = m[i] * a;
	return res;
}

Matrix44 Matrix44::operator/(const float a) const{
	Matrix44 res;
	for(int i=0; i<16; i++)
		res.m[i] = m[i] / a;
	return res;
}


Matrix44 Matrix44::operator-() const{
	Matrix44 res;
	for(int i=0; i<16; i++)
		res.m[i] = -m[i];
	return res;
}

Matrix44 Matrix44::operator+() const{
	Matrix44 res(m);
	return res;
}


bool Matrix44::operator==(const Matrix44& mat) const{
	for(int i=0; i<16; i++)
		if( fabs(m[i] - mat.m[i]) > EPS )
			return false;
	return true;
}

bool Matrix44::operator!=(const Matrix44& mat) const{
	for(int i=0; i<16; i++)
		if( fabs(m[i] - mat.m[i]) > EPS )
			return true;
	return true;
}


Matrix44 Matrix44RotationX(float angle){
	Matrix44 m;
	return m.RotationX(angle);
}

Matrix44 Matrix44RotationY(float angle){
	Matrix44 m;
	return m.RotationY(angle);
}

Matrix44 Matrix44RotationZ(float angle){
	Matrix44 m;
	return m.RotationZ(angle);
}

Matrix44 Matrix44Rotation(float x_angle, float y_angle, float z_angle){
	Matrix44 m;
	return m.Rotation(x_angle, y_angle, z_angle);
}

Matrix44 Matrix44RotationAroundAxis(const Vector3& axis, float angle){
	Matrix44 m;
	return m.RotationAroundAxis(axis, angle);
}

Matrix44 Matrix44RotationAroundAxis(const Vector3& axis_origin, const Vector3& axis, float angle){
	Matrix44 m;
	return m.RotationAroundAxis(axis_origin, axis, angle);
}

Matrix44 Matrix44Translation(float px, float py, float pz){
	Matrix44 m;
	return m.Translation(px, py, pz);
}

Matrix44 Matrix44Translation(const Vector3& pos){
	Matrix44 m;
	return m.Translation(pos.x, pos.y, pos.z);
}

Matrix44 Matrix44Scaling(float sx, float sy, float sz){
	Matrix44 m;
	return m.Scaling(sx, sy, sz);
}

Matrix44 Matrix44Scaling(const Vector3 scale){
	Matrix44 m;
	return m.Scaling(scale.x, scale.y, scale.z);
}

Matrix44 Matrix44View(const Vector3& right, const Vector3& up, const Vector3& look, const Vector3& eye){
	Matrix44 m;
	return m.View(right, up, look, eye);
}

Matrix44 Matrix44PerspectiveFovProjection(float fovY, float aspectRatio, float zNear, float zFar){
	Matrix44 m;
	return m.PerspectiveFovProjection(fovY, aspectRatio, zNear, zFar);
}

Matrix44 Matrix44PerspectiveOffCenterProjection(float vWidth, float vHeight, float zNear, float zFar){
	Matrix44 m;
	return m.PerspectiveOffCenterProjection(vWidth, vHeight, zNear, zFar);
}

Matrix44 Matrix44OrthoProjection(float vWidth, float vHeight, float zNear, float zFar){
	Matrix44 m;
	return m.OrthoProjection(vWidth, vHeight, zNear, zFar);
}

}
}
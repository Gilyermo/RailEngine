#include "Frustum.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix44.h"


namespace RGE
{
namespace Math
{

void Frustum::Set(const Vector3& ntl, const Vector3& ntr, const Vector3& nbr, const Vector3& nbl,
				  const Vector3& ftl, const Vector3& ftr, const Vector3& fbr, const Vector3& fbl)
{
	plane[Near].Set(ntr, ntl, nbl);
	plane[Far].Set(ftl, ftr, fbr);
	plane[Lft].Set(ntl, ftl, fbl);
	plane[Rgt].Set(ftr, ntr, nbr);
	plane[Top].Set(ntl, ntr, ftr);
	plane[Btm].Set(fbl, fbr, nbr);
}

void Frustum::Set(const Matrix44& viewProj){
	Vector4 col3 = viewProj.GetCol(3);
	plane[Near].Set( viewProj.GetCol(2) );
	plane[Far].Set( col3 - viewProj.GetCol(2) );
	plane[Lft].Set( col3 + viewProj.GetCol(0) );
	plane[Rgt].Set( col3 - viewProj.GetCol(0) );
	plane[Top].Set( col3 - viewProj.GetCol(1) );
	plane[Btm].Set( col3 + viewProj.GetCol(1) );
}


bool Frustum::IsPointInside(const Vector3& pt) const{
	for(int i=0; i<PlaneCount; i++)
		if(plane[i].SideOf(pt) == PlaneSide::Back)
			return false;
	return true;
}

}
}
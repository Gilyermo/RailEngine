#include "CppUnitTest.h"
#include "CppUnitTestAssert.h"
#include <Math\Matrix44.h>
#include <Math\Matrix33.h>
#include <Math\Vector4.h>
#include <Math\AABB.h>
#include <Math\Sphere.h>
#include <Math\Plane.h>
#include <Math\Triangle.h>
#include <Math\Rect.h>
#include <Math\Frustum.h>
#include <Math\Line.h>
#include <Math\Ray.h>
#include <Math\Segment.h>
#include <Math\Intersection.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <cstdint>
#include <random>
#include <Windows.h>


namespace RGE
{
namespace UnitTest
{

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Math;


TEST_CLASS(MathTest){
public:
	TEST_METHOD(MatrixTest){
		Matrix44 m1;
		Matrix33 m2;

		for(int i = 0; i<3; i++)
			for(int j = 0; j<3; j++)
				m2(i, j) = m1(i, j) = 2 * (i == j) + 0.5f * (rand() % 10);
		m1.SetRow(3, Vector4(0.0f, 0.0f, 0.0f, 1.0f));

		float det11 = 0.0f, det12 = 0.0f;
		float det21 = 0.0f, det22 = 0.0f;
		det12 = m1.Inverse(&det11).Determinant();
		det22 = m2.Inverse(&det21).Determinant();

		float val1 = det11 + det21;
		float val2 = 1.0f / det12 + 1.0f / det22;

		bool fail_1 = fabs(det11 - det21) > EPS;
		bool fail_2 = fabs(det12 - det22) > EPS;
		bool fail_3 = fabs(val1 - val2) > EPS;

		Assert::IsFalse(fail_1 || fail_2 || fail_3);
	}

	template<class C1, class C2>
	struct IntersectionTestData{
		void Set(const C1& _c1, const C2& _c2, bool Intersect){
			c1 = _c1;	c2 = _c2;	hasIntersection = Intersect;
		}

		C1		c1;
		C2		c2;
		bool	hasIntersection;
	};

	TEST_METHOD(LineBoxIntersectionTest){
		AABB aabb(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));
		Segment s1(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f)),
			s2(Vector3(-2.0f, 0.0f, 0.0f), Vector3(2.0f, 0.0f, 0.0f)),
			s3(Vector3(-2.0f, -2.0f, -2.0f), Vector3()),
			s4(Vector3(-1.0f, 2.0f, -1.0f), Vector3(1.0f, 1.5f, 1.0f));
		Ray r1(Vector3(), Vector3(0.5f, 0.5f, 0.5f)),
			r2(Vector3(2.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
		Line line1(Vector3(), Vector3(0.5f, 0.5f, 0.5f)),
			line2(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));

		float t1, t2;

		bool b = Intersect(s1, aabb, &t1, &t2);
		b = Intersect(s2, aabb, &t1, &t2);
		b = Intersect(s3, aabb, &t1, &t2);
		b = Intersect(s4, aabb, &t1, &t2);
		b = Intersect(r1, aabb, &t1, &t2);
		b = Intersect(r2, aabb, &t1, &t2);
		b = Intersect(line1, aabb, &t1, &t2);
		b = Intersect(line2, aabb, &t1, &t2);
	}

	TEST_METHOD(LineSphereIntersectionTest){
		Sphere s(Vector3(), 1.0f);
		Line line1(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f)),
			line2(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, -1.0f, 1.0f)),
			line3(Vector3(), Vector3(1.0f));
		Ray ray1(Vector3(1.0f, 1.0f, 1.0f), Vector3(-1.0f, -1.0f, -1.0f)),
			ray2(Vector3(-1.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f)),
			ray3(Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f)),
			ray4(Vector3(), Vector3(1.0f, 0.0f, 0.0f));
		Segment seg1(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f)),
			seg2(Vector3(0.5f, 0.0f, 0.0f), Vector3(2.0f, 0.0f, 0.0f)),
			seg3(Vector3(2.0f, 0.0f), Vector3(2.0f, 1.0f));

		float t1, t2;

		bool b = Intersect(s, line1, &t1, &t2);
		b = Intersect(s, line2, &t1, &t2);
		b = Intersect(s, line3, &t1, &t2);
		b = Intersect(s, ray1, &t1, &t2);
		b = Intersect(s, ray2, &t1, &t2);
		b = Intersect(s, ray3, &t1, &t2);
		b = Intersect(s, ray4, &t1, &t2);
		b = Intersect(s, seg1, &t1, &t2);
		b = Intersect(s, seg2, &t1, &t2);
		b = Intersect(s, seg3, &t1, &t2);
	}

	TEST_METHOD(LineTriangleIntersectionTest){
		LARGE_INTEGER start, stop;
		Triangle tri(Vector3(-1.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(1.0f, 0.0f, 1.0f));
		//Triangle t(Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f));
		Rect window(Vector2(-1.0f, -1.0f), Vector2(1.0f, 1.0f));
		Vector3 eye(0.0f, 0.1f, 0.0f);

		int n = 1000, m = 1000;
		float hx = (window.rgt_top.x - window.lft_btm.x) / n;
		float hy = (window.rgt_top.y - window.lft_btm.y) / m;
		float hz = 0.1f;

		std::ofstream out("Triangle.txt");

		float u, v, t;
		Ray r1(Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f));
		bool b = Intersect(r1, tri, &u, &v, &t);
		Vector3 pt1 = (1 - u - v)*tri.GetA() + u*tri.GetB() + v*tri.GetC();
		Vector3 pt2 = r1.GetPoint(t);

		b = Intersect(pt2, tri, &u, &v);

		QueryPerformanceCounter(&start);
		float lookAt_y = eye.y - hz;
		for(int i = 0; i<m; i++){
			float lookAt_z = window.lft_btm.y + i*hy;

			for(int j = 0; j<n; j++){
				Vector3 lookAt(window.lft_btm.x + j*hx, lookAt_y, lookAt_z);
				Ray ray(eye, lookAt - eye);
				Intersect(ray, tri);
				/*if(Intersect(ray, tri))
				out << "x";
				else
				out << "o";*/
			}
			//out << "\n";
		}
		QueryPerformanceCounter(&stop);

		printf("LineTriangleTest: ");
		double ticks = 1.0*(stop.QuadPart - start.QuadPart) / (n*m);
		std::cout << "ticks = " << ticks << "\n";

		out.close();
	}

	TEST_METHOD(SegmentSegmentIntersectionTest){
		Segment s1(Vector3(), Vector3(-1.0f, -1.0f, -1.0f)),
			s2(Vector3(1.0f, 1.0f, 1.0f), Vector3(-1.1f, -1.1f, -1.1f)),
			s3(Vector3(1.0f, 1.0f, 1.1f), Vector3(0.0f, 0.0f, 0.1f)),
			s4(Vector3(2.0f, 2.0f, 2.0f), Vector3(1.5f, 1.5f, 1.5f)),
			s5(Vector3(-0.5f, -0.5f, -0.5f), Vector3(-1.5f, -1.5f, -1.5f));

		IntersectionTestData<Segment, Segment> testData[9];

		testData[0].Set(s2, s1, true);
		testData[1].Set(s1, s2, true);
		testData[2].Set(s1, s3, false);
		testData[3].Set(s1, s4, false);
		testData[4].Set(s1, s5, true);
		testData[5].Set(s5, s1, true);
		testData[6].Set(Segment(Vector3(0.0f, 0.0f), Vector3(1.0f, 1.0f)),
			Segment(Vector3(1.0f, 0.0f), Vector3(0.0f, 1.0f)), true);
		testData[7].Set(Segment(Vector3(0.0f, 0.0f), Vector3(1.0f, 1.0f)),
			Segment(Vector3(2.0f, 2.0f), Vector3(2.1f, 2.1f)), false);
		testData[8].Set(Segment(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f)),
			Segment(Vector3(1.0f, 0.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f)), true);

		printf("Segment-Segment Intersection Test: ");
		bool failed = false;
		float t1, t2;
		Vector3 pt1, pt2;
		for(auto test : testData){
			if(Intersect(test.c1, test.c2, &t1, &t2) != test.hasIntersection)
				failed = true;

			pt1 = test.c1.GetPoint(t1);
			pt2 = test.c2.GetPoint(t2);
		}
		if(failed)	printf("Failed\n");
		else		printf("Passed\n");
	}

	TEST_METHOD(AABB_SphereIntersectionTest){
		AABB aabb(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));
		Sphere	s1(Vector3(), 1.0f),
			s2(Vector3(), 3.0f),
			s3(Vector3(-1.0f, -1.0f, -1.0f), 1.0f),
			s4(Vector3(-2.0f, -1.0f, -1.0f), 0.5f);

		bool b = Intersect(aabb, s1);
		b = Intersect(aabb, s2);
		b = Intersect(aabb, s3);
		b = Intersect(aabb, s4);
	}

	TEST_METHOD(PlaneIntersectionTest){
		Frustum f(Vector3(-1.0f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, -1.0f, 0.0f), Vector3(-1.0f, -1.0f, 0.0f),
				  Vector3(-2.0f, 2.0f, 1.0f), Vector3(2.0f, 2.0f, 1.0f), Vector3(2.0f, -2.0f, 1.0f), Vector3(-2.0f, -2.0f, 1.0f));
		Plane p(0.0f, 0.0f, 1.0f, 2.0f);

		Line data[2];
		bool b = Intersect(f.plane[Frustum::Near], f.plane[Frustum::Lft], &data[0]);
		b = Intersect(f.plane[Frustum::Top], f.plane[Frustum::Lft], &data[1]);

		float t1, t2;
		b = Intersect(data[0], data[1], &t1, &t2);
		Vector3 pt = data[1].GetPoint(t2);

		Ray r(Vector3(0.0f, 0.0f, 0.5), Vector3(0.5f, 0.0f, 0.5f));
		float data4;
		b = Intersect(r, f.plane[Frustum::Far], &data4);
		b = Intersect(r, f.plane[Frustum::Lft], &data4);
		b = Intersect(r, f.plane[Frustum::Rgt], &data4);
		b = Intersect(r, f.plane[Frustum::Near], &data4);

		Triangle tri1(Vector3(-0.5f, 0.0f, -0.5f), Vector3(0.0f, 1.0f, 0.5f), Vector3(1.0f, -0.5f, -1.0f)),
			tri2(Vector3(-0.5f, 0.0f, -0.5f), Vector3(0.0f, 1.0f, -0.5f), Vector3(1.0f, -0.5f, -0.5f)),
			tri3(Vector3(-0.5f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, -0.5f, 0.0f));
		Segment seg;
		b = Intersect(tri1, f.plane[Frustum::Near], &seg);
		b = Intersect(tri2, f.plane[Frustum::Near], &seg);
		b = Intersect(tri3, f.plane[Frustum::Near], &seg);
	}

	TEST_METHOD(RectTriangleIntersectionTest){
		Rect rt(Vector2(1.0f, 1.0f), Vector2(2.0f, 2.0f));
		IntersectionTestData<Rect, Triangle> testData[5];
		testData[0].Set(rt, Triangle(Vector3(1.34f, 1.61f), Vector3(1.6f, 1.6f), Vector3(1.5f, 1.3f)), true);
		testData[1].Set(rt, Triangle(Vector3(0.86f, 2.0f), Vector3(2.13f, 2.0f), Vector3(1.5f, 0.9f)), true);
		testData[2].Set(rt, Triangle(Vector3(0.2f, 2.1f), Vector3(2.8f, 2.1f), Vector3(1.5f, 0.16f)), true);
		testData[3].Set(rt, Triangle(Vector3(1.1f, 2.3f), Vector3(1.5f, 2.3f), Vector3(1.1f, 2.2f)), false);
		testData[4].Set(rt, Triangle(Vector3(1.9f, 1.1f), Vector3(2.3f, 1.1f), Vector3(1.9f, 7.8f)), true);

		bool failed = false;
		for(auto test : testData){
			if(Intersect(test.c1, test.c2) != test.hasIntersection)
				failed = true;
		}
		printf("Rect-Triangle Intersection Test: ");
		if(failed)	printf("Failed\n");
		else		printf("Passed\n");
	}

	TEST_METHOD(FrustumTest){
		Frustum f1(Vector3(-1.0f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, -1.0f, 0.0f), Vector3(-1.0f, -1.0f, 0.0f),
			Vector3(-1.0f, 1.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, -1.0f, 1.0f), Vector3(-1.0f, -1.0f, 1.0f));
		Frustum f2(Vector3(-1.0f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f, -1.0f, 0.0f), Vector3(-1.0f, -1.0f, 0.0f),
			Vector3(-2.0f, 2.0f, 1.0f), Vector3(2.0f, 2.0f, 1.0f), Vector3(2.0f, -2.0f, 1.0f), Vector3(-2.0f, -2.0f, 1.0f));

		AABB aabb1(Vector3(0.1f, 0.1f, 0.1f), Vector3(0.5f, 0.5f, 0.5f)),
			aabb2(Vector3(-3.0f, -3.0f, -3.0f), Vector3(3.0f, 3.0f, 3.0f)),
			aabb3(Vector3(-2.0f, -2.0f, 0.0f), Vector3(-1.0f, 1.0f, 0.5f)),
			aabb4(Vector3(-3.0f, -3.0f, -3.0f), Vector3(-2.0f, -2.0f, 0.0f));
		Triangle t1(Vector3(-3.0f, 0.0f, 0.1f), Vector3(-3.0f, 2.0f, 0.5f), Vector3(-3.0f, 0.0f, 1.1f));

		bool b = Intersect(f2, aabb1);
		b = Intersect(f2, aabb2);
		b = Intersect(f2, aabb3);
		b = Intersect(f2, aabb4);
		b = Intersect(f2, t1);

		Vector3 eye(5.0f, 5.0f, 5.0f), look(1.0f, 0.0f, 0.0f), right(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f);
		Matrix44 view, proj, viewProj;
		float fovY = DegreeToRad(60.0f), aspectRatio = 4.0f / 3.0f, zNear = 0.001f, zFar = 5.0f;
		view.View(right, up, look, eye);
		proj.PerspectiveFovProjection(fovY, aspectRatio, zNear, zFar);
		viewProj = view*proj;
	}

	TEST_METHOD(TriangleTest){
		LARGE_INTEGER start, stop;
		Triangle t(Vector3(0.0f, 0.0f), Vector3(1.0f, 1.0f), Vector3(0.8f, 0.0f));
		Vector3 pt, pt_from(0.0f, 1.0f), pt_to(1.0f, 0.0f);
		float hx, hy;
		int n = 24, m = 24;

		hx = (pt_to.x - pt_from.x) / n;
		hy = (pt_to.y - pt_from.y) / m;

		QueryPerformanceCounter(&start);

		for(int j = 0; j <= m; j++){
			pt.y = pt_from.y + j*hy;

			for(int i = 0; i <= n; i++){
				pt.x = pt_from.x + i*hx;
				bool b = t.IsPointInside(pt);
				if(b)	std::cout << "x";
				else	std::cout << "o";
			}
			std::cout << "\n";
		}

		QueryPerformanceCounter(&stop);

		long long ticks = stop.QuadPart - start.QuadPart;

		printf("TriangleTest: ");
		std::cout << "ticks = " << ticks << "\n";

		Triangle t1(Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.5f, 0.5f), Vector3(1.0f, 0.0f, 0.0f)),
			t2(Vector3(-1.0f, 0.0f, 0.2f), Vector3(0.0f, 0.5f, 0.2f), Vector3(1.0f, 0.0f, 0.2f)),
			t3(Vector3(-1.0f, -0.1f, 0.2f), Vector3(0.0f, 0.4f, 0.2f), Vector3(1.0f, -0.1f, 0.2f)),
			t4(Vector3(-1.0f, -0.2f, 0.0f), Vector3(0.0f, -0.2f, 0.5f), Vector3(1.0f, -0.2f, 0.0f));
		Segment seg;

		bool b = Intersect(t1, t2, &seg);
		Vector3 pt1 = seg.GetA(), pt2 = seg.GetB();
		b = Intersect(t2, t3, &seg);
		b = Intersect(t3, t4, &seg);
		b = Intersect(t3, t1, &seg);
		b = Intersect(t4, t1, &seg);
	}
};

}
}
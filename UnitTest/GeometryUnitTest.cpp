#include "stdafx.h"
#include "CppUnitTest.h"
#include "../src/application/Engine/Geometry.h"
#include "cinder/gl/gl.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

typedef ci::vec2 vec2;
#define EPSILON_DISTANCE 0.01f

namespace UnitTest
{		
	TEST_CLASS(GeometryUnitTest)
	{
	public:

		template <class T>
		static vector<T> inverse(const vector<T>& arr) {
			vector<T> inverseArr(arr.size());
			int nSub1 = (int)arr.size() - 1;
			for (int i = 0; i <= nSub1; i++) {
				inverseArr[i] = arr[nSub1 - i];
			}
			return inverseArr;
		}
		static bool compare(const vector<vec2>& poly1, const vector<vec2>& poly2, float epsilonDistance) {
			if (poly1.size() != poly2.size()) return false;

			int n1Sub1 = (int)poly1.size() - 1;
			int n2Sub1 = (int)poly2.size() - 1;

			int j = 0;
			int i;

			for (i = 0; i <= n2Sub1; i++) {
				if (sqrt(lengthSquared(poly1[j] - poly2[i])) <= epsilonDistance) {
					break;
				}
			}

			if (i > n2Sub1) return false;

			int backupI = i;

			for (j = 1, i = next(i, n2Sub1); j <= n1Sub1; j++, i = next(i, n2Sub1)) {
				if (sqrt(lengthSquared(poly1[j] - poly2[i])) > epsilonDistance) {
					break;
				}
			}
			if (j == n1Sub1 + 1) {
				return true;
			}
			i = backupI;
			for (j = 1, i = back(i, n2Sub1); j <= n1Sub1; j++, i = back(i, n2Sub1)) {
				if (sqrt(lengthSquared(poly1[j] - poly2[i])) > epsilonDistance) {
					break;
				}
			}
			if (j == n1Sub1 + 1) {
				return true;
			}

			return true;
		}

		static bool invokePolyIntersect(const vector<vec2>& poly1, const vector<vec2>& poly2, vector<vec2>& polyOut) {
			try {
				return geometry::polyIntersect(poly1, poly2, polyOut);
			}
			catch (std::exception& e) {
				std::string errorStr = e.what();
				std::wstring error(errorStr.begin(), errorStr.end());
				Assert::Fail(error.c_str());
			}
			catch (...) {
				Assert::Fail(L"Unhandled Exception");
			}
			Assert::Fail(L"Unreachable code reached");
			return false;
		}

		template <class CheckFunc>
		static void RunPolyIntersectionUnitTestSet(const vector<vec2>& poly1, const vector<vec2>& poly2, CheckFunc f) {
			vector<vec2> inversePoly1 = inverse(poly1);
			vector<vec2> inversePoly2 = inverse(poly2);

			vector<vec2> polyOut;
			std::wstring failedMessage;
			bool res;

			res = invokePolyIntersect(poly1, poly2, polyOut);
			Assert::IsTrue( (failedMessage = f(polyOut, res)).size() == 0, (L"sub case 1.1 " + failedMessage).c_str());

			polyOut.clear();
			res = invokePolyIntersect(poly1, inversePoly2, polyOut);
			Assert::IsTrue( (failedMessage = f(polyOut, res)).size() == 0, (L"sub case 1.2 " + failedMessage).c_str());

			polyOut.clear();
			res = invokePolyIntersect(inversePoly1, poly2, polyOut);
			Assert::IsTrue( (failedMessage = f(polyOut, res)).size() == 0, (L"sub case 1.3 " + failedMessage).c_str());

			polyOut.clear();
			res = invokePolyIntersect(inversePoly1, inversePoly2, polyOut);
			Assert::IsTrue( (failedMessage = f(polyOut, res)).size() == 0, (L"sub case 1.4 " + failedMessage).c_str());

			polyOut.clear();
			res = invokePolyIntersect(poly2, poly1, polyOut);
			Assert::IsTrue( (failedMessage = f(polyOut, res)).size() == 0, (L"sub case 2.1 " + failedMessage).c_str());

			polyOut.clear();
			res = invokePolyIntersect(poly2, inversePoly1, polyOut);
			Assert::IsTrue( (failedMessage = f(polyOut, res)).size() == 0, (L"sub case 2.2 " + failedMessage).c_str());

			polyOut.clear();
			res = invokePolyIntersect(inversePoly2, poly1, polyOut);
			Assert::IsTrue( (failedMessage = f(polyOut, res)).size() == 0, (L"sub case 2.3 " + failedMessage).c_str());

			polyOut.clear();
			res = invokePolyIntersect(inversePoly2, inversePoly1, polyOut);
			Assert::IsTrue( (failedMessage = f(polyOut, res)).size() == 0, (L"sub case 2.4 " + failedMessage).c_str());
		}

		void RunPolyIntersectionUnitTestForInvalidPolygon(const vector<vec2>& poly1, const vector<vec2>& poly2) {
			vector<vec2> polyOut;
			try {
				geometry::polyIntersect(poly1, poly2, polyOut);
				Assert::Fail(L"Exception should occur for invalid polygon");
			}
			catch (std::exception& e) {
				Assert::AreEqual("invalid polygon", e.what(), "Unexpected exception");
			}
			catch (...) {
				Assert::Fail(L"Unexpected exception");
			}
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// merge an empty polygon
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyMerge_EmptyPoly) {
			vector<vec2> poly = {};
			geometry::merge(poly, EPSILON_DISTANCE);

			Assert::AreEqual(0, (int)poly.size(), L"merge empty polygon should make an empty polygon");
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// merge one vertex polygon
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyMerge_PolyOnePoint) {
			vector<vec2> poly = { {0, 0} };
			vector<vec2> mergePoly = poly;
			geometry::merge(mergePoly, EPSILON_DISTANCE);

			Assert::AreEqual(1, (int)mergePoly.size(), L"merge one vertex polygon should make the same polygon");
			Assert::IsTrue(compare(poly, mergePoly, EPSILON_DISTANCE), L"merge one vertex polygon should make the same polygon");
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// merge two different vertices polygon
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyMerge_PolyTwoDifferentPoints) {
			vector<vec2> poly = { { 0, 0 },{ 1, 0 } };
			vector<vec2> mergePoly = poly;
			geometry::merge(mergePoly, EPSILON_DISTANCE);

			Assert::AreEqual(2, (int)mergePoly.size());
			Assert::IsTrue(compare(poly, mergePoly, EPSILON_DISTANCE));
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// merge two same vertices polygon
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyMerge_PolyTwoSamePoints) {
			vector<vec2> poly = { { 0, 0 },{ 0, 0 } };
			vector<vec2> mergePoly = poly;
			geometry::merge(mergePoly, EPSILON_DISTANCE);

			Assert::AreEqual(1, (int)mergePoly.size());

			poly.resize(1);
			Assert::IsTrue(compare(poly, mergePoly, EPSILON_DISTANCE));
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// merge three different vertices polygon
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyMerge_PolyThreeDifferentPoints) {
			vector<vec2> poly = { { 0, 0 },{ 1, 0 },{ 0, 1 } };
			vector<vec2> mergePoly = poly;
			geometry::merge(mergePoly, EPSILON_DISTANCE);

			Assert::AreEqual(3, (int)mergePoly.size());
			Assert::IsTrue(compare(poly, mergePoly, EPSILON_DISTANCE));
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// merge three vertices polygon which has two same vertices
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyMerge_PolyThreePoints_1) {
			vector<vec2> poly = { { 0, 0 },{ 1, 0 },{ 0, 0 } };
			vector<vec2> mergePoly = poly;
			geometry::merge(mergePoly, EPSILON_DISTANCE);

			poly.resize(2);
			Assert::IsTrue(compare(poly, mergePoly, EPSILON_DISTANCE));
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// merge three vertices polygon which has two same vertices
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyMerge_PolyThreePoints_2) {
			vector<vec2> poly = { { 0, 0 },{ 0, 0 },{ 1, 0 } };
			vector<vec2> mergePoly = poly;
			geometry::merge(mergePoly, EPSILON_DISTANCE);

			poly[1] = poly[2];
			poly.resize(2);
			Assert::IsTrue(compare(poly, mergePoly, EPSILON_DISTANCE));
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// merge three vertices polygon which has two same vertices
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyMerge_PolyThreePoints_3) {
			vector<vec2> poly = { { 0, 0 },{ 1, 0 },{ 1, 0 } };
			vector<vec2> mergePoly = poly;
			geometry::merge(mergePoly, EPSILON_DISTANCE);

			poly.resize(2);
			Assert::IsTrue(compare(poly, mergePoly, EPSILON_DISTANCE));
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// merge four different vertices polygon
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyMerge_PolyFourPoints_1) {
			vector<vec2> poly = { { 0, 0 },{ 1, 0 },{ 0, 1 },{ 1, 1 } };
			vector<vec2> mergePoly = poly;
			geometry::merge(mergePoly, EPSILON_DISTANCE);
			Assert::IsTrue(compare(poly, mergePoly, EPSILON_DISTANCE));
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// merge four vertices polygon which has two pair of same vertices
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyMerge_PolyFourPoints_2) {
			vector<vec2> poly = { { 0, 0 },{ 1, 0 },{ 1, 0 },{ 0, 0 } };
			vector<vec2> mergePoly = poly;
			geometry::merge(mergePoly, EPSILON_DISTANCE);

			poly.resize(2);
			Assert::IsTrue(compare(poly, mergePoly, EPSILON_DISTANCE));
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// merge seven vertices polygon which has points in template 1, 2, 2, 2, 3, 3, 1
		/// the result should be 1, 2, 3
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyMerge_PolySevenPoints_1) {
			vector<vec2> poly = { { 1, 1 },{ 2, 2 },{ 2, 2 },{ 2, 2 },{ 3, 3 },{ 3, 3 },{ 1, 1 } };
			vector<vec2> mergePoly = poly;
			geometry::merge(mergePoly, EPSILON_DISTANCE);

			poly[2] = poly[4];
			poly.resize(3);
			Assert::IsTrue(compare(poly, mergePoly, EPSILON_DISTANCE));
		}

		TEST_METHOD(PolyIntersection_InvalidPolygon1)
		{
			vector<vec2> poly1 = {};
			vector<vec2> poly2 = { { 9.24f, 4.6f },{ 6.74f, 1.76f },{ 10.1f, -2.32f },{ 13.72f, 1.24f } };
			RunPolyIntersectionUnitTestForInvalidPolygon(poly1, poly2);
			RunPolyIntersectionUnitTestForInvalidPolygon(poly2, poly1);
		}

		TEST_METHOD(PolyIntersection_InvalidPolygon2)
		{
			vector<vec2> poly1 = { { 5.16f, 3.98f } };
			vector<vec2> poly2 = { { 9.24f, 4.6f },{ 6.74f, 1.76f },{ 10.1f, -2.32f },{ 13.72f, 1.24f } };
			RunPolyIntersectionUnitTestForInvalidPolygon(poly1, poly2);
			RunPolyIntersectionUnitTestForInvalidPolygon(poly2, poly1);
		}

		TEST_METHOD(PolyIntersection_InvalidPolygon3)
		{
			vector<vec2> poly1 = { { 5.16f, 3.98f },{ 3.56f, 1.04f }};
			vector<vec2> poly2 = { { 9.24f, 4.6f },{ 6.74f, 1.76f },{ 10.1f, -2.32f },{ 13.72f, 1.24f } };
			RunPolyIntersectionUnitTestForInvalidPolygon(poly1, poly2);
			RunPolyIntersectionUnitTestForInvalidPolygon(poly2, poly1);
		}
		
		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons are separate
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_NoIntersection1)
		{
			vector<vec2> poly1 = { { 5.16f, 3.98f },{ 3.56f, 1.04f },{ 7.64f, -1.72f } };
			vector<vec2> poly2 = { { 9.24f, 4.6f },{ 6.74f, 1.76f },{ 10.1f, -2.32f },{ 13.72f, 1.24f } };

			RunPolyIntersectionUnitTestSet(poly1, poly2 , [](const vector<vec2>& polyRes, bool res) -> std::wstring {
				return res ? L"should not intersect" : L"";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// one vertex of polygon 1 lies on an edge of polygon 2
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_TouchVertex1)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 10.0f, 0.0f },{ 14.64f, -1.72f },{ 18.26f, 1.84f } };
			vector<vec2> expectedIntersection = { {10.0f, 0.0f} };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					geometry::merge(polyRes, EPSILON_DISTANCE);
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// one vertex of polygon 1 lies on two edge2 of polygon 2
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_TouchVertex2)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 12.0f, -2.0f },{ 14.64f, -1.72f },{ 18.26f, 1.84f } };

			vector<vec2> expectedIntersection = { { 12.0f, -2.0f } };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two vertices of polygon 1 lies on an edge of polygon 2
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_TouchEdge1)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 8.0f, 2.0f },{ 11.0f, -1.0f },{ 18.26f, 1.84f } };

			vector<vec2> expectedIntersection = { { 8.0f, 2.0f },{ 11.0f, -1.0f } };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// one vertex of polygon 1 lies on an edge of polygon 2 and
		/// one vertex of polygon 2 lies on an edge of polygon 1
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_TouchEdge2)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 8.0f, 2.0f },{ 13.0f, -3.0f },{ 18.26f, 1.84f } };
			vector<vec2> expectedIntersection = { { 8.0f, 2.0f },{ 12.0f, -2.0f } };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two vertices of polygon 1 are same two vertices of polygon 2
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_TouchEdge3)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 7.0f, 3.0f },{ 12.0f, -2.0f },{ 18.26f, 1.84f } };
			vector<vec2> expectedIntersection = { { 7.0f, 3.0f },{ 12.0f, -2.0f } };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a triangle
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_01)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 7.0f, 3.0f },{ 8.0f, -1.0f },{ 18.26f, 1.84f } };
			vector<vec2> expectedIntersection = { { 7.0f, 3.0f },{ 8.0f, -1.0f }, {10.35f, -0.35f} };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a triangle
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_02)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 8.0f, 2.0f },{ 8.0f, -1.0f },{ 18.26f, 1.84f } };
			vector<vec2> expectedIntersection = { { 8.0f, 2.0f },{ 8.0f, -1.0f },{ 10.35f, -0.35f } };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a quadrangle
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_03)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 7.48f, 1.54f },{ 8.0f, -1.0f },{ 18.26f, 1.84f } };
			vector<vec2> expectedIntersection = { { 8.1f, 1.9f }, { 7.48f, 1.54f },{ 8.0f, -1.0f },{ 10.35f, -0.35f } };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a pentangon
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_04)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 7.48f, 1.54f },{ 8.0f, -1.0f },{ 9.76f, -1.62f } };
			vector<vec2> expectedIntersection = { { 8.1f, 1.9f },{ 7.48f, 1.54f },{ 8.0f, -1.0f },{ 9.76f, -1.62f }, { 10.45f, -0.45f } };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a quadrangle
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_05)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 7.48f, 1.54f },{ 8.0f, -1.0f },{ 12.0f, -2.0f } };
			vector<vec2> expectedIntersection = { { 8.1f, 1.9f },{ 7.48f, 1.54f },{ 8.0f, -1.0f },{ 12.0f, -2.0f } };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a pentagon
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_06)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 7.48f, 1.54f },{6.8408275862069f, -1.57006896551724f }, { 12.0f, -2.0f }};
			vector<vec2> expectedIntersection = { { 8.1f, 1.9f },{ 7.48f, 1.54f },{ 6.8408275862069f, -1.57006896551724f },{ 12.0f, -2.0f } };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a quadrangle
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_07)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 7.48f, 1.54f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> expectedIntersection = { { 8.1f, 1.9f },{ 7.48f, 1.54f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a quadrangle
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_08)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{6.44518367696125f, 1.7488325776371f},{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> expectedIntersection = { {7.67331078781121f, 2.32668921218880f }, { 6.44518367696125f, 1.7488325776371f }, { 5.04f, -1.42f }, { 12.0f, -2.0f }};

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a quadrangle
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_09)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 6.44518367696125f, 1.7488325776371f },{ 5.04f, -1.42f },{ 10.58f, -3.22f } };
			vector<vec2> expectedIntersection = {
				{ 7.67331078781121f, 2.32668921218880f },
				{ 6.44518367696125f, 1.7488325776371f },
				{ 5.04f, -1.42f },
				{ 11.0730161166539f, -1.92275134305449f },
				{ 11.3070223752152f, -1.30702237521515f } };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a quadrangle
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_10)
		{
			vector<vec2> poly1 = { { 7.0f, 3.0f },{ 5.04f, -1.42f },{ 12.0f, -2.0f } };
			vector<vec2> poly2 = { { 13.78f, 5.2f },{ 6.44518367696125f, 1.7488325776371f },{ 3.76f, -1.84f },{ 10.58f, -3.22f } };
			vector<vec2> expectedIntersection = {
				{ 7.67331078781121f, 2.32668921218880f },
				{ 6.44518367696125f, 1.7488325776371f },
				{ 5.04f, -1.42f },
				{ 11.0730161166539f, -1.92275134305449f },
				{ 11.3070223752152f, -1.30702237521515f } };

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a hexagon
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_11)
		{
			vector<vec2> poly1 = {
				{ 9.96f, 3.48f },
				{ 5.28f, -1.36f },
				{ 6.2f, -6.54f },
				{ 13.18f, -4.56f },
				{ 14.86f, 0.4f }
			};

			vector<vec2> poly2 = {
				{ 7.04f, 2.6f },
				{ 8.36f, -8.06f },
				{ 11.38f, -7.02f },
				{ 14.22f, 2.12f },
			};
			vector<vec2> expectedIntersection = {
				{ 13.88f, 1.02f },
				{ 11.87f, 2.28f },
				{ 8.98f, 2.47f },
				{ 7.27f, 0.7f },
				{ 8.1f, -6.0f },
				{ 12.04f, -4.88f },
			};

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a quadrangle
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_12)
		{
			vector<vec2> poly1 = {
				{ 9.96f, 3.48f },
				{ 5.28f, -1.36f },
				{ 6.2f, -6.54f },
				{ 13.18f, -4.56f },
				{ 14.86f, 0.4f }
			};

			vector<vec2> poly2 = {
				{ 7.04f, 2.6f },
				{ 8.36f, -8.06f },
				{ 11.38f, -7.02f },
			};
			vector<vec2> expectedIntersection = {
				{ 7.27f, 0.7f },
				{ 7.7f, 1.14f },
				{ 10.6f, -5.29f },
				{ 8.1f, -6.0f },
			};

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a quadrangle
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_13)
		{
			vector<vec2> poly1 = {
				{ 9.96f, 3.48f },
				{ 5.28f, -1.36f },
				{ 6.2f, -6.54f },
				{ 14.0f, -4.0f },
				{ 14.86f, 0.4f },
			};

			vector<vec2> poly2 = {
				{ 6.0f, 4.0f },
				{ 8.36f, -8.06f },
				{ 16.0f, -6.0f },
			};
			vector<vec2> expectedIntersection = {
				{ 6.7511582124f, 0.1614542197f },
				{ 8.268907563f, 1.731092437f },
				{ 14.0f, -4.0f },
				{ 7.9509745919f, -5.969810838f },
			};

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons have a common area is a pentagon
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_14)
		{
			vector<vec2> poly1 = {
				{ 10.0f, 4.0f },
				{ 5.28f, -1.36f },
				{ 6.0f, -6.0f },
				{ 14.0f, -4.0f },
				{ 14.86f, 0.4f },
			};

			vector<vec2> poly2 = {
				{ 10.0f, 4.0f },
				{ 4.0f, -8.0f },
				{ 16.0f, -6.0f },
			};
			vector<vec2> expectedIntersection = {
				{ 10.0f, 4.0f },
				{ 5.7631578947f, -4.4736842105f },
				{ 6.0f, -6.0f },
				{ 14.0f, -4.0f },
				{ 14.1965714286f, -2.9942857143f },
			};

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}

		/////////////////////////////////////////////////////////////////////
		///
		/// two polygons are same
		///
		/////////////////////////////////////////////////////////////////////
		TEST_METHOD(PolyIntersection_Intersection_15)
		{
			vector<vec2> poly1 = {
				{ 10.0f, 4.0f },
				{ 5.28f, -1.36f },
				{ 6.0f, -6.0f },
				{ 14.0f, -4.0f },
				{ 14.86f, 0.4f },
			};

			vector<vec2> poly2 = poly1;
			vector<vec2> expectedIntersection = poly1;

			RunPolyIntersectionUnitTestSet(poly1, poly2, [&expectedIntersection](const vector<vec2>& polyRes, bool res) -> std::wstring {
				if (res) {
					if (compare(expectedIntersection, polyRes, EPSILON_DISTANCE)) {
						return L"";
					}
					return L"wrong intersection result";
				}

				return L"should intersect";
			});
		}
	};
}
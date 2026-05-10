#pragma once
// c++
#include <algorithm>
#include<vector>
#define _USE_MATH_DEFINES


// engine
#include "DirectXGame/engine/math/MathFunctions.h"
#include "DirectXGame/engine/math/LineCurveMath.h"


#pragma region IsCollision

namespace Engine {

	// 判定関数
	namespace Collision
	{
		// 衝突判定
		namespace Detection {
			// AABBと点
			bool Check(const AABB& aabb, const Vector3& point);
			//衝突判定(球と球)
			bool Check(const Sphere& s1, const Sphere& s2);
			//衝突判定(球と点)
			bool Check(const Sphere& s1, const Vector3& v1);
			//衝突判定(球と平面)
			bool Check(const Sphere& sphere, const Plane& plane);
			//衝突判定(線と平面)
			bool Check(const Segment& segment, const Plane& plane);
			//三角形と線の衝突判定
			bool Check(const Triangle& triangle, const Segment& segment);
			// Triangle と Capsule の衝突判定
			bool Check(const Triangle& triangle, const Capsule& capsule);
			// 三角面と球の衝突判定
			bool Check(const Triangle& triangle, const Sphere& sphere);
			// Rayと球の衝突判定
			bool Check(const Ray& ray, const Sphere& sphere);
			// RayとAABBの衝突判定
			bool Check(const Ray& ray, const AABB& aabb);
			// スプラインと点
			bool Check(const std::vector<Vector3>& controlPoints, float radius, const Vector3& point, int segmentPerCurve = 10);
			// AABBとAABB
			bool Check(const AABB& aabb1, const AABB& aabb2);
			// AABBと球
			bool Check(const AABB& aabb, const Sphere& sphere);
			//四角形と線の衝突判定
			bool Check(const AABB& aabb, const Segment& segment);
			// 球と線
			bool Check(const Sphere& sphere, const Segment& segment);

			// 球とカプセル
			bool Check(const Sphere& sphere, const Capsule& cap);
			// OBBと球
			bool Check(const OBB& obb, const Sphere& sphere);
			// OBBとSegmentの衝突判定
			bool Check(const OBB& obb, const Segment& segment);
			// OBBとAABBの衝突判定
			bool Check(const OBB& obb, const AABB& aabb);
			// OBBとカプセル
			bool Check(const OBB& obb, const Capsule& cap);
			// 分離軸定理による OBB vs OBB の衝突判定
			bool Check(const OBB& obb0, const OBB& obb1);
			// 分離軸定理による OBB vs OBB の衝突判定
			bool Check2(const OBB& obb0, const OBB& obb1);
			// カプセルとカプセル
			bool Check(const Capsule& cap0, const Capsule& cap1);
			//カプセルと三角形の衝突判定
			bool Check(const Capsule& capsule, const Triangle& tri);
			// AABB とカプセルの衝突判定
			bool Check(const Capsule& capsule, const AABB& box);
		}

		// 衝突応答
		namespace Response {
			// 球と球の衝突応答
			bool ReflectVelocity(const Sphere& s1, const Sphere& s2, Vector3& outPushVec);
		}

	}

	// 点とカプセル（線分 + 半径）の距離チェック
	static bool PointInCapsule(const Vector3& point, const Vector3& a, const Vector3& b, float radius) {
		Vector3 ab = b - a;
		Vector3 ap = point - a;

		float abLengthSq = Dot(ab, ab);
		float t = abLengthSq > 0.0f ? std::clamp(Dot(ap, ab) / abLengthSq, 0.0f, 1.0f) : 0.0f;

		Vector3 closest = a + ab * t;
		float distSq = LengthSquared(point - closest);

		return distSq <= radius * radius;
	}
	// 線分と三角形の辺の交差判定
	static bool LineSegmentIntersectEdge(const Segment& line, const Vector3& A, const Vector3& B) {
		// 2D 平面上での交差判定
		// 線分ABと線分lineが交差しているかを計算
		Vector3 edge = B - A;
		Vector3 segmentDir = line.end - line.origin;
		Vector3 normal = edge.Cross(segmentDir);

		if (normal.LengthSq() < 1e-6f) {
			return false; // 平行な場合は交差しない
		}

		Vector3 intersect = A - line.origin;
		float t = intersect.Cross(normal).Length() / normal.Length();
		return t >= 0 && t <= 1;
	}
}
#pragma endregion
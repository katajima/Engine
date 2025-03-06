#pragma once

// c++
#include<cmath>
#include <algorithm>
#include<vector>
#define _USE_MATH_DEFINES
#include <math.h>

// engine
#include "DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/math/LineCurveMath.h"
#include "DirectXGame/engine/collider/Octree/Octree.h"

#pragma region IsCollision

// AABBと点
static bool IsCollision(const AABB& aabb, const Vector3& point) {
	return (point.x >= aabb.min_.x && point.x <= aabb.max_.x) &&
		(point.y >= aabb.min_.y && point.y <= aabb.max_.y) &&
		(point.z >= aabb.min_.z && point.z <= aabb.max_.z);
}

//衝突判定(球と球)
static bool IsCollision(const Sphere& s1, const Sphere& s2)
{
	Vector3 result{};
	float isDistance = false;

	result.x = s2.center.x - s1.center.x;
	result.y = s2.center.y - s1.center.y;
	result.z = s2.center.z - s1.center.z;

	float distance = Length(result);

	if (distance <= s2.radius + s1.radius) {

		isDistance = true;

	}
	else {

		isDistance = false;

	}

	return isDistance;
}

//衝突判定(球と平面)
static bool IsCollision(const Sphere& sphere, const Plane& plane)
{
	Vector3 dis = Normalize(plane.normal);

	float distance = Dot(sphere.center, dis) - plane.distance;
	return std::abs(distance) < sphere.radius;
}

//衝突判定(線と平面)
static bool IsCollision(const Segment& segment, const Plane& plane)
{
	Vector3 diff = segment.diff();


	//まず垂直判定を行うために、法線と線の内積を求める
	float dot = Dot(plane.normal, diff);

	// 垂直=平行であるので、衝突しているはずがない
	if (dot == 0.0f) {
		return false;
	}

	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

	if (t >= 0.0f && t <= 1.0f) {
		return true;
	}
	return false;
}

//三角形と線の衝突判定
static bool IsCollision(const Triangle& triangle, const Segment& segment) {
	Vector3 diff = segment.diff();

	// 三角形の平面を取得
	Plane plane = PlaneFromPoints(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]);

	float dot = Dot(plane.normal, diff);

	// 平行チェック（浮動小数点誤差を考慮）
	if (fabs(dot) < 1e-6f) {
		return false;
	}

	float t = -(Dot(segment.origin, plane.normal) + plane.distance) / dot;

	// tが線分の範囲 [0, 1] にない場合、衝突なし
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	// 衝突点を求める
	Vector3 p = segment.origin + diff * t;

	Vector3 v01 = triangle.vertices[1] - triangle.vertices[0];
	Vector3 v12 = triangle.vertices[2] - triangle.vertices[1];
	Vector3 v20 = triangle.vertices[0] - triangle.vertices[2];

	Vector3 v0p = p - triangle.vertices[0];
	Vector3 v1p = p - triangle.vertices[1];
	Vector3 v2p = p - triangle.vertices[2];

	Vector3 cross01 = Cross(v01, v0p);
	Vector3 cross12 = Cross(v12, v1p);
	Vector3 cross20 = Cross(v20, v2p);

	float dot01 = Dot(cross01, plane.normal);
	float dot12 = Dot(cross12, plane.normal);
	float dot20 = Dot(cross20, plane.normal);

	// 浮動小数点誤差に対する閾値を適用して判定
	const float epsilon = 1e-6f;
	if (dot01 >= -epsilon && dot12 >= -epsilon && dot20 >= -epsilon) {
		return true;
	}

	return false;
}

// Triangle と Capsule の衝突判定関数
static bool IsCollision(const Triangle& triangle, const Capsule& capsule)
{
	// カプセルの半径の二乗
	float radiusSquared = capsule.radius * capsule.radius;

	// 三角形の各頂点とカプセルの線分との距離を判定
	for (const Vector3& vertex : triangle.vertices)
	{
		if (PointLineDistanceSquared(vertex, capsule.segment.origin, capsule.segment.end) <= radiusSquared)
		{
			return true;
		}
	}

	// 三角形のエッジとカプセルの線分の距離を判定
	Segment edge1 = { triangle.vertices[0], triangle.vertices[1] };
	Segment edge2 = { triangle.vertices[1], triangle.vertices[2] };
	Segment edge3 = { triangle.vertices[2], triangle.vertices[0] };

	if (SegmentSegmentDistanceSquared(capsule.segment, edge1) <= radiusSquared ||
		SegmentSegmentDistanceSquared(capsule.segment, edge2) <= radiusSquared ||
		SegmentSegmentDistanceSquared(capsule.segment, edge3) <= radiusSquared)
	{
		return true;
	}

	return false;
}

// AABBとAABB
static bool IsCollision(const AABB& aabb1, const AABB& aabb2)
{
	if ((aabb1.min_.x <= aabb2.max_.x && aabb1.max_.x >= aabb2.min_.x) &&
		(aabb1.min_.y <= aabb2.max_.y && aabb1.max_.y >= aabb2.min_.y) &&
		(aabb1.min_.z <= aabb2.max_.z && aabb1.max_.z >= aabb2.min_.z)) {

		return true;
	}

	return false;
}

// AABBと球
static bool IsCollision(const AABB& aabb, const Sphere& sphere)
{
	Vector3 closestPoint = ClosestPointAABBSphere(sphere, aabb);

	float dist = Distance(sphere.center, closestPoint);

	if (dist <= sphere.radius) {
		return true;
	}
	else {
		return false;
	}
}

//四角形と線の衝突判定
static bool IsCollision(const AABB& aabb, const Segment& segment)
{
	//点がAABB内にあるなら
	if (IsCollision(aabb, segment.origin) || IsCollision(aabb, segment.end)) {

		return true;
	}

	Plane planeX1, planeY1, planeZ1;
	planeX1.normal = { 1,0,0 };
	planeY1.normal = { 0,1,0 };
	planeZ1.normal = { 0,0,1 };

	Vector3 diff = segment.diff();

	float dotX = Dot(planeX1.normal, diff);
	float dotY = Dot(planeY1.normal, diff);
	float dotZ = Dot(planeZ1.normal, diff);


	// 特異点チェック: 線分が軸に平行である場合
	if (std::abs(dotX) < 1e-6) {
		if (segment.origin.x < aabb.min_.x || segment.origin.x > aabb.max_.x) return false;
	}
	if (std::abs(dotY) < 1e-6) {
		if (segment.origin.y < aabb.min_.y || segment.origin.y > aabb.max_.y) return false;
	}
	if (std::abs(dotZ) < 1e-6) {
		if (segment.origin.z < aabb.min_.z || segment.origin.z > aabb.max_.z) return false;
	}


	Vector3 tMin, tMax;

	tMin.x = (aabb.min_.x - Dot(segment.origin, planeX1.normal)) / dotX;
	tMin.y = (aabb.min_.y - Dot(segment.origin, planeY1.normal)) / dotY;
	tMin.z = (aabb.min_.z - Dot(segment.origin, planeZ1.normal)) / dotZ;

	tMax.x = (aabb.max_.x - Dot(segment.origin, planeX1.normal)) / dotX;
	tMax.y = (aabb.max_.y - Dot(segment.origin, planeY1.normal)) / dotY;
	tMax.z = (aabb.max_.z - Dot(segment.origin, planeZ1.normal)) / dotZ;


	Vector3 tNear, tFar;

	tNear.x = (std::min)(tMin.x, tMax.x);
	tNear.y = (std::min)(tMin.y, tMax.y);
	tNear.z = (std::min)(tMin.z, tMax.z);
	tFar.x = (std::max)(tMin.x, tMax.x);
	tFar.y = (std::max)(tMin.y, tMax.y);
	tFar.z = (std::max)(tMin.z, tMax.z);


	// AABBとの衝突点(貫通点)のtが小さい方
	float tmin = (std::max)((std::max)(tNear.x, tNear.y), tNear.z);

	// AABBとの衝突点(貫通点)のtが大きい方
	float tmax = (std::min)((std::min)(tFar.x, tFar.y), tFar.z);

	if (tmin <= tmax) {
		return true;
	}


	return false;
}

// 球と線
static bool IsCollision(const Sphere& sphere, const Segment& segment) {
	Vector3 m = Subtract(segment.origin, sphere.center);
	//Vector3 segEnd = Add(segment.origin, segment.diff);

	// 線分の方向ベクトル
	Vector3 d = segment.diff();
	float a = Dot(d, d);

	// 線分の長さがゼロの場合（無効な線分）
	if (a == 0.0f) return false;

	float b = Dot(m, d);
	float c = Dot(m, m) - sphere.radius * sphere.radius;

	// 判別式の計算
	float discriminant = b * b - a * c;

	// 判別式が負の場合、衝突していない
	if (discriminant < 0.0f) return false;

	// 解の計算
	float sqrtDiscriminant = std::sqrt(discriminant);
	float t1 = (-b - sqrtDiscriminant) / a;
	float t2 = (-b + sqrtDiscriminant) / a;

	// 衝突が線分内（0 <= t <= 1）の場合にのみ true を返す
	if ((t1 >= 0.0f && t1 <= 1.0f) || (t2 >= 0.0f && t2 <= 1.0f)) {
		return true;
	}

	return false;
}

// OBBと球
static bool IsCollision(const OBB& obb, const Sphere& sphere)
{
	// OBBのローカル座標系に球の中心を変換
	Vector3 d = sphere.center - obb.center;
	Vector3 closestPoint = obb.center;

	for (int i = 0; i < 3; ++i) {
		float distance = Dot(d, obb.orientations[i]);
		float clampedDistance;
		if (i == 0) {
			clampedDistance = (std::max)(-obb.size.x, (std::min)(distance, obb.size.x));
		}
		else if (i == 1) {
			clampedDistance = (std::max)(-obb.size.y, (std::min)(distance, obb.size.y));
		}
		else {
			clampedDistance = (std::max)(-obb.size.z, (std::min)(distance, obb.size.z));
		}
		closestPoint += obb.orientations[i] * clampedDistance;
	}


	// 球の中心と最近接点の距離を比較
	float distSquared = LengthSquared(sphere.center - closestPoint);
	return distSquared <= (sphere.radius * sphere.radius);
}

// OBBとSegmentの衝突判定
static bool IsCollision(const OBB& obb, const Segment& segment) {
	// OBBの各軸に対して投影を行い、交差を確認
	for (int i = 0; i < 3; ++i) {
		// OBBの軸ベクトル
		Vector3 axis = obb.orientations[i].Normalize();

		// セグメントの始点・終点の投影
		float projStart = axis.Dot(segment.origin);
		float projEnd = axis.Dot(segment.end);

		// 線分の投影範囲
		float minSegment = (std::min)(projStart, projEnd);
		float maxSegment = (std::max)(projStart, projEnd);

		// OBBのサイズに基づく投影範囲
		float obbCenterProjection = axis.Dot(obb.center);
		float obbHalfSize = std::abs(obb.size.x * axis.x) +
			std::abs(obb.size.y * axis.y) +
			std::abs(obb.size.z * axis.z);
		float obbMin = obbCenterProjection - obbHalfSize;
		float obbMax = obbCenterProjection + obbHalfSize;

		// 交差判定 (分離軸定理)
		if (maxSegment < obbMin || minSegment > obbMax) {
			return false; // 交差しない
		}
	}
	return true; // 交差する
}

// 分離軸定理による OBB vs OBB の衝突判定
static bool IsCollision(const OBB& obb0, const OBB& obb1) {
	const Vector3* A = obb0.orientations; // OBB0の軸ベクトル
	const Vector3* B = obb1.orientations; // OBB1の軸ベクトル

	Vector3 T = obb1.center - obb0.center; // OBB0からOBB1の中心位置ベクトル
	T = Vector3(T.Dot(A[0]), T.Dot(A[1]), T.Dot(A[2])); // OBB0の座標系に変換

	constexpr float EPSILON = std::numeric_limits<float>::epsilon();

	float ra, rb;
	float R[3][3], AbsR[3][3];

	// 回転行列の計算と絶対値の取得
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			R[i][j] = A[i].Dot(B[j]);
			AbsR[i][j] = std::abs(R[i][j]) + EPSILON;  // 数値誤差対策
		}
	}

	// 1. Aの各軸を分離軸としてチェック
	for (int i = 0; i < 3; ++i) {
		ra = obb0.size.x * std::abs(A[i].x) + obb0.size.y * std::abs(A[i].y) + obb0.size.z * std::abs(A[i].z);
		rb = obb1.size.x * AbsR[i][0] + obb1.size.y * AbsR[i][1] + obb1.size.z * AbsR[i][2];

		if (std::abs(T.Dot(A[i])) > ra + rb) return false;
	}

	// 2. Bの各軸を分離軸としてチェック
	for (int i = 0; i < 3; ++i) {
		ra = obb0.size.x * AbsR[0][i] + obb0.size.y * AbsR[1][i] + obb0.size.z * AbsR[2][i];
		rb = obb1.size.x * std::abs(B[i].x) + obb1.size.y * std::abs(B[i].y) + obb1.size.z * std::abs(B[i].z);

		if (std::abs(T.Dot(B[i])) > ra + rb) return false;
	}

	// 3. Aの各軸とBの各軸のクロス積（9軸）を分離軸としてチェック
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			ra = obb0.size.y * AbsR[(i + 1) % 3][j] + obb0.size.z * AbsR[(i + 2) % 3][j];
			rb = obb1.size.y * AbsR[i][(j + 1) % 3] + obb1.size.z * AbsR[i][(j + 2) % 3];

			float t;
			if (i == 0) {
				t = std::abs(T.y * R[2][j] - T.z * R[1][j]);
			}
			else if (i == 1) {
				t = std::abs(T.z * R[0][j] - T.x * R[2][j]);
			}
			else { // i == 2
				t = std::abs(T.x * R[1][j] - T.y * R[0][j]);
			}

			if (t > ra + rb) return false;
		}
	}

	// すべての軸で分離がない場合、衝突あり
	return true;
}

// カプセルとカプセル
static bool IsCollision(const Capsule& cap0, const Capsule& cap1)
{
	float distanceSq = SegmentClosestDistanceSq(cap0.segment, cap1.segment);
	float radiusSum = cap0.radius + cap1.radius;
	return distanceSq <= radiusSum * radiusSum;
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

//カプセルと三角形の衝突判定
static bool CapsuleIntersectsTriangle(const Capsule& capsule, const Triangle& tri) {
	// カプセルの端点と三角形の面との距離を判定
	if (DistancePointToPlane(capsule.segment.origin, tri.vertices[0], tri.vertices[1], tri.vertices[2]) <= capsule.radius) {
		return true;
	}
	if (DistancePointToPlane(capsule.segment.end, tri.vertices[0], tri.vertices[1], tri.vertices[2]) <= capsule.radius) {
		return true;
	}

	// カプセルの軸（線分）と三角形の辺との交差を判定
	if (LineSegmentIntersectEdge(capsule.segment, tri.vertices[0], tri.vertices[1])) {
		return true;
	}
	if (LineSegmentIntersectEdge(capsule.segment, tri.vertices[1], tri.vertices[2])) {
		return true;
	}
	if (LineSegmentIntersectEdge(capsule.segment, tri.vertices[2], tri.vertices[0])) {
		return true;
	}

	return false; // 衝突していない場合
}

// AABB とカプセルの衝突判定
static bool CapsuleIntersectsAABB(const Capsule& capsule, const AABB& box) {
	// AABB に対するカプセルの線分の最近接点を求める
	Vector3 closestPoint = ClosestPointSegmentAABB(capsule.segment, box);

	// 最近接点とカプセルの中心軸の最近接点を求める
	Vector3 capsuleClosestPoint = ClosestPointOnSegment(capsule.segment, closestPoint);

	// 2点間の距離を計算
	float distanceSquared = (closestPoint - capsuleClosestPoint).LengthSq();

	// カプセルの半径の二乗と比較
	return distanceSquared <= (capsule.radius * capsule.radius);
}

// カプセルとおくつりー衝突判定
bool CapsuleIntersectsWithOctree(const Capsule& capsule, OctreeNode* node);

#pragma endregion
#pragma once
// c++
#include<cmath>
#include <algorithm>
#include<vector>
#define _USE_MATH_DEFINES
#include <math.h>

// engine
#include "DirectXGame/engine/math/MathFanctions.h"



#pragma region Collision

// Boxと点
static bool IsCollision(const Box& aabb, const Vector2& point) {
	return (point.x >= aabb.min_.x && point.x <= aabb.max_.x) &&
		(point.y >= aabb.min_.y && point.y <= aabb.max_.y);
}

// 線分同士の交差判定
static bool IsSegmentIntersect(const Segment2D& s1, const Segment2D& s2) {
	auto cross = [](const Vector2& a, const Vector2& b) {
		return a.x * b.y - a.y * b.x;
		};

	Vector2 r = s1.diff();
	Vector2 s = s2.diff();

	float denom = cross(r, s);
	if (denom == 0.0f) {
		return false; // 平行
	}

	float t = cross(s2.origin - s1.origin, s) / denom;
	float u = cross(s2.origin - s1.origin, r) / denom;

	return (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f);
}

// Baxと線
static bool IsCollision(const Box& aabb, const Segment2D& segment) {
	// 1. 線分の端点がAABB内にある
	if (IsCollision(aabb, segment.origin) || IsCollision(aabb, segment.end)) {
		return true;
	}

	// 2. AABBの4辺を線分に変換して交差判定
	Segment2D edges[4] = {
		{ {aabb.min_.x, aabb.min_.y}, {aabb.max_.x, aabb.min_.y} }, // 下辺
		{ {aabb.max_.x, aabb.min_.y}, {aabb.max_.x, aabb.max_.y} }, // 右辺
		{ {aabb.max_.x, aabb.max_.y}, {aabb.min_.x, aabb.max_.y} }, // 上辺
		{ {aabb.min_.x, aabb.max_.y}, {aabb.min_.x, aabb.min_.y} }  // 左辺
	};

	for (int i = 0; i < 4; i++) {
		if (IsSegmentIntersect(segment, edges[i])) {
			return true;
		}
	}

	return false;
}

//衝突判定(球と球)
static bool IsCollision(const Sphere2D& s1, const Sphere2D& s2)
{
	Vector3 result{};
	float isDistance = false;

	result.x = s2.center.x - s1.center.x;
	result.y = s2.center.y - s1.center.y;

	float distance = Length(result);

	if (distance <= s2.radius + s1.radius) {

		isDistance = true;

	}
	else {

		isDistance = false;

	}

	return isDistance;
}

// BoxとBox
static bool IsCollision(const Box& aabb1, const Box& aabb2)
{
	if ((aabb1.min_.x <= aabb2.max_.x && aabb1.max_.x >= aabb2.min_.x) &&
		(aabb1.min_.y <= aabb2.max_.y && aabb1.max_.y >= aabb2.min_.y)) {

		return true;
	}

	return false;
}
// 三角と円の衝突判定
bool IsCollision(const Triangle2D& triangle, const Sphere2D& sphere);
// 点と三角に判定
bool PointInTriangle(const Vector2& pt, const Triangle2D& triangle);

// 点と線分の最短距離
float DistancePointToSegment(const Vector2& p, const Vector2& a, const Vector2& b);


#pragma endregion

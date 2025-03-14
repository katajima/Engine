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

#pragma endregion

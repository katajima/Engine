#pragma once
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/struct/Structs2D.h"
#include<vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include<assert.h>
#include<random>
#include<numbers>
#include"Easing.h"


#pragma region Math


//線形補間（Linear Interpolation）を行うための関数です。線形補間は、2つの値の間を滑らかに移動するための方法
//float Lerp(float t, float a, float b) {
//	return a + t * (b - a);
//}

template<typename T>
static T Clamp(T value, T min, T max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

static float LerpShortAngle(float a, float b, float t) {
	// 角度差分を求める
	float diff = b - a;

	// 角度を [-2*PI, +2*PI] に補正する
	diff = float(fmod(double(diff + 2.0f) * M_PI, double(4.0f) * M_PI)) - 2.0f * float(M_PI);

	// 角度を [-PI, +PI] に補正する
	if (diff > float(M_PI)) {
		diff -= 2 * float(M_PI);
	}
	if (diff < -float(M_PI)) {

		diff += 2 * float(M_PI);
	}

	return Lerp(a, a + diff, t);
}

float Length(const float& v);

float Clamp(float t);
float Clamp(float t, float min, float max);

float Clamp3(float value, float min, float max);

//球と四角形に対する最近接点
Vector3 ClosestPointAABBSphere(const Sphere& sphere, const AABB& aabb);

Vector3 ClosestPointSegment(const Segment& segment, const Vector3& point);

Vector3 ClosestPointOnPlane(const Plane& plane, const Vector3& point);

Plane PlaneFromPoints(const Vector3& p1, const Vector3& p2, const Vector3& p3);
//反射ベクトル
Vector3 Reflect(const Vector3& input, const Vector3& normal);

Vector3 Reflect(const Vector3& input, const Vector3& normal, float restitution);

#pragma endregion //数学関数

float DegreesToRadians(float degrees);

float RadiansToDegrees(float radians);

static std::pair<Vector3, Vector3> ComputeCollisionVelocities(float mass1, const Vector3& velocity1, float mass2, const Vector3& velocity2,
	float coefficientOfRestitution, const Vector3& normal) {
	// 衝突面法線方向の速度成分を射影
	Vector3 project1 = Project(velocity1, normal); // 質点1の法線方向成分
	Vector3 project2 = Project(velocity2, normal); // 質点2の法線方向成分
	Vector3 sub1 = velocity1 - project1;           // 質点1の接線方向成分
	Vector3 sub2 = velocity2 - project2;           // 質点2の接線方向成分

	// 衝突後の法線方向速度を計算 (反発係数と運動量保存則に基づく)
	Vector3 relativeVelocity = project1 - project2; // 衝突前の相対速度（法線方向）
	float impulse = (-(1 + coefficientOfRestitution) * Dot(relativeVelocity, normal)) /
		(1.0f / mass1 + 1.0f / mass2); // 衝撃量の計算
	Vector3 impulseVector = normal * impulse;       // 衝撃量ベクトル

	// 衝突後の法線方向速度を更新
	Vector3 velocityAfter1 = project1 + impulseVector / mass1;
	Vector3 velocityAfter2 = project2 - impulseVector / mass2;

	// 接線成分を加算して最終的な速度を計算
	return std::make_pair(velocityAfter1 + sub1, velocityAfter2 + sub2);
}


static float DistancePointToPlane(const Vector3& point, const Vector3& A, const Vector3& B, const Vector3& C) {
	// 三角形の辺ベクトル
	Vector3 AB = B - A;
	Vector3 AC = C - A;

	// 平面の法線
	Vector3 normal = AB.Cross(AC).Normalize();

	// 点と平面の距離
	return std::abs((point - A).Dot(normal));
}


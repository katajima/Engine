#include"MathFanctions.h"
#include<cmath>
#include"assert.h"

#pragma region Math

float Length(const float& v) {
	float result;

	result = sqrtf((v * v));

	return result;
};

float Clamp(float t) {

	if (1.0f <= t) {
		t = 1.0f;
	}
	if (0.0f >= t) {
		t = 0.0f;
	}

	return t;
}

float Clamp(float t, float min, float max) {

	if (max <= t) {
		t = max;
	}
	if (min >= t) {
		t = min;
	}

	return t;
}

float Clamp3(float value, float min, float max) {
	return (std::max)(min, (std::min)(value, max));
}

//
Vector3 ClosestPointAABBSphere(const Sphere& sphere, const AABB& aabb)
{
	Vector3 closestPoint;
	closestPoint.x = Clamp(sphere.center.x, aabb.min_.x, aabb.max_.x);
	closestPoint.y = Clamp(sphere.center.y, aabb.min_.y, aabb.max_.y);
	closestPoint.z = Clamp(sphere.center.z, aabb.min_.z, aabb.max_.z);

	return closestPoint;
}

//
Plane PlaneFromPoints(const Vector3& p1, const Vector3& p2, const Vector3& p3) {
	Plane result{};

	// 2つのベクトルを求める
	Vector3 v1 = Subtract(p2, p1);
	Vector3 v2 = Subtract(p3, p1);

	// 法線を計算 (外積)
	result.normal = Cross(v1, v2);

	// ゼロベクトルチェック (3点が同一直線上の場合)
	if (Length(result.normal) == 0.0f) {
		// 法線が求まらない場合のエラーハンドリング
		result.normal = { 0.0f, 0.0f, 0.0f };
		result.distance = 0.0f;
		return result;
	}

	// 正規化
	result.normal = Normalize(result.normal);

	// 平面の距離 D の計算 (符号の修正)
	result.distance = -Dot(result.normal, p1);

	return result;
}

Vector3 Reflect(const Vector3& input, const Vector3& normal)
{
	Vector3 result;
	float dotProduct = Dot(input, normal);
	result.x = input.x - normal.x * (2 * dotProduct);
	result.y = input.y - normal.y * (2 * dotProduct);
	result.z = input.z - normal.z * (2 * dotProduct);
	return result;
}

Vector3 Reflect(const Vector3& input, const Vector3& normal, float restitution)
{
	// まず normal が正規化されている前提だが、明示的に normalize する場合:
	Vector3 norm = Normalize(normal);

	// 計算用変数
	float scale = 2.0f * Dot(input, norm) * restitution;

	// 反射ベクトルの計算
	return input - norm * scale;
}

Vector3 ClosestPointOnPlane(const Plane& plane, const Vector3& point) {
	// 平面の法線ベクトル
	Vector3 normal = plane.normal.Normalize();

	// 点から平面までの距離 d = dot(N, P) - D
	float distance = Dot(normal, point) - plane.distance;

	// 最近接点を計算: P - d * N
	Vector3 closestPoint = Subtract(point, Multiply(normal, distance));

	return closestPoint;
}

float DegreesToRadians(float degrees) { return float(degrees * ((float)M_PI / 180.0)); }

float RadiansToDegrees(float radians) { return float(radians * (180.0 / (float)M_PI)); }

#pragma endregion //数学関数
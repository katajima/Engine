#pragma once
#include <DirectXGame/engine/struct/Structs3D.h>
#include <DirectXGame/engine/struct/Structs2D.h>
#include<vector>
#define _USE_MATH_DEFINES
#include<random>
#include<numbers>
#include"Easing.h"


#pragma region Math

// 数学関数
namespace Math {

	// クランプ
	template<typename T>
	static T Clamp(T value, T min, T max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	/// <summary>
	/// 最短方向で角度を補間する
	/// </summary>
	float LerpShortAngle(float current, float target, float t);
	/// <summary>
	/// 長さ
	/// </summary>
	float Length(const float& v);
	/// <summary>
	/// クランプ
	/// </summary>
	float Clamp(float t);
	/// <summary>
	/// クランプ
	/// </summary>
	float Clamp(float t, float min, float max);
	/// <summary>
	/// 正規化クランプ
	/// </summary>
	float NormalizeClamp(float value, float minValue, float maxValue);
	/// <summary>
	/// 浮動小数点値が許容誤差内で等しいか判定
	/// </summary>
	bool NearlyEqual(float left, float right, float epsilon = 0.000001f);
	/// <summary>
	/// 区間内の値を0から1の補間率へ変換
	/// </summary>
	float InverseLerp(float start, float end, float value);
	/// <summary>
	/// 値を元の区間から別の区間へ変換
	/// </summary>
	float Remap(float value, float sourceMin, float sourceMax, float destinationMin, float destinationMax, bool clamp = false);
	/// <summary>
	/// 現在値を最大変化量の範囲で目標値へ近づける
	/// </summary>
	float MoveTowards(float current, float target, float maxDelta);
	/// <summary>
	/// 現在位置を最大移動量の範囲で目標位置へ近づける
	/// </summary>
	Vector3 MoveTowards(const Vector3& current, const Vector3& target, float maxDistanceDelta);
	/// <summary>
	/// 値を0以上length未満の範囲で繰り返す
	/// </summary>
	float Repeat(float value, float length);
	/// <summary>
	/// 値を0からlengthの間で往復させる
	/// </summary>
	float PingPong(float value, float length);
	/// <summary>
	/// 最大長を超えないように2Dベクトルを制限
	/// </summary>
	Vector2 ClampLength(const Vector2& value, float maxLength = 1.0f);
	/// <summary>
	/// ゼロに近い3Dベクトルをフォールバック方向で安全に正規化
	/// </summary>
	Vector3 NormalizeSafe(const Vector3& value, const Vector3& fallback = Vector3{ 0.0f,0.0f,1.0f }, float epsilon = 0.0001f);
	/// <summary>
	/// XZ平面の前方向から右方向を生成
	/// </summary>
	Vector3 MakeRightFromForwardXZ(const Vector3& forward);
	/// <summary>
	/// 2Dワールド行列を使って座標をローカル空間へ変換
	/// </summary>
	Vector2 TransformPointToLocal(const Vector2& worldPosition, const Matrix3x3& worldMatrix);

	/// <summary>
	/// 反射関数
	/// </summary>
	Vector3 Reflect(const Vector3& input, const Vector3& normal);
	/// <summary>
	/// 反射関数(反射率付き)
	/// </summary>
	Vector3 Reflect(const Vector3& input, const Vector3& normal, float restitution);

	/// <summary>
	/// 方向による回転位置計算
	/// </summary>
	Vector3 DirectionToRotate(const Vector3& direction, Dire dire);
	/// <summary>
	/// 方向による回転位置計算Z
	/// </summary>
	float DirectionToRotateZ(const Vector3& direction);
	/// <summary>
	/// 角度からラジアン
	/// </summary>
	float DegreesToRadians(float degrees);
	/// <summary>
	/// ラジアンから角度
	/// </summary>
	float RadiansToDegrees(float radians);
	/// <summary>
	/// 角度からラジアン
	/// </summary>
	Vector3 DegreesToRadians(Vector3 degrees);
	/// <summary>
	/// ラジアンから角度
	/// </summary>
	Vector3 RadiansToDegrees(Vector3 radians);
};

// 最近接点
namespace ClosestPoint {

	/// <summary>
	/// 球と四角形に対する最近接点
	/// </summary>
	Vector3 AABBSphere(const Sphere& sphere, const AABB& aabb);
	/// <summary>
	/// 点と線
	/// </summary>
	Vector3 PointSegment(const Segment& segment, const Vector3& point);
	/// <summary>
	/// 点と面
	/// </summary>
	Vector3 PointOnPlane(const Plane& plane, const Vector3& point);
	/// <summary>
	/// 線と三角
	/// </summary>
	Vector3 SegmentTriangle(const Segment& segment, const Triangle& triangle);
	/// <summary>
	/// 球と三角
	/// </summary>
	Vector3 SphereTriangle(const Sphere& sphere,const Triangle& triangle);
	/// <summary>
	/// 線と線の最近接点
	/// </summary>
	Vector3 SegmentSegment(const Segment& seg1, const Segment& seg2, Vector3 currentClosest);
	/// <summary>
	/// 線とAABB
	/// </summary>
	Vector3 SegmentAABB(const Segment& segment, const AABB& box);
	/// <summary>
	/// 点とOBB
	/// </summary>
	Vector3 PointOnOBB(const Vector3& point, const OBB& obb);
	/// <summary>
	/// 線とAABB
	/// </summary>
	Vector3 SegmentAABB(const Vector3& segStart, const Vector3& segEnd, const AABB& aabb);

}

#pragma endregion //数学関数


// 衝突速度を計算する
static std::pair<Vector3, Vector3> ComputeCollisionVelocities(float mass1, const Vector3& velocity1, float mass2, const Vector3& velocity2,
	float coefficientOfRestitution, const Vector3& normal) {
	/// <summary>
	/// 衝突面法線方向の速度成分を射影
	/// </summary>
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

	/// <summary>
	/// 接線成分を加算して最終的な速度を計算
	/// </summary>
	return std::make_pair(velocityAfter1 + sub1, velocityAfter2 + sub2);
}

/// <summary>
/// 点から平面への距離
/// </summary>
static float DistancePointToPlane(const Vector3& point, const Vector3& A, const Vector3& B, const Vector3& C) {
	// 三角形の辺ベクトル
	Vector3 AB = B - A;
	Vector3 AC = C - A;

	/// <summary>
	/// 平面の法線
	/// </summary>
	Vector3 normal = AB.Cross(AC).Normalize();

	/// <summary>
	/// 点と平面の距離
	/// </summary>
	return std::abs((point - A).Dot(normal));
}


/// <summary>
/// オブジェクトがカメラ内に映っているか
/// </summary>
bool IsInFrustum(const Matrix4x4& viewProjectionMatrix, const Vector3& position);

// 前方宣言
namespace Engine {
	class Camera;
	class WorldTransform;
}

/// <summary>
/// スクリーン位置取得
/// </summary>
Vector2 GetScreenPos(Engine::WorldTransform worldTransform, Engine::Camera* camera);

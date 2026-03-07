#include"MathFunctions.h"
#include<cmath>
#include"assert.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include"DirectXGame/engine/Camera/Camera.h"

#pragma region Math

float Math::Length(const float& v) {
	float result;

	result = sqrtf((v * v));

	return result;
};

float Math::Clamp(float t) {

	if (1.0f <= t) {
		t = 1.0f;
	}
	if (0.0f >= t) {
		t = 0.0f;
	}

	return t;
}

float Math::Clamp(float t, float min, float max) {

	if (max <= t) {
		t = max;
	}
	if (min >= t) {
		t = min;
	}

	return t;
}

float Math::Clamp3(float value, float min, float max) {
	return (std::max)(min, (std::min)(value, max));
}
float Math::NormalizeClamp(float value, float minValue, float maxValue)

{
	if (maxValue - minValue == 0.0f) {
		return 0.0f; // ゼロ除算防止
	}

	float t = (value - minValue) / (maxValue - minValue);
	return Math::Clamp(t,0.0f, 1.0f);
}
// 反射
Vector3 Math::Reflect(const Vector3& input, const Vector3& normal)
{
	Vector3 result;
	float dotProduct = Dot(input, normal);
	result.x = input.x - normal.x * (2 * dotProduct);
	result.y = input.y - normal.y * (2 * dotProduct);
	result.z = input.z - normal.z * (2 * dotProduct);
	return result;
}
// 反射
Vector3 Math::Reflect(const Vector3& input, const Vector3& normal, float restitution)
{
	// まず normal が正規化されている前提だが、明示的に normalize する場合:
	Vector3 norm = Normalize(normal);

	// 計算用変数
	float scale = 2.0f * Dot(input, norm) * restitution;

	// 反射ベクトルの計算
	return input - norm * scale;
}
//
Vector3 Math::DirectionToRotate(const Vector3& direction, Dire dire)
{
	Vector3 result{};

	Vector3 dir = Normalize(direction);
	if (dir.LengthSq() < 1e-6f)
		return result; // 無効方向

	// ----------------------------------------
	// 各軸をforward基準とした場合の変換
	// ----------------------------------------
	switch (dire)
	{
	case Dire::Z: // +Z軸が前方向（標準）
	{
		result.y = std::atan2(dir.x, dir.z);                  // ヨー角
		float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
		result.x = std::atan2(dir.y, -len);                   // ピッチ角
		result.z = 0.0f;                                      // ロール角不要
		break;
	}
	case Dire::mZ: // -Z軸が前方向
	{
		result.y = std::atan2(-dir.x, -dir.z);
		float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
		result.x = std::atan2(dir.y, len);
		result.z = 0.0f;
		break;
	}
	case Dire::X: // +X軸が前方向
	{
		result.z = std::atan2(-dir.y, dir.z);                 // ロール角（Z軸回転）
		float len = std::sqrt(dir.y * dir.y + dir.z * dir.z);
		result.y = -std::atan2(dir.z, dir.x);                 // ヨー角（Y軸回転）
		result.x = 0.0f;                                      // ピッチ不要
		break;
	}
	case Dire::mX: // -X軸が前方向
	{
		result.z = std::atan2(dir.y, -dir.z);
		float len = std::sqrt(dir.y * dir.y + dir.z * dir.z);
		result.y = std::atan2(dir.z, -dir.x);
		result.x = 0.0f;
		break;
	}
	case Dire::Y: // +Y軸が前方向
	{
		result.x = std::atan2(dir.z, dir.y);                  // ピッチ角（X軸回転）
		float len = std::sqrt(dir.z * dir.z + dir.y * dir.y);
		result.z = std::atan2(dir.x, len);                    // ロール角（Z軸回転）
		result.y = 0.0f;
		break;
	}
	case Dire::mY: // -Y軸が前方向
	{
		result.x = std::atan2(-dir.z, -dir.y);
		float len = std::sqrt(dir.z * dir.z + dir.y * dir.y);
		result.z = std::atan2(-dir.x, -len);
		result.y = 0.0f;
		break;
	}
	}

	return result;
}
//
float Math::DirectionToRotateZ(const Vector3& direction)
{
	return 0.0f;
}
// 
float Math::DegreesToRadians(float degrees) { return float(degrees * ((float)M_PI / 180.0)); }
// 
float Math::RadiansToDegrees(float radians) { return float(radians * (180.0 / (float)M_PI)); }
//
Vector3 Math::DegreesToRadians(Vector3 degrees)
{
	return Vector3(DegreesToRadians(degrees.x), DegreesToRadians(degrees.y), DegreesToRadians(degrees.z));
}
//
Vector3 Math::RadiansToDegrees(Vector3 radians)
{
	return Vector3(RadiansToDegrees(radians.x), RadiansToDegrees(radians.y), RadiansToDegrees(radians.z));
}
#pragma endregion //数学関数


#pragma region ClosestPoint
// AABBと球最近接点
Vector3 ClosestPoint::AABBSphere(const Sphere& sphere, const AABB& aabb)
{
	Vector3 closestPoint;
	closestPoint.x = Math::Clamp(sphere.center.x, aabb.min.x, aabb.max.x);
	closestPoint.y = Math::Clamp(sphere.center.y, aabb.min.y, aabb.max.y);
	closestPoint.z = Math::Clamp(sphere.center.z, aabb.min.z, aabb.max.z);

	return closestPoint;
}
// Planeと点の最近接点
Vector3 ClosestPoint::PointOnPlane(const Plane& plane, const Vector3& point) {
	// 平面の法線ベクトル
	Vector3 normal = plane.normal.Normalize();

	// 点から平面までの距離 d = dot(N, P) - D
	float distance = Dot(normal, point) - plane.distance;

	// 最近接点を計算: P - d * N
	Vector3 closestPoint = Subtract(point, Multiply(normal, distance));

	return closestPoint;
}
// 点と線の最近接点
Vector3 ClosestPoint::PointSegment(const Segment& segment, const Vector3& point) {
	Vector3 diff = segment.diff();
	float lenSq = Dot(diff, diff);

	if (lenSq == 0.0f) {
		return segment.origin;  // 線分が点の場合は始点を返す
	}

	float t = Dot(point - segment.origin, diff) / lenSq;
	t = Math::Clamp3(t, 0.0f, 1.0f);

	return segment.origin - Multiply(diff, t);
}
// 線と三角
Vector3 ClosestPoint::SegmentTriangle(const Segment& segment, const Triangle& triangle) {
	// 三角形の3辺を定義
	Segment edge1 = { triangle.vertices[0], triangle.vertices[1] };
	Segment edge2 = { triangle.vertices[1], triangle.vertices[2] };
	Segment edge3 = { triangle.vertices[2], triangle.vertices[0] };

	// 最初に線分の始点を初期最近接点とする
	Vector3 closest = PointSegment(segment, triangle.vertices[0]);

	// 三角形のエッジに対して最近接点を更新
	closest = SegmentSegment(segment, edge1, closest);
	closest = SegmentSegment(segment, edge2, closest);
	closest = SegmentSegment(segment, edge3, closest);

	// 三角形の面上の最近接点も考慮（重要）
	Plane plane = Plane::PlaneFromPoints(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]);
	Vector3 pointOnPlane = PointOnPlane(plane, segment.origin);
	closest = LengthSquared(Subtract(closest, segment.origin)) < LengthSquared(Subtract(pointOnPlane, segment.origin))
		? closest
		: pointOnPlane;

	return closest;
}
// 線と線
Vector3 ClosestPoint::SegmentSegment(const Segment& seg1, const Segment& seg2, Vector3 currentClosest) {
	Vector3 u = seg1.diff();
	Vector3 v = seg2.diff();
	Vector3 w = Subtract(seg1.origin, seg2.origin);

	float a = Dot(u, u);
	float b = Dot(u, v);
	float c = Dot(v, v);
	float d = Dot(u, w);
	float e = Dot(v, w);

	float denom = a * c - b * b;

	// ゼロ除算の回避
	float s = 0.0f, t = 0.0f;
	if (denom != 0.0f) {
		s = Math::Clamp3((b * e - c * d) / denom, 0.0f, 1.0f);
		t = Math::Clamp3((a * e - b * d) / denom, 0.0f, 1.0f);
	}

	Vector3 closestOnSeg1 = Add(seg1.origin, Multiply(u, s));
	Vector3 closestOnSeg2 = Add(seg2.origin, Multiply(v, t));

	return LengthSquared(Subtract(closestOnSeg1, closestOnSeg2)) < LengthSquared(Subtract(currentClosest, seg1.origin))
		? closestOnSeg1
		: currentClosest;
}
// 線とAABB
Vector3 ClosestPoint::SegmentAABB(const Segment& segment, const AABB& box) {
	Vector3 closestPoint = segment.origin;

	for (int i = 0; i < 3; i++) { // X, Y, Z 各軸
		if (closestPoint[i] < box.min[i]) closestPoint[i] = box.min[i];
		if (closestPoint[i] > box.max[i]) closestPoint[i] = box.max[i];
	}

	return closestPoint;
}
// 点とOBB
Vector3 ClosestPoint::PointOnOBB(const Vector3& point, const OBB& obb) {
	Vector3 d = point - obb.center;
	Vector3 closest = obb.center;

	for (int i = 0; i < 3; ++i) {
		Vector3 axis = obb.orientations[i];
		float extent = obb.size[i];
		float dist = d.Dot(axis);
		dist = std::clamp(dist, -extent, extent);
		closest = closest + axis * dist;
	}
	return closest;
}
// 線とAABB
Vector3 ClosestPoint::SegmentAABB(const Vector3& segStart, const Vector3& segEnd, const AABB& aabb) {
	// 線分の最近接点をAABB内にクランプ
	Vector3 ab = segEnd - segStart;
	float t = 0.0f;
	float abLenSq = ab.LengthSq();
	if (abLenSq > 0.0001f) {
		Vector3 aabbCenter = (aabb.min + aabb.max) * 0.5f;
		t = (aabbCenter - segStart).Dot(ab) / abLenSq;
		t = std::clamp(t, 0.0f, 1.0f);
	}
	Vector3 closest = segStart + ab * t;

	// AABB内にクランプ（closest点がAABBの内外どちらでも動作する）
	return Vector3::Clamp(closest, aabb.min, aabb.max);
}


#pragma endregion // 最近接点

bool IsInFrustum(const Matrix4x4& viewProjectionMatrix, const Vector3& position) {
	// クリップスペース座標を取得
	Vector4 clipSpacePosition = Transforms(Vector4(position.x, position.y, position.z, 1.0f), viewProjectionMatrix);

	// w が負の場合、カメラの後ろにあるため視錐台外
	if (clipSpacePosition.w <= 0.0f) {
		return false;
	}

	// 視錐台内にあるかチェック
	if (clipSpacePosition.x < -clipSpacePosition.w || clipSpacePosition.x > clipSpacePosition.w ||
		clipSpacePosition.y < -clipSpacePosition.w || clipSpacePosition.y > clipSpacePosition.w ||
		clipSpacePosition.z < 0 || clipSpacePosition.z > clipSpacePosition.w)
	{
		return false;
	}
	return true;
}

Vector2 GetScreenPos(Engine::WorldTransform worldTransform, Engine::Camera* camera)
{
	Vector3 wPos = worldTransform.worldMat_.GetWorldPosition();

	// カメラのビュープロジェクション行列を取得
	Matrix4x4 matViewProjection;
	if (camera) {
		matViewProjection = Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix());
	}
	else {
		matViewProjection = Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix());;
	}

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, static_cast<float>(Engine::WinApp::GetClientWidth()), static_cast<float> (Engine::WinApp::GetClientHeight()), 0, 1);

	// 視錐台内にオブジェクトがあるかチェック (matViewProjection を渡す)
	if (!IsInFrustum(matViewProjection, wPos)) {
		return Vector2{ -100, -100 }; // 視錐台外にある場合、無効なスクリーン座標を返す
	}

	// ワールド座標をクリップ空間座標へ変換
	Vector4 clipSpacePos = Transforms(Vector4(wPos.x, wPos.y, wPos.z, 1.0f), matViewProjection);

	// 透視除算 (NDC へ変換)
	if (clipSpacePos.w == 0.0f) {
		return Vector2{ -100, -100 }; // 透視除算エラー
	}
	Vector3 ndcPos = {
		clipSpacePos.x / clipSpacePos.w,
		clipSpacePos.y / clipSpacePos.w,
		clipSpacePos.z / clipSpacePos.w
	};

	// NDC → スクリーン座標変換
	Vector4 screenPos = Transforms(Vector4(ndcPos.x, ndcPos.y, ndcPos.z, 1.0f), matViewport);

	return Vector2{ screenPos.x, screenPos.y };
}


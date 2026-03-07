#include "ColliderFunction3D.h"


#pragma region Detection
// AABBと点
bool Engine::Collision::Detection::Check(const AABB& aabb, const Vector3& point) {
	return (point >= aabb.min && point <= aabb.max);
}
//衝突判定(球と球)
bool Engine::Collision::Detection::Check(const Sphere& s1, const Sphere& s2)
{
	Vector3 result = s2.center - s1.center;
	// 半径以内なら
	if (result.Length() <= s2.radius + s1.radius) {
		return true;
	}
	else {
		return false;
	}
}
//衝突判定(球と点)
bool Engine::Collision::Detection::Check(const Sphere& s1, const Vector3& v1)
{
	Vector3 result = v1 - s1.center;
	// 半径以内なら
	if (result.Length() <= s1.radius) {
		return true;
	}
	else {
		return false;
	}
}
//衝突判定(球と平面)
bool Engine::Collision::Detection::Check(const Sphere& sphere, const Plane& plane)
{
	Vector3 dis = plane.normal.Normalize();

	float distance = Dot(sphere.center, dis) - plane.distance;
	return std::abs(distance) < sphere.radius;
}
//衝突判定(線と平面)
bool Engine::Collision::Detection::Check(const Segment& segment, const Plane& plane)
{
	Vector3 diff = segment.diff();

	//まず垂直判定を行うために、法線と線の内積を求める
	float dot = Dot(plane.normal, diff);

	// 垂直=平行であるので、衝突しているはずがない
	if (dot == 0.0f) {
		return false;
	}

	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

	// tが0.0fから1.0f以内なら
	if (t >= 0.0f && t <= 1.0f) {
		return true;
	}
	return false;
}
//三角形と線の衝突判定
bool Engine::Collision::Detection::Check(const Triangle& triangle, const Segment& segment) {
	Vector3 diff = segment.diff();

	// 三角形の平面を取得
	Plane plane = Plane::PlaneFromPoints(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]);

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
bool Engine::Collision::Detection::Check(const Triangle& triangle, const Capsule& capsule)
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
// Triangle と 球の衝突判定
bool Engine::Collision::Detection::Check(const Triangle& triangle, const Sphere& sphere)
{
	constexpr float kEpsilon = 1e-6f;

	const Vector3& A = triangle.vertices[0];
	const Vector3& B = triangle.vertices[1];
	const Vector3& C = triangle.vertices[2];

	const Vector3& P = sphere.center;
	const float r = sphere.radius;
	const float rr = r + kEpsilon;
	const float r2 = rr * rr;

	const Vector3 AB = B - A;
	const Vector3 AC = C - A;
	const Vector3 AP = P - A;

	// 三角形法線
	Vector3 N = AB.Cross(AC);
	const float nLenSq = N.LengthSq();

	// 退化三角形
	if (nLenSq <= kEpsilon) {
		if (PointLineDistanceSquared(P, A, B) <= r2) return true;
		if (PointLineDistanceSquared(P, B, C) <= r2) return true;
		if (PointLineDistanceSquared(P, C, A) <= r2) return true;
		return false;
	}

	N /= std::sqrt(nLenSq);

	// 球中心から平面までの符号付き距離
	const float dist = AP.Dot(N);

	// 平面上への射影点
	const Vector3 Q = P - N * dist;

	// 三角形内部判定
	const Vector3 AQ = Q - A;
	const Vector3 BQ = Q - B;
	const Vector3 CQ = Q - C;

	const Vector3 BC = C - B;
	const Vector3 CA = A - C;

	const float c0 = (AB.Cross(AQ)).Dot(N);
	const float c1 = (BC.Cross(BQ)).Dot(N);
	const float c2 = (CA.Cross(CQ)).Dot(N);

	if ((c0 >= -kEpsilon && c1 >= -kEpsilon && c2 >= -kEpsilon) ||
		(c0 <= kEpsilon && c1 <= kEpsilon && c2 <= kEpsilon))
	{
		return std::fabs(dist) <= rr;
	}

	// 三角形外部なら各辺との最近距離
	if (PointLineDistanceSquared(P, A, B) <= r2) return true;
	if (PointLineDistanceSquared(P, B, C) <= r2) return true;
	if (PointLineDistanceSquared(P, C, A) <= r2) return true;

	return false;
}
// Rayと球の衝突判定
bool Engine::Collision::Detection::Check(const Ray& ray, const Sphere& sphere) {
	Vector3 m = ray.origin - sphere.center;
	float b = Dot(m, ray.diff);
	float c = Dot(m, m) - sphere.radius * sphere.radius;

	// 判別式が負なら交差なし
	float discriminant = b * b - c;
	if (discriminant < 0.0f) return false;

	return true; // 交差あり
}
// RayとAABBの衝突判定
bool Engine::Collision::Detection::Check(const Ray& ray, const AABB& aabb) {
	float tMin = 0.0f;
	float tMax = FLT_MAX;

	for (int i = 0; i < 3; ++i) {
		float rayOrig = (&ray.origin.x)[i];
		float rayDir = (&ray.diff.x)[i];
		float aabbMin = (&aabb.min.x)[i];
		float aabbMax = (&aabb.max.x)[i];

		if (fabsf(rayDir) < 1e-6f) {
			// レイが軸と平行 → 原点が範囲外なら交差なし
			if (rayOrig < aabbMin || rayOrig > aabbMax)
				return false;
		}
		else {
			float invD = 1.0f / rayDir;
			float t1 = (aabbMin - rayOrig) * invD;
			float t2 = (aabbMax - rayOrig) * invD;
			if (t1 > t2) std::swap(t1, t2);
			tMin = (std::max)(tMin, t1);
			tMax = (std::min)(tMax, t2);
			if (tMin > tMax)
				return false;
		}
	}

	return true;
}
//
bool Engine::Collision::Detection::Check(const std::vector<Vector3>& controlPoints, float radius, const Vector3& point, int segmentPerCurve) {
	if (controlPoints.size() < 4) return false;

	int totalSegment = static_cast<int>(controlPoints.size() - 3) * segmentPerCurve;
	for (int i = 0; i < totalSegment; ++i) {
		float t0 = i / static_cast<float>(totalSegment);
		float t1 = (i + 1) / static_cast<float>(totalSegment);

		Vector3 pA = CatmullRom(controlPoints, t0);
		Vector3 pB = CatmullRom(controlPoints, t1);

		if (PointInCapsule(point, pA, pB, radius)) {
			return true;
		}
	}

	return false;
}
// AABBとAABB
bool Engine::Collision::Detection::Check(const AABB& aabb1, const AABB& aabb2)
{
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {

		return true;
	}

	return false;
}
// AABBと球
bool Engine::Collision::Detection::Check(const AABB& aabb, const Sphere& sphere)
{
	Vector3 closestPoint = ClosestPoint::AABBSphere(sphere, aabb);

	float dist = Distance(sphere.center, closestPoint);

	if (dist <= sphere.radius) {
		return true;
	}
	else {
		return false;
	}
}
//四角形と線の衝突判定
bool Engine::Collision::Detection::Check(const AABB& aabb, const Segment& segment)
{
	//点がAABB内にあるなら
	if (Check(aabb, segment.origin) || Check(aabb, segment.end)) {

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
		if (segment.origin.x < aabb.min.x || segment.origin.x > aabb.max.x) return false;
	}
	if (std::abs(dotY) < 1e-6) {
		if (segment.origin.y < aabb.min.y || segment.origin.y > aabb.max.y) return false;
	}
	if (std::abs(dotZ) < 1e-6) {
		if (segment.origin.z < aabb.min.z || segment.origin.z > aabb.max.z) return false;
	}


	Vector3 tMin, tMax;

	tMin.x = (aabb.min.x - Dot(segment.origin, planeX1.normal)) / dotX;
	tMin.y = (aabb.min.y - Dot(segment.origin, planeY1.normal)) / dotY;
	tMin.z = (aabb.min.z - Dot(segment.origin, planeZ1.normal)) / dotZ;

	tMax.x = (aabb.max.x - Dot(segment.origin, planeX1.normal)) / dotX;
	tMax.y = (aabb.max.y - Dot(segment.origin, planeY1.normal)) / dotY;
	tMax.z = (aabb.max.z - Dot(segment.origin, planeZ1.normal)) / dotZ;


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
bool Engine::Collision::Detection::Check(const Sphere& sphere, const Segment& segment) {
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
// 球とカプセル
bool Engine::Collision::Detection::Check(const Sphere& sphere, const Capsule& cap) {
	Vector3 closest = ClosestPoint::PointSegment(cap.segment, sphere.center);
	Vector3 diff = closest - sphere.center;

	float sumRadius = sphere.radius + cap.radius;
	return diff.LengthSq() <= (sumRadius * sumRadius);
}
// OBBと球
bool Engine::Collision::Detection::Check(const OBB& obb, const Sphere& sphere)
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
bool Engine::Collision::Detection::Check(const OBB& obb, const Segment& segment) {
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
// OBBとAABBの衝突判定
bool Engine::Collision::Detection::Check(const OBB& obb, const AABB& aabb) {
	// AABBの中心と半径
	Vector3 aabbCenter = (aabb.min + aabb.max) * 0.5f;
	Vector3 aabbHalf = (aabb.max - aabb.min) * 0.5f;

	// AABBの軸（固定）
	Vector3 aabbAxes[3] = {
		{1, 0, 0}, {0, 1, 0}, {0, 0, 1}
	};

	// OBBの軸
	const Vector3* obbAxes = obb.orientations;

	// 軸間のベクトル
	Vector3 t = aabbCenter - obb.center;

	// 分離軸15本をすべてテスト
	for (int i = 0; i < 3; ++i) {
		Vector3 axis = Normalize(obbAxes[i]);
		float r1 = obb.size[i];
		float r2 =
			aabbHalf.x * AbsDot(axis, aabbAxes[0]) +
			aabbHalf.y * AbsDot(axis, aabbAxes[1]) +
			aabbHalf.z * AbsDot(axis, aabbAxes[2]);

		if (std::abs(Dot(t, axis)) > r1 + r2) return false;
	}

	for (int i = 0; i < 3; ++i) {
		Vector3 axis = aabbAxes[i];
		float r1 =
			obb.size.x * AbsDot(axis, obbAxes[0]) +
			obb.size.y * AbsDot(axis, obbAxes[1]) +
			obb.size.z * AbsDot(axis, obbAxes[2]);
		float r2 = aabbHalf[i];

		if (std::abs(Dot(t, axis)) > r1 + r2) return false;
	}

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			Vector3 axis = Cross(obbAxes[i], aabbAxes[j]);
			if (axis.x == 0 && axis.y == 0 && axis.z == 0) continue;
			axis = Normalize(axis);

			float r1 =
				obb.size.x * AbsDot(axis, obbAxes[0]) +
				obb.size.y * AbsDot(axis, obbAxes[1]) +
				obb.size.z * AbsDot(axis, obbAxes[2]);
			float r2 =
				aabbHalf.x * AbsDot(axis, aabbAxes[0]) +
				aabbHalf.y * AbsDot(axis, aabbAxes[1]) +
				aabbHalf.z * AbsDot(axis, aabbAxes[2]);

			if (std::abs(Dot(t, axis)) > r1 + r2) return false;
		}
	}

	return true;
};
// OBBとカプセル
bool Engine::Collision::Detection::Check(const OBB& obb, const Capsule& cap) {
	const int kSamples = 10;

	for (int i = 0; i <= kSamples; ++i) {
		float t = static_cast<float>(i) / kSamples;
		Vector3 capsulePoint = cap.segment.pointAt(t);
		Vector3 closest = ClosestPoint::PointOnOBB(capsulePoint, obb);
		Vector3 delta = capsulePoint - closest;
		if (delta.LengthSq() <= cap.radius * cap.radius) {
			return true;
		}
	}

	return false;
}
// 分離軸定理による OBB vs OBB の衝突判定
bool Engine::Collision::Detection::Check(const OBB& obb0, const OBB& obb1) {
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
// 分離軸定理による OBB vs OBB の衝突判定
bool Engine::Collision::Detection::Check2(const OBB& obb0, const OBB& obb1) {
	const Vector3* A = obb0.orientations;
	const Vector3* B = obb1.orientations;

	// OBB1の中心をOBB0のローカル空間に投影
	Vector3 T = obb1.center - obb0.center;
	T = { T.Dot(A[0]), T.Dot(A[1]), T.Dot(A[2]) };

	constexpr float EPSILON = 1e-6f;
	float ra, rb;
	float R[3][3], AbsR[3][3];

	// R行列とAbsRの計算
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			R[i][j] = A[i].Dot(B[j]);
			AbsR[i][j] = std::abs(R[i][j]) + EPSILON;
		}
	}

	// 1. A[i] 軸
	for (int i = 0; i < 3; ++i) {
		ra = obb0.size[i] * 1.0f;
		rb = obb1.size[0] * 1.0f * AbsR[i][0] +
			obb1.size[1] * 1.0f * AbsR[i][1] +
			obb1.size[2] * 1.0f * AbsR[i][2];

		if (std::abs(T[i]) > ra + rb) return false;
	}

	// 2. B[i] 軸
	for (int i = 0; i < 3; ++i) {
		ra = obb0.size[0] * 1.0f * AbsR[0][i] +
			obb0.size[1] * 1.0f * AbsR[1][i] +
			obb0.size[2] * 1.0f * AbsR[2][i];

		rb = obb1.size[i] * 1.0f;

		float t = T[0] * R[0][i] + T[1] * R[1][i] + T[2] * R[2][i];
		if (std::abs(t) > ra + rb) return false;
	}

	// 3. 交差軸 A[i] x B[j]
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			ra = obb0.size[(i + 1) % 3] * 1.0f * AbsR[(i + 2) % 3][j] +
				obb0.size[(i + 2) % 3] * 1.0f * AbsR[(i + 1) % 3][j];

			rb = obb1.size[(j + 1) % 3] * 1.0f * AbsR[i][(j + 2) % 3] +
				obb1.size[(j + 2) % 3] * 1.0f * AbsR[i][(j + 1) % 3];

			float t = std::abs(
				T[(i + 1) % 3] * R[(i + 2) % 3][j] -
				T[(i + 2) % 3] * R[(i + 1) % 3][j]
			);

			if (t > ra + rb) return false;
		}
	}

	return true;
}
// カプセルとカプセル
bool Engine::Collision::Detection::Check(const Capsule& cap0, const Capsule& cap1)
{
	float distanceSq = SegmentClosestDistanceSq(cap0.segment, cap1.segment);
	float radiusSum = cap0.radius + cap1.radius;
	return distanceSq <= radiusSum * radiusSum;
}
//カプセルと三角形の衝突判定
bool Engine::Collision::Detection::Check(const Capsule& capsule, const Triangle& tri) {
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
bool Engine::Collision::Detection::Check(const Capsule& capsule, const AABB& box) {
	// AABB に対するカプセルの線分の最近接点を求める
	Vector3 closestPoint = ClosestPoint::SegmentAABB(capsule.segment, box);

	// 最近接点とカプセルの中心軸の最近接点を求める
	Vector3 capsuleClosestPoint = ClosestPoint::PointSegment(capsule.segment, closestPoint);

	// 2点間の距離を計算
	float distanceSquared = (closestPoint - capsuleClosestPoint).LengthSq();

	// カプセルの半径の二乗と比較
	return distanceSquared <= (capsule.radius * capsule.radius);
}

#pragma endregion // 判定

#pragma region Response

bool Engine::Collision::Response::ReflectVelocity(const Sphere& s1, const Sphere& s2, Vector3& outPushVec)
{
	Vector3 delta = s1.center - s2.center;
	float distSq = delta.LengthSq();
	float radiusSum = s1.radius + s2.radius;

	// 離れているなら反応なし
	if (distSq >= radiusSum * radiusSum)
		return false;

	float dist = sqrtf(distSq);
	if (dist < 1e-6f)
		return false; // 同一位置の場合は反射方向が定義できない

	// 衝突法線（正規化）
	Vector3 normal = delta / dist;

	// 押し戻しベクトル（penetration分）
	float penetration = radiusSum - dist;
	outPushVec = normal * penetration;

	return true;
}



#pragma endregion // 応答


#pragma once
#include "LineStruct.h"
#include "array"
#include "Structs.h"


// 方向
enum class Dire {
	X,		// X軸
	Y,		// Y軸
	Z,		// Z軸
	mX,		// -X軸
	mY,		// -Y軸
	mZ		// -Z軸
};



//速度や加速度を管理する構造体
struct Kinematics {
	Vector3 velocity{ 0.0f, 0.0f, 0.0f };
	Vector3 acceleration{ 0.0f, 0.0f, 0.0f };

	// デフォルトコンストラクタ
	Kinematics() = default;

	// 初期値を設定できるコンストラクタ
	Kinematics(const Vector3& v, const Vector3& a)
		: velocity(v), acceleration(a) {
	}

	// 更新処理
	void Update(float deltaTime) {
		velocity += acceleration * deltaTime; // 加速度から速度更新
	}

	// 減衰などが必要なら追加
	void ApplyDamping(float damping) {
		velocity *= damping; // 0.0f〜1.0fの範囲
	}
};

//Transform
struct Transform {
	Vector3 scale = { 1.0f,1.0f,1.0f };
	Vector3 rotate{};
	Vector3 translate{};

	// ワールドMatrix取得
	Matrix4x4 GetWorldMatrix() const {
		return MakeAffineMatrix(scale, rotate, translate);
	}
	// スケールMatrix取得
	Matrix4x4 GetScaleMatrix() const {
		return MakeScaleMatrix(scale);
	}
	// 回転Matrix取得
	Matrix4x4 GetRotateMatrix() const {
		return MakeRotateXYZ(rotate);
	}
	// 移動Matrix取得
	Matrix4x4 GetTranslateMatrix() const {
		return MakeTranslateMatrix(translate);
	}
	// 逆行列取得
	Matrix4x4 GetInverseWorldMatrix() const {
		return Inverse(GetWorldMatrix());
	}

	// トランスフォーム差分の補間
	Transform LerpTransform(const Transform& to, float t) const {
		Transform result;
		result.scale = Lerp(scale, to.scale, t);
		result.rotate = Lerp(rotate, to.rotate, t); // クォータニオン推奨だが簡易的に
		result.translate = Lerp(translate, to.translate, t);
		return result;
	}

};

/// <summary>
/// Quaternionトランスフォーム情報(Quaternion回転)
/// </summary>
struct QuaternionTransform
{
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;

	// ワールド行列生成
	Matrix4x4 GetWorldMatrix() const {
		Matrix4x4 s = MakeScaleMatrix(scale);
		Matrix4x4 r = rotate.MakeRotateMatrix(); // クォータニオン → 回転行列
		Matrix4x4 t = MakeTranslateMatrix(translate);
		return s * r * t;
	}
	// 逆行列
	Matrix4x4 GetInverseWorldMatrix() const {
		return Inverse(GetWorldMatrix());
	}

	// スケール行列
	Matrix4x4 GetScaleMatrix() const { return MakeScaleMatrix(scale); } // スケール行列取得
	// 回転行列
	Matrix4x4 GetRotateMatrix() const { return rotate.MakeRotateMatrix(); } // 回転行列取得
	// 移動行列
	Matrix4x4 GetTranslateMatrix() const { return MakeTranslateMatrix(translate); } // 移動行列取得

	// 点を変換する
	Vector3 TransformPoint(const Vector3& localPos) const {
		Vector3 scaled = localPos * scale;
		Vector3 rotated = rotate.RotateVector(scaled);
		return rotated + translate;
	}
	// 右
	Vector3 Right() const { return rotate.RotateVector(Vector3(1, 0, 0)); }
	// 上
	Vector3 Up() const { return rotate.RotateVector(Vector3(0, 1, 0)); }
	// 前
	Vector3 Forward() const { return rotate.RotateVector(Vector3(0, 0, 1)); }

	// 補間
	QuaternionTransform Lerp(const QuaternionTransform& to, float t) const {
		return {
			Vector3::Lerp(scale, to.scale, t),
			Slerp(rotate, to.rotate, t),
			Vector3::Lerp(translate, to.translate, t)
		};
	}
};

//AABB
struct AABB {
	Vector3 min; //!< 最小点
	Vector3 max; //!< 最大点

	// 判定
	bool intersects(const AABB& other) const {
		return (min.x <= other.max.x && max.x >= other.min.x &&
			min.y <= other.max.y && max.y >= other.min.y &&
			min.z <= other.max.z && max.z >= other.min.z);
	}

	// 点がAABBを完全に内包しているか判定
	bool Contains(const Vector3& point) const {
		return (point.x >= min.x && point.x <= max.x) &&
			(point.y >= min.y && point.y <= max.y) &&
			(point.z >= min.z && point.z <= max.z);
	}

	// AABBが別のAABBを完全に内包しているか判定
	bool Contains(const AABB& other) const {
		return (other.min.x >= min.x && other.max.x <= max.x) &&
			(other.min.y >= min.y && other.max.y <= max.y) &&
			(other.min.z >= min.z && other.max.z <= max.z);
	}

	// 中心点
	Vector3 Center() const {
		return (min + max) * 0.5f;
	}

	// サイズ取得
	Vector3 Size() const {
		return max - min;
	}
	// 半径
	Vector3 Extents() const {
		return Size() * 0.5f;
	}
	// 最近接点
	Vector3 ClosestPoint(const Vector3& point) const {
		return {
			std::clamp(point.x, min.x, max.x),
			std::clamp(point.y, min.y, max.y),
			std::clamp(point.z, min.z, max.z)
		};
	}

	// AABBの有効性チェック
	bool IsValid() const {
		return (min.x <= max.x) && (min.y <= max.y) && (min.z <= max.z);
	}
};

// 球
struct Sphere {
	Vector3 center; //!<中心点
	float radius;   //!<半径 

	// 点との衝突判定
	bool Contains(const Vector3& point) const {
		return (point - center).LengthSq() <= radius * radius;
	}

	// 球同士の交差判定
	bool Intersects(const Sphere& other) const {
		float distSq = (center - other.center).LengthSq();
		float radiiSum = radius + other.radius;
		return distSq <= radiiSum * radiiSum;
	}

	// 最近接点
	Vector3 ClosestPoint(const Vector3& point) const {
		Vector3 dir = point - center;
		if (dir.LengthSq() <= radius * radius) {
			return point; // 内部
		}
		return center + dir.Normalize() * radius;
	}

	// 線分との交差判定（最近点距離）
	bool IntersectsSegment(const Segment& seg) const {
		Vector3 closest = seg.ClosestPoint(center);
		return (closest - center).LengthSq() <= radius * radius;
	}
};

//平面
struct Plane {
	Vector3 normal;  //!< 法線
	float distance; //!< 距離 

	// 距離の計算
	float GetSignedDistance(const Vector3& point) const {
		return normal.Dot(point) + distance;
	}

	// 点が平面上にあるか
	bool IsOnPlane(const Vector3& point, float epsilon = 1e-5f) const {
		return std::abs(GetSignedDistance(point)) < epsilon;
	}

	// 平面から最も近い点
	Vector3 ClosestPoint(const Vector3& point) const {
		float d = GetSignedDistance(point);
		return point - normal * d;
	}

	// 平面での反射ベクトル計算
	Vector3 Reflect(const Vector3& direction) const {
		return direction - normal * normal.Dot(direction) * 2.0f;
	}

	// 平面の生成関数
	static Plane FromPointNormal(const Vector3& point, const Vector3& normal) {
		Plane p;
		p.normal = normal.Normalize();
		p.distance = -p.normal.Dot(point); // Ax + By + Cz + D = 0
		return p;
	}
	// 三点から平面の生成関数
	static Plane FromThreePoints(const Vector3& a, const Vector3& b, const Vector3& c) {
		Vector3 ab = b - a;
		Vector3 ac = c - a;
		Vector3 n = ab.Cross(ac).Normalize();
		return FromPointNormal(a, n);
	}
	// 3 点から平面を求める
	static Plane PlaneFromPoints(const Vector3& p1, const Vector3& p2, const Vector3& p3) {
		Plane result{};

		// 2つのベクトルを求める
		Vector3 v1 = p2 - p1;
		Vector3 v2 = p3 - p1;

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
		result.normal = result.normal.Normalize();

		// 平面の距離 D の計算 (符号の修正)
		result.distance = -Dot(result.normal, p1);

		return result;
	}


	// 線分と平面の交点
	bool IntersectSegment(const Segment& seg, Vector3& outPoint) const {
		Vector3 dir = seg.end - seg.origin;
		float denom = normal.Dot(dir);
		if (std::abs(denom) < FLT_EPSILON) return false; // 平行

		float t = -(normal.Dot(seg.origin) + distance) / denom;
		if (t < 0.0f || t > 1.0f) return false; // 線分上にない

		outPoint = seg.origin + dir * t;
		return true;
	}
	




};

/// <summary>
/// こーな
/// </summary>
struct CornerSegment {
	Vector3 center;
	int segment;
	float radius;
};


//三角形
struct Triangle
{
	Vector3 vertices[3]; // !頂点
	
	// +=オペレーターのオーバーロード 
	Triangle& operator+=(const Vector3& offset) {
		for (auto& vertex : vertices) {
			vertex += offset;
		}
		return *this;
	}

	// ずらす
	Triangle OffsetVector3(const Vector3& offset) const {
		Triangle result = *this;  // コピーを作成
		for (auto& vertex : result.vertices) {
			vertex += offset;
		}
		return result;
	}

	// 法線方向の取得
	Vector3 GetNormal() const {
		Vector3 edge1 = vertices[1] - vertices[0];
		Vector3 edge2 = vertices[2] - vertices[0];
		return Normalize(Cross(edge1, edge2));
	}

	// 面積の取得
	float GetArea() const {
		Vector3 edge1 = vertices[1] - vertices[0];
		Vector3 edge2 = vertices[2] - vertices[0];
		return 0.5f * Length(Cross(edge1, edge2));
	}

	// 重心の取得
	Vector3 GetCentroid() const {
		return (vertices[0] + vertices[1] + vertices[2]) / 3.0f;
	}

	// 点が三角形の中にあるか判定（バリセン式）
	bool ContainsPoint(const Vector3& point) const {
		Vector3 v0 = vertices[2] - vertices[0];
		Vector3 v1 = vertices[1] - vertices[0];
		Vector3 v2 = point - vertices[0];

		float d00 = Dot(v0, v0);
		float d01 = Dot(v0, v1);
		float d11 = Dot(v1, v1);
		float d20 = Dot(v2, v0);
		float d21 = Dot(v2, v1);

		float denom = d00 * d11 - d01 * d01;
		if (denom == 0.0f) return false;

		float v = (d11 * d20 - d01 * d21) / denom;
		float w = (d00 * d21 - d01 * d20) / denom;
		float u = 1.0f - v - w;

		return (u >= 0.0f) && (v >= 0.0f) && (w >= 0.0f);
	}

	// Rayとの交差判定（Möller–Trumbore法）
	bool IntersectRay(const Vector3& rayOrigin, const Vector3& rayDir, float& outT) const {
		const float EPSILON = 1e-6f;
		Vector3 edge1 = vertices[1] - vertices[0];
		Vector3 edge2 = vertices[2] - vertices[0];
		Vector3 h = Cross(rayDir, edge2);
		float a = Dot(edge1, h);
		if (fabs(a) < EPSILON) return false;

		float f = 1.0f / a;
		Vector3 s = rayOrigin - vertices[0];
		float u = f * Dot(s, h);
		if (u < 0.0f || u > 1.0f) return false;

		Vector3 q = Cross(s, edge1);
		float v = f * Dot(rayDir, q);
		if (v < 0.0f || u + v > 1.0f) return false;

		float t = f * Dot(edge2, q);
		if (t > EPSILON) {
			outT = t;
			return true;
		}

		return false;
	}
	
	// AABB 取得
	AABB GetAABB() const {
		AABB result{};
		result.min = Min(Min(vertices[0], vertices[1]), vertices[2]);
		result.max = Max(Max(vertices[0], vertices[1]), vertices[2]);
		return result;
	}
};

//ばね
struct Spring
{
	Vector3 anchor;      // アンカーの固定された端の位置
	float naturalLength; // 自然長
	float stiffness;     // 剛性。バネ定数k
	float dampingCoefficient; // 減衰係数
};

// ボール
struct Ball {
	Vector3 position;		//位置
	Vector3 veloctiy;		//速度
	Vector3 acceleration;	//加速度
	float mass;				//質量
	float radius;			//半径
	unsigned int color;		//色
};

// 振り子
struct Pendulum {
	Vector3 anchor;				// アンカーポイント
	float length;				// 紐の長さ
	float angle;				// 現在の角度
	float angularVelocity;		// 角度ω
	float angularAcceleration;	// 角加速度
};

// 円錐振り子
struct ConicalPendulum {
	Vector3 anchor;				// アンカーポイント
	float length;				// 紐の長さ
	float halfApexAngle;		// 円錐の頂角の半分
	float angle;				// 現在の角度
	float angularVelocity;		// 角度ω

};

// カプセル
struct Capsule
{
	Segment segment;
	float radius;


	// コンストラクタ
	Capsule(const Vector3& p0, const Vector3& p1, float r) : segment(p0, p1), radius(r) {}

	// カプセルの AABB を取得
	AABB computeAABB() const {
		Vector3 minPoint = Min(segment.origin, segment.end) - Vector3(radius, radius, radius);
		Vector3 maxPoint = Max(segment.origin, segment.end) + Vector3(radius, radius, radius);
		return AABB(minPoint, maxPoint);
	}

	// 最近接点
	Vector3 ClosestPoint(const Vector3& point) const {
		return segment.ClosestPoint(point);
	}

	// 点との交差判定
	bool Contains(const Vector3& point) const {
		Vector3 closest = segment.ClosestPoint(point);
		return (point - closest).LengthSq() <= radius * radius;
	}

	// 始点球
	Sphere GetStartSphere() const { return Sphere{ segment.origin, radius }; }
	// 終点球
	Sphere GetEndSphere()   const { return Sphere{ segment.end, radius }; }
};
// OBB
struct OBB {
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size;
	// OBB生成
	static Matrix4x4 MakeOBBMatrix(const OBB& obb) {
		Matrix4x4 m{};

		// 各軸方向を行ベクトルとして格納（左手座標系）
		m.m[0][0] = obb.orientations[0].x;
		m.m[0][1] = obb.orientations[1].x;
		m.m[0][2] = obb.orientations[2].x;
		m.m[1][0] = obb.orientations[0].y;
		m.m[1][1] = obb.orientations[1].y;
		m.m[1][2] = obb.orientations[2].y;
		m.m[2][0] = obb.orientations[0].z;
		m.m[2][1] = obb.orientations[1].z;
		m.m[2][2] = obb.orientations[2].z;

		// 平行移動
		m.m[3][0] = obb.center.x;
		m.m[3][1] = obb.center.y;
		m.m[3][2] = obb.center.z;
		m.m[3][3] = 1.0f;

		return m;
	}
	// 各角
	std::array<Vector3, 8> GetCorners() const {
		Vector3 axes[3] = {
			orientations[0] * size.x * 0.5f,
			orientations[1] * size.y * 0.5f,
			orientations[2] * size.z * 0.5f
		};

		std::array<Vector3, 8> corners;
		int i = 0;
		for (int x = -1; x <= 1; x += 2)
			for (int y = -1; y <= 1; y += 2)
				for (int z = -1; z <= 1; z += 2)
					corners[i++] = center + axes[0] * float(x) + axes[1] * float(y) + axes[2] * float(z);

		return corners;
	}
	// 点との包含チェック
	bool Contains(const Vector3& point) const {
		Vector3 local = point - center;
		for (int i = 0; i < 3; ++i) {
			float proj = local.Dot(orientations[i]);
			if (fabsf(proj) > size[i] * 0.5f) return false;
		}
		return true;
	}

	// 右
	Vector3 Right() const { return orientations[0]; }
	// 上
	Vector3 Up()    const { return orientations[1]; }
	// 前
	Vector3 Forward() const { return orientations[2]; }
};

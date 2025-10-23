#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include"DirectXGame/engine/collider/CollisionTypeIdDef.h"
#include "ColliderFanction3D.h"

constexpr float kFloatMax = 3.4028235e+38f;

struct SATResult {
	bool hit;
	float minOverlap = FLT_MAX; // 初期化は最大値
	Vector3 pushDir; // 押し出し軸（world space）
};
// コライダー基底クラス
class Collider
{
public:
	void* owner = nullptr; // 通知先ポインタ
	bool enabled = true;
	bool isStatic = false;  // 動かさない
	bool isDebugLine = false;// ライン描画するか	
	Vector3 centerWorld;
	CollisionLayer layer = CollisionLayer::Default;
	CollisionTag tag = CollisionTag::None; // タグ
	uint32_t collisionMask = 0xFFFFFFFF; // ビットで衝突対象を指定（全部と当たる）
	uint32_t id = 0; // コライダーID（ユニーク）

	// 判定有効
	void Enable() { enabled = true; }
	// 判定無効
	void Disable() { enabled = false; }
	// 判定効力取得
	bool IsEnabled() const { return enabled; }
	// 更新
	virtual void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) = 0;	
	// 判定
	virtual bool CheckHit(const Collider& other) const = 0;									
	// 押し戻し
	virtual bool ResolveCollision(const Collider& other, Vector3& outPushVec) const = 0;	
	// コライダータイプ取得
	virtual ColliderType GetType() const = 0;
	virtual ~Collider() = default;

	// 追加: このコライダーのワールド空間AABBを返す（Broad Phase用）
	virtual AABB GetAABB() const {
		// 基底は点AABB（派生でオーバーライド推奨）
		return AABB{ centerWorld, centerWorld };
	}
};

// 球コライダークラス
class SphereCollider : public Collider
{
public:
	float radius = 1.0f;

	void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) override;
	bool CheckHit(const Collider& other) const override;
	bool ResolveCollision(const Collider& other, Vector3& outPushVec) const override;
	ColliderType GetType() const override {
		return ColliderType::Sphere;
	}

	AABB GetAABB() const override {
		Vector3 r{ radius, radius, radius };
		return AABB{ centerWorld - r, centerWorld + r };
	}

};

// AABBコライダークラス
class AABBCollider : public Collider
{
public:
	AABB aabb{ {-0.5f,-0.5f,-0.5f} ,{0.5f,0.5f,0.5f}};
	Vector3 minWorld;
	Vector3 maxWorld;

	void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) override;
	bool CheckHit(const Collider& other) const override;
	bool ResolveCollision(const Collider& other, Vector3& outPushVec) const override;

	ColliderType GetType() const override {
		return ColliderType::AABB;
	}

	AABB GetAABB() const override {
		return AABB{ minWorld, maxWorld };
	}
};

// カプセルコライダークラス
class CapsuleCollider : public Collider
{
public:
	Capsule capsule{ Vector3{0.0f,1.0f,0.0f},Vector3{0.0f,-1.0f,0.0f},{1.0f}};
	Capsule capWorld_;


	void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) override;
	bool CheckHit(const Collider& other) const override;
	bool ResolveCollision(const Collider& other, Vector3& outPushVec) const override;
	ColliderType GetType() const override {
		return ColliderType::Capsule;
	}

	AABB GetAABB() const override {
		// Capsule 型に computeAABB() がある前提
		return capWorld_.computeAABB();
	}
};

// OBBコライダークラス
class OBBCollider : public Collider
{
public:
	OBB obb{ {0,0,0},{0,0,0},{0.5f,0.5f,0.5f}};


	void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) override;
	bool CheckHit(const Collider& other) const override;
	bool ResolveCollision(const Collider& other, Vector3& outPushVec) const override;
	ColliderType GetType() const override {
		return ColliderType::OBB;
	}
	
	AABB GetAABB() const override {
		// OBB の 8 頂点から AABB を作る
		Vector3 mins{ FLT_MAX, FLT_MAX, FLT_MAX };
		Vector3 maxs{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
		Vector3 c = obb.center;
		Vector3 u0 = obb.orientations[0] * (obb.size[0] * 0.5f);
		Vector3 u1 = obb.orientations[1] * (obb.size[1] * 0.5f);
		Vector3 u2 = obb.orientations[2] * (obb.size[2] * 0.5f);
		for (int i = 0; i < 8; ++i) {
			Vector3 corner = c;
			corner += (i & 1) ? u0 : -u0;
			corner += (i & 2) ? u1 : -u1;
			corner += (i & 4) ? u2 : -u2;
			mins = Min(mins, corner);
			maxs = Max(maxs, corner);
		}
		return AABB{ mins, maxs };
	}
private:

};


// Rayコライダークラス
class RayCollider : public Collider
{
public:
	Ray ray_;

	void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) override;
	bool CheckHit(const Collider& other) const override;
	bool ResolveCollision(const Collider& other, Vector3& outPushVec) const override;
	ColliderType GetType() const override {
		return ColliderType::Ray;
	}

	AABB GetAABB() const override {
		// Ray が origin, direction, length を持っている前提
		Vector3 p0 = ray_.origin;
		Vector3 p1 = ray_.origin + ray_.Direction() * ray_.diff; // length が無ければ direction を想定
		Vector3 mins = Min(p0, p1);
		Vector3 maxs = Max(p0, p1);
		return AABB{ mins, maxs };
	}

private:

};


static SATResult CheckOBBCollisionSAT(const OBB& obb0, const OBB& obb1)
{
	SATResult result;
	const Vector3* A = obb0.orientations;
	const Vector3* B = obb1.orientations;

	Vector3 T = obb1.center - obb0.center;

	float R[3][3], AbsR[3][3];
	constexpr float EPSILON = 1e-6f;

	// 回転行列の生成
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j) {
			R[i][j] = A[i].Dot(B[j]);
			AbsR[i][j] = std::abs(R[i][j]) + EPSILON;
		}

	Vector3 tLocal{
		T.Dot(A[0]),
		T.Dot(A[1]),
		T.Dot(A[2])
	};

	float ra, rb;// , overlap;
	//Vector3 axis;

	auto testAxis = [&](const Vector3& testAxis, float ra, float rb, float tProj) {
		float overlap = ra + rb - std::abs(tProj);
		if (overlap < 0.0f) {
			result.hit = false;
			return false;
		}
		if (overlap < result.minOverlap) {
			result.minOverlap = overlap;
			result.pushDir = testAxis;
		}
		return true;
		};

	result.hit = true;

	// 各軸のテスト（A[0~2], B[0~2], A×Bの9軸）
	for (int i = 0; i < 3; ++i) {
		ra = obb0.size[i] * 0.5f;
		rb = obb1.size[0] * AbsR[i][0] + obb1.size[1] * AbsR[i][1] + obb1.size[2] * AbsR[i][2];
		if (!testAxis(A[i], ra, rb, tLocal[i])) return result;
	}

	for (int i = 0; i < 3; ++i) {
		ra = obb0.size[0] * AbsR[0][i] + obb0.size[1] * AbsR[1][i] + obb0.size[2] * AbsR[2][i];
		rb = obb1.size[i] * 0.5f;
		if (!testAxis(B[i], ra, rb, T.Dot(B[i]))) return result;
	}

	// 交差軸は省略（まずはここまでで試す）

	// pushDir の向き補正
	if (T.Dot(result.pushDir) < 0.0f)
		result.pushDir = -result.pushDir;

	return result;
}




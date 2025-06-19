#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include"DirectXGame/engine/collider/CollisionTypeIdDef.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include <DirectXGame/engine/Collider/CollisionTypeIdDef.h>


// コライダー基底クラス
class Collider
{
public:
	void* owner = nullptr; // 通知先ポインタ
	bool enabled = true;
	bool isStatic = false;  // 動かさない
	Vector3 centerWorld;
	CollisionLayer layer = CollisionLayer::Default;
	CollisionTag tag = CollisionTag::None; // タグ
	uint32_t collisionMask = 0xFFFFFFFF; // ビットで衝突対象を指定（全部と当たる）

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

};

// カプセルコライダークラス
class CapsuleCollider : public Collider
{
public:
	Capsule capsule{ Vector3{0.0f,1.0f,0.0f},Vector3{0.0f,-1.0f,0.0f},{1.0f}};

	void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) override;
	bool CheckHit(const Collider& other) const override;
	bool ResolveCollision(const Collider& other, Vector3& outPushVec) const override;
	ColliderType GetType() const override {
		return ColliderType::Capsule;
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
};





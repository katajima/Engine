#pragma once

#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include"DirectXGame/engine/collider/CollisionTypeIdDef.h"
#include "ColliderFanction2D.h"
#include"DirectXGame/engine/Transform/WorldTransform/WorldTransform2d.h"

// コライダー基底クラス
class Collider2D
{
public:
	void* owner = nullptr; // 通知先ポインタ
	bool enabled = true;
	bool isStatic = false;  // 動かさない
	Vector2 centerWorld;
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
	virtual void Update(const WorldTransform2d& worldTransform) = 0;
	// 判定
	virtual bool CheckHit(const Collider2D& other) const = 0;
	// 押し戻し
	virtual bool ResolveCollision(const Collider2D& other, Vector2& outPushVec) const = 0;
	// コライダータイプ取得
	virtual ColliderType GetType() const = 0;
	virtual ~Collider2D() = default;
};

// 球コライダークラス
class SphereCollider2D : public Collider2D
{
public:
	float radius = 1.0f;

	void Update(const WorldTransform2d& worldTransform) override;
	bool CheckHit(const Collider2D& other) const override;
	bool ResolveCollision(const Collider2D& other, Vector2& outPushVec) const override;
	ColliderType GetType() const override {
		return ColliderType::Sphere;
	}

};

// AABBコライダークラス
class AABBCollider2D : public Collider2D
{
public:
	Box box{ {-0.5f,-0.5f} ,{0.5f,0.5f} };
	Vector2 minWorld;
	Vector2 maxWorld;

	void Update(const WorldTransform2d& worldTransform) override;
	bool CheckHit(const Collider2D& other) const override;
	bool ResolveCollision(const Collider2D& other, Vector2& outPushVec) const override;

	ColliderType GetType() const override {
		return ColliderType::AABB;
	}

};





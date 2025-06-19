#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include"DirectXGame/engine/collider/CollisionTypeIdDef.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include <DirectXGame/engine/Collider/CollisionTypeIdDef.h>



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


	void Enable() { enabled = true; }
	void Disable() { enabled = false; }
	bool IsEnabled() const { return enabled; }

	virtual void Update(const WorldTransform& worldTransform, LineCommon* lineCommon) = 0;
	virtual bool CheckHit(const Collider& other) const = 0;
	virtual bool ResolveCollision(const Collider& other, Vector3& outPushVec) const = 0;
	virtual ColliderType GetType() const = 0;
	virtual ~Collider() = default;
};


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


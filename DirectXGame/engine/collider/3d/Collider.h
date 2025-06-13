#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include"DirectXGame/engine/collider/CollisionTypeIdDef.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"

class Collider {
public:

	// 初期化
	void Initialize(Camera* camera );

	// ワールドトランスフォームの初期化
	void UpdateWorldTransform(LineCommon* lineCommon);
	// 描画
	void Draw();

	



	// 衝突時に呼ばれる関数
	virtual void OnCollision([[maybe_unused]] Collider* other){};
	
	virtual void NoCollision([[maybe_unused]] Collider* other){};

	virtual Vector3 GetCenterPosition() const = 0;

	virtual ~Collider() = default; 

	// 種別IDの取得
	uint32_t GetTypeID() const { return typeID_; };
	// 種別IDの設定
	void SetTypeID(uint32_t typeID);
	
	// コライダーID取得
	uint32_t GetColliderType() const { return colliderTypeID_; }

	// コライダーID設定
	void SetColliderType(uint32_t collType) { colliderTypeID_ = collType; }


	// 半径取得
	float GetRadius() const { return radius_; }
	// 半径設定
	void SetRadius(float radius) { radius_ = radius; }
	// カプセル
	Capsule GetCapsule() const { return capsule_; }
	// カプセル
	void SetCapsule(Capsule& capsule) { capsule_ = capsule; }
	// AABB
	AABB GetAABB() const { return aabb_; }
	// OBB
	OBB GetOBB() const { return obb_; }

	// 色
	void SetColor(Vector4 color) { color_ = color; }

private:
	// 衝突判定
	float radius_ = 1.5f; // 半径

	// AABB
	AABB aabb_ = {{ -1.0f, -1.0f, -1.0f}, {1.0f,1.0f,1.0f}};

	// OBB
	OBB obb_ = {};

	// Capsule
	Capsule capsule_ = Capsule(Vector3{},Vector3{},1.5f);

	// 色
	Vector4 color_ = { 1,1,1,1 };

	// 種別ID
	uint32_t typeID_ = 0u;

	// コライダー形状
	uint32_t colliderTypeID_ = 0u;
};


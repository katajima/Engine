#pragma once
#include "HitBoxFunction.h"

class Entity3DManager; // 前方宣言
class BaseCharacter;


enum class HitBoxShape {
	kOBB,
	kAABB,
	kSphere,
};

// コライダーデータ
struct HitBoxCollData
{
	std::string name = "";
	CollisionTag tag;
	CollisionLayer layer;
	CollisionLayer mask;
	bool isEneble = true;
	bool isLine = false;
	HitBoxShape shape = HitBoxShape::kOBB;
	Vector3 offset{};
	Vector3 size = { 1.0f,1.0f,1.0f };
	float radius = 1.0f;



};

/// <summary>
/// ヒットボックスクラス
/// </summary>
class HitBox : public IHitReceiver {
public:
	// 初期化
	void Initialize(Entity3DManager* entity3dManager, BaseCharacter* character, HitBoxUseType type);

	// 更新
	void Update(float dt);

public:

	// コライダー追加
	void AddCollider(std::unique_ptr<Collider> collider, const Vector3& offset);
	// 有効化
	void Enable();
	// 無効化
	void Disable();
	// 履歴削除
	void Clear() { GetContactRecord().Clear(); }
public:
	// ワールドトランスフォーム取得
	WorldTransform& GetWorldTransform() { return worldTransform_; }
	// コンポーネント取得
	ColliderComponent* GetColliderComponent() { return colliderComponent_.get(); }

private:
	// 衝突履歴取得
	ContactRecord& GetContactRecord() { return colliderComponent_->contactRecord_; }
private:
	// コライダーコンポーネント
	std::unique_ptr<ColliderComponent> colliderComponent_;
	// ヒットボックス衝突応答クラス
	std::unique_ptr<HitBoxFunction> hitBoxFunction_;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// 使用者タイプ
	HitBoxUseType type_;

	// コライダーデータ構造体
	struct ColliderData {
		WorldTransform worldTransform;
		Collider* collider = nullptr;
		int colliderID = -1;
	};

	// コライダーのワールドトランスフォームマップ
	std::map<int, ColliderData> colliders_;
private:
	// コライダー数
	int colliderCount = 0;
	BaseCharacter* character_ = nullptr;
	Entity3DManager* entity3dManager_ = nullptr;
};


template <typename T>
static std::unique_ptr<T> CreateCollider(CollisionTag tag, CollisionLayer layer, CollisionLayer mask, bool isEneble = true, bool isLine = false)
{
	std::unique_ptr<T> coll = std::make_unique<T>();

	// 有効化
	if (isEneble) {
		coll->Enable();
	}
	else {
		coll->Disable();
	}
	// デバック用表示
	if (isLine) {
		coll->isDebugLine = true;
	}

	coll->tag = tag;			// タグ設定
	coll->layer = layer;		// レイヤー設定
	coll->collisionMask = (1 << static_cast<uint32_t>(mask));	// マスク設定

	return std::move(coll);
}

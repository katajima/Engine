#pragma once
#include "HitBoxFunction.h"



namespace Engine {
	class EntityManager; // 前方宣言
}
namespace Character {
	class BaseCharacter;
}

namespace HitBox {

	/// <summary>
	/// ヒットボックスクラス
	/// </summary>
	class HitBoxInstance : public IHitReceiver {
	public:
		// 初期化
		void Initialize(Engine::EntityManager* entityManager, Character::BaseCharacter* character, UseType type);

		// 更新
		void Update(float dt);

	public:

		// コライダー追加
		void AddCollider(std::unique_ptr<Engine::Collider> collider, const Vector3& offset, const AttackReactionData& reaction);
		// 有効化
		void Enable();
		// 無効化
		void Disable();
		// 履歴削除
		void Clear() { GetContactRecord().Clear(); }
	public:
		// ワールドトランスフォーム取得
		Engine::WorldTransform& GetWorldTransform() { return worldTransform_; }
		// コンポーネント取得
		Engine::ColliderComponent* GetColliderComponent() { return colliderComponent_.get(); }
	private:
		// 衝突履歴取得
		Engine::ContactRecord& GetContactRecord() { return colliderComponent_->contactRecord_; }
	private:
		// コライダーコンポーネント
		std::unique_ptr<Engine::ColliderComponent> colliderComponent_;
		// ヒットボックス衝突応答クラス
		std::unique_ptr<HitBoxFunction> hitBoxFunction_;

		// ワールドトランスフォーム
		Engine::WorldTransform worldTransform_;
		// 使用者タイプ
		UseType type_;

		// コライダーデータ構造体
		struct ColliderData {
			Engine::WorldTransform worldTransform;
			Engine::Collider* collider = nullptr;

			float damage = 0.0f;
			AttackReactionData reactionData;

			int colliderID = -1;
		};

		// コライダーのワールドトランスフォームマップ
		std::map<int, ColliderData> colliders_;
	private:
		// コライダー数
		int colliderCount = 0;
		Character::BaseCharacter* character = nullptr;
		Engine::EntityManager* entityManager = nullptr;
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
};
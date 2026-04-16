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
		~HitBoxInstance() {
			colliders_.clear();
		}

		// 初期化
		void Initialize(Engine::EntityManager* entityManager, Character::BaseCharacter* character, UseType type, bool useContactRecord);

		// 更新
		void Update(float dt);

	public:

		// コライダー追加
		void AddCollider(std::unique_ptr<Engine::Collider> collider, const Vector3& offset, const HitReactionData& reaction);
		// 有効化
		void Enable(Vector4 color = {1,1,1,1});
		// 無効化
		void Disable(Vector4 color = { 1,1,1,1 });
		// 履歴削除
		void ClearContactRecord() { GetContactRecord().Clear(); }
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
		// ヒット記録を使用するか（使用した場合連続ヒットしない）
		bool useContactRecord = true;


		// コライダーデータ構造体
		struct ColliderData {
			Engine::WorldTransform worldTransform;
			Engine::Collider* collider = nullptr;

			float damage = 0.0f;
			HitReactionData reactionData;

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
};
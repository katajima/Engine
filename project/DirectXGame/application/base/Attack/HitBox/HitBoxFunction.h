#pragma once
#include "HitBoxData.h"
#include <DirectXGame/engine/Collider/3d/ColliderComponent.h>

namespace Character {
	class BaseCharacter;
}
namespace HitBox {

/// <summary>
/// HitBoxFunctionを管理・実装するクラス。
/// </summary>
	class HitBoxFunction {
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Engine::ColliderComponent* owner, Character::BaseCharacter* character, UseType type) {
			this->owner = owner;
			this->character = character;
			type_ = type;
		};

		/// <summary>
		/// 開始
		/// </summary>
		bool Begin(Engine::Collider* self, Engine::Collider* other, bool useContactRecord);

		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		void SetAttackInstanceId(uint32_t id) { attackInstanceId_ = id; }
		void SetRecordPerCollider(bool enabled) { recordPerCollider_ = enabled; }

		void SetData(HitReactionData data) {
			data_ = data; 
		};

	private:
		/// <summary>
		/// 受け手の防御状態や属性耐性を反映した最終ヒット結果を作る。
		/// </summary>
		HitResult BuildHitResult() const;
		/// <summary>
		/// 使用者がプレイヤーの場合の更新処理
		/// </summary>
		void UpdateTypePlayer();
		/// <summary>
		/// 使用者が敵の場合の更新処理
		/// </summary>
		void UpdateTypeEnemy();
		/// <summary>
		/// 使用者がキャラクター以外の場合の更新処理
		/// </summary>
		void UpdateTypeOther();
	private:
		/// <summary>
		/// 衝突履歴取得
		/// </summary>
		Engine::ContactRecord& GetContactRecord() { return owner->contactRecord_; }
	private:
		Engine::ColliderComponent* owner = nullptr;	// コライダーコンポーネント(使用者)
		Engine::ColliderComponent* other = nullptr;	// コライダーコンポーネント(相手)
		Character::BaseCharacter* character = nullptr;	// 使用キャラクター
		UseType type_;							// 使用者

		Engine::Collider* selfColl = nullptr;	// ヒットした自分側コライダー
		Engine::Collider* otherColl = nullptr;
		HitReactionData data_;

		bool hasAppliedSelfHitStop_ = false;	// FirstHitOnly用
		uint32_t attackInstanceId_ = 0;
		bool recordPerCollider_ = true;
	};
};

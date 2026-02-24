#pragma once
#include "HitBoxData.h"

namespace Character {
	class BaseCharacter;
}
namespace HitBox {

	class HitBoxFunction {
	public:

		// 初期化
		void Initialize(Engine::ColliderComponent* owner, Character::BaseCharacter* character, UseType type) {
			this->owner = owner;
			this->character = character;
			type_ = type;
		};

		// 開始
		bool Begin(Engine::Collider* self, Engine::Collider* other);

		// 更新
		void Update();


		void SetData(AttackReactionData data) { data_ = data; };

	private:
		// 使用者がプレイヤーの場合の更新処理
		void UpdateTypePlayer();
		// 使用者が敵の場合の更新処理
		void UpdateTypeEnemy();
		// 使用者がキャラクター以外の場合の更新処理
		void UpdateTypeOther();
	private:
		// 衝突履歴取得
		Engine::ContactRecord& GetContactRecord() { return owner->contactRecord_; }
	private:
		Engine::ColliderComponent* owner = nullptr;	// コライダーコンポーネント(使用者)
		Engine::ColliderComponent* other = nullptr;	// コライダーコンポーネント(相手)
		Character::BaseCharacter* character = nullptr;	// 使用キャラクター
		UseType type_;							// 使用者

		Engine::Collider* otherColl = nullptr;
		AttackReactionData data_;
	};
};

#include "HitBoxFunction.h"
#include <DirectXGame/engine/MyGame/MyGame.h>
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"


namespace HitBox {

	bool HitBoxFunction::Begin(Engine::Collider* self, Engine::Collider* otherColl) {
		other = static_cast<Engine::ColliderComponent*>(otherColl->owner);
		this->otherColl = otherColl;
		if (!other) return false;

		const uint32_t otherId = other->GetUniqueId();	// ID取得
		const float nowTime = Engine::MyGame::NowTime();		// 現在時間
		if (GetContactRecord().CheckHistory(otherId)) {
			return false; // クールタイム中のため無視
		}
		// 履歴追加
		GetContactRecord().AddHistory(otherId, nowTime);

		return true;
	}


	void HitBoxFunction::Update() {

		if (type_ == UseType::kPlayer) {
			UpdateTypePlayer();
		}
		else if (type_ == UseType::kEnemy) {
			UpdateTypeEnemy();
		}
		else {
			UpdateTypeOther();
		}
	}

	void HitBoxFunction::UpdateTypePlayer() {

		if (otherColl->tag != CollisionTag::Enemy) return;

		Character::BaseEnemy* enemy = static_cast<Character::BaseEnemy*>(other->GetHitReceiver());
		if (!enemy) return;

		Character::BasePlayer* player = static_cast<Character::BasePlayer*>(character);
		if (!player) return;


		// ノックバック方向
		data_.GetKnockbackData().SetNormal(player->GetMoveComponent()->GetDirection());

		// リアクションデータ
		enemy->GetResponseSystem()->GetHitMotionSystem()->SetReactionData(data_);

		enemy->Emit();	//	エフェクト出現
		enemy->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Move); // 敵ステート設定
		
		// プレイヤーのロックオンシステムに相手タグを設定
		player->GetAttackController()->GeyLockOnSysutem()->SetHitTag(enemy->GetTagNumber());
		// ヒットカウンターにヒットを通知
		player->GetAttackController()->GetHitCounter().Hit();
	}

	void HitBoxFunction::UpdateTypeEnemy() {
		if (otherColl->tag != CollisionTag::Player) return;

		Character::BaseEnemy* enemy = static_cast<Character::BaseEnemy*>(character);
		if (!enemy) return;

		Character::BasePlayer* player = static_cast<Character::BasePlayer*>(other->GetHitReceiver());
		if (!player) return;
		player->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Damage); // 敵ステート設定


		// ノックバック方向
		data_.GetKnockbackData().SetNormal(enemy->GetMoveComponent()->GetDirection());

		// リアクションデータ
		player->GetResponseSystem()->GetHitMotionSystem()->SetReactionData(data_);
	}

	void HitBoxFunction::UpdateTypeOther() {
	}

}

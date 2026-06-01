#include "HitBoxFunction.h"
#include <DirectXGame/engine/MyGame/MyGame.h>
#include"DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include <DirectXGame/application/base/Attack/AttackController.h>
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"

namespace HitBox {

	bool HitBoxFunction::Begin(Engine::Collider* self, Engine::Collider* otherColl) {
		other = static_cast<Engine::ColliderComponent*>(otherColl->GetOwner());
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
		if (otherColl->GetTag() != CollisionTag::Enemy) return;
		// 敵
		Character::BaseEnemy* enemy = static_cast<Character::BaseEnemy*>(other->GetHitReceiver());
		if (!enemy) return;
		// プレイヤー
		Character::BasePlayer* player = static_cast<Character::BasePlayer*>(character);
		if (!player) return;
		// リアクションデータ
		enemy->GetHitMotionSystem()->SetReactionData(data_);

		if (data_.isSingleHitStop) {
			isHitStop = false; // 一回しかヒットストップしない場合は無効化
			player->GetHitMotionSystem()->SetSelfHitStopTime(data_.selfHitStopTime);
		}
		//	エフェクト出現
		enemy->GetHitMotionSystem()->EmitHitEffect();
		// 敵ステート設定
		enemy->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Damage);
		// プレイヤーのロックオンシステムに相手タグを設定
		player->GetAttackController()->GeyLockOnSysutem()->SetHitTag(enemy->GetTagNumber());
		// ヒットカウンターにヒットを通知
		player->GetAttackController()->GetHitCounter().Hit();
		player->GetAttackController()->GetComboSystem()->NotifyAttackHit();
	}

	void HitBoxFunction::UpdateTypeEnemy() {
		if (otherColl->GetTag() != CollisionTag::Player) return;
		// 敵
		Character::BaseEnemy* enemy = static_cast<Character::BaseEnemy*>(character);
		if (!enemy) return;
		// プレイヤー
		Character::BasePlayer* player = static_cast<Character::BasePlayer*>(other->GetHitReceiver());
		if (!player) return;
		if(player->GetCurrentMainState() == Character::CharacterMainState::Special){
			return; // プレイヤーが必殺技中は無効
		}

		if (data_.isSingleHitStop) {
			isHitStop = false; // 一回しかヒットストップしない場合は無効化
			enemy->GetHitMotionSystem()->SetSelfHitStopTime(data_.selfHitStopTime);
		}

		// リアクションデータ
		player->GetHitMotionSystem()->SetReactionData(data_);
		// プレイヤーステート設定
		player->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Damage);
		// プレイヤーのロックオンシステムに相手タグを設定
		enemy->GetAttackController()->GeyLockOnSysutem()->SetHitTag(player->GetTagNumber());
	}

	void HitBoxFunction::UpdateTypeOther() {}

}

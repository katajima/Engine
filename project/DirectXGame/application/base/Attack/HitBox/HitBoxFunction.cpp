#include "HitBoxFunction.h"
#include <DirectXGame/engine/MyGame/MyGame.h>
#include"DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include <DirectXGame/application/base/Attack/AttackController.h>
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"

namespace HitBox {

	bool HitBoxFunction::Begin(Engine::Collider* self, Engine::Collider* otherColl, bool useContactRecord) {
		other = static_cast<Engine::ColliderComponent*>(otherColl->GetOwner());
		this->selfColl = self;
		this->otherColl = otherColl;
		if (!other) return false;
		if (!useContactRecord) {
			return true;
		}

		// 同一攻撃内のヒット履歴は「攻撃インスタンス + 自分のコライダー + 相手」で分ける。
		// 多段攻撃や複数コライダー攻撃で、意図せず別判定まで潰れないようにするため。
		const uint32_t selfId = recordPerCollider_ ? self->GetId() : 0;
		const uint32_t otherId = other->GetUniqueId();
		const uint32_t contactKey =
			(attackInstanceId_ * 73856093u) ^
			(selfId * 19349663u) ^
			(otherId * 83492791u);
		const float nowTime = Engine::MyGame::NowTime();		// 現在時間
		if (GetContactRecord().CheckHistory(contactKey)) {
			return false; // クールタイム中のため無視
		}
		// 履歴追加
		GetContactRecord().AddHistory(contactKey, nowTime);

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

	HitResult HitBoxFunction::BuildHitResult() const {
		HitResult result{};
		result.reaction = data_;

		// 吸い付きはヒットした瞬間の「相手位置 -> ヒットボックス中心」を移動方向にする。
		if (result.reaction.type == HitReactionType::Suction && selfColl && otherColl) {
			result.reaction.normal = selfColl->GetCenterWorld() - otherColl->GetCenterWorld();
		}

		// 今後、ここでガード、無敵、属性耐性、ジャスト回避などを吸収する。
		// 既存挙動維持のため、現時点では必殺技中のプレイヤーだけ無効化する。
		if (type_ == UseType::kEnemy && otherColl && otherColl->GetTag() == CollisionTag::Player) {
			Character::BasePlayer* player = other ? static_cast<Character::BasePlayer*>(other->GetHitReceiver()) : nullptr;
			if (player && player->GetCurrentMainState() == Character::CharacterMainState::Special) {
				result.accepted = false;
				result.applyDamage = false;
				result.applyReaction = false;
				result.applySelfHitStop = false;
				result.notifyComboHit = false;
			}
		}

		return result;
	}

	void HitBoxFunction::UpdateTypePlayer() {
		if (otherColl->GetTag() != CollisionTag::Enemy) return;
		HitResult result = BuildHitResult();
		if (!result.accepted) return;
		// 敵
		Character::BaseEnemy* enemy = static_cast<Character::BaseEnemy*>(other->GetHitReceiver());
		if (!enemy) return;
		// プレイヤー
		Character::BasePlayer* player = static_cast<Character::BasePlayer*>(character);
		if (!player) return;
		// リアクションデータ
		if (result.applyReaction) {
			enemy->GetHitMotionSystem()->SetReactionData(result.reaction);
		}

		const bool shouldSelfHitStop =
			data_.selfHitStopPolicy == SelfHitStopPolicy::EveryHit ||
			(data_.selfHitStopPolicy == SelfHitStopPolicy::FirstHitOnly && !hasAppliedSelfHitStop_);
		if (result.applySelfHitStop && shouldSelfHitStop) {
			player->GetHitMotionSystem()->SetSelfHitStopTime(data_.selfHitStopTime);
			hasAppliedSelfHitStop_ = true;
		}
		//	エフェクト出現
		enemy->GetHitMotionSystem()->EmitHitEffect();
		// 敵ステート設定
		enemy->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Damage);
		// プレイヤーのロックオンシステムに相手タグを設定
		player->GetAttackController()->GeyLockOnSysutem()->SetHitTag(enemy->GetTagNumber());
		// ヒットカウンターにヒットを通知
		player->GetAttackController()->GetHitCounter().Hit();
		if (result.notifyComboHit) {
			player->GetAttackController()->GetComboSystem()->NotifyAttackHit();
		}
	}

	void HitBoxFunction::UpdateTypeEnemy() {
		if (otherColl->GetTag() != CollisionTag::Player) return;
		HitResult result = BuildHitResult();
		if (!result.accepted) return;
		// 敵
		Character::BaseEnemy* enemy = static_cast<Character::BaseEnemy*>(character);
		if (!enemy) return;
		// プレイヤー
		Character::BasePlayer* player = static_cast<Character::BasePlayer*>(other->GetHitReceiver());
		if (!player) return;

		const bool shouldSelfHitStop =
			data_.selfHitStopPolicy == SelfHitStopPolicy::EveryHit ||
			(data_.selfHitStopPolicy == SelfHitStopPolicy::FirstHitOnly && !hasAppliedSelfHitStop_);
		if (result.applySelfHitStop && shouldSelfHitStop) {
			enemy->GetHitMotionSystem()->SetSelfHitStopTime(data_.selfHitStopTime);
			hasAppliedSelfHitStop_ = true;
		}

		// リアクションデータ
		if (result.applyReaction) {
			player->GetHitMotionSystem()->SetReactionData(result.reaction);
		}
		// プレイヤーステート設定
		player->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Damage);
		// プレイヤーのロックオンシステムに相手タグを設定
		enemy->GetAttackController()->GeyLockOnSysutem()->SetHitTag(player->GetTagNumber());
	}

	void HitBoxFunction::UpdateTypeOther() {}

}

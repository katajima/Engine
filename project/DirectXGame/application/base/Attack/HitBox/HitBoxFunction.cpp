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

		// 今後、ここでガード、無敵、属性耐性などを吸収する。
		// 既存挙動維持のため、回避・必殺技・被弾後無敵中のプレイヤーだけ無効化する。
		if (type_ == UseType::kEnemy && otherColl && otherColl->GetTag() == CollisionTag::Player) {
			Character::BasePlayer* player = other ? static_cast<Character::BasePlayer*>(other->GetHitReceiver()) : nullptr;
			if (player && player->GetCurrentMainState() == Character::CharacterMainState::Avoidance) {
				player->OnDodgeSuccess();		// 回避成功後コンボの受付を開く
				result.accepted = false;
				result.applyDamage = false;
				result.applyReaction = false;
				result.applySelfHitStop = false;
				result.notifyComboHit = false;
			}
			else if (player && player->GetCurrentMainState() == Character::CharacterMainState::Special) {
				result.accepted = false;
				result.applyDamage = false;
				result.applyReaction = false;
				result.applySelfHitStop = false;
				result.notifyComboHit = false;
			}
			else if (player && player->IsDamageInvincible()) {
				// 被弾後無敵中は、敵攻撃のダメージとリアクションをまとめて無効化する。
				result.accepted = false;
				result.applyDamage = false;
				result.applyReaction = false;
				result.applySelfHitStop = false;
				result.notifyComboHit = false;
			}
		}

		// プレイヤーと敵の両方で、共通防御状態を最終ヒット結果へ反映する
		Character::BaseCharacter* targetCharacter = other ?
			dynamic_cast<Character::BaseCharacter*>(other->GetHitReceiver()) : nullptr;
		if (targetCharacter) {
			result.defenseResult = targetCharacter->ResolveActionDefense();
			switch (result.defenseResult) {
			case GameAction::DefenseResult::Invincible:
			case GameAction::DefenseResult::Parried:
			case GameAction::DefenseResult::Guarded:
				// 無敵、パリィ、ガードはダメージ・リアクション・コンボ命中を無効化する
				result.accepted = false;
				result.applyDamage = false;
				result.applyReaction = false;
				result.notifyComboHit = false;
				break;
			case GameAction::DefenseResult::SuperArmored:
				// スーパーアーマーはダメージだけを受け、被弾リアクションを抑制する
				result.applyReaction = false;
				break;
			default:
				break;
			}
		}

		return result;
	}

	void HitBoxFunction::UpdateTypePlayer() {
		if (otherColl->GetTag() != CollisionTag::Enemy) return;
		HitResult result = BuildHitResult();
		// 敵
		Character::BaseEnemy* enemy = static_cast<Character::BaseEnemy*>(other->GetHitReceiver());
		if (!enemy) return;
		// プレイヤー
		Character::BasePlayer* player = static_cast<Character::BasePlayer*>(character);
		if (!player) return;
		if (!result.accepted) {
			// 防御成立を受け手へ通知し、通常のヒット処理を行わない
			if (result.defenseResult == GameAction::DefenseResult::Parried ||
				result.defenseResult == GameAction::DefenseResult::Guarded) {
				const GameAction::EventType eventType = result.defenseResult == GameAction::DefenseResult::Parried ?
					GameAction::EventType::Parried : GameAction::EventType::Guarded;
				enemy->EmitActionEvent({ eventType, enemy, player });
			}
			return;
		}
		// リアクションデータ
		if (result.defenseResult == GameAction::DefenseResult::SuperArmored) {
			enemy->GetHitMotionSystem()->QueueDamageOnly(result.reaction);
		}
		else if (result.applyReaction) {
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
		// スーパーアーマー中は敵ステートを被弾へ変更しない
		if (result.applyReaction) {
			enemy->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Damage);
		}
		// プレイヤーのロックオンシステムに相手タグを設定
		player->GetAttackController()->GetLockOnSystem()->SetHitTag(enemy->GetTagNumber());
		// コンボ中はコンボ設定側で加算を判定し、コンボ外は従来どおり加算する。
		const bool handledByCombo = result.notifyComboHit &&
			player->GetAttackController()->GetComboSystem()->NotifyAttackHit();
		if (!handledByCombo) {
			player->GetAttackController()->GetHitCounter().Hit();
		}
	}

	void HitBoxFunction::UpdateTypeEnemy() {
		if (otherColl->GetTag() != CollisionTag::Player) return;
		HitResult result = BuildHitResult();
		// 敵
		Character::BaseEnemy* enemy = static_cast<Character::BaseEnemy*>(character);
		if (!enemy) return;
		// プレイヤー
		Character::BasePlayer* player = static_cast<Character::BasePlayer*>(other->GetHitReceiver());
		if (!player) return;
		if (!result.accepted) {
			// 防御成立を受け手へ通知し、通常のヒット処理を行わない
			if (result.defenseResult == GameAction::DefenseResult::Parried ||
				result.defenseResult == GameAction::DefenseResult::Guarded) {
				const GameAction::EventType eventType = result.defenseResult == GameAction::DefenseResult::Parried ?
					GameAction::EventType::Parried : GameAction::EventType::Guarded;
				player->EmitActionEvent({ eventType, player, enemy });
			}
			return;
		}

		const bool shouldSelfHitStop =
			data_.selfHitStopPolicy == SelfHitStopPolicy::EveryHit ||
			(data_.selfHitStopPolicy == SelfHitStopPolicy::FirstHitOnly && !hasAppliedSelfHitStop_);
		if (result.applySelfHitStop && shouldSelfHitStop) {
			enemy->GetHitMotionSystem()->SetSelfHitStopTime(data_.selfHitStopTime);
			hasAppliedSelfHitStop_ = true;
		}

		// スーパーアーマー中はダメージだけを適用し、被弾リアクションを生成しない
		if (result.defenseResult == GameAction::DefenseResult::SuperArmored) {
			player->GetHitMotionSystem()->QueueDamageOnly(result.reaction);
		}
		else if (result.applyReaction) {
			player->GetHitMotionSystem()->SetReactionData(result.reaction);
		}
		// スーパーアーマー中はプレイヤーステートを被弾へ変更しない
		if (result.applyReaction) {
			player->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Damage);
		}
		// プレイヤーのロックオンシステムに相手タグを設定
		enemy->GetAttackController()->GetLockOnSystem()->SetHitTag(player->GetTagNumber());
		// 敵側コンボにも命中を通知し、ヒット音と命中条件を同じタイミングで処理する。
		if (result.notifyComboHit) {
			enemy->GetAttackController()->GetComboSystem()->NotifyAttackHit();
		}
	}

	void HitBoxFunction::UpdateTypeOther() {}

}

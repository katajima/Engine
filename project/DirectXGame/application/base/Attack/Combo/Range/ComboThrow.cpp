#include "ComboThrow.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include <algorithm>

namespace Combo {

	void ComboThrow::Enter(Character::BaseCharacter* owner, const GlobalRange& data) {
		owner_ = owner;							// コンボ使用者
		throwCount_ = 0;						// 投擲数リセット
		nextThrowTime_ = data.rangeWindowStart;	// 次の投擲時間
		isEffectActive_ = false;				// 効果発動状態リセット
		isHit_ = false;							// ヒット状態リセット
		isRecallWaiting_ = false;				// 回収待機状態リセット
		stayTimer_ = 0.0f;						// 停滞時間リセット
	}

	void ComboThrow::Update(const Character::CharacterContext& ctx, float timer, const GlobalRange& data,
		const Vector3& aimDirection, const Vector3& aimTarget) {
		UpdateThrowSpawn(ctx, timer, data, aimDirection, aimTarget);
		UpdateEffectTrigger(ctx, timer, data);
		UpdateRecallTrigger(ctx, timer, data);
	}

	void ComboThrow::Exit(const GlobalRange& data) {
		// 回収条件が有効な投擲物はコンボ終了時にも回収へ移す
		if (data.recallTriggerType != RangeRecallTriggerType::kNone) {
			if (PlayerSubWeapon* subWeapon = GetPlayerSubWeapon()) {
				subWeapon->Recall();
			}
		}

		owner_ = nullptr;			// コンボ使用者を解除
		throwCount_ = 0;			// 投擲数リセット
		nextThrowTime_ = 0.0f;		// 次投擲時間リセット
		isEffectActive_ = false;		// 効果発動状態リセット
		isHit_ = false;				// ヒット状態リセット
		isRecallWaiting_ = false;	// 回収待機状態リセット
		stayTimer_ = 0.0f;			// 停滞時間リセット
	}

	void ComboThrow::NotifyHit(const GlobalRange& data) {
		isHit_ = true;	// ヒット状態を記録
		if (data.throwMoveType == RangeThrowMoveType::kStopOnHit) {
			if (PlayerSubWeapon* subWeapon = GetPlayerSubWeapon()) {
				subWeapon->StopAtCurrentPosition();
			}
		}
	}

	void ComboThrow::UpdateThrowSpawn(const Character::CharacterContext& ctx, float timer, const GlobalRange& data,
		const Vector3& aimDirection, const Vector3& aimTarget) {
		// 投擲受付時間外なら新しい投擲物を出さない
		if (throwCount_ >= data.count || timer < data.rangeWindowStart || timer > data.rangeWindowEnd) {
			return;
		}

		// 投擲間隔に従って複数投擲を処理する
		const float interval = (std::max)(data.interval, 0.001f);
		while (throwCount_ < data.count && timer >= nextThrowTime_ && nextThrowTime_ <= data.rangeWindowEnd) {
			PlayerSubWeapon* subWeapon = GetPlayerSubWeapon();
			if (!subWeapon || subWeapon->IsThrowing()) {
				break;
			}

			PlayerSubWeaponThrowData throwData{};						// サブ武器投擲データ
			throwData.idleOffset = data.subWeaponIdleOffset;				// 待機位置
			throwData.targetPosition = aimTarget;							// 移動目標位置
			throwData.throwSpeed = data.subWeaponThrowSpeed;				// 投擲速度
			throwData.throwLifeTime = data.subWeaponThrowLifeTime;			// 投擲時間
			throwData.returnTime = data.subWeaponReturnTime;				// 戻り時間
			throwData.lerpTime = data.throwLerpTime;						// 補間時間
			throwData.spinSpeed = data.subWeaponSpinSpeed;					// 回転速度
			throwData.autoReturn = false;									// 回収条件で戻す
			throwData.alignToDirection = data.subWeaponAlignToDirection;	// 投擲方向へ向けるか
			throwData.useSpin = data.subWeaponUseSpin;						// スピンするか
			throwData.moveType = ToSubWeaponMoveType(data);					// 移動タイプ
			throwData.rotateOffset = data.subWeaponRotateOffset;			// 回転オフセット
			subWeapon->SetThrowData(throwData);
			subWeapon->Throw(ctx.position + data.subWeaponStartOffset, aimDirection);

			throwCount_++;				// 投擲済み数
			nextThrowTime_ += interval;	// 次の投擲時間
		}
	}

	void ComboThrow::UpdateEffectTrigger(const Character::CharacterContext& ctx, float timer, const GlobalRange& data) {
		// 一度発動した効果は同じコンボ中に再発動しない
		if (isEffectActive_ || data.effectTriggerType == RangeEffectTriggerType::kNone) {
			return;
		}

		bool shouldActivate = false;	// 効果を発動するか
		switch (data.effectTriggerType) {
		case RangeEffectTriggerType::kButton:
			shouldActivate = ctx.inputData.skillTrigger;
			break;
		case RangeEffectTriggerType::kTimer:
			shouldActivate = timer >= data.effectTriggerTime;
			break;
		case RangeEffectTriggerType::kHit:
			shouldActivate = isHit_;
			break;
		default:
			break;
		}

		if (shouldActivate) {
			isEffectActive_ = true;	// 効果発動済みにする
			stayTimer_ = 0.0f;		// 停滞時間を初期化
			if (PlayerSubWeapon* subWeapon = GetPlayerSubWeapon()) {
				subWeapon->StopAtCurrentPosition();
			}
		}
	}

	void ComboThrow::UpdateRecallTrigger(const Character::CharacterContext& ctx, float timer, const GlobalRange& data) {
		PlayerSubWeapon* subWeapon = GetPlayerSubWeapon();	// 回収対象のサブ武器
		if (!subWeapon || !subWeapon->IsThrowing()) {
			return;
		}

		if (isRecallWaiting_) {
			UpdateRecallStay(ctx.dt, data, subWeapon);
			return;
		}

		bool shouldRecall = false;	// 回収を開始するか
		switch (data.recallTriggerType) {
		case RangeRecallTriggerType::kButton:
			shouldRecall = ctx.inputData.skillTrigger;
			break;
		case RangeRecallTriggerType::kTimer:
			shouldRecall = timer >= data.recallTriggerTime;
			break;
		case RangeRecallTriggerType::kNearOwner:
			shouldRecall = ctx.position.DistanceXZ(subWeapon->GetWorldTransform().GetWorldPosition()) <= data.recallNearRadius;
			break;
		default:
			break;
		}

		if (!shouldRecall) {
			return;
		}

		if (data.throwStayTime > 0.0f) {
			isRecallWaiting_ = true;		// 回収前の停滞に入る
			stayTimer_ = 0.0f;			// 停滞時間リセット
			subWeapon->StopAtCurrentPosition();
			return;
		}

		subWeapon->Recall();
	}

	void ComboThrow::UpdateRecallStay(float dt, const GlobalRange& data, PlayerSubWeapon* subWeapon) {
		stayTimer_ += dt;	// 停滞時間を加算
		if (stayTimer_ >= data.throwStayTime) {
			isRecallWaiting_ = false;	// 停滞終了
			stayTimer_ = 0.0f;			// 停滞時間リセット
			subWeapon->Recall();
		}
	}

	PlayerSubWeaponThrowData::MoveType ComboThrow::ToSubWeaponMoveType(const GlobalRange& data) const {
		switch (data.throwMoveType) {
		case RangeThrowMoveType::kStopOnHit:
			return PlayerSubWeaponThrowData::MoveType::kStopOnHit;
		case RangeThrowMoveType::kTeleportToTarget:
			return PlayerSubWeaponThrowData::MoveType::kTeleportToTarget;
		case RangeThrowMoveType::kLerpToTarget:
			return PlayerSubWeaponThrowData::MoveType::kLerpToTarget;
		case RangeThrowMoveType::kStraight:
		default:
			return PlayerSubWeaponThrowData::MoveType::kStraight;
		}
	}

	PlayerSubWeapon* ComboThrow::GetPlayerSubWeapon() const {
		auto* player = dynamic_cast<Character::BasePlayer*>(owner_);	// プレイヤー型へ変換
		if (!player) {
			return nullptr;
		}
		return dynamic_cast<PlayerSubWeapon*>(player->GetSubWeapon());
	}
}

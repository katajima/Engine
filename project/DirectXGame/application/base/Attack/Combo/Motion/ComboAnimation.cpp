#include "ComboAnimation.h"
#include"DirectXGame/application/base/Character/Base/CharacterManager.h"
#include <DirectXGame/engine/Animation/AnimationComponent.h>
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/engine/Entity/ObjectComponent.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include <algorithm>

namespace Combo {

	// 開始
	void ComboAnimation::Enter(Character::BaseCharacter* owner) {
		// 前回のコンボで保持していた実行状態を初期化する
		isAnimation = false;
		worldTransform_ = &owner->GetObjectComponent()->GetWorldTransform();
		appliedTransformOffset_ = Transform{};
		appliedTransformOffset_.scale = {};
		isPlayingPreMoveAnimation_ = false;
		hasStartedAttackAnimation_ = false;
		attackAnimationStartTime_ = 0.0f;

		// アニメーションの設定
		if(owner->GetObjectComponent()->GetObject3D())
		animationComponent = owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		if (animationComponent) isAnimation = true;

		// 移動コンポーネント取得
		movementComponent = owner->GetMoveComponent();
		// スケルタルアニメーションが存在しなくてもTransformアニメーションは処理する
		if (!isAnimation) return;
		if (CanUsePreMoveAnimation()) {
			// 攻撃前移動アニメーションを先に再生し、指定時間後に攻撃アニメーションへ切り替える
			isPlayingPreMoveAnimation_ = true;
			attackAnimationStartTime_ = data_.preMoveAnimationEndTime;
			PlaySkeletalAnimation(data_.preMoveAnimationName, data_.preMoveAnimationBlendTime, data_.preMoveAnimationLoop, data_.preMoveAnimationSpeed);
		}
		else {
			// 攻撃前移動アニメーションを使わない場合は従来通り攻撃アニメーションから開始する
			StartAttackAnimation(0.0f);
		}
	}

	// 更新
	void ComboAnimation::Update(const Character::CharacterContext& ctx, float timer, bool isDebug) {
		// コンボ時間に合わせてTransformオフセットを更新する
		ApplyTransformAnimation(timer);

		// アニメーションが存在しないなら終了
		if (!isAnimation) return;

		// 着地状態か 
		onGlound = movementComponent ? movementComponent->GetIsLanding() : true;

		// 攻撃前移動アニメーション中なら、攻撃アニメーション側の停止処理はまだ行わない
		if (UpdatePreMoveAnimation(timer, ctx.isSelfHitStop)) {
			return;
		}

		// 攻撃アニメーションスピード設定
		animationComponent->SetAnimationSpeed(data_.animationSpeed);

		// アニメーション再生に関する
		if (!onGlound && endType == EndConditionType::kOnGround && data_.animationStop) {
			float t = animationComponent->GetAnimationTime();
			float stopT = data_.animationStopTime;
			if (t > stopT) {
				animationComponent->SetIsPlaying(false);
				animationComponent->SetAnimationTime(stopT);
			}
			else {
				if (ctx.isSelfHitStop) {
					animationComponent->SetIsPlaying(false);
				}
				else {
					animationComponent->SetIsPlaying(true);
				}
			}
		}
		else {
			animationComponent->SetIsPlaying(true);

			if (isDebug && endType == EndConditionType::kOnTimer) {
				// アニメーション時間設定
				animationComponent->SetAnimationTime(CalculateAttackAnimationTime(timer) * data_.animationSpeed);
			}

			if (ctx.isSelfHitStop) {
				animationComponent->SetIsPlaying(false);
			}
			else {
				animationComponent->SetIsPlaying(true);
			}
		}
	}

	// 終了
	void ComboAnimation::Exit(Character::BaseCharacter* owner) {
		// 設定に応じてコンボ中に加算したTransformオフセットを除去する
		if (data_.restoreTransformOnExit) {
			RemoveTransformAnimationOffset();
		}
		worldTransform_ = nullptr;
		isPlayingPreMoveAnimation_ = false;
		hasStartedAttackAnimation_ = false;

		// アニメーションが存在しないなら終了
		if (!isAnimation) return;
		animationComponent->SetAnimationSpeed(1.0f);			// アニメーションスピード設定
		animationComponent->SetIsPlaying(true);
	}

	bool ComboAnimation::CanUsePreMoveAnimation() const {
		// 有効化され、名前が入り、切り替え時間が0より大きい場合だけ攻撃前移動アニメーションとして扱う
		return data_.usePreMoveAnimation &&
			!data_.preMoveAnimationName.empty() &&
			data_.preMoveAnimationEndTime > 0.0f;
	}

	float ComboAnimation::CalculateAttackAnimationTime(float timer) const {
		// 攻撃前移動アニメーション分の時間を差し引き、攻撃アニメーションは0秒から再生させる
		return (std::max)(timer - attackAnimationStartTime_, 0.0f);
	}

	void ComboAnimation::PlaySkeletalAnimation(const std::string& animationName, float blendTime, bool isLoop, float speed) {
		// アニメーションコンポーネントが無い場合は呼び出し元の状態だけを維持する
		if (!animationComponent) {
			return;
		}

		// 指定されたアニメーションを先頭から再生できる状態へ整える
		animationComponent->SetAnimation(animationName, blendTime);
		animationComponent->SetStratAnimeTime();
		animationComponent->SetIsLoop(isLoop);
		animationComponent->SetAnimationSpeed(speed);
		animationComponent->SetIsPlaying(true);
	}

	void ComboAnimation::StartAttackAnimation(float startTime) {
		// 攻撃アニメーションの開始時間を記録し、デバッグ時の時間指定にも同じ基準を使う
		attackAnimationStartTime_ = startTime;
		isPlayingPreMoveAnimation_ = false;
		hasStartedAttackAnimation_ = true;
		PlaySkeletalAnimation(data_.animationName, data_.animationBlendTime, data_.animationLoop, data_.animationSpeed);
	}

	bool ComboAnimation::UpdatePreMoveAnimation(float timer, bool isHitStop) {
		// 攻撃前移動アニメーションを使っていない場合は攻撃アニメーション処理へ進ませる
		if (!isPlayingPreMoveAnimation_) {
			if (!hasStartedAttackAnimation_) {
				StartAttackAnimation(0.0f);
			}
			return false;
		}

		// 指定時間に到達したら、攻撃アニメーションを0秒から開始する
		if (timer >= data_.preMoveAnimationEndTime) {
			StartAttackAnimation(data_.preMoveAnimationEndTime);
			return false;
		}

		// ヒットストップ中は移動用アニメーションも停止し、それ以外は設定速度で再生する
		animationComponent->SetAnimationSpeed(data_.preMoveAnimationSpeed);
		animationComponent->SetIsPlaying(!isHitStop);
		return true;
	}

	float ComboAnimation::CalculateTransformAnimationRate(float timer) const {
		// 時間幅が無い設定では終了値を即時適用する
		const float duration = data_.transformEndTime - data_.transformStartTime;
		if (duration <= 0.0001f) {
			return timer >= data_.transformStartTime ? 1.0f : 0.0f;
		}

		// Transformの開始・終了時間を0～1の補間率へ変換する
		return std::clamp((timer - data_.transformStartTime) / duration, 0.0f, 1.0f);
	}

	void ComboAnimation::ApplyTransformAnimation(float timer) {
		// 適用先が無い場合はTransformを変更しない
		if (!worldTransform_) {
			return;
		}
		// エディターで無効化された場合は、それまでのオフセットを即座に除去する
		if (!data_.isTransformAnimation) {
			RemoveTransformAnimationOffset();
			return;
		}

		// 開始値と終了値をコンボ時間で補間する
		const float rate = CalculateTransformAnimationRate(timer);
		const Transform currentOffset = data_.transformStart.LerpTransform(data_.transformEnd, rate);

		// 差分だけを加算し、ComboMoveなどが行った移動・回転を維持する
		worldTransform_->scale_ += currentOffset.scale - appliedTransformOffset_.scale;
		worldTransform_->rotate_ += currentOffset.rotate - appliedTransformOffset_.rotate;
		worldTransform_->translate_ += currentOffset.translate - appliedTransformOffset_.translate;
		appliedTransformOffset_ = currentOffset;
		worldTransform_->Update();
	}

	void ComboAnimation::RemoveTransformAnimationOffset() {
		// 適用先が無い場合は除去処理を行わない
		if (!worldTransform_) {
			return;
		}

		// 最後に加算したオフセットだけを戻し、他システムのTransform変更を保持する
		worldTransform_->scale_ -= appliedTransformOffset_.scale;
		worldTransform_->rotate_ -= appliedTransformOffset_.rotate;
		worldTransform_->translate_ -= appliedTransformOffset_.translate;
		appliedTransformOffset_ = Transform{};
		appliedTransformOffset_.scale = {};
		worldTransform_->Update();
	}
}

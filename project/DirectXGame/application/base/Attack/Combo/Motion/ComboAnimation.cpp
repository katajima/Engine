#include "ComboAnimation.h"
#include"DirectXGame/application/base/Character/Base/CharacterManager.h"
#include <DirectXGame/engine/Animation/AnimationComponent.h>
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
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

		// アニメーションの設定
		if(owner->GetObjectComponent()->GetObject3D())
		animationComponent = owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		if (animationComponent) isAnimation = true;

		// 移動コンポーネント取得
		movementComponent = owner->GetMoveComponent();
		// スケルタルアニメーションが存在しなくてもTransformアニメーションは処理する
		if (!isAnimation) return;
		// アニメーションが存在するならアニメーション設定
		animationComponent->SetAnimation(data_.animationName, data_.animationBlendTime);	// 再生するアニメーション設定
		animationComponent->SetStratAnimeTime();						// アニメーション時間初期化
		animationComponent->SetIsLoop(data_.animationLoop);				// ループ再生
		animationComponent->SetAnimationSpeed(data_.animationSpeed);	// アニメーションスピード設定
		animationComponent->SetIsPlaying(true);							// アニメーション再生
	}

	// 更新
	void ComboAnimation::Update(const Character::CharacterContext& ctx, float timer, bool isDebug) {
		// コンボ時間に合わせてTransformオフセットを更新する
		ApplyTransformAnimation(timer);

		// アニメーションが存在しないなら終了
		if (!isAnimation) return;

		// 着地状態か 
		onGlound = movementComponent ? movementComponent->GetIsLanding() : true;

		// アニメーションスピード設定
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
				animationComponent->SetAnimationTime(timer * data_.animationSpeed);
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

		// アニメーションが存在しないなら終了
		if (!isAnimation) return;
		animationComponent->SetAnimationSpeed(1.0f);			// アニメーションスピード設定
		animationComponent->SetIsPlaying(true);
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

#include "ComboAnimation.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"
#include <DirectXGame/engine/Animation/AnimationComponent.h>
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"

namespace Combo {

	// 開始
	void ComboAnimation::Enter(Character::BaseCharacter* owner) {
		// アニメーションの設定
		if(owner->GetObjectComponent()->GetObject3D())
		animationComponent = owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		if (animationComponent) isAnimation = true;
		// アニメーションが存在しないなら終了
		if (!isAnimation) return;

		// 移動コンポーネント取得
		movementComponent = owner->GetMoveComponent();
		// アニメーションが存在するならアニメーション設定
		animationComponent->SetAnimation(data_.animationName, data_.animationBlendTime);	// 再生するアニメーション設定
		animationComponent->SetStratAnimeTime();						// アニメーション時間初期化
		animationComponent->SetIsLoop(data_.animationLoop);				// ループ再生
		animationComponent->SetAnimationSpeed(data_.animationSpeed);	// アニメーションスピード設定
		animationComponent->SetIsPlaying(true);							// アニメーション再生
	}

	// 更新
	void ComboAnimation::Update(const Character::CharacterContext& ctx, float timer, bool isDebug) {
		// アニメーションが存在しないなら終了
		if (!isAnimation) return;

		// 着地状態か 
		onGlound = movementComponent->GetIsLanding();

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
		// アニメーションが存在しないなら終了
		if (!isAnimation) return;
		animationComponent->SetAnimationSpeed(1.0f);			// アニメーションスピード設定
		animationComponent->SetIsPlaying(true);
	}
}

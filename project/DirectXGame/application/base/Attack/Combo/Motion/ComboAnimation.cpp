#include "ComboAnimation.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"

namespace Combo {

	// 開始
	void ComboAnimation::Enter(Character::BaseCharacter* owner) {
		// アニメーションの設定
		animationComponent = owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		movementComponent = owner->GetMoveComponent();
		// アニメーション設定
		animationComponent->SetAnimation(data_.animationName, data_.animationBlendTime);	// 再生するアニメーション設定
		animationComponent->SetStratAnimeTime();						// アニメーション時間初期化
		animationComponent->SetIsLoop(data_.animationLoop);				// ループ再生
		animationComponent->SetAnimationSpeed(data_.animationSpeed);	// アニメーションスピード設定
		animationComponent->SetIsPlaying(true);							// アニメーション再生

		
	}

	// 更新
	void ComboAnimation::Update(float timer, float dt, bool isDebug) {
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
			}else{
				animationComponent->SetIsPlaying(true);
			}
		}
		else {
			animationComponent->SetIsPlaying(true);

			if (isDebug && endType == EndConditionType::kOnTimer) {
				// アニメーション時間設定
				animationComponent->SetAnimationTime(timer * data_.animationSpeed);
			}
		}

		
	}

	// 終了
	void ComboAnimation::Exit(Character::BaseCharacter* owner) {
		animationComponent->SetAnimationSpeed(1.0f);			// アニメーションスピード設定
		animationComponent->SetIsPlaying(true);
	}
}

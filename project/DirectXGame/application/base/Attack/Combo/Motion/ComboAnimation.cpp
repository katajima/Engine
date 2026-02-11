#include "ComboAnimation.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"

namespace Combo {

	// 開始
	void ComboAnimation::Enter(Character::BaseCharacter* owner) {
		// アニメーションの設定
		animationComponent = owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		// アニメーション設定
		animationComponent->SetAnimation(data_.animationName_, data_.animationBlendTime_);	// 再生するアニメーション設定
		animationComponent->SetStratAnimeTime();						// アニメーション時間初期化
		animationComponent->SetIsLoop(data_.animationLoop_);			// ループ再生
		animationComponent->SetAnimationSpeed(data_.animationSpeed_);	// アニメーションスピード設定
		animationComponent->SetIsPlaying(true);							// アニメーション再生


	}

	// 更新
	void ComboAnimation::Update(const Engine::Input& input, float timer, float dt) {
		// アニメーションスピード設定
		animationComponent->SetAnimationSpeed(data_.animationSpeed_);
		// アニメーション時間設定
		animationComponent->SetAnimationTime(timer * data_.animationSpeed_);
	}

	// 終了
	void ComboAnimation::Exit(Character::BaseCharacter* owner) {
		animationComponent->SetStratAnimeTime();				// アニメーション時間初期化
		animationComponent->SetIsLoop(data_.animationLoop_);	// ループ再生
		animationComponent->SetAnimationSpeed(1.0f);			// アニメーションスピード設定
		animationComponent->SetIsPlaying(true);					// アニメーション再生
	}
}

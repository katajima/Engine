#include "ComboMotion.h"
#include"DirectXGame/application/base/Move/MoveComponent.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"


namespace Combo {

#pragma region ComboMotion

	// 開始
	void ComboMotion::Enter(BaseCharacter* owner) {
		isMove_ = false;	// 移動しない

		// アニメーションの設定
		Engine::AnimationComponent* anima = owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		SetAnimation(anima);
		SetMove(owner->GetMoveComponent());
		SetWorld(&owner->GetObjectComponent()->GetWorldTransform());
		SetRigid(owner->GetObjectComponent()->GetRigidBodyComponent());

		// アニメーション設定
		animationComponent->SetAnimation(data_.animationName_, data_.animationBlendTime_);	// 再生するアニメーション設定
		animationComponent->SetStratAnimeTime();						// アニメーション時間初期化
		animationComponent->SetIsLoop(data_.animationLoop_);			// ループ再生
		animationComponent->SetAnimationSpeed(data_.animationSpeed_);	// アニメーションスピード設定
		animationComponent->SetIsPlaying(true);							// アニメーション再生



		// 回転
		owner->GetMoveComponent()->GetMoveSystem()->AttackProcess(owner->GetWorldTransform(), direction_);

		// 座標更新
		owner->GetWorldTransform().Update();
	}

	// 更新
	void ComboMotion::Update(const Engine::Input& input, float timer, float dt) {
		bool isStart = data_.moveWindowStart_ <= timer;		// 受付開始時間を過ぎたら
		bool isEnd = data_.moveWindowEnd_ >= timer;			// 受付終了時間より前なら

		// ゲームパッドの左スティックを動かしているか
		bool isMoveStick = input.GetGamePadLeftStick().Length() != 0;

		// アニメーションスピード設定
		animationComponent->SetAnimationSpeed(data_.animationSpeed_);

		// アニメーション時間設定
		animationComponent->SetAnimationTime(timer * data_.animationSpeed_);

		// 移動可能か
		moveComponent->GetMoveSystem()->SetIsAttackCanMove(IsMove());

		// 受付時間内なら
		// 強制的に移動
		if (data_.isCompulsionMove_) {
			isMove_ = true;
		}
		else {
			// 動かしていたら
			if (isMoveStick) {
				isMove_ = true;
			}
		}

		// 移動
		if (isMove_ && isStart && isEnd) {
			worldTransform->translate_ += Multiply(direction_, dt) * data_.speed_;
		}

		// 重力の設定
		if (!data_.isGravity_) {
			rigidBodyComponent->Velocity().y = 0;
		}
		rigidBodyComponent->SetGravityScale(data_.gravityScale_);
		rigidBodyComponent->SetIsGravity(data_.isGravity_);

		// 移動可能なら
		if (isMove_) {
			moveComponent->SetCanMove(true);				// 前進する
		}
	}

	// 終了
	void ComboMotion::Exit(BaseCharacter* owner) {
		isMove_ = false;
		animationComponent->SetStratAnimeTime();				// アニメーション時間初期化
		animationComponent->SetIsLoop(data_.animationLoop_);	// ループ再生
		animationComponent->SetAnimationSpeed(1.0f);			// アニメーションスピード設定
		animationComponent->SetIsPlaying(true);					// アニメーション再生
	}

#pragma endregion // コンボモーション
};


#include "ComboData.h"
#include "DirectXGame/engine/Animation/AnimationComponent.h"
#include"DirectXGame/application/base/Move/MoveComponent.h"
#include"DirectXGame/application/base/Character/Base/BaseCharacterManeger.h"

#pragma region ComboSequence

/// <summary>
/// コンボボタンを順番に登録
/// </summary>
void ComboSequence::RegisterCombo(const std::vector<ComboButton>& buttons) {
	comboButtons_.clear();
	for (auto& b : buttons) {
		comboButtons_.emplace_back(b);
	}
}

/// <summary>
/// コンボ成立チェック
/// </summary>
bool ComboSequence::Update(const Input& input, float deltaTime) {
	if (comboButtons_.empty()) return false;


	// 次に押すべきボタン
	const ComboButton& target = comboButtons_[currentIndex_];

	// 入力判定
	if (target.IsInput(input)) {
		currentIndex_++;
		// 全て成功
		if (currentIndex_ >= comboButtons_.size()) {
			return true;
		}
	}
	return false;
}
#pragma endregion // コンボボタン


#pragma region ComboCondition

void ComboCondition::Update(const Input& input, float timer, float dt) {
	bool isStart = data_.inputWindowStart_ <= timer;	// 受付開始時間を過ぎたら
	bool isEnd = data_.inputWindowEnd_ >= timer;			// 受付終了時間より前なら

	// 受付時間内なら
	if (isStart && isEnd) {

		// ボタン条件を満たしているなら
		isNextCombo_ = data_.comboSequence_.Update(input, dt);

		// 強制的にコンボに移行
		if (data_.isCompulsionNextCombo_) {
			isNextCombo_ = true;
		}
	}
}

void ComboCondition::Enter() {
	isNextCombo_ = false;
};

void ComboCondition::Exit() {
	isNextCombo_ = false;
};

#pragma endregion // コンボ条件


#pragma region ComboMotion

// 開始
void ComboMotion::Enter(BaseCharacter* owner) {
	isMove_ = false;	// 移動しない

	// アニメーションの設定
	AnimationComponent* anima = owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	SetAnimation(anima);
	SetMove(owner->GetMoveComponent());
	SetWorld(&owner->GetObjectComponent()->GetWorldTransform());
	SetRigid(owner->GetObjectComponent()->GetRigidBodyComponent());

	// アニメーション設定
	animationComponent->SetAnimetion(data_.animationName_, 0.0f);	// 再生するアニメーション設定
	animationComponent->SetStratAnimeTime();						// アニメーション時間初期化
	animationComponent->SetIsLoop(data_.animationLoop_);			// ループ再生
	animationComponent->SetAnimationSpeed(data_.animationSpeed_);	// アニメーションスピード設定
	animationComponent->SetIsPlaying(true);							// アニメーション再生

	// 座標更新
	owner->GetWorldTransform().Update();
}

// 更新
void ComboMotion::Update(const Input& input, float timer, float dt) {
	bool isStart = data_.moveWindowStart_ <= timer;		// 受付開始時間を過ぎたら
	bool isEnd = data_.moveWindowEnd_ >= timer;			// 受付終了時間より前なら

	// ゲームパッドの左スティックを動かしているか
	bool isMoveStick = input.GetGamePadLeftStick().Length() != 0;

	// アニメーションスピード設定
	animationComponent->SetAnimationSpeed(data_.animationSpeed_);

	// 移動可能か
	moveComponent->GetMoveSystem()->SetIsAttackCanMove(IsMove());

	// 受付時間内なら
	if (isStart && isEnd) {
		// 動かしていたら
		if (isMoveStick) {
			isMove_ = true;
		}
		// 強制的に移動
		if (data_.isCompulsionMove_) {
			isMove_ = true;
		}
	}
	else {
		isMove_ = false;
	}


	// 重力の設定
	if (data_.isGravity_) {
		rigidBodyComponent->Velocity().y = 0;
	}
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


#pragma region ComboHitBox

// 開始
void ComboHitBox::Enter(BaseCharacter* owner) {
	// ヒットボックスシステムを渡す
	hitBoxSystem_ = owner->GetAttackController()->GetHitBoxSystem();
}

// 更新
void ComboHitBox::Update(float timer, float dt) {
	
	if (timer >= data_.hitBpxWindowStart_) {
		if (!isPopHitBox_) {
			hitBoxSystem_->AddHitBox(data_.hitBoxUseType_,collData_,useHitBox_,data_.lifeTime_, perent_);
			isPopHitBox_ = true;
		}
	}
}

// 終了
void ComboHitBox::Exit() {
	isPopHitBox_ = false;
	
}

#pragma endregion // コンボヒットボックス


#pragma region ComboCamera

// 開始
void ComboCamera::Enter() {}

// 更新
void ComboCamera::Update(float timer,float dt) {

}

// 終了
void ComboCamera::Exit() {}

#pragma endregion // コンボカメラ


#pragma region ComboEffect

// 開始
void ComboEffect::Enter(BaseCharacter* owner) {
	// 武器情報取得
	weapon = owner->GetWeapon();
	// トレイル終了
	weapon->GetObject3D()->isEmitTrailEffect = false;
}

// 更新
void ComboEffect::Update(float timer, float dt) {
	// トレイル使用可能か
	bool isTrail = false;
	if (IsEffectTrail(timer)) { isTrail = true; }
	// トレイルを出すか設定
	weapon->GetObject3D()->isEmitTrailEffect = isTrail;
}

// 終了
void ComboEffect::Exit(BaseCharacter* owner) {
	// トレイル終了
	weapon->GetObject3D()->isEmitTrailEffect = false;
}

#pragma endregion //コンボエフェクト


#pragma region ComboData

// 開始
void ComboData::Enter(BaseCharacter* owner) {
	// 時間初期化
	timer_ = 0.0f;	
	// コンボ用モーションクラス開始
	motion.Enter(owner);				
	// コンボ用条件クラス開始
	comboCondition.Enter();					
	// コンボ用カメラクラス開始
	camera.Enter();							
	// コンボ用ヒットボックスクラス開始
	hitBox.Enter(owner);
	// コンボ用エフェクトクラス開始
	effect.Enter(owner);
}

// 更新
void ComboData::Update(const Input& input, float dt) {
	// 時間更新
	timer_ += dt;
	// コンボ用モーションクラス更新
	motion.Update(input, timer_, dt);				
	// コンボ用条件クラス更新
	comboCondition.Update(input, timer_, dt);		
	// コンボ用カメラクラス更新
	camera.Update(timer_,dt);						
	// コンボ用ヒットボックスクラス更新
	hitBox.Update(timer_,dt);
	// コンボ用エフェクトクラス更新
	effect.Update(timer_, dt);
}

// 終了
void ComboData::Exit(BaseCharacter* owner) {
	// 時間リセット
	timer_ = 0.0f;
	// コンボ用モーションクラス終了
	motion.Exit(owner);					
	// 条件クラス終了
	comboCondition.Exit();					
	// コンボ用カメラクラス終了
	camera.Exit();							
	// コンボ用ヒットボックスクラス終了
	hitBox.Exit();	
	// コンボ用エフェクトクラス終了
	effect.Exit(owner);
}

#pragma endregion // コンボデータ
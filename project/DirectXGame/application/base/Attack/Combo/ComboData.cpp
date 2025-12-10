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
			currentIndex_ = 0;
			return true;
		}
	}
	currentIndex_ = 0;
	return false;
}
#pragma endregion // コンボボタン


#pragma region ComboCondition

void ComboCondition::Update(const Input& input, float timer, float dt) {
	bool isStart = inputData_.inputWindowStart_ <= timer;		// 受付開始時間を過ぎたら
	bool isEnd = inputData_.inputWindowEnd_ >= timer;			// 受付終了時間より前なら

	// 受付時間内なら
	if (isStart && isEnd) {

		// まだ移行フラグがONではなくボタン条件を満たしているなら移行させるフラグをONに
		if (!isNextCombo_) {
			isNextCombo_ = inputData_.comboSequence_.Update(input, dt);
		}

		// 強制的にコンボに移行フラグNOに
		if (inputData_.isCompulsionNextCombo_) {
			isNextCombo_ = true;
		}
	}

	// 終了処理
	EndComboUpdate(input, timer, dt);

}

void ComboCondition::Enter(BaseCharacter* owner) {
	jumpSystem = owner->GetMoveComponent()->GetJumpSystem();
	isNextCombo_ = false;
	endTime_ = data_.stateEndTime;
	nextTime_ = data_.stateNextTime;
	isPress_ = true;
};

void ComboCondition::Exit() {
	isNextCombo_ = false;
	isPress_ = false;
}

void ComboCondition::EndComboUpdate(const Input& input, float timer, float dt) {
	// 終了タイプ
	switch (data_.type)
	{
	case ComboCondition::EndConditionType::kOnGround:	// 着地したら
		if (!jumpSystem->GetIsLanding()) {
			endTime_ += dt;
			nextTime_ += dt;
		}
		break;
	case ComboCondition::EndConditionType::kOnButtonRelease:	// 押し続けているなら
		if (data_.button_.IsPressed(input)) {
			if (isPress_) {
				endTime_ += dt;
				nextTime_ += dt;
			}
		}
		else {
			isPress_ = false;
		}
		break;
	case ComboCondition::EndConditionType::kOnMeterEmpty:	// 何かのメータが空なら

		break;
	case ComboCondition::EndConditionType::kOnTimer:	// 時間が過ぎたら
		endTime_ = data_.stateEndTime;
		break;
	case ComboCondition::EndConditionType::kOnHit:	// 何かに当たったら

		break;
	case ComboCondition::EndConditionType::kManual:	// 特殊

		break;
	default:
		break;
	}
}

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



	// 方向指定
	Vector2 velo = owner->GetInput()->GetGamePadLeftStick();
	if (velo.Length() != 0.0f) {
		direction_ = { velo.x,0.0f,velo.y };
	}
	else {
		direction_ = moveComponent->GetMoveSystem()->GetDirection();
	}
	// 方向
	moveComponent->GetMoveSystem()->CameraDirectionToMoveDirection(direction_);
	
	// 回転
	owner->GetMoveComponent()->GetMoveSystem()->AttackProcess(owner->GetWorldTransform(),direction_);

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


#pragma region ComboHitBox

// 開始
void ComboHitBox::Enter(BaseCharacter* owner) {
	// ヒットボックスシステムを渡す
	hitBoxSystem_ = owner->GetAttackController()->GetHitBoxSystem();
	// ジャンプシステムを渡す
	jumpSystem_ = owner->GetMoveComponent()->GetJumpSystem();
}

// 更新
void ComboHitBox::Update(const Input& input, float timer, float dt) {


	switch (data_.spawnType_)
	{
	case ComboHitBox::HitBoxSpawnType::kOnTime: // 時間経過で
		if (timer >= data_.hitBpxWindowStart_) {
			if (!isPopHitBox_) {
				hitBoxSystem_->AddHitBox(data_.hitBoxUseType_, collData_, useHitBox_, data_.lifeTime_, data_.dependenceType_, data_.offset_, perent_);
				isPopHitBox_ = true;
			}
		}
		break;
	case ComboHitBox::HitBoxSpawnType::kOnGround: // 着地したら
		if (jumpSystem_->GetIsLanding()) {
			if (!isPopHitBox_) {
				hitBoxSystem_->AddHitBox(data_.hitBoxUseType_, collData_, useHitBox_, data_.lifeTime_, data_.dependenceType_, data_.offset_, perent_);
				isPopHitBox_ = true;
			}
		}
		break;
	case ComboHitBox::HitBoxSpawnType::kOnAir:
		break;
	case ComboHitBox::HitBoxSpawnType::kOnButtonRelease: // ボタンを離したら
		if (button_.IsReleased(input)) {
			if (!isPopHitBox_) {
				hitBoxSystem_->AddHitBox(data_.hitBoxUseType_, collData_, useHitBox_, data_.lifeTime_, data_.dependenceType_, data_.offset_, perent_);
				isPopHitBox_ = true;
			}
		}
		break;
	default:
		break;
	}



}

// 終了
void ComboHitBox::Exit() {
	isPopHitBox_ = false;

}

void ComboHitBox::AddCollider(const HitBoxCollData& hitBoxData, const ComboGlovalData& combo) {
	HitBoxCollData data = hitBoxData;

	// リアクションデータ
	data.reactionData.GetDamageData().GetOne().damage = combo.reaction.damage;
	data.reactionData.GetKnockbackData().GetData().power_ = combo.reaction.knockbackPower;
	data.reactionData.GetKnockbackData().GetData().verticalBoost_ = combo.reaction.knockbackPowerY;
	data.reactionData.GetKnockbackData().GetData().duration_ = combo.reaction.knockbackDuration_;
	data.reactionData.GetKnockbackData().GetData().isVerticalBoost_ = combo.reaction.isVerticalBoost_;

	collData_.push_back(data);
};

#pragma endregion // コンボヒットボックス


#pragma region ComboCamera

// 開始
void ComboCamera::Enter() {}

// 更新
void ComboCamera::Update(float timer, float dt) {

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
	comboCondition.Enter(owner);
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
	camera.Update(timer_, dt);
	// コンボ用ヒットボックスクラス更新
	hitBox.Update(input, timer_, dt);
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
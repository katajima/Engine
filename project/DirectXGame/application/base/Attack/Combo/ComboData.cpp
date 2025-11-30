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

void ComboCondition::Update(const Input& input, float dt) {
	// 時間加算
	timer_ += dt;


	bool isStart = data_.inputWindowStart_ <= timer_;	// 受付開始時間を過ぎたら
	bool isEnd = data_.inputWindowEnd_ >= timer_;			// 受付終了時間より前なら

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
	timer_ = 0.0f;
	isNextCombo_ = false;
};

void ComboCondition::Exit() {
	timer_ = 0.0f;
	isNextCombo_ = false;
};

#pragma endregion // コンボ条件


#pragma region ComboMotion

// 開始
void ComboMotion::Enter() {
	timer_ = 0;			// 時間を初期化
	isMove_ = false;	// 移動しない
	// アニメーション設定
	animationComponent->SetAnimetion(data_.animationName_, 0.0f);	// 再生するアニメーション設定
	animationComponent->SetStratAnimeTime();				// アニメーション時間初期化
	animationComponent->SetIsLoop(data_.animationLoop_);			// ループ再生
	animationComponent->SetAnimationSpeed(data_.animationSpeed_);	// アニメーションスピード設定
	animationComponent->SetIsPlaying(true);					// アニメーション再生
}

// 更新
void ComboMotion::Update(const Input& input, float dt) {

	// 時間加算
	timer_ += dt;

	bool isStart = data_.moveWindowStart_ <= timer_;		// 受付開始時間を過ぎたら
	bool isEnd = data_.moveWindowEnd_ >= timer_;			// 受付終了時間より前なら

	// ゲームパッドの左スティックを動かしているか
	bool isMoveStick = input.GetGamePadLeftStick().Length() != 0;

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


	if (isMove_) {
		moveComponent->SetCanMove(true);				// 前進する
	}

}

// 終了
void ComboMotion::Exit() {
	timer_ = 0;
	isMove_ = false;

	animationComponent->SetStratAnimeTime();				// アニメーション時間初期化
	animationComponent->SetIsLoop(data_.animationLoop_);			// ループ再生
	animationComponent->SetAnimationSpeed(data_.animationSpeed_);	// アニメーションスピード設定
	animationComponent->SetIsPlaying(true);					// アニメーション再生
}

#pragma endregion // コンボモーション


#pragma region ComboHitBox

// 開始
void ComboHitBox::Enter() {
	timer_ = 0.0f;
}

// 更新
void ComboHitBox::Update(float dt) {
	timer_ += dt;

	if (timer_ >= data_.hitBpxWindowStart_) {
		if (!isPopHitBox_) {
			hitBoxSystem_->AddHitBox(data_.hitBoxUseType_,collData_,useHitBox_,data_.lifeTime_, perent_);
			isPopHitBox_ = true;
		}
	}
}

// 終了
void ComboHitBox::Exit() {
	isPopHitBox_ = false;
	timer_ = 0.0f;
}

#pragma endregion // コンボヒットボックス



#pragma region ComboDamage

// 開始
void ComboDamage::Enter() {
	isDamage_ = false;
	timer_ = 0.0f;
}

// 更新
void ComboDamage::Update(float dt) {
	// 時間加算
	timer_ += dt;


	bool isStart = data_.damageWindowStart_ <= timer_;	// 受付開始時間を過ぎたら
	bool isEnd = data_.damageWindowEnd_ >= timer_;			// 受付終了時間より前なら



	// 受付時間内なら
	if (isStart && isEnd) {
		isDamage_ = true;
	}
	else {
		isDamage_ = false;
	}
}

// 終了
void ComboDamage::Exit() {
	isDamage_ = false;
	timer_ = 0.0f;
}

#pragma endregion //ダメージ


#pragma region ComboCamera

// 開始
void ComboCamera::Enter() {
	timer_ = 0.0f;
}

// 更新
void ComboCamera::Update(float dt) {
	timer_ += dt;




}

// 終了
void ComboCamera::Exit() {
	timer_ = 0.0f;
}

#pragma endregion // コンボカメラ


#pragma region ComboData

// 開始
void ComboData::Enter() {
	comboCondition.Enter();					// コンボ用条件クラス開始
	motion.Enter();							// コンボ用モーションクラス開始
	damage.Enter();							// コンボ用ダメージクラス開始
	camera.Enter();							// コンボ用カメラクラス開始
	hitBox.Enter();							// コンボ用ヒットボックスクラス開始
}

// 更新
void ComboData::Update(const Input& input, float dt) {
	comboCondition.Update(input, dt);		// コンボ用条件クラス更新
	motion.Update(input, dt);				// コンボ用モーションクラス更新
	damage.Update(dt);						// コンボ用ダメージクラス更新
	camera.Update(dt);						// コンボ用カメラクラス更新
	hitBox.Update(dt);						// コンボ用ヒットボックスクラス更新
}

// 終了
void ComboData::Exit() {
	comboCondition.Exit();					// 条件クラス終了
	motion.Exit();							// コンボ用モーションクラス終了
	damage.Exit();							// コンボ用ダメージクラス終了
	camera.Exit();							// コンボ用カメラクラス終了
	hitBox.Exit();							// コンボ用ヒットボックスクラス終了
}

#pragma endregion // コンボデータ
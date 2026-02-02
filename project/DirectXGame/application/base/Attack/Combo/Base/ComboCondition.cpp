#include "ComboCondition.h"
#include"DirectXGame/application/base/Move/MoveComponent.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"


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
bool ComboSequence::Update(const Engine::Input& input, float deltaTime) {
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

void ComboCondition::Update(const Engine::Input& input, float timer, float dt) {
	bool isInputStart = inputData_.inputWindowStart_ <= timer;		// 受付開始時間を過ぎたら
	bool isInputEnd = inputData_.inputWindowEnd_ >= timer;			// 受付終了時間より前なら


	bool isCanselStart = inputData_.cancelStart_ <= timer;			// キャンセル開始時間を過ぎたら
	bool isCanselEnd = inputData_.cancelEnd_ >= timer;				// キャンセル終了時間より前なら


	// キャンセル受付時間ないなら
	if (isCanselStart && isCanselEnd && inputData_.isCancel_) {
		isCansel_ = true;
		return;
	}



	// 受付時間内なら
	if (!isCansel_) {
		if (isInputStart && isInputEnd) {

			// まだ移行フラグがONではなくボタン条件を満たしているなら移行させるフラグをONに
			if (!isNextCombo_) {
				isNextCombo_ = inputData_.comboSequence_.Update(input, dt);
			}

			// 強制的にコンボに移行フラグNOに
			if (inputData_.isCompulsionNextCombo_) {
				isNextCombo_ = true;
			}
		}
	}

	// 終了処理
	EndComboUpdate(input, timer, dt);

}

void ComboCondition::Enter(BaseCharacter* owner) {
	jumpSystem = owner->GetMoveComponent()->GetJumpSystem();	// ジャンプシステム取得
	isNextCombo_ = false;			// 次回
	isCansel_ = false;				// キャンセルフラグをfalseに
	endTime_ = data_.stateEndTime;	// 終了時間
	nextTime_ = data_.stateNextTime;// 移行タイミング時間
	isPress_ = true;				// おし
};

void ComboCondition::Exit() {
	isNextCombo_ = false;
	isPress_ = false;
	isCansel_ = false;
}

void ComboCondition::EndComboUpdate(const Engine::Input& input, float timer, float dt) {
	// 終了タイプ
	switch (data_.type)
	{
	case EndConditionType::kOnGround:	// 着地したら
		if (!jumpSystem->GetIsLanding()) {
			endTime_ += dt;
			nextTime_ += dt;
		}
		break;
	case EndConditionType::kOnButtonRelease:	// 押し続けているなら
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
	case EndConditionType::kOnMeterEmpty:	// 何かのメータが空なら

		break;
	case EndConditionType::kOnTimer:	// 時間が過ぎたら
		endTime_ = data_.stateEndTime;
		break;
	case EndConditionType::kOnHit:	// 何かに当たったら

		break;
	case EndConditionType::kManual:	// 特殊

		break;
	default:
		break;
	}
}

#pragma endregion // コンボ条件

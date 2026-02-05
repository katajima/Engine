#include "ComboCondition.h"
#include"DirectXGame/application/base/Move/MoveComponent.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"

#pragma region ComboCondition

void Combo::ComboCondition::Update(const Engine::Input& input, float timer, float dt) {
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

void Combo::ComboCondition::Enter(BaseCharacter* owner) {
	jumpSystem = owner->GetMoveComponent()->GetJumpSystem();	// ジャンプシステム取得
	isNextCombo_ = false;			// 次回
	isCansel_ = false;				// キャンセルフラグをfalseに
	endTime_ = data_.stateEndTime;	// 終了時間
	nextTime_ = data_.stateNextTime;// 移行タイミング時間
	isPress_ = true;				// おし
};

void Combo::ComboCondition::Exit() {
	isNextCombo_ = false;
	isPress_ = false;
	isCansel_ = false;
}

void Combo::ComboCondition::EndComboUpdate(const Engine::Input& input, float timer, float dt) {
	// 終了タイプ
	switch (data_.type)
	{
	case Combo::EndConditionType::kOnGround:	// 着地したら
		if (!jumpSystem->GetIsLanding()) {
			endTime_ += dt;
			nextTime_ += dt;
		}
		break;
	case Combo::EndConditionType::kOnButtonRelease:	// 押し続けているなら
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
	case Combo::EndConditionType::kOnMeterEmpty:	// 何かのメータが空なら

		break;
	case Combo::EndConditionType::kOnTimer:	// 時間が過ぎたら
		endTime_ = data_.stateEndTime;
		break;
	case Combo::EndConditionType::kOnHit:	// 何かに当たったら

		break;
	case Combo::EndConditionType::kManual:	// 特殊

		break;
	default:
		break;
	}
}

#pragma endregion // コンボ条件

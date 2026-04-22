#pragma once
#include "ComboConditionFunction.h"


namespace Combo {

	class NextCondition {
	public:
		// 開始
		void Enter(const GlobalCondition& data);
		//　終了
		void Exit();
		// 更新
		void Update(const Character::CharacterContext& ctx, const GlobalCondition& data, float time);
	public:
		// 次のコンボ移行する時間
		float GetComboNextTime() const { return nextTime_; }
	private:
		ComboButton button_ = ComboButton(ComboGamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed);			// コンボボタン
		float nextTime_ = 0.0f;				// コンボ終了時間
		bool isPress_ = false;
	};


}

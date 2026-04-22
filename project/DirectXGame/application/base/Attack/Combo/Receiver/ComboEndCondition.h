#pragma once
#include "ComboConditionFunction.h"

namespace Combo {

	class EndCondition {
	public:
		// 開始
		void Enter(const GlobalCondition& data);
		//　終了
		void Exit();
		// 更新
		void Update(const Character::CharacterContext& ctx, const GlobalCondition& data,float time);
	public:
		//
		float GetComboEndTime() const { return endTime_; }
	private:
		ComboButton button_ = ComboButton(ComboGamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed);			// コンボボタン

		float endTime_ = 0.0f;				// コンボ終了時間
		bool isPress_ = false;
	};

}
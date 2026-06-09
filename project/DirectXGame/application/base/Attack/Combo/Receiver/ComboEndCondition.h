#pragma once
#include "ComboConditionFunction.h"

namespace Combo {

	/// <summary>
	/// コンボを終了する条件と終了時刻を管理する。
	/// </summary>
	class EndCondition {
	public:
		// 終了条件の初期値をコンボデータから設定する
		void Enter(const GlobalCondition& data);
		// 終了条件の状態をリセットする
		void Exit();
		// 現在の状態と経過時間から終了条件を更新する
		void Update(const Character::CharacterContext& ctx, const GlobalCondition& data,float time);
	public:
		// コンボが終了可能になる時間を取得する
		float GetComboEndTime() const { return endTime_; }
	private:
		ComboButton button_ = ComboButton(ComboGamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed);			// コンボボタン

		float endTime_ = 0.0f;				// コンボ終了時間
		bool isPress_ = false;				// 押しっぱなし条件を継続中か
	};

}

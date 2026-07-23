#pragma once
#include "ComboConditionFunction.h"

namespace Combo {

	/// <summary>
	/// コンボを終了する条件と終了時刻を管理する。
	/// </summary>
	class EndCondition {
	public:
		/// <summary>
		/// 終了条件の初期値をコンボデータから設定する
		/// </summary>
		void Enter(const GlobalCondition& data);
		/// <summary>
		/// 終了条件の状態をリセットする
		/// </summary>
		void Exit();
		/// <summary>
		/// 現在の状態と経過時間から終了条件を更新する
		/// </summary>
		void Update(const Character::CharacterContext& ctx, const GlobalCondition& data,float time);
	public:
		/// <summary>
		/// コンボが終了可能になる時間を取得する
		/// </summary>
		float GetComboEndTime() const { return endTime_; }
	private:
		ComboButton button_ = ComboButton(ComboGamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed);			// コンボボタン

		float endTime_ = 0.0f;				// コンボ終了時間
		bool isPress_ = false;				// 押しっぱなし条件を継続中か
	};

}

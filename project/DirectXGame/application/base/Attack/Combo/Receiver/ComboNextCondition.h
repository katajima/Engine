#pragma once
#include "ComboConditionFunction.h"


namespace Combo {

	/// <summary>
	/// 次のコンボへ移行できる条件と受付時刻を管理する。
	/// </summary>
	class NextCondition {
	public:
		// 次段条件の初期値をコンボデータから設定する
		void Enter(const GlobalCondition& data);
		// 次段条件の状態をリセットする
		void Exit();
		// 現在の状態と経過時間から次段受付条件を更新する
		void Update(const Character::CharacterContext& ctx, const GlobalCondition& data, float time);
	public:
		// 次のコンボ移行する時間
		float GetComboNextTime() const { return nextTime_; }
	private:
		ComboButton button_ = ComboButton(ComboGamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed);			// コンボボタン
		float nextTime_ = 0.0f;				// 次コンボへ移行可能になる時間
		bool isPress_ = false;				// 押しっぱなし条件を継続中か
	};


}

#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"


namespace Combo {

	/// <summary>
	/// 次に移行
	/// </summary>
	class NextReceiver {
	public:
		// 開始
		void Enter();
		//　終了
		void Exit();
		// 更新
		void Update(const Character::CharacterContext& ctx,const GlobalCondition& data, float time);

	public: // 取得
		// 受付可能か取得
		bool GetIsActive() const { return isActive_; }
		// 次のコンボに移行するか
		bool GetIsNext() const { return isNext_; }
		// ボタン設定
		void SetButton(const std::vector<ComboButton>& button) { comboSequence_.RegisterCombo(button); }
	private:
		ComboSequence comboSequence_;			// ボタン条件
		// 受付可能か
		bool isActive_ = false;
		// 次のコンボに移行するか
		bool isNext_ = false;
	};
}
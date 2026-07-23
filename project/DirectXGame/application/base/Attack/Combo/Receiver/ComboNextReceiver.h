#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"


namespace Combo {

	/// <summary>
	/// 次に移行
	/// </summary>
	class NextReceiver {
	public:
		/// <summary>
		/// 開始
		/// </summary>
		void Enter(const GlobalCondition& data);
		/// <summary>
		/// 終了
		/// </summary>
		void Exit();
		/// <summary>
		/// 更新
		/// </summary>
		void Update(const Character::CharacterContext& ctx,const GlobalCondition& data, float time);

	public: // 取得
		/// <summary>
		/// 受付可能か取得
		/// </summary>
		bool GetIsActive() const { return isActive_; }
		/// <summary>
		/// 次のコンボに移行するか
		/// </summary>
		bool GetIsNext() const { return isNext_; }
		/// <summary>
		/// ボタン設定
		/// </summary>
		void SetButton(const std::vector<ComboButton>& button);
	private:
		ComboSequence comboSequence_;			// ボタン条件
		// 受付可能か
		bool isActive_ = false;
		// 次のコンボに移行するか
		bool isNext_ = false;
		// 受付開始から次のコンボに移行するまでの時間
		float stateInputToNextTime_ = 0.0f;
		// 入力されたか
		bool isInput_ = false;
	};
}
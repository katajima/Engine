#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"

namespace Combo {

	// キャンセル受付
/// <summary>
/// CancelReceiverを管理・実装するクラス。
/// </summary>
	class CancelReceiver {
	public:
		/// <summary>
		/// 開始
		/// </summary>
		void Enter();
		/// <summary>
		/// 終了
		/// </summary>
		void Exit();
		/// <summary>
		/// 更新
		/// </summary>
		void Update(const Character::CharacterContext& ctx, const GlobalCondition& data, float time);

	public: // 設定
		/// <summary>
		/// 受付可能か取得
		/// </summary>
		bool GetIsActive() const { return isActive_; }
		/// <summary>
		/// キャンセルするか
		/// </summary>
		bool GetIsCancel() const { return isCancel_; }
	private:
		// ボタン条件
		ComboSequence comboSequence_;
		// 受付可能か
		bool isActive_ = false;
		// キャンセルするか
		bool isCancel_ = false;
	};
};
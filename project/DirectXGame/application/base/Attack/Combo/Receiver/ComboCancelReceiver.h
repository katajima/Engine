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
		// 開始
		void Enter();
		//　終了
		void Exit();
		// 更新
		void Update(const Character::CharacterContext& ctx, const GlobalCondition& data, float time);

	public: // 設定
		// 受付可能か取得
		bool GetIsActive() const { return isActive_; }
		// キャンセルするか
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
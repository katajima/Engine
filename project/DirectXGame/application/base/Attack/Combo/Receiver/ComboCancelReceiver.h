#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"

namespace Combo {

	// キャンセル受付
	class CancelReceiver {
	public:
		// データ構造体
		struct Data
		{

			float start = 0.1f;				// キャンセル開始時間
			float end = 0.5f;				// キャンセル終了時間
			bool isCancel = false;			// キャンセル可能かどうか

			ComboSequence comboSequence_;			// ボタン条件
		};

		// 開始
		void Enter();
		//　終了
		void Exit();
		// 更新
		void Update(float time);
	public:
		// データ取得
		Data& GetData() { return data_;  };
 		// 受付可能か取得
		bool GetIsActive() const { return isActive_; }
		// キャンセルするか
		bool GetIsCancel() const { return isCancel_; }
	private:
		// データ
		Data data_;
		// 受付可能か
		bool isActive_ = false;
		// キャンセルするか
		bool isCancel_ = false;
	};
};
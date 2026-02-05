#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"


namespace Combo {

	/// <summary>
	/// 次に移行
	/// </summary>
	class NextReceiver {
	public:
		// データ構造体
		struct Data
		{

			float inputStart = 0.1f;				// コンボ移行入力受付開始時間
			float inputEnd = 0.5f;				// コンボ移行入力受付終了時間
			bool isNext = false;			// コンボ移行可能かどうか

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
		Data& GetData() { return data_; };
		// 受付可能か取得
		bool GetIsActive() const { return isActive_; }
		// 次のコンボに移行するか
		bool GetIsNext() const { return isNext_; }
	private:
		// データ
		Data data_;
		// 受付可能か
		bool isActive_ = false;
		// 次のコンボに移行するか
		bool isNext_ = false;
	};
}
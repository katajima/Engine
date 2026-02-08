#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"

namespace Combo {

	// キャンセル受付
	class CancelReceiver {
	public:
		// データ構造体
		struct Data {
			float inputStart = 0.1f;			// キャンセル開始時間
			float inputEnd = 0.5f;				// キャンセル終了時間
			bool isCancel = true;				// キャンセル可能かどうか


			// キャンセル開始時間(スティック移動での)
			float inputMoveStart = 0.1f;			
			// キャンセル終了時間(スティック移動での)
			float inputMoveEnd = 0.5f;	
			// キャンセル可能かどうか(スティック移動での)
			bool isMoveCancel = true;				


			// ボタン条件
			ComboSequence comboSequence_;		
		};

		// 開始
		void Enter();
		//　終了
		void Exit();
		// 更新
		void Update(const Engine::Input& input, float time);

	public: // 設定
		// コンボ条件発動時間設定
		void ConditionStartEnd(float start, float end) {
			data_.inputStart = start;
			data_.inputEnd = end;
		};
	public:
		// データ取得
		Data& GetData() { return data_;  };
 		// 受付可能か取得
		bool GetIsActive() const { return isActive_; }
		// キャンセルするか
		bool GetIsCancel() const { return isCancel_; }
		// コンボ受付可能か
		bool IsInputWindow(float timer) const {
			return timer >= data_.inputStart && timer <= data_.inputEnd;
		};
		// 入力受付開始時間取得
		float GetInputStart() const { return data_.inputStart; }
		// 入力受付終了時間取得
		float GetInputEnd() const { return data_.inputEnd; }
		// 入力受付開始時間(移動)取得
		float GetInputMoveStart() const { return data_.inputMoveStart; }
		// 入力受付終了時間(移動)取得
		float GetInputMoveEnd() const { return data_.inputMoveEnd; }
	private:
		// データ
		Data data_;
		// 受付可能か
		bool isActive_ = false;
		// キャンセルするか
		bool isCancel_ = false;
	};
};
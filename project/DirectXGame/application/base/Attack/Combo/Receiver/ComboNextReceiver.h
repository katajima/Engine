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
			// コンボ移行入力受付開始時間
			float inputStart = 0.1f;				
			// コンボ移行入力受付終了時間
			float inputEnd = 0.5f;				
			// コンボ移行可能かどうか
			bool isNext = false;			

			// 強制的に次のコンボに移行するか 
			bool isCompulsionNext = false;	


			ComboSequence comboSequence_;			// ボタン条件
		};

		// 開始
		void Enter();
		//　終了
		void Exit();
		// 更新
		void Update(const Character::CharacterContext& ctx,float time);

	public: // 設定
		// コンボ条件発動時間設定
		void ConditionStartEnd(float start, float end) {
			data_.inputStart = start;
			data_.inputEnd = end;
		};
	public: // 取得
		// データ取得
		Data& GetData() { return data_; };
		// 受付可能か取得
		bool GetIsActive() const { return isActive_; }
		// 次のコンボに移行するか
		bool GetIsNext() const { return isNext_; }
		// コンボ受付可能か
		bool IsInputWindow(float timer) const {
			return timer >= data_.inputStart && timer <= data_.inputEnd;
		};
		// 入力受付開始時間取得
		float GetInputStart() const { return data_.inputStart; }
		// 入力受付終了時間取得
		float GetInputEnd() const { return data_.inputEnd; }
	private:
		// データ
		Data data_;
		// 受付可能か
		bool isActive_ = false;
		// 次のコンボに移行するか
		bool isNext_ = false;
	};
}
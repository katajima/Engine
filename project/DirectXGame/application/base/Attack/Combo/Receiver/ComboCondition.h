#pragma once
#include "ComboNextReceiver.h"
#include "ComboCancelReceiver.h"
#include "ComboEndCondition.h"
#include "ComboNextCondition.h"

namespace Combo {

	// コンボ受付条件クラス
	class ComboCondition {
	public:
		// 開始
		void Enter(Character::BaseCharacter* owner);

		// 更新
		void Update(const Character::CharacterContext& ctx, float timer);

		// 終了
		void Exit();

	public: // 取得
		// 次のコンボ移行する時間
		float GetComboNextTime() const { return nextCondition_.GetComboNextTime(); }
		// コンボ終了時間
		float GetComboEndTime() const { return endCondition_.GetComboEndTime(); }
		
		// コンボ受付可能か
		bool IsComdoNextInputWindow(float timer) const {
			return timer >= data_.stateInput.startTime && timer <= data_.stateInput.endTime;
		};

		// キャンセル受付可能か
		bool IsComdoCancelInputWindow(float timer) const {
			return timer >= data_.stateCancel.startTime && timer <= data_.stateCancel.startTime;
		};
		// 入力受付開始時間取得
		float GetNextInputStart() const { return data_.stateInput.startTime; }
		// 入力受付終了時間取得
		float GetNextInputEnd() const { return data_.stateInput.endTime; }

		// 入力受付開始時間取得
		float GetCancelInputStart() const { return data_.stateCancel.startTime; }
		// 入力受付終了時間取得
		float GetCancelInputEnd() const { return data_.stateCancel.endTime; }
		// 入力受付開始時間(移動)取得
		float GetMoveCancelInputStart() const { return data_.stateMoveCancel.startTime; }
		// 入力受付終了時間(移動)取得
		float GetMoveCancelInputEnd() const { return data_.stateMoveCancel.endTime; }

	public:
		// データ取得
		GlobalCondition& GetData() { return data_; }
		// コンボ移行受付クラス取得
		NextReceiver GetNextReceiver() const { return nextRecever_; }
		// コンボキャンセル受付クラス取得
		CancelReceiver GetCancelReceiver() const { return cancelReceiver_;}
		// コンボ終了条件クラス取得
		EndCondition GetEndCondition() const { return endCondition_; }
		// コンボ移行条件クラス取得
		NextCondition GetNextCondition() const { return nextCondition_; }
	private:
		GlobalCondition data_{};

		// コンボ移行受付クラス
		NextReceiver nextRecever_;
		// キャンセル受付クラス
		CancelReceiver cancelReceiver_;
		// 終了条件クラス
		EndCondition endCondition_;
		// 移行条件クラス
		NextCondition nextCondition_;
	};

}

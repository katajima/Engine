#pragma once
#include "ComboNextReceiver.h"
#include "ComboCancelReceiver.h"
#include "ComboEndCondition.h"
#include "ComboNextCondition.h"

namespace Combo {

	// コンボ受付条件クラス
/// <summary>
/// ComboConditionを管理・実装するクラス。
/// </summary>
	class ComboCondition {
	public:
		/// <summary>
		/// 開始
		/// </summary>
		void Enter(Character::BaseCharacter* owner);

		/// <summary>
		/// 更新
		/// </summary>
		void Update(const Character::CharacterContext& ctx, float timer);

		/// <summary>
		/// 終了
		/// </summary>
		void Exit();

	public: // 取得
		/// <summary>
		/// 次のコンボ移行する時間
		/// </summary>
		float GetComboNextTime() const { return nextCondition_.GetComboNextTime(); }
		/// <summary>
		/// コンボ終了時間
		/// </summary>
		float GetComboEndTime() const { return endCondition_.GetComboEndTime(); }
		
		/// <summary>
		/// コンボ受付可能か
		/// </summary>
		bool IsComdoNextInputWindow(float timer) const {
			return timer >= data_.stateInput.startTime && timer <= data_.stateInput.endTime;
		};

		/// <summary>
		/// キャンセル受付可能か
		/// </summary>
		bool IsComdoCancelInputWindow(float timer) const {
			return timer >= data_.stateCancel.startTime && timer <= data_.stateCancel.endTime;
		};
		/// <summary>
		/// 入力受付開始時間取得
		/// </summary>
		float GetNextInputStart() const { return data_.stateInput.startTime; }
		/// <summary>
		/// 入力受付終了時間取得
		/// </summary>
		float GetNextInputEnd() const { return data_.stateInput.endTime; }

		/// <summary>
		/// 入力受付開始時間取得
		/// </summary>
		float GetCancelInputStart() const { return data_.stateCancel.startTime; }
		/// <summary>
		/// 入力受付終了時間取得
		/// </summary>
		float GetCancelInputEnd() const { return data_.stateCancel.endTime; }
		/// <summary>
		/// 入力受付開始時間(移動)取得
		/// </summary>
		float GetMoveCancelInputStart() const { return data_.stateMoveCancel.startTime; }
		/// <summary>
		/// 入力受付終了時間(移動)取得
		/// </summary>
		float GetMoveCancelInputEnd() const { return data_.stateMoveCancel.endTime; }

	public:
		/// <summary>
		/// データ取得
		/// </summary>
		GlobalCondition& GetData() { return data_; }
		/// <summary>
		/// コンボ移行受付クラス取得
		/// </summary>
		NextReceiver& GetNextReceiver() { return nextRecever_; }
		/// <summary>
		/// コンボキャンセル受付クラス取得
		/// </summary>
		CancelReceiver GetCancelReceiver() const { return cancelReceiver_;}
		/// <summary>
		/// コンボ終了条件クラス取得
		/// </summary>
		EndCondition GetEndCondition() const { return endCondition_; }
		/// <summary>
		/// コンボ移行条件クラス取得
		/// </summary>
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

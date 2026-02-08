#pragma once
#include "ComboNextReceiver.h"
#include "ComboCancelReceiver.h"
#include "ComboEndCondition.h"
#include "ComboNextCondition.h"

class BaseCharacter;		// キャラクター
class JumpSystem;			// ジャンプシステム


namespace Combo {

	// コンボ受付条件クラス
	class ComboCondition {
	public:
		// 開始
		void Enter(BaseCharacter* owner);

		// 更新
		void Update(const Engine::Input& input, float timer, float dt);

		// 終了
		void Exit();

	public: // 取得
		// 次のコンボ移行する時間
		float GetComboNextTime() const { return nextCondition_.GetComboNextTime(); }
		// コンボ終了時間
		float GetComboEndTime() const { return endCondition_.GetComboEndTime(); }
		
		// コンボ受付可能か
		bool IsComdoNextInputWindow(float timer) const {
			return nextRecever_.IsInputWindow(timer);
		};
		// コンボキャンセル受付可能か
		bool IsComdoCancelInputWindow(float timer) const {
			return cancelReceiver_.IsInputWindow(timer);
		}
		
	public:
		// コンボ移行受付クラス取得
		NextReceiver& GetNextReceiver() { return nextRecever_; }
		// コンボキャンセル受付クラス取得
		CancelReceiver& GetCancelReceiver() { return cancelReceiver_;}
		// コンボ終了条件クラス取得
		EndCondition& GetEndCondition() { return endCondition_; }
		// コンボ移行条件クラス取得
		NextCondition& GetNextCondition() { return nextCondition_; }
	private:
		// コンボ移行受付クラス
		NextReceiver nextRecever_;
		// キャンセル受付クラス
		CancelReceiver cancelReceiver_;
		// 終了条件クラス
		EndCondition endCondition_;
		// 移行条件クラス
		NextCondition nextCondition_;
	private:
		JumpSystem* jumpSystem = nullptr;
	};

}

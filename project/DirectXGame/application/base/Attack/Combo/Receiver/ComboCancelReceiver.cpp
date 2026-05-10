#include "ComboCancelReceiver.h"
#include <DirectXGame/application/base/Character/Base/CharacterContext.h>

namespace Combo {

	void CancelReceiver::Enter() {
		isCancel_ = false;				// キャンセルフラグをfalseに
	};
	
	void CancelReceiver::Exit() {
		isCancel_ = false;				// キャンセルフラグをfalseに
	};

	void CancelReceiver::Update(const Character::CharacterContext& ctx, const GlobalCondition& data,float timer) {
		bool isCancelStart = data.stateCancel.startTime <= timer;			// キャンセル開始時間を過ぎたら
		bool isCancelEnd = data.stateCancel.endTime >= timer;				// キャンセル終了時間より前なら

		bool isCancelMoveStart = data.stateMoveCancel.startTime  <= timer;			// キャンセル開始時間を過ぎたら
		bool isCancelMoveEnd = data.stateMoveCancel.endTime >= timer;				// キャンセル終了時間より前なら
		
		// ゲームパッドの左スティックを動かしているか
		bool isMoveStick = ctx.worldStickDirection.Length() != 0;
		bool isJumpCancel = ctx.inputData.jumpTrigger;

		
		
		// 移動スティックによる
		if (isCancelMoveStart && isCancelMoveEnd && data.isMoveCancel) {
			if (!isCancel_ && isMoveStick) {
				isCancel_ = true;
			}
		}


		if (isJumpCancel && !ctx.isJumpAttacking && data.isCancel) {
			if (!isCancel_) {
				isCancel_ = true;
			}
		}



		// キャンセル受付時間内なら
		if (isCancelStart && isCancelEnd && data.isCancel) {
			if (!isCancel_) {
				isCancel_ = comboSequence_.Update(ctx);
			}
			return;
		}
	};

}
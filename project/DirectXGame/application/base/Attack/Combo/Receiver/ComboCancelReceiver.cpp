#include "ComboCancelReceiver.h"

namespace Combo {

	void CancelReceiver::Enter() {
		isCancel_ = false;				// キャンセルフラグをfalseに
	};
	
	void CancelReceiver::Exit() {
		isCancel_ = false;				// キャンセルフラグをfalseに
	};

	void CancelReceiver::Update(const Character::CharacterContext& ctx,float timer) {
		bool isCancelStart = data_.inputStart <= timer;			// キャンセル開始時間を過ぎたら
		bool isCancelEnd = data_.inputEnd >= timer;				// キャンセル終了時間より前なら

		bool isCancelMoveStart = data_.inputMoveStart <= timer;			// キャンセル開始時間を過ぎたら
		bool isCancelMoveEnd = data_.inputMoveEnd >= timer;				// キャンセル終了時間より前なら
		
		// ゲームパッドの左スティックを動かしているか
		bool isMoveStick = ctx.worldStickDirection.Length() != 0;
		bool isJumpCancel = ctx.inputData.jumpTrigger;

		// 移動スティックによる
		if (isCancelMoveStart && isCancelMoveEnd && data_.isMoveCancel) {
			if (!isCancel_ && isMoveStick) {
				isCancel_ = true;
			}
		}
		if (isJumpCancel && !ctx.isJumpAttacking) {
			if (!isCancel_) {
				isCancel_ = true;
			}
		}



		// キャンセル受付時間内なら
		if (isCancelStart && isCancelEnd && data_.isCancel) {
			if (!isCancel_) {
				isCancel_ = data_.comboSequence_.Update(ctx);
			}
			return;
		}
	};

}
#include "ComboCancelReceiver.h"
#include <DirectXGame/application/base/Character/Base/CharacterContext.h>

namespace Combo {

	void CancelReceiver::Enter() {
		// コンボ開始時はキャンセル未成立の状態から始める
		isCancel_ = false;				// キャンセルフラグをfalseに
	};
	
	void CancelReceiver::Exit() {
		// 次のコンボへ状態を持ち越さないようにリセットする
		isCancel_ = false;				// キャンセルフラグをfalseに
	};

	void CancelReceiver::Update(const Character::CharacterContext& ctx, const GlobalCondition& data,float timer) {
		// 通常キャンセルの受付時間内かを判定する
		bool isCancelStart = data.stateCancel.startTime <= timer;			// キャンセル開始時間を過ぎたら
		bool isCancelEnd = data.stateCancel.endTime >= timer;				// キャンセル終了時間より前なら

		// 移動キャンセルの受付時間内かを判定する
		bool isCancelMoveStart = data.stateMoveCancel.startTime  <= timer;			// キャンセル開始時間を過ぎたら
		bool isCancelMoveEnd = data.stateMoveCancel.endTime >= timer;				// キャンセル終了時間より前なら
		
		// ゲームパッドの左スティックを動かしているか
		bool isMoveStick = ctx.worldStickDirection.Length() != 0;
		bool isJumpCancel = ctx.inputData.jumpTrigger;


		
		// 移動スティックによる
		if (isCancelMoveStart && isCancelMoveEnd && data.isMoveCancel) {
			// 移動入力があれば、移動キャンセル成立として扱う
			if (!isCancel_ && isMoveStick) {
				isCancel_ = true;
			}
		}


		if (isJumpCancel && !ctx.isJumpAttacking && data.isCancel) {
			// ジャンプ攻撃中でなければ、ジャンプ入力によるキャンセルを許可する
			if (!isCancel_) {
				isCancel_ = true;
			}
		}



		// キャンセル受付時間内なら
		if (isCancelStart && isCancelEnd && data.isCancel) {
			if (!isCancel_) {
				// 登録済みの次コンボ入力が成立したらキャンセル可能にする
				isCancel_ = comboSequence_.Update(ctx);
			}
			return;
		}
	};

}

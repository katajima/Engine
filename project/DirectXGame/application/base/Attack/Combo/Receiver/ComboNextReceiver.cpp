#include "ComboNextReceiver.h"


namespace Combo {

	// 開始
	void NextReceiver::Enter() {
		// 移行フラグをfalse
		isNext_ = false;
	};
	//　終了
	void NextReceiver::Exit() {
		// 移行フラグをfalse
		isNext_ = false;
	};
	// 更新
	void NextReceiver::Update(const InputSystem& inputSystem,float timer) {
		bool isInputStart = data_.inputStart <= timer;		// 受付開始時間を過ぎたら
		bool isInputEnd = data_.inputEnd >= timer;			// 受付終了時間より前なら


		if (isInputStart && isInputEnd) {

			// まだ移行フラグがONではなくボタン条件を満たしているなら移行させるフラグをONに
			if (!isNext_) {
				isNext_ = data_.comboSequence_.Update(inputSystem, 0);
			}

			// 強制的にコンボに移行フラグNOに
			if (data_.isCompulsionNext) {
				isNext_ = true;
			}
		}
	};
	
}

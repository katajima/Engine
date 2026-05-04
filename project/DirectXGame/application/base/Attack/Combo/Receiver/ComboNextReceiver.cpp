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
	void NextReceiver::Update(const Character::CharacterContext& ctx, const GlobalCondition& data,float timer) {
		bool isInputStart = data.stateInput.startTime <= timer;		// 受付開始時間を過ぎたら
		bool isInputEnd = data.stateInput.endTime >= timer;			// 受付終了時間より前なら


		if (isInputStart && isInputEnd) {
			// まだ移行フラグがONではなくボタン条件を満たしているなら移行させるフラグをONに
			if (!isNext_) {
				isNext_ = comboSequence_.Update(ctx);
			}
			// 強制的にコンボに移行フラグNOに
			if (data.isCompulsionNext) {
				isNext_ = true;
			}
		}
	};

	void NextReceiver::SetButton(const std::vector<ComboButton>& button) { 
		comboSequence_.RegisterCombo(button); 
	}

	
}

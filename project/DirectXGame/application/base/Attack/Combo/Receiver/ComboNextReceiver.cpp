#include "ComboNextReceiver.h"
#include <DirectXGame/application/base/Character/Base/CharacterContext.h>

namespace Combo {

	// 開始
	void NextReceiver::Enter(const GlobalCondition& data) {
		// 移行フラグをfalse
		isNext_ = false;
		isInput_ = false;
		// 受付可能に
		stateInputToNextTime_ = data.inputDelay;
	};
	//　終了
	void NextReceiver::Exit() {
		// 移行フラグをfalse
		isNext_ = false;
		isInput_ = false;
	};
	// 更新
	void NextReceiver::Update(const Character::CharacterContext& ctx, const GlobalCondition& data,float timer) {
		bool isInputStart = data.stateInput.startTime <= timer;		// 受付開始時間を過ぎたら
		bool isInputEnd = data.stateInput.endTime >= timer;			// 受付終了時間より前なら


		if (isInputStart && isInputEnd) {
			// まだ移行フラグがONではなくボタン条件を満たしているなら移行させるフラグをONに
			if (!isNext_) {
				if (!isInput_) {
					isInput_ =	comboSequence_.Update(ctx);
				}
				else {
					stateInputToNextTime_ -= ctx.dt;
					if(stateInputToNextTime_ <= 0.0f) {
						isNext_ = true;
					}
				}
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

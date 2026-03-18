#include "ComboNextCondition.h"


namespace Combo {

	// 開始
	void NextCondition::Enter() {
		nextTime_ = data_.stateTime;	// 移行時間
	}
	//　終了
	void NextCondition::Exit() {
	}
	// 更新
	void NextCondition::Update(const Character::CharacterContext& ctx, float time) {
		ConditionFunction::ConditionTypeUpdate(ctx,data_.type, data_.button_, nextTime_, data_.stateTime, isPress_);
	}


}

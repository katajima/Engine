#include "ComboNextCondition.h"


namespace Combo {

	// 開始
	void NextCondition::Enter(const GlobalCondition& data) {
		nextTime_ = data.stateNextTime;	// 移行時間
	}
	//　終了
	void NextCondition::Exit() {
	}
	// 更新
	void NextCondition::Update(const Character::CharacterContext& ctx, const GlobalCondition& data, float time) {
		ConditionFunction::ConditionTypeUpdate(ctx,data.endConditionType, button_, nextTime_, data.stateNextTime, isPress_);
	}


}

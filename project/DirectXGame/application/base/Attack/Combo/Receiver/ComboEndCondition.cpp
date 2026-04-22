#include "ComboEndCondition.h"

namespace Combo {

	// 開始
	void EndCondition::Enter(const GlobalCondition& data) {
		endTime_ = data.stateEndTime;	// 終了時間
	};
	
	//　終了
	void EndCondition::Exit() {

	};
	
	// 更新
	void EndCondition::Update(const Character::CharacterContext& ctx, const GlobalCondition& data,float time) {
		ConditionFunction::ConditionTypeUpdate(ctx, data.endConditionType,button_, endTime_, data.stateEndTime,isPress_);
	};
};
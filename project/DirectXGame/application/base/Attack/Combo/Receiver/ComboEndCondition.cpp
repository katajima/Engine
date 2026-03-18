#include "ComboEndCondition.h"

namespace Combo {

	// 開始
	void EndCondition::Enter() {
		endTime_ = data_.stateTime;	// 終了時間
	};
	
	//　終了
	void EndCondition::Exit() {

	};
	
	// 更新
	void EndCondition::Update(const Character::CharacterContext& ctx,float time) {
		ConditionFunction::ConditionTypeUpdate(ctx, data_.type,data_.button_, endTime_, data_.stateTime,isPress_);
	};
};
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
	void EndCondition::Update(const InputSystem& inputSystem, MovementComponent* movementComponent, float time,float dt) {
		ConditionFunction::ConditionTypeUpdate(inputSystem, movementComponent, data_.type,data_.button_, endTime_, data_.stateTime,isPress_,dt);
	};
};
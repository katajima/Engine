#include "ComboNextCondition.h"


namespace Combo {

	// 開始
	void NextCondition::Enter(const GlobalCondition& data) {
		nextTime_ = data.stateNextTime;	// 移行時間
		// 新しいノード開始時は、ボタン保持条件を評価できる状態に戻す
		isPress_ = true;
	}
	//　終了
	void NextCondition::Exit() {
		// 次回のノード開始へ押下状態を持ち越さない
		isPress_ = false;
	}
	// 更新
	void NextCondition::Update(const Character::CharacterContext& ctx, const GlobalCondition& data, float time) {
		ConditionFunction::ConditionTypeUpdate(ctx,data.endConditionType, button_, nextTime_, data.stateNextTime, isPress_);
	}


}

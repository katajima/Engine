#include "ComboEndCondition.h"

namespace Combo {

	// 開始
	void EndCondition::Enter(const GlobalCondition& data) {
		endTime_ = data.stateEndTime;	// 終了時間
		// 新しいノード開始時は、ボタン保持条件を評価できる状態に戻す
		isPress_ = true;
	};
	
	//　終了
	void EndCondition::Exit() {
		// 次回のノード開始へ押下状態を持ち越さない
		isPress_ = false;
	};
	
	// 更新
	void EndCondition::Update(const Character::CharacterContext& ctx, const GlobalCondition& data,float time) {
		ConditionFunction::ConditionTypeUpdate(ctx, data.endConditionType,button_, endTime_, data.stateEndTime,isPress_);
	};
};

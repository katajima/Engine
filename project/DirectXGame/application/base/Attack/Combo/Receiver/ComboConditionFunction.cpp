#include "ComboConditionFunction.h"
#include"DirectXGame/application/base/Move/Base/MoveComponent.h"

void Combo::ConditionFunction::ConditionTypeUpdate(const InputSystem& inputSystem, JumpRequest* jumpSystem, EndConditionType type, ComboButton button, float timer, float endTime, bool isPress, float dt){
	// 終了タイプ
	switch (type)
	{
	case Combo::EndConditionType::kOnGround:	// 着地したら
		if (!jumpSystem->GetIsLanding()) {
			timer += dt;
		}
		break;
	case Combo::EndConditionType::kOnButtonRelease:	// 押し続けているなら
		if (button.IsPressed(inputSystem)) {
			if (isPress) {
				timer += dt;
			}
		}
		else {
			isPress = false;
		}
		break;
	case Combo::EndConditionType::kOnMeterEmpty:	// 何かのメータが空なら

		break;
	case Combo::EndConditionType::kOnTimer:	// 時間が過ぎたら
		timer = endTime;
		break;
	case Combo::EndConditionType::kOnHit:	// 何かに当たったら

		break;
	case Combo::EndConditionType::kManual:	// 特殊

		break;
	default:
		break;
	}
}

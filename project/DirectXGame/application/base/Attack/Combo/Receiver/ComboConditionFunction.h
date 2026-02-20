#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"

// 前方宣言
class JumpSystem;			// ジャンプシステム

namespace Combo {

	class ConditionFunction {
	public:
		// 終了条件によっての処理
		static void ConditionTypeUpdate(const InputSystem& inputSystem, JumpSystem* jumpSystem,  EndConditionType type, ComboButton button,
		float timer, float endTime, bool isPress, float dt);
	};

}

#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"

// 前方宣言
class JumpRequest;			// ジャンプシステム

namespace Combo {

	class ConditionFunction {
	public:
		// 終了条件によっての処理
		static void ConditionTypeUpdate(const InputSystem& inputSystem, JumpRequest* jumpSystem,  EndConditionType type, ComboButton button,
		float timer, float endTime, bool isPress, float dt);
	};

}

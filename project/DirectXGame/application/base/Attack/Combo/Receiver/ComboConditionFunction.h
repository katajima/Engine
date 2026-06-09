#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"

// 前方宣言
class MovementComponent;			// ジャンプシステム

namespace Combo {

	/// <summary>
	/// コンボの終了条件タイプごとのタイマー更新をまとめる補助クラス。
	/// </summary>
	class ConditionFunction {
	public:
		// 終了条件によってタイマーや押下状態を更新する
		static void ConditionTypeUpdate(const Character::CharacterContext& ctx, EndConditionType type, ComboButton button,
		float timer, float endTime, bool isPress);
	};

}

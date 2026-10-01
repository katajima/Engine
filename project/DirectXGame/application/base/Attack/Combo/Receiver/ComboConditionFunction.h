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
		/// <summary>
		/// 終了条件によってタイマーと押下状態を更新する。
		/// </summary>
		/// <param name="ctx">現在フレームのキャラクターコンテキストです。</param>
		/// <param name="type">評価する終了条件の種類です。</param>
		/// <param name="button">条件判定に使用するボタンです。</param>
		/// <param name="timer">更新対象の経過タイマーです。</param>
		/// <param name="endTime">条件が成立する基準時間です。</param>
		/// <param name="isPress">押下継続状態の更新対象です。</param>
		static void ConditionTypeUpdate(const Character::CharacterContext& ctx, EndConditionType type, ComboButton button,
		float& timer, float endTime, bool& isPress);
	};

}

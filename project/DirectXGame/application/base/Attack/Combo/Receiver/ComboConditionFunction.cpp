#include "ComboConditionFunction.h"
#include <DirectXGame/application/base/Character/Base/CharacterContext.h>

void Combo::ConditionFunction::ConditionTypeUpdate(const Character::CharacterContext& ctx,EndConditionType type, ComboButton button, float timer, float endTime, bool isPress){
	// 終了条件の種類に応じて、コンボ終了判定に使うタイマーを進める
	// 終了タイプ
	switch (type)
	{
	case Combo::EndConditionType::kOnGround:	// 着地したら
		// 空中にいる間だけタイマーを進め、着地で終了条件を満たす
		if (!ctx.onGround) {
			timer += ctx.dt;
		}
		break;
	case Combo::EndConditionType::kOnButtonRelease:	// 押し続けているなら
		// 指定ボタンを押し続けている間だけ受付時間を伸ばす
		if (button.IsPressed(*ctx.input)) {
			if (isPress) {
				timer += ctx.dt;
			}
		}
		else {
			isPress = false;
		}
		break;
	case Combo::EndConditionType::kOnMeterEmpty:	// 何かのメータが空なら
		// メーター連携は今後の拡張用

		break;
	case Combo::EndConditionType::kOnTimer:	// 時間が過ぎたら
		// タイマー終了型は終了時刻へ直接合わせる
		timer = endTime;
		break;
	case Combo::EndConditionType::kOnHit:	// 何かに当たったら
		// ヒット通知連携は今後の拡張用

		break;
	case Combo::EndConditionType::kManual:	// 特殊
		// 手動終了型は外部側で終了を制御する

		break;
	default:
		break;
	}
}

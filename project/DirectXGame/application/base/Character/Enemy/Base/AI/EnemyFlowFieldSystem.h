#pragma once
#include <DirectXGame/engine/struct/Vector3.h>

namespace Character {
	class BaseEnemy;

	/// <summary>
	/// 個別の追跡処理に渡す、移動先へ向かう基本方向を求めるシステム
	/// </summary>
	class EnemyFlowFieldSystem {
	public:
		// 敵の現在位置から指定された移動先への水平移動方向を返す
		Vector3 CalculateDirection(BaseEnemy* owner, const Vector3& targetPos) const;

	private:
		// 高さを除外して移動に使用するXZ方向だけを正規化する
		Vector3 NormalizeXZ(const Vector3& value) const;
	};
}

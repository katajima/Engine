#include "EnemyFlowFieldSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"

namespace Character {
	Vector3 EnemyFlowFieldSystem::CalculateDirection(BaseEnemy* owner, const Vector3& targetPos) const {
		if (!owner) {
			return {};
		}

		// 地上での群衆移動なので、上下差は流れ方向へ含めない
		Vector3 flow = targetPos - owner->GetWorldPosition();
		flow.y = 0.0f;
		return NormalizeXZ(flow);
	}

	Vector3 EnemyFlowFieldSystem::NormalizeXZ(const Vector3& value) const {
		Vector3 result = value;
		result.y = 0.0f;
		float length = result.Length();
		if (length <= 0.001f) {
			return {};
		}

		return result / length;
	}
}

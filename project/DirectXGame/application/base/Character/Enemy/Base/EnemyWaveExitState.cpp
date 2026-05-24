#include "EnemyWaveExitState.h"
#include "BaseEnemy.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"

#include <algorithm>

namespace Character {
	void EnemyWaveExitState::Enter(BaseEnemy* enemy, float duration) {
		if (!enemy || isActive_ || !enemy->GetAlive()) {
			return;
		}

		isActive_ = true;
		hasStarted_ = true;
		elapsedTime_ = 0.0f;
		duration_ = (std::max)(duration, 0.1f);
		startScale_ = enemy->GetObjectComponent()->GetWorldTransform().scale_;
		startShadowScale_ = enemy->GetObjectComponentShadow()->GetWorldTransform().scale_;
		startHeight_ = enemy->GetObjectComponent()->GetWorldTransform().translate_.y;
	}

	void EnemyWaveExitState::Update(BaseEnemy* enemy, float dt) {
		if (!isActive_ || !enemy) {
			return;
		}

		elapsedTime_ += dt;
		const float progress = std::clamp(elapsedTime_ / duration_, 0.0f, 1.0f);
		// 始点と終点で速度が急変しない補間にし、退場の開始と終了を柔らかく見せる
		const float eased = progress * progress * (3.0f - 2.0f * progress);
		const float remain = 1.0f - eased;

		Engine::WorldTransform& world = enemy->GetObjectComponent()->GetWorldTransform();
		world.scale_ = startScale_ * remain;
		world.translate_.y = startHeight_ - eased * 1.5f;
		enemy->GetObjectComponent()->SetColor({ 1.0f, 1.0f, 1.0f, remain });
		enemy->GetObjectComponent()->Update();

		// 本体より少し早く影を小さくし、地面へ吸い込まれる印象を支える
		Engine::WorldTransform& shadow = enemy->GetObjectComponentShadow()->GetWorldTransform();
		shadow.scale_ = startShadowScale_ * remain;
		enemy->GetObjectComponentShadow()->SetColor({ 0.0f, 0.0f, 0.0f, remain });
		enemy->GetObjectComponentShadow()->Update();

		if (progress < 1.0f) {
			return;
		}

		isActive_ = false;
		enemy->SetAlive(false);
		enemy->Delete();
		enemy->GetObjectComponent()->IsDelete();
		enemy->GetObjectComponentShadow()->IsDelete();
		enemy->GetObjectComponent()->SetIsDraw(false);
		enemy->GetObjectComponentShadow()->SetIsDraw(false);
	}
}

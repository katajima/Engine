#include "EnemyAiSystem.h"

namespace Character {

	void EnemyAiSystem::Initialize() {
		// 攻撃要求を先に準備し、移動側から参照できる状態にする
		attackRequestSystem_ = std::make_unique<EnemyAttackRequestSystem>();
		attackRequestSystem_->Initialize();

		// 攻撃可能な立ち位置を作るシステムを準備する
		attackSlotSystem_ = std::make_unique<EnemyAttackSlotSystem>();
		attackSlotSystem_->Initialize();

		// 群衆の隊形とフロッキングを計算するシステムを準備する
		crowdSystem_ = std::make_unique<EnemyCrowdSystem>();
		crowdSystem_->Initialize();
	}

	void EnemyAiSystem::UpdateSlot(
		const std::vector<BaseEnemy*>& enemies,
		const Vector3& targetPos,
		float targetRotateY,
		float dt
	) {
		// 先に群衆の移動目標を決め、攻撃スロットは同じターゲット情報で同期させる
		crowdSystem_->Update(enemies, targetPos, targetRotateY, dt);
		attackSlotSystem_->Update(enemies, targetPos, targetRotateY, dt);
	}

	void EnemyAiSystem::UpdateRequest(const std::vector<BaseEnemy*>& enemies, float dt) {
		// 攻撃要求は移動更新とは分け、攻撃タイミング側の状態だけを更新する
		attackRequestSystem_->Update(enemies, dt);
	}
}

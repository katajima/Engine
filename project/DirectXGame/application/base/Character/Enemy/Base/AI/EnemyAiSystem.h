#pragma once
#include "EnemyAttackRequestSystem.h"
#include "EnemyAttackSlotSystem.h"
#include "EnemyCrowdSystem.h"
#include <memory>

namespace Character {

	class BaseEnemy;

	/// <summary>
	/// 敵AIの攻撃要求、攻撃位置、群衆移動をまとめて更新する統合システム。
	/// </summary>
	class EnemyAiSystem {
	public:

		// 初期化
		void Initialize();

		// 群衆移動と攻撃スロットを更新し、各敵の移動目標を決める
		void UpdateSlot(
			const std::vector<BaseEnemy*>& enemies,
			const Vector3& targetPos,
			float targetRotateY,
			float dt
		);

		// 攻撃したい敵の要求を整理し、攻撃許可を割り当てる
		void UpdateRequest(const std::vector<BaseEnemy*>& enemies, float dt);

		// 攻撃スロット情報を外部システムから参照する
		EnemyAttackSlotSystem* GetAttackSlotSystem() const { return attackSlotSystem_.get(); }
		// 群衆移動情報を外部システムから参照する
		EnemyCrowdSystem* GetCrowdSystem() const { return crowdSystem_.get(); }

	private:
		// 敵ごとの攻撃要求と攻撃許可を管理する
		std::unique_ptr<EnemyAttackRequestSystem> attackRequestSystem_ = nullptr;
		// プレイヤー周囲の攻撃位置を管理する
		std::unique_ptr<EnemyAttackSlotSystem> attackSlotSystem_ = nullptr;
		// 敵の隊形、役割、移動目標を管理する
		std::unique_ptr<EnemyCrowdSystem> crowdSystem_ = nullptr;
	};
}

#pragma once
#include "EnemyAttackRequest.h"
#include <vector>
#include <cstdint>


namespace Character {
	class BaseEnemy;

	class EnemyAttackRequestSystem {
	public:
		void Initialize();

		void Update(const std::vector<BaseEnemy*>& enemies, float dt);

	private:
		uint32_t requestCounter_ = 1;

		// 同時に攻撃できる敵数
		uint32_t maxAttackCount_ = 1;

		// 攻撃待ちの中でも前に詰める敵数
		uint32_t approachRingCount_ = 3;

		// 攻撃許可の間隔
		float attackInterval_ = 0.1f;
		float intervalTimer_ = 0.0f;

		// 攻撃後、同じ敵が再攻撃できない時間
		float reAttackCooldown_ = 3.0f;

		// 距離差がこの値以内なら要求順を優先
		float sameDistanceRange_ = 1.0f;
	};
}

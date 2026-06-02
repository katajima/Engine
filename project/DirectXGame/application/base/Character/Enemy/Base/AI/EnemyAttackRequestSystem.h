#pragma once
#include "EnemyAttackRequest.h"
#include "EnemyCrowdBehaviorData.h"
#include <vector>
#include <cstdint>


namespace Character {
	class BaseEnemy;

	class EnemyAttackRequestSystem {
	public:
		void Initialize();

		void Update(const std::vector<BaseEnemy*>& enemies, float dt);

	private:
		struct AttackCandidate {
			BaseEnemy* enemy = nullptr;
			EnemyAttackRequest* request = nullptr;
		};

		// 群衆タイプと設定から、攻撃許可の出し方を決める
		EnemyCrowdAttackStyle ResolveAttackStyle(const BaseEnemy* enemy, size_t groupSize) const;
		// 個人順で攻撃許可を出す
		void AssignIndividual(std::vector<AttackCandidate>& candidates, uint32_t& attackCount, uint32_t& approachCount);
		// 同じ群衆から複数体へ同時に攻撃許可を出す
		void AssignGroupVolley(std::vector<AttackCandidate>& candidates, uint32_t& attackCount);
		// 統率者だけを攻撃役へ回し、他メンバーは接近・待機へ回す
		void AssignLeaderOnly(std::vector<AttackCandidate>& candidates, uint32_t& attackCount);
		// 包囲がある程度成立した時だけ複数体へ攻撃許可を出す
		void AssignSurroundThenAttack(std::vector<AttackCandidate>& candidates, uint32_t& attackCount, uint32_t& approachCount);

		uint32_t requestCounter_ = 1;

		// 同時に攻撃できる敵数
		uint32_t maxAttackCount_ = 2;

		// 攻撃待ちの中でも前に詰める敵数
		uint32_t approachRingCount_ = 2;

		// 攻撃許可の間隔
		float attackInterval_ = 0.1f;
		float intervalTimer_ = 0.0f;

		// 攻撃後、同じ敵が再攻撃できない時間
		float reAttackCooldown_ = 3.0f;

		// 距離差がこの値以内なら要求順を優先
		float sameDistanceRange_ = 1.0f;

		uint32_t maxGroupAttackCount_ = 4;	// 一斉攻撃で同時に許可する最大数
		uint32_t surroundReadyCount_ = 3;	// 包囲型で攻撃開始とみなす最低人数
		float surroundReadyDistance_ = 12.0f;	// 包囲成立判定に使う距離
	};
}

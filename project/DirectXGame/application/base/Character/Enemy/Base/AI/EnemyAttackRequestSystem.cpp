#include "EnemyAttackRequestSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Enemy/Base/AI/EnemyAttackSystem.h"

#include <algorithm>
#include <cmath>

namespace Character {

	void EnemyAttackRequestSystem::Initialize() {
		requestCounter_ = 1;
		intervalTimer_ = 0.0f;
	}

	void EnemyAttackRequestSystem::Update(const std::vector<BaseEnemy*>& enemies, float dt) {
		if (intervalTimer_ > 0.0f) {
			intervalTimer_ -= dt;
			if (intervalTimer_ < 0.0f) {
				intervalTimer_ = 0.0f;
			}
		}

		std::vector<EnemyAttackRequest*> activeRequests;

		for (BaseEnemy* enemy : enemies) {
			if (!enemy) {
				continue;
			}

			EnemyAttackSystem* attackSystem = enemy->GetEnemyAttackSystem();
			if (!attackSystem) {
				continue;
			}

			EnemyAttackRequest* request = attackSystem->GetAttackRequest();
			if (!request) {
				continue;
			}

			request->UpdateCooldown(dt);
			request->Deny();

			if (!request->GetIsActive()) {
				continue;
			}

			// 近すぎて離脱中の敵は、十分に距離を取るまで攻撃役へ戻さない
			// ここでAttackに再割当てすると、攻撃スロットと後退の間を往復して震える
			if (request->GetRing() == EnemyAttackRing::TooClose) {
				continue;
			}

			// クールダウン中は攻撃候補に入れない
			if (request->IsCooldown()) {
				continue;
			}

			if (request->GetNumber() == 0) {
				request->SetNumber(requestCounter_++);
			}

			float distance = enemy->GetTargetDistance();
			request->SetDistance(distance);

			activeRequests.push_back(request);
		}

		if (activeRequests.empty()) {
			return;
		}

		if (intervalTimer_ > 0.0f) {
			return;
		}

		std::sort(activeRequests.begin(), activeRequests.end(),
			[this](const EnemyAttackRequest* a, const EnemyAttackRequest* b) {
				float distanceDiff = std::abs(a->GetDistance() - b->GetDistance());

				// 距離差が大きいなら近い敵を優先
				if (distanceDiff > sameDistanceRange_) {
					return a->GetDistance() < b->GetDistance();
				}

				// 近い距離帯なら先に要求した敵を優先
				return a->GetNumber() < b->GetNumber();
			});

		uint32_t attackRingCount = 0;
		uint32_t approachRingCount = 0;

		for (EnemyAttackRequest* request : activeRequests) {
			if (attackRingCount < maxAttackCount_) {
				request->SetRing(EnemyAttackRing::Attack);
				request->Grant();
				attackRingCount++;
				continue;
			}

			if (approachRingCount < approachRingCount_) {
				request->SetRing(EnemyAttackRing::Approach);
				approachRingCount++;
				continue;
			}

			request->SetRing(EnemyAttackRing::Standby);
		}

		intervalTimer_ = attackInterval_;
	}
}

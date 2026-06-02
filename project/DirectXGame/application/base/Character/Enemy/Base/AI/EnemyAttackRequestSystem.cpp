#include "EnemyAttackRequestSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Enemy/Base/AI/EnemyAttackSystem.h"

#include <algorithm>
#include <cmath>
#include <map>

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

		std::vector<AttackCandidate> activeRequests;

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

			activeRequests.push_back(AttackCandidate{ enemy, request });
		}

		if (activeRequests.empty()) {
			return;
		}

		if (intervalTimer_ > 0.0f) {
			return;
		}

		std::sort(activeRequests.begin(), activeRequests.end(),
			[this](const AttackCandidate& a, const AttackCandidate& b) {
				float distanceDiff = std::abs(a.request->GetDistance() - b.request->GetDistance());

				// 距離差が大きいなら近い敵を優先
				if (distanceDiff > sameDistanceRange_) {
					return a.request->GetDistance() < b.request->GetDistance();
				}

				// 近い距離帯なら先に要求した敵を優先
				return a.request->GetNumber() < b.request->GetNumber();
			});

		// 群衆IDごとにまとめ、集団タイプに応じて攻撃の出し方を変える
		std::map<int, std::vector<AttackCandidate>> groups;
		for (const AttackCandidate& candidate : activeRequests) {
			groups[candidate.enemy->GetCrowdGroupId()].push_back(candidate);
		}

		uint32_t attackRingCount = 0;
		uint32_t approachRingCount = 0;

		for (auto& [groupId, candidates] : groups) {
			(void)groupId;
			if (candidates.empty()) {
				continue;
			}

			std::sort(candidates.begin(), candidates.end(),
				[](const AttackCandidate& a, const AttackCandidate& b) {
					return a.enemy->GetCrowdMemberIndex() < b.enemy->GetCrowdMemberIndex();
				});

			switch (ResolveAttackStyle(candidates.front().enemy, candidates.size())) {
			case EnemyCrowdAttackStyle::GroupVolley:
				AssignGroupVolley(candidates, attackRingCount);
				break;
			case EnemyCrowdAttackStyle::LeaderOnly:
				AssignLeaderOnly(candidates, attackRingCount);
				break;
			case EnemyCrowdAttackStyle::SurroundThenAttack:
				AssignSurroundThenAttack(candidates, attackRingCount, approachRingCount);
				break;
			case EnemyCrowdAttackStyle::Individual:
			case EnemyCrowdAttackStyle::Auto:
			default:
				AssignIndividual(candidates, attackRingCount, approachRingCount);
				break;
			}
		}

		intervalTimer_ = attackInterval_;
	}

	EnemyCrowdAttackStyle EnemyAttackRequestSystem::ResolveAttackStyle(const BaseEnemy* enemy, size_t groupSize) const {
		if (!enemy) {
			return EnemyCrowdAttackStyle::Individual;
		}

		const CrowdBehaviorSettings& behavior = enemy->GetCrowdBehavior();
		if (behavior.attackStyle != EnemyCrowdAttackStyle::Auto) {
			return behavior.attackStyle;
		}

		// Autoでは群衆タイプの性格から攻撃方法を決める
		switch (behavior.type) {
		case EnemyCrowdBehaviorType::WaveAssault:
		case EnemyCrowdBehaviorType::PressureCrowd:
			return EnemyCrowdAttackStyle::GroupVolley;
		case EnemyCrowdBehaviorType::Encircle:
		case EnemyCrowdBehaviorType::DistributedEncircle:
			return EnemyCrowdAttackStyle::SurroundThenAttack;
		case EnemyCrowdBehaviorType::Formation:
			return behavior.useCommander ? EnemyCrowdAttackStyle::LeaderOnly : EnemyCrowdAttackStyle::GroupVolley;
		case EnemyCrowdBehaviorType::DensityAdaptive:
			return groupSize >= 8 ? EnemyCrowdAttackStyle::GroupVolley : EnemyCrowdAttackStyle::Individual;
		case EnemyCrowdBehaviorType::Rush:
		case EnemyCrowdBehaviorType::Flocking:
		case EnemyCrowdBehaviorType::FlowCrowd:
		case EnemyCrowdBehaviorType::Spiral:
		case EnemyCrowdBehaviorType::Hunting:
		default:
			return EnemyCrowdAttackStyle::Individual;
		}
	}

	void EnemyAttackRequestSystem::AssignIndividual(std::vector<AttackCandidate>& candidates, uint32_t& attackCount, uint32_t& approachCount) {
		std::sort(candidates.begin(), candidates.end(),
			[this](const AttackCandidate& a, const AttackCandidate& b) {
				const float distanceDiff = std::abs(a.request->GetDistance() - b.request->GetDistance());
				if (distanceDiff > sameDistanceRange_) {
					return a.request->GetDistance() < b.request->GetDistance();
				}
				return a.request->GetNumber() < b.request->GetNumber();
			});

		for (AttackCandidate& candidate : candidates) {
			if (attackCount < maxAttackCount_) {
				candidate.request->SetRing(EnemyAttackRing::Attack);
				candidate.request->Grant();
				attackCount++;
				continue;
			}

			if (approachCount < approachRingCount_) {
				candidate.request->SetRing(EnemyAttackRing::Approach);
				approachCount++;
				continue;
			}

			candidate.request->SetRing(EnemyAttackRing::Standby);
		}
	}

	void EnemyAttackRequestSystem::AssignGroupVolley(std::vector<AttackCandidate>& candidates, uint32_t& attackCount) {
		const uint32_t groupLimit = (std::min)(maxGroupAttackCount_, static_cast<uint32_t>(candidates.size()));
		for (uint32_t i = 0; i < candidates.size(); ++i) {
			AttackCandidate& candidate = candidates[i];
			if (attackCount < maxGroupAttackCount_ && i < groupLimit) {
				candidate.request->SetRing(EnemyAttackRing::Attack);
				candidate.request->Grant();
				attackCount++;
			}
			else {
				candidate.request->SetRing(i < groupLimit + approachRingCount_ ? EnemyAttackRing::Approach : EnemyAttackRing::Standby);
			}
		}
	}

	void EnemyAttackRequestSystem::AssignLeaderOnly(std::vector<AttackCandidate>& candidates, uint32_t& attackCount) {
		const CrowdBehaviorSettings& behavior = candidates.front().enemy->GetCrowdBehavior();
		const uint32_t leaderOrder = behavior.useCommander ?
			(std::min)(behavior.commanderOrder, static_cast<uint32_t>(candidates.size() - 1)) :
			0;

		for (uint32_t i = 0; i < candidates.size(); ++i) {
			AttackCandidate& candidate = candidates[i];
			if (i == leaderOrder && attackCount < maxAttackCount_) {
				candidate.request->SetRing(EnemyAttackRing::Attack);
				candidate.request->Grant();
				attackCount++;
			}
			else {
				candidate.request->SetRing(i < leaderOrder + approachRingCount_ ? EnemyAttackRing::Approach : EnemyAttackRing::Standby);
			}
		}
	}

	void EnemyAttackRequestSystem::AssignSurroundThenAttack(
		std::vector<AttackCandidate>& candidates,
		uint32_t& attackCount,
		uint32_t& approachCount
	) {
		uint32_t readyCount = 0;
		for (const AttackCandidate& candidate : candidates) {
			if (candidate.request->GetDistance() <= surroundReadyDistance_) {
				readyCount++;
			}
		}

		if (readyCount < (std::min)(surroundReadyCount_, static_cast<uint32_t>(candidates.size()))) {
			for (AttackCandidate& candidate : candidates) {
				if (approachCount < approachRingCount_) {
					candidate.request->SetRing(EnemyAttackRing::Approach);
					approachCount++;
				}
				else {
					candidate.request->SetRing(EnemyAttackRing::Standby);
				}
			}
			return;
		}

		AssignGroupVolley(candidates, attackCount);
	}
}

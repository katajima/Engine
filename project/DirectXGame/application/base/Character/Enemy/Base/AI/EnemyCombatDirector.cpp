#include "EnemyCombatDirector.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace Character {

	void EnemyCombatDirector::Initialize() {
		// 現状は固定パラメータで動かす。将来ImGuiや難易度設定から変更する入口にする。
		sameDistanceRange_ = 1.0f;
		surroundReadyCount_ = 3;
		surroundReadyDistance_ = 12.0f;
	}

	void EnemyCombatDirector::Direct(std::vector<EnemyAttackCandidate>& candidates, EnemyAttackTokenSystem& tokenSystem) {
		if (candidates.empty()) {
			// 攻撃候補が無い場合は、作戦判断を行わない
			return;
		}

		// 全体の優先順を先に揃える。距離が大きく違う時は近い敵、近い距離帯では要求順を優先する。
		std::sort(candidates.begin(), candidates.end(),
			[this](const EnemyAttackCandidate& a, const EnemyAttackCandidate& b) {
				const float distanceDiff = std::abs(a.request->GetDistance() - b.request->GetDistance());
				if (distanceDiff > sameDistanceRange_) {
					return a.request->GetDistance() < b.request->GetDistance();
				}
				return a.request->GetNumber() < b.request->GetNumber();
			});

		// 群衆IDごとにまとめ、同じ群衆内で攻撃方式を共有する
		std::map<int, std::vector<EnemyAttackCandidate>> groupMap;
		for (const EnemyAttackCandidate& candidate : candidates) {
			if (!candidate.enemy || !candidate.request) {
				// 不完全な候補は攻撃判断から外す
				continue;
			}
			groupMap[candidate.enemy->GetCrowdGroupId()].push_back(candidate);
		}

		struct GroupOrder {
			int groupId = 0;				// 群衆ID
			float nearestDistance = 0.0f;	// 群衆内で一番近い敵の距離
		};

		std::vector<GroupOrder> groupOrders;
		groupOrders.reserve(groupMap.size());
		for (const auto& [groupId, groupCandidates] : groupMap) {
			if (groupCandidates.empty()) {
				continue;
			}

			// 群衆同士の順番は、一番近いメンバーの距離で決める
			groupOrders.push_back(GroupOrder{ groupId, groupCandidates.front().request->GetDistance() });
		}

		std::sort(groupOrders.begin(), groupOrders.end(),
			[](const GroupOrder& a, const GroupOrder& b) {
				return a.nearestDistance < b.nearestDistance;
			});

		for (const GroupOrder& order : groupOrders) {
			std::vector<EnemyAttackCandidate>& groupCandidates = groupMap[order.groupId];
			if (groupCandidates.empty()) {
				continue;
			}

			// 群衆内の並び順で揃え、LeaderOnlyやVolleyで安定した順番を使う
			std::sort(groupCandidates.begin(), groupCandidates.end(),
				[](const EnemyAttackCandidate& a, const EnemyAttackCandidate& b) {
					return a.enemy->GetCrowdMemberIndex() < b.enemy->GetCrowdMemberIndex();
				});

			// 群衆ごとの攻撃スタイルに応じて、Attack / Approach / Standbyへ振り分ける
			switch (ResolveAttackStyle(groupCandidates.front().enemy, groupCandidates.size())) {
			case EnemyCrowdAttackStyle::GroupVolley:
				AssignGroupVolley(groupCandidates, tokenSystem);
				break;
			case EnemyCrowdAttackStyle::LeaderOnly:
				AssignLeaderOnly(groupCandidates, tokenSystem);
				break;
			case EnemyCrowdAttackStyle::SurroundThenAttack:
				AssignSurroundThenAttack(groupCandidates, tokenSystem);
				break;
			case EnemyCrowdAttackStyle::Individual:
			case EnemyCrowdAttackStyle::Auto:
			default:
				AssignIndividual(groupCandidates, tokenSystem);
				break;
			}
		}
	}

	EnemyCrowdAttackStyle EnemyCombatDirector::ResolveAttackStyle(const BaseEnemy* enemy, size_t groupSize) const {
		if (!enemy) {
			// 敵情報が取れない場合は最も単純な個別攻撃にする
			return EnemyCrowdAttackStyle::Individual;
		}

		const CrowdBehaviorSettings& behavior = enemy->GetCrowdBehavior();
		if (behavior.attackStyle != EnemyCrowdAttackStyle::Auto) {
			// スポーンエディタなどで明示された攻撃方式を優先する
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

	void EnemyCombatDirector::AssignIndividual(std::vector<EnemyAttackCandidate>& candidates, EnemyAttackTokenSystem& tokenSystem) const {
		// 個別攻撃では近い敵を優先し、同程度なら要求順を優先する
		std::sort(candidates.begin(), candidates.end(),
			[this](const EnemyAttackCandidate& a, const EnemyAttackCandidate& b) {
				const float distanceDiff = std::abs(a.request->GetDistance() - b.request->GetDistance());
				if (distanceDiff > sameDistanceRange_) {
					return a.request->GetDistance() < b.request->GetDistance();
				}
				return a.request->GetNumber() < b.request->GetNumber();
			});

		for (EnemyAttackCandidate& candidate : candidates) {
			if (tokenSystem.TryGrantAttack(candidate.request, tokenSystem.GetIndividualAttackLimit())) {
				// 攻撃トークンを取れた敵はそのまま攻撃へ進む
				continue;
			}

			if (tokenSystem.TryAssignApproach(candidate.request)) {
				// 攻撃できないが前に詰められる敵は、次の攻撃候補にする
				continue;
			}

			// それ以外は密集を避けるため待機へ回す
			tokenSystem.AssignStandby(candidate.request);
		}
	}

	void EnemyCombatDirector::AssignGroupVolley(std::vector<EnemyAttackCandidate>& candidates, EnemyAttackTokenSystem& tokenSystem) const {
		// 一斉攻撃ではグループ内の先頭から一定数まで攻撃許可を出す
		const uint32_t groupLimit = (std::min)(tokenSystem.GetGroupAttackLimit(), static_cast<uint32_t>(candidates.size()));
		for (uint32_t i = 0; i < candidates.size(); ++i) {
			EnemyAttackCandidate& candidate = candidates[i];

			if (i < groupLimit && tokenSystem.TryGrantAttack(candidate.request, tokenSystem.GetGroupAttackLimit())) {
				// 一斉攻撃枠に入った敵だけ攻撃トークンを取る
				continue;
			}

			if (tokenSystem.TryAssignApproach(candidate.request)) {
				// 一斉攻撃枠外の近い敵は、次の波として前に出す
				continue;
			}

			tokenSystem.AssignStandby(candidate.request);
		}
	}

	void EnemyCombatDirector::AssignLeaderOnly(std::vector<EnemyAttackCandidate>& candidates, EnemyAttackTokenSystem& tokenSystem) const {
		if (candidates.empty()) {
			return;
		}

		// 統率者が指定されていればその順番を、無ければ先頭メンバーをリーダー扱いにする
		const CrowdBehaviorSettings& behavior = candidates.front().enemy->GetCrowdBehavior();
		const uint32_t leaderOrder = behavior.useCommander ?
			(std::min)(behavior.commanderOrder, static_cast<uint32_t>(candidates.size() - 1)) :
			0;

		for (uint32_t i = 0; i < candidates.size(); ++i) {
			EnemyAttackCandidate& candidate = candidates[i];
			if (i == leaderOrder && tokenSystem.TryGrantAttack(candidate.request, tokenSystem.GetIndividualAttackLimit())) {
				// リーダーだけに攻撃許可を出し、他メンバーは接近か待機にする
				continue;
			}

			if (tokenSystem.TryAssignApproach(candidate.request)) {
				continue;
			}

			tokenSystem.AssignStandby(candidate.request);
		}
	}

	void EnemyCombatDirector::AssignSurroundThenAttack(std::vector<EnemyAttackCandidate>& candidates, EnemyAttackTokenSystem& tokenSystem) const {
		uint32_t readyCount = 0;
		for (const EnemyAttackCandidate& candidate : candidates) {
			// 包囲成立判定として、一定距離内に入っている敵数を数える
			if (candidate.request->GetDistance() <= surroundReadyDistance_) {
				readyCount++;
			}
		}

		if (readyCount < (std::min)(surroundReadyCount_, static_cast<uint32_t>(candidates.size()))) {
			// 包囲ができるまでは攻撃せず、前進役と待機役に分ける
			for (EnemyAttackCandidate& candidate : candidates) {
				if (tokenSystem.TryAssignApproach(candidate.request)) {
					continue;
				}

				tokenSystem.AssignStandby(candidate.request);
			}
			return;
		}

		// 包囲が成立したら、グループ一斉攻撃として許可を出す
		AssignGroupVolley(candidates, tokenSystem);
	}
}

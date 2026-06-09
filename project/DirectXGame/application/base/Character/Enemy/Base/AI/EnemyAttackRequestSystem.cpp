#include "EnemyAttackRequestSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Enemy/Base/AI/EnemyAttackSystem.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace Character {

	void EnemyAttackRequestSystem::Initialize() {
		// 攻撃要求の連番と、攻撃許可の間隔タイマーを初期状態へ戻す
		requestCounter_ = 1;
		intervalTimer_ = 0.0f;
	}

	void EnemyAttackRequestSystem::Update(const std::vector<BaseEnemy*>& enemies, float dt) {
		
		// 毎フレーム、攻撃要求のクールダウンを進めて許可をリセット
		UpdateIntervalTimer(dt);

		std::vector<AttackCandidate> activeRequests;

		for (BaseEnemy* enemy : enemies) {
			if (!enemy) {
				// 無効な敵ポインタは候補に入れない
				continue;
			}

			EnemyAttackSystem* attackSystem = enemy->GetEnemyAttackSystem();
			if (!attackSystem) {
				// 攻撃システムを持たない敵はAIの攻撃許可対象外
				continue;
			}

			EnemyAttackRequest* request = attackSystem->GetAttackRequest();
			if (!request) {
				// 攻撃要求コンポーネントが無い場合はスキップする
				continue;
			}

			// 毎フレーム一度許可を取り消し、この更新で改めて許可を割り当てる
			request->UpdateCooldown(dt);
			request->Deny();

			if (!request->GetIsActive()) {
				// 攻撃したい状態でない敵は候補に入れない
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
				// 新規要求には連番を付け、距離が同じ時の優先順に使う
				request->SetNumber(requestCounter_++);
			}

			// ソートで使うターゲット距離を要求側へ記録する
			float distance = enemy->GetTargetDistance();
			request->SetDistance(distance);

			activeRequests.push_back(AttackCandidate{ enemy, request });
		}

		if (activeRequests.empty()) {
			// 攻撃候補が無い場合はリング割り当てを行わない
			return;
		}

		if (intervalTimer_ > 0.0f) {
			// 攻撃許可の間隔中は、前回の分類だけを維持する
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

		// 攻撃リング数
		uint32_t attackRingCount = 0;
		// 接近リング数
		uint32_t approachRingCount = 0;

		// 群衆ごとに攻撃スタイルを決めて、攻撃役・接近役・待機役へ振り分ける
		for (auto& [groupId, candidates] : groups) {
			(void)groupId;
			if (candidates.empty()) {
				continue;
			}

			// 群衆内の並び順で揃え、LeaderOnlyやVolleyで安定した順番を使う
			std::sort(candidates.begin(), candidates.end(),
				[](const AttackCandidate& a, const AttackCandidate& b) {
					return a.enemy->GetCrowdMemberIndex() < b.enemy->GetCrowdMemberIndex();
				});

			// 群衆ごとの攻撃スタイルに応じて、Attack / Approach / Standbyへ振り分ける
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

		// 今回攻撃許可を更新したので、次回まで少し間隔を空ける
		intervalTimer_ = attackInterval_;
	}

	EnemyCrowdAttackStyle EnemyAttackRequestSystem::ResolveAttackStyle(const BaseEnemy* enemy, size_t groupSize) const {
		if (!enemy) {
			// 敵情報が取れない場合は最も単純な個別攻撃にする
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
		// 個別攻撃では近い敵を優先し、同程度なら要求順を優先する
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
				// 同時攻撃上限に収まる敵だけ攻撃許可を出す
				candidate.request->SetRing(EnemyAttackRing::Attack);
				candidate.request->Grant();
				attackCount++;
				continue;
			}

			if (approachCount < approachRingCount_) {
				// 攻撃待ちの一部は前に詰め、次の攻撃候補にする
				candidate.request->SetRing(EnemyAttackRing::Approach);
				approachCount++;
				continue;
			}

			// それ以外は密集を避けるため待機レイヤーへ回す
			candidate.request->SetRing(EnemyAttackRing::Standby);
		}
	}

	void EnemyAttackRequestSystem::AssignGroupVolley(std::vector<AttackCandidate>& candidates, uint32_t& attackCount) {
		// 一斉攻撃ではグループ内の先頭から一定数まで攻撃許可を出す
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
		// 統率者が指定されていればその順番を、無ければ先頭メンバーをリーダー扱いにする
		const CrowdBehaviorSettings& behavior = candidates.front().enemy->GetCrowdBehavior();
		const uint32_t leaderOrder = behavior.useCommander ?
			(std::min)(behavior.commanderOrder, static_cast<uint32_t>(candidates.size() - 1)) :
			0;

		for (uint32_t i = 0; i < candidates.size(); ++i) {
			AttackCandidate& candidate = candidates[i];
			if (i == leaderOrder && attackCount < maxAttackCount_) {
				// リーダーだけに攻撃許可を出し、他メンバーは接近か待機にする
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
			// 包囲成立判定として、一定距離内に入っている敵数を数える
			if (candidate.request->GetDistance() <= surroundReadyDistance_) {
				readyCount++;
			}
		}

		if (readyCount < (std::min)(surroundReadyCount_, static_cast<uint32_t>(candidates.size()))) {
			// 包囲ができるまでは攻撃せず、前進役と待機役に分ける
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

		// 包囲が成立したら、グループ一斉攻撃として許可を出す
		AssignGroupVolley(candidates, attackCount);
	}

	void EnemyAttackRequestSystem::UpdateIntervalTimer(float dt){
		if (intervalTimer_ > 0.0f) {
			// 攻撃許可を連続で出しすぎないよう、間隔タイマーを進める
			intervalTimer_ -= dt;
			if (intervalTimer_ < 0.0f) {
				intervalTimer_ = 0.0f;
			}
		}
	}
}

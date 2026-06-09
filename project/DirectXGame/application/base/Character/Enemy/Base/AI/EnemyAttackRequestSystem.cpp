#include "EnemyAttackRequestSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Enemy/Base/AI/EnemyAttackSystem.h"

namespace Character {

	void EnemyAttackRequestSystem::Initialize() {
		// 攻撃要求の連番と、攻撃許可の間隔タイマーを初期状態へ戻す
		requestCounter_ = 1;
		intervalTimer_ = 0.0f;

		// Attack Tokenは攻撃数の制限、Combat Directorは群衆ごとの攻撃判断を担当する
		attackTokenSystem_.Initialize();
		attackTokenSystem_.SetLimits(maxAttackCount_, maxGroupAttackCount_, approachRingCount_);
		combatDirector_.Initialize();
	}

	void EnemyAttackRequestSystem::Update(const std::vector<BaseEnemy*>& enemies, float dt) {
		// 毎フレーム、攻撃要求のクールダウンを進めて許可をリセット
		UpdateIntervalTimer(dt);
		attackTokenSystem_.BeginFrame();

		std::vector<EnemyAttackCandidate> activeRequests;

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
			const float distance = enemy->GetTargetDistance();
			request->SetDistance(distance);

			activeRequests.push_back(EnemyAttackCandidate{ enemy, request });
		}

		if (activeRequests.empty()) {
			// 攻撃候補が無い場合はリング割り当てを行わない
			return;
		}

		if (intervalTimer_ > 0.0f) {
			// 攻撃許可の間隔中は、前回の分類だけを維持する
			return;
		}

		// Combat Directorへ渡し、群衆タイプに応じた攻撃方式でAttack Tokenを配る
		combatDirector_.Direct(activeRequests, attackTokenSystem_);

		// 今回攻撃許可を更新したので、次回まで少し間隔を空ける
		intervalTimer_ = attackInterval_;
	}

	void EnemyAttackRequestSystem::UpdateIntervalTimer(float dt) {
		if (intervalTimer_ > 0.0f) {
			// 攻撃許可を連続で出しすぎないよう、間隔タイマーを進める
			intervalTimer_ -= dt;
			if (intervalTimer_ < 0.0f) {
				intervalTimer_ = 0.0f;
			}
		}
	}
}

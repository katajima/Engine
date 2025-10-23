#include "CollisionManager.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"


void CollisionManager::CheckAll() {
	debugTimer_.StartTimer();

	// ==== 動的Octreeを再構築 ====
	octreeCollider_->Clear();

	// 動的コライダーを挿入（enabledチェック）
	for (auto* colliderComp : dynamicColliders) {
		for (auto* collider : colliderComp->GetAllColliders()) {
			if (collider && collider->enabled) {
				octreeCollider_->Insert(collider);
			}
		}
	}

	// 再利用する候補ベクタを外側で用意（アロケーション削減）
	std::vector<Collider*> candidates;
	std::vector<Collider*> staticCandidates;
	candidates.reserve(64);
	staticCandidates.reserve(64);




	// ==== 動的 vs 動的 ====
	for (auto* colliderComp : dynamicColliders) {
		for (auto* collider : colliderComp->GetAllColliders()) {
			if (!collider || !collider->enabled) continue;

			// 自分のAABBを一度だけ取得
			AABB selfAabb = collider->GetAABB();

			// Query して候補取得
			candidates.clear();
			octreeCollider_->Query(selfAabb, candidates);

			// seen: 重複候補（同じポインタ）を除去するため
			std::unordered_set<Collider*> seen;
			seen.reserve(64);

			for (auto* other : candidates) {
				if (!other || !other->enabled) continue;

				// 同一ポインタの重複を排除
				if (!seen.insert(other).second) continue;

				// 自分自身をスキップ
				if (collider == other) continue;

				// 同一owner 内の自己衝突はスキップ（必要ならコメントアウト）
				if (collider->owner && other->owner && collider->owner == other->owner) continue;

				// 二重判定回避：ポインタアドレス順で一方向のみ処理
				// （同じペアを collider->other、other->collider の両方で処理しない）
				if (collider >= other) continue;

				// レイヤー・マスクチェック（Broad filter）
				if (!CheckMask(collider, other)) continue;

				// Narrow phase（詳細判定）
				if (collider->CheckHit(*other)) {
					NotifyHit(colliderComp, collider, other);
				}
			}
		}
	}

	// ==== 動的 vs 静的 ====
	for (auto* colliderComp : dynamicColliders) {
		for (auto* collider : colliderComp->GetAllColliders()) {
			if (!collider || !collider->enabled) continue;

			AABB selfAabb = collider->GetAABB();

			staticCandidates.clear();
			octreeColliderStatic_->Query(selfAabb, staticCandidates);

			// 静的候補は重複が少ない想定だが、念のため seenStatic を用意しても良い
			std::unordered_set<Collider*> seenStatic;
			seenStatic.reserve(32);

			for (auto* other : staticCandidates) {
				if (!other || !other->enabled) continue;
				if (!seenStatic.insert(other).second) continue;

				// 同一owner は基本的に起こらないがチェック
				if (collider->owner && other->owner && collider->owner == other->owner) continue;

				// Mask check
				if (!CheckMask(collider, other)) continue;

				if (collider->CheckHit(*other)) {
					NotifyHit(colliderComp, collider, other);
				}
			}
		}
	}

	debugTimer_.EndTimer();
	debugTimer_.LogTimeSec("");
}
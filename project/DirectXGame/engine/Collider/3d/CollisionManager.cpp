#include "CollisionManager.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"



void Engine::CollisionManager::Initialize(GlobalVariables* globalVariables, const AABB& sceneBounds) {
	this->globalVariables = globalVariables;	// 保存項目

	float size = (sceneBounds.max - sceneBounds.min).Length();

	int depth = 4;
	if (size > 1000.0f) depth = 5;
	else if (size < 50.0f) depth = 3;

	// オクツリー初期化（シーン全体のAABBと深さなど指定）
	octreeCollider_ = std::make_unique<OctreeCollider>(sceneBounds, depth, 3, 3, 3);
	octreeColliderStatic_ = std::make_unique<OctreeCollider>(sceneBounds, depth, 2, 2, 2);
	}

void Engine::CollisionManager::DrawLine(LineCommon* lineCommon) {
	//octreeCollider_->Draw(*lineCommon);
	octreeColliderStatic_->Draw(*lineCommon);
}

void Engine::CollisionManager::BuildStaticSceneOctree()
{
	if (!octreeColliderStatic_) return;
	// 静的コライダーの登録（地形など）
	debugTimer_.StartTimer();
	for (auto* staticComp : staticColliders) {
		for (auto* collider : staticComp->GetAllColliders()) {
			if (collider->enabled) {
				octreeColliderStatic_->Insert(collider);
			}
		}
	}
	debugTimer_.LogTimeSec("BuildStaticSceneOctree");
	debugTimer_.EndTimer();
}

void Engine::CollisionManager::BuildDynamicSceneOctree() {
	if (!octreeCollider_) return;
	// 動的コライダーの登録
	for (auto* staticComp : dynamicColliders) {
		for (auto* collider : staticComp->GetAllColliders()) {
			if (collider->enabled) {
				octreeCollider_->Insert(collider);
			}
		}
	}
}

void Engine::CollisionManager::Register(ColliderComponent* comp)
{
	if (comp && registeredDynamic_.insert(comp).second) {
		dynamicColliders.push_back(comp);
	}
}

void Engine::CollisionManager::RegisterStatic(ColliderComponent* comp)
{
	if (comp && registeredStatic_.insert(comp).second) {
		staticColliders.push_back(comp);
	}

}

void Engine::CollisionManager::Clear(){
	ClearDynamic();
	ClearStatic();
}

void Engine::CollisionManager::CheckAll() {
	debugTimer_.StartTimer();
	// ==== 動的 vs 動的 ====
	CheckDynamicVsDynamicMT();
	debugTimer_.LogTimeSec("DynamicVsDynamic");
	debugTimer_.EndTimer();

	debugTimer_.StartTimer();
	// ==== 動的 vs 静的 ====
	CheckDynamicVsStaticMT();
	debugTimer_.LogTimeSec("DynamicVsStatic");
	debugTimer_.EndTimer();
}

void Engine::CollisionManager::CheckByLayer(ColliderComponent& a, ColliderComponent& b) {
	for (auto* colA : a.GetAllColliders()) {
		for (auto* colB : b.GetAllColliders()) {
			if (!colA->enabled || !colB->enabled) continue;

			// ビットマスク判定（どちらかが相手を対象にしていないならスキップ）
			if (!((1 << static_cast<uint32_t>(colB->layer)) & colA->collisionMask) ||
				!((1 << static_cast<uint32_t>(colA->layer)) & colB->collisionMask)) {
				continue;
			}

			if (colA->CheckHit(*colB)) {
				if (a.onHitCallback) a.onHitCallback(colA, colB);
				if (b.onHitCallback) b.onHitCallback(colB, colA);
			}
		}
	}
}

void Engine::CollisionManager::CheckDynamicVsDynamicMT()
{
	// ==== 動的Octreeを再構築 ====
	octreeCollider_->Clear();

	// 動的コライダーを挿入（enabledチェック）
	BuildDynamicSceneOctree();


	// ==========================
	// 動的 vs 動的（マルチスレッド）
	// ==========================
	{
		const size_t jobCount = dynamicColliders.size();
		if (jobCount > 0) {

			const uint32_t threadCount = (std::max)(1u, std::thread::hardware_concurrency());
			const size_t chunkSize = (jobCount + threadCount - 1) / threadCount;

			std::vector<std::future<std::vector<HitPair>>> futures;
			futures.reserve(threadCount);

			for (uint32_t threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
				const size_t begin = threadIndex * chunkSize;
				const size_t end = (std::min)(begin + chunkSize, jobCount);

				if (begin >= end) {
					break;
				}

				futures.emplace_back(std::async(std::launch::async,
					[this, begin, end]() -> std::vector<HitPair>
					{
						std::vector<HitPair> localHits;
						localHits.reserve(128);

						// スレッドローカル
						std::vector<Collider*> candidates;
						candidates.reserve(64);

						std::unordered_set<Collider*> seen;
						seen.reserve(64);

						for (size_t i = begin; i < end; ++i) {
							auto* colliderComp = dynamicColliders[i];
							if (!colliderComp) {
								continue;
							}

							const auto& colliders = colliderComp->GetAllColliders();
							for (auto* collider : colliders) {
								if (!collider || !collider->enabled) {
									continue;
								}

								const AABB selfAabb = collider->GetAABB();

								candidates.clear();
								seen.clear();

								// Query が読み取り専用でスレッドセーフであることが前提
								octreeCollider_->Query(selfAabb, candidates);

								for (auto* other : candidates) {
									if (!other || !other->enabled) {
										continue;
									}

									// 同一ポインタの重複排除
									if (!seen.insert(other).second) {
										continue;
									}

									// 自分自身をスキップ
									if (collider == other) {
										continue;
									}

									// 同一owner 内の自己衝突はスキップ
									if (collider->owner && other->owner && collider->owner == other->owner) {
										continue;
									}

									// 二重判定回避
									if (collider >= other) {
										continue;
									}

									// Broad filter
									if (!CheckMask(collider, other)) {
										continue;
									}

									// Narrow phase
									// CheckHit が副作用なしであることが前提
									if (collider->CheckHit(*other)) {
										localHits.push_back({ colliderComp, collider, other });
									}
								}
							}
						}

						return localHits;
					}
				));
			}

			// 全スレッドの結果を集約
			std::vector<HitPair> allHits;
			for (auto& future : futures) {
				std::vector<HitPair> localHits = future.get();
				allHits.insert(allHits.end(), localHits.begin(), localHits.end());
			}

			// Notify は単スレッドで実行
			for (const HitPair& hit : allHits) {
				NotifyHit(hit.selfComp, hit.self, hit.other);
			}
		}
	}

	octreeCollider_->Clear();
}

void Engine::CollisionManager::CheckDynamicVsStaticMT()
{
	//  ==== 静的Octreeを再構築 ====
	//octreeColliderStatic_->Clear();

	if (dynamicColliders.empty()) {
		return;
	}

	// スレッド数
	const uint32_t threadCount = (std::max)(1u, std::thread::hardware_concurrency());
	const size_t jobCount = dynamicColliders.size();
	const size_t chunkSize = (jobCount + threadCount - 1) / threadCount;

	std::vector<std::future<std::vector<HitPair>>> futures;
	futures.reserve(threadCount);

	for (uint32_t threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
		const size_t begin = threadIndex * chunkSize;
		const size_t end = (std::min)(begin + chunkSize, jobCount);

		if (begin >= end) {
			break;
		}

		futures.emplace_back(std::async(std::launch::async,
			[this, begin, end]() -> std::vector<HitPair>
			{
				std::vector<HitPair> localHits;
				localHits.reserve(128);

				std::vector<Collider*> staticCandidates;
				staticCandidates.reserve(128);

				std::unordered_set<Collider*> seenStatic;
				seenStatic.reserve(64);

				for (size_t i = begin; i < end; ++i) {
					auto* colliderComp = dynamicColliders[i];
					if (!colliderComp) {
						continue;
					}

					const auto& colliders = colliderComp->GetAllColliders();
					for (auto* collider : colliders) {
						if (!collider || !collider->enabled) {
							continue;
						}

						const AABB selfAabb = collider->GetAABB();

						staticCandidates.clear();
						seenStatic.clear();

						// ここが読み取り専用でスレッドセーフであることが前提
						octreeColliderStatic_->Query(selfAabb, staticCandidates);

						for (auto* other : staticCandidates) {
							if (!other || !other->enabled) {
								continue;
							}

							if (!seenStatic.insert(other).second) {
								continue;
							}

							if (collider->owner && other->owner && collider->owner == other->owner) {
								continue;
							}

							if (!CheckMask(collider, other)) {
								continue;
							}

							// ここは副作用なし前提
							if (collider->CheckHit(*other)) {
								localHits.push_back({ colliderComp, collider, other });
							}
						}
					}
				}

				return localHits;
			}
		));
	}

	// 集約
	std::vector<HitPair> allHits;
	for (auto& future : futures) {
		std::vector<HitPair> localHits = future.get();
		allHits.insert(allHits.end(), localHits.begin(), localHits.end());
	}

	// 通知は単スレッドで実行
	for (const HitPair& hit : allHits) {
		NotifyHit(hit.selfComp, hit.self, hit.other);
	}
}

void Engine::CollisionManager::NotifyHit(ColliderComponent* ownerComp, Collider* self, Collider* other) const {
	if (!self || !other) {
		return;
	}

	if (ownerComp) {
		auto& callback = ownerComp->onHitCallback;
		if (callback) {
			callback(self, other);
		}
	}

	ColliderComponent* otherComp = other->owner;
	if (!otherComp) {
		return;
	}

	auto& callback = otherComp->onHitCallback;
	if (callback) {
		callback(other, self);
	}
}

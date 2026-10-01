#include "CollisionManager.h"
#include"DirectXGame/engine/GlobalVariables/GlobalVariables.h"

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <exception>
#include <functional>
#include <mutex>
#include <thread>
#include <utility>

namespace Engine {

// 衝突判定のタスクを固定ワーカーへ分配し、毎フレームのスレッド生成をなくす。
class CollisionJobSystem {
public:
	// 指定された数のワーカースレッドを生成する。
	explicit CollisionJobSystem(std::size_t workerCount)
		: workerCount_((std::max)(std::size_t(1), workerCount)) {
		// ワーカーを初期化時に一度だけ生成する。
		workers_.reserve(workerCount_);
		for (std::size_t workerIndex = 0; workerIndex < workerCount_; ++workerIndex) {
			workers_.emplace_back([this]() { WorkerLoop(); });
		}
	}

	// 停止通知後に全ワーカーが終了するまで待機する。
	~CollisionJobSystem() {
		{
			// 停止状態をワーカーへ公開する。
			std::lock_guard<std::mutex> lock(mutex_);
			stopRequested_ = true;
		}

		// 待機中のワーカーを起こして終了させる。
		workCondition_.notify_all();
		for (std::thread& worker : workers_) {
			if (worker.joinable()) {
				worker.join();
			}
		}
	}

	// 現在利用できるワーカー数を返す。
	std::size_t GetWorkerCount() const {
		return workerCount_;
	}

	// タスクを分配し、全タスクの完了を待機する。
	void Run(std::size_t taskCount, std::function<void(std::size_t)> task) {
		// タスクが無い場合はワーカーを起こさず終了する。
		if (taskCount == 0) {
			return;
		}

		{
			// 新しいタスク一式を登録して、全ワーカーを次の世代へ進める。
			std::lock_guard<std::mutex> lock(mutex_);
			task_ = std::move(task);
			taskCount_ = taskCount;
			nextTask_.store(0);
			remainingWorkers_.store(workerCount_);
			firstException_ = nullptr;
			++generation_;
		}

		// タスクが登録されたことを全ワーカーへ通知する。
		workCondition_.notify_all();

		// 全ワーカーの完了を待つ。
		std::unique_lock<std::mutex> lock(mutex_);
		completedCondition_.wait(lock, [this]() {
			return remainingWorkers_.load() == 0;
			});

		// ワーカー内で発生した例外を呼び出し側へ戻す。
		std::exception_ptr exception = firstException_;
		firstException_ = nullptr;
		if (exception) {
			std::rethrow_exception(exception);
		}
	}

private:
	// ワーカーがタスクを取り出して処理し続けるループ。
	void WorkerLoop() {
		// このワーカーが最後に処理したタスク世代。
		std::size_t observedGeneration = 0;

		while (true) {
			{
				// 新しい世代のタスクまたは停止通知を待つ。
				std::unique_lock<std::mutex> lock(mutex_);
				workCondition_.wait(lock, [this, &observedGeneration]() {
					return stopRequested_ || generation_ != observedGeneration;
					});

				// 停止要求後は新しいタスクを処理せず終了する。
				if (stopRequested_) {
					return;
				}

				// 処理対象の世代を記録してロックを解放する。
				observedGeneration = generation_;
			}

			// 共有インデックスから次のタスクを取得して処理する。
			while (true) {
				const std::size_t taskIndex = nextTask_.fetch_add(1);
				if (taskIndex >= taskCount_) {
					break;
				}

				try {
					// 実際の衝突判定処理を呼び出す。
					task_(taskIndex);
				} catch (...) {
					// 最初の例外だけ保存し、他のワーカーの完了を待てるようにする。
					std::lock_guard<std::mutex> lock(mutex_);
					if (!firstException_) {
						firstException_ = std::current_exception();
					}
				}
			}

			// 最後に完了したワーカーが待機中のRunを起こす。
			if (remainingWorkers_.fetch_sub(1) == 1) {
				completedCondition_.notify_one();
			}
		}
	}

	// ワーカースレッドの固定数。
	const std::size_t workerCount_;
	// タスクを実行するワーカースレッド一覧。
	std::vector<std::thread> workers_;
	// タスクと世代の共有を保護するミューテックス。
	std::mutex mutex_;
	// 新しいタスクを待つワーカーを起こす条件変数。
	std::condition_variable workCondition_;
	// 全ワーカー完了をRunへ通知する条件変数。
	std::condition_variable completedCondition_;
	// 現在のタスク処理関数。
	std::function<void(std::size_t)> task_;
	// 現在のタスク数。
	std::size_t taskCount_ = 0;
	// 次に取得されるタスク番号。
	std::atomic<std::size_t> nextTask_ = 0;
	// 現在のタスク世代。
	std::size_t generation_ = 0;
	// 処理中ワーカー数。
	std::atomic<std::size_t> remainingWorkers_ = 0;
	// ワーカー停止要求。
	bool stopRequested_ = false;
	// ワーカー内で発生した最初の例外。
	std::exception_ptr firstException_;
};

} // namespace Engine


Engine::CollisionManager::CollisionManager() = default;
Engine::CollisionManager::~CollisionManager() = default;


void Engine::CollisionManager::Initialize(GlobalVariables* globalVariables, const AABB& sceneBounds) {
	this->globalVariables = globalVariables;	// 保存項目

	// メインスレッドを残しつつ、衝突判定用ワーカーを初期化時に一度だけ生成する。
	const unsigned int hardwareThreadCount = std::thread::hardware_concurrency();
	const std::size_t workerCount = hardwareThreadCount > 1 ? hardwareThreadCount - 1 : 1;
	jobSystem_ = std::make_unique<CollisionJobSystem>(workerCount);

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
			if (collider->IsEnabled()) {
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
			if (collider->IsEnabled()) {
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

void Engine::CollisionManager::Clear() {
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
			if (!colA->IsEnabled() || !colB->IsEnabled()) continue;

			// CollisionLayerはビット値なので、シフトせずそのままマスク判定に使う。
			if (!CheckMask(colA, colB)) {
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
			// 1フレームで使うタスク数をワーカー数とコライダー数の小さい方に制限する。
			const std::size_t taskCount = (std::min)(jobSystem_->GetWorkerCount(), jobCount);
			const std::size_t chunkSize = (jobCount + taskCount - 1) / taskCount;
			std::vector<std::vector<HitPair>> localHits(taskCount);

			// 固定ワーカーへコライダー範囲を分配する。
			jobSystem_->Run(taskCount, [this, jobCount, chunkSize, &localHits](std::size_t taskIndex) {
				// このタスクが担当するコライダー範囲を計算する。
				const std::size_t begin = taskIndex * chunkSize;
				const std::size_t end = (std::min)(begin + chunkSize, jobCount);
				std::vector<HitPair>& taskHits = localHits[taskIndex];
				taskHits.reserve(128);

				// スレッドごとに候補と重複除外集合を保持する。
				std::vector<Collider*> candidates;
				candidates.reserve(64);
				std::unordered_set<Collider*> seen;
				seen.reserve(64);

				for (std::size_t i = begin; i < end; ++i) {
					// 担当するコンポーネントを取得する。
					auto* colliderComp = dynamicColliders[i];
					if (!colliderComp) {
						continue;
					}

					// コンポーネント内の有効なコライダーを調べる。
					const auto& colliders = colliderComp->GetAllColliders();
					for (auto* collider : colliders) {
						if (!collider || !collider->IsEnabled()) {
							continue;
						}

						// AABBで候補を絞り込む。
						const AABB selfAabb = collider->GetAABB();
						candidates.clear();
						seen.clear();
						octreeCollider_->Query(selfAabb, candidates);

						for (auto* other : candidates) {
							if (!other || !other->IsEnabled()) {
								continue;
							}
							if (!seen.insert(other).second) {
								continue;
							}
							if (collider == other) {
								continue;
							}
							if (collider->GetOwner() && other->GetOwner() && collider->GetOwner() == other->GetOwner()) {
								continue;
							}
							if (collider >= other) {
								continue;
							}
							if (!CheckMask(collider, other)) {
								continue;
							}
							if (collider->CheckHit(*other)) {
								taskHits.push_back({ colliderComp, collider, other });
							}
						}
					}
				}
			});

			// 各タスクの結果をメインスレッドで集約する。
			std::vector<HitPair> allHits;
			for (std::vector<HitPair>& taskHits : localHits) {
				allHits.insert(allHits.end(), taskHits.begin(), taskHits.end());
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

	const size_t jobCount = dynamicColliders.size();
	// 1フレームで使うタスク数をワーカー数とコライダー数の小さい方に制限する。
	const std::size_t taskCount = (std::min)(jobSystem_->GetWorkerCount(), jobCount);
	const std::size_t chunkSize = (jobCount + taskCount - 1) / taskCount;
	std::vector<std::vector<HitPair>> localHits(taskCount);

	// 固定ワーカーへ動的コライダーの範囲を分配する。
	jobSystem_->Run(taskCount, [this, jobCount, chunkSize, &localHits](std::size_t taskIndex) {
		// このタスクが担当する範囲を計算する。
		const std::size_t begin = taskIndex * chunkSize;
		const std::size_t end = (std::min)(begin + chunkSize, jobCount);
		std::vector<HitPair>& taskHits = localHits[taskIndex];
		taskHits.reserve(128);

		// スレッドごとに静的候補と重複除外集合を保持する。
		std::vector<Collider*> staticCandidates;
		staticCandidates.reserve(128);
		std::unordered_set<Collider*> seenStatic;
		seenStatic.reserve(64);

		for (std::size_t i = begin; i < end; ++i) {
			// 担当するコンポーネントを取得する。
			auto* colliderComp = dynamicColliders[i];
			if (!colliderComp) {
				continue;
			}

			// コンポーネント内の有効なコライダーを調べる。
			const auto& colliders = colliderComp->GetAllColliders();
			for (auto* collider : colliders) {
				if (!collider || !collider->IsEnabled()) {
					continue;
				}

				// AABBで静的候補を絞り込む。
				const AABB selfAabb = collider->GetAABB();
				staticCandidates.clear();
				seenStatic.clear();
				octreeColliderStatic_->Query(selfAabb, staticCandidates);

				for (auto* other : staticCandidates) {
					if (!other || !other->IsEnabled()) {
						continue;
					}
					if (!seenStatic.insert(other).second) {
						continue;
					}
					if (collider->GetOwner() && other->GetOwner() && collider->GetOwner() == other->GetOwner()) {
						continue;
					}
					if (!CheckMask(collider, other)) {
						continue;
					}
					if (collider->CheckHit(*other)) {
						taskHits.push_back({ colliderComp, collider, other });
					}
				}
			}
		}
	});

	// 各タスクの結果をメインスレッドで集約する。
	std::vector<HitPair> allHits;
	for (std::vector<HitPair>& taskHits : localHits) {
		allHits.insert(allHits.end(), taskHits.begin(), taskHits.end());
	}

	// 通知は単スレッドで実行
	for (const HitPair& hit : allHits) {
		NotifyHit(hit.selfComp, hit.self, hit.other);
	}
}

bool Engine::CollisionManager::CheckMask(Collider* a, Collider* b) const {
	uint32_t aLayer = static_cast<uint32_t>(a->GetLayer());
	uint32_t bLayer = static_cast<uint32_t>(b->GetLayer());

	bool is = (bLayer & a->GetCollisionMask()) != 0;
	bool is2 = (aLayer & b->GetCollisionMask()) != 0;

	return is && is2;
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

	ColliderComponent* otherComp = other->GetOwner();
	if (!otherComp) {
		return;
	}

	auto& callback = otherComp->onHitCallback;
	if (callback) {
		callback(other, self);
	}
}

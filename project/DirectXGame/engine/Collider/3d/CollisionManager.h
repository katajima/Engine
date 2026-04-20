#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"
#include "ColliderComponent.h"
#include"list"
#include"Collider.h"
#include <unordered_set>
#include <future>
#include <thread>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include "DirectXGame/engine/collider/Octree/Octree.h"  


namespace Engine {

	struct HitPair
	{
		ColliderComponent* selfComp;
		Collider* self;
		Collider* other;
	};


	// 前方宣言
	class GlobalVariables;
	/// <summary>
	/// 衝突マネージャ
	/// </summary>
	class CollisionManager {
	public:
		// 初期化
		void Initialize(GlobalVariables* globalVariables, const AABB& sceneBounds);
		// ライン描画
		void DrawLine(LineCommon* lineCommon);
		// 静的コライダをオクツリーに入れる
		void BuildStaticSceneOctree();
		// 動的コライダ
		void BuildDynamicSceneOctree();
		// 動的コライダーコンポーネント追加
		void Register(ColliderComponent* comp);
		// 静的コライダーコンポーネント追加
		void RegisterStatic(ColliderComponent* comp);
		// 全削除（次フレームから再登録）
		void Clear();
		// 動的コライダの削除
		void ClearDynamic() {
			dynamicColliders.clear();
			registeredDynamic_.clear();
		}
		// 静的コライダの削除
		void ClearStatic() {
			staticColliders.clear();
			registeredStatic_.clear();
		}
		// 衝突チェック：全てのColliderComponentのペアをチェック
		void CheckAllOrld() {
			size_t n = dynamicColliders.size();
			debugTimer_.StartTimer();
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = i + 1; j < n; ++j) {
					CheckByLayer(*dynamicColliders[i], *dynamicColliders[j]);
				}
			}
			debugTimer_.EndTimer();
			debugTimer_.LogTimeSec("");
		}
		// コライダ全走査
		void CheckAll();
		// レイヤーとマスクに基づいて衝突を行う（以前の全当たり用）
		void CheckByLayer(ColliderComponent& a, ColliderComponent& b);
	private:
		// 動的コライダー同士の衝突判定
		void CheckDynamicVsDynamicMT();
		// 動的コライダーと静的コライダーの衝突判定
		void CheckDynamicVsStaticMT();
	private:
		// マスク処理
		bool CheckMask(Collider* a, Collider* b) const;
		// 処理応答
		void NotifyHit(ColliderComponent* ownerComp, Collider* self, Collider* other) const;

	private:
		std::vector<ColliderComponent*> dynamicColliders; // 動的コライダー
		std::vector<ColliderComponent*> staticColliders;  // 静的コライダー

		std::unordered_set<ColliderComponent*> registeredDynamic_;
		std::unordered_set<ColliderComponent*> registeredStatic_;

		GlobalVariables* globalVariables = nullptr;      // 保存

		std::unique_ptr<OctreeCollider> octreeCollider_; // オクツリー管理
		std::unique_ptr<OctreeCollider> octreeColliderStatic_; // オクツリー管理

		DebugTimer debugTimer_;
	public:
		// デストラクタ
		CollisionManager() = default;
	};
}
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
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(GlobalVariables* globalVariables, const AABB& sceneBounds);
		/// <summary>
		/// ライン描画
		/// </summary>
		void DrawLine(LineCommon* lineCommon);
		/// <summary>
		/// 静的コライダをオクツリーに入れる
		/// </summary>
		void BuildStaticSceneOctree();
		/// <summary>
		/// 動的コライダ
		/// </summary>
		void BuildDynamicSceneOctree();
		/// <summary>
		/// 動的コライダーコンポーネント追加
		/// </summary>
		void Register(ColliderComponent* comp);
		/// <summary>
		/// 静的コライダーコンポーネント追加
		/// </summary>
		void RegisterStatic(ColliderComponent* comp);
		/// <summary>
		/// 全削除（次フレームから再登録）
		/// </summary>
		void Clear();
		/// <summary>
		/// 動的コライダの削除
		/// </summary>
		void ClearDynamic() {
			dynamicColliders.clear();
			registeredDynamic_.clear();
		}
		/// <summary>
		/// 静的コライダの削除
		/// </summary>
		void ClearStatic() {
			staticColliders.clear();
			registeredStatic_.clear();
		}
		/// <summary>
		/// 衝突チェック：全てのColliderComponentのペアをチェック
		/// </summary>
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
		/// <summary>
		/// コライダ全走査
		/// </summary>
		void CheckAll();
		/// <summary>
		/// レイヤーとマスクに基づいて衝突を行う（以前の全当たり用）
		/// </summary>
		void CheckByLayer(ColliderComponent& a, ColliderComponent& b);
	private:
		/// <summary>
		/// 動的コライダー同士の衝突判定
		/// </summary>
		void CheckDynamicVsDynamicMT();
		/// <summary>
		/// 動的コライダーと静的コライダーの衝突判定
		/// </summary>
		void CheckDynamicVsStaticMT();
	private:
		/// <summary>
		/// マスク処理
		/// </summary>
		bool CheckMask(Collider* a, Collider* b) const;
		/// <summary>
		/// 処理応答
		/// </summary>
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
		/// <summary>
		/// デストラクタ
		/// </summary>
		CollisionManager() = default;
	};
}

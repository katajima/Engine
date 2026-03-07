#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"
#include "ColliderComponent2D.h"
#include"list"
#include"Collider2D.h"
#include <unordered_set>
#include "DirectXGame/engine/collider/Octree/Octree.h"  

// 前方宣言
class GlobalVariables;

namespace Engine {
    /// <summary>
    /// 衝突2dマネージャ
    /// </summary>
    class CollisionManager2d {
    public:
        // 初期化
        // 初期化
        void Initialize(GlobalVariables* globalVariables, const AABB& sceneBounds) {
            this->globalVariables = globalVariables;
        }

        // 動的コライダーコンポーネント追加
        void Register(ColliderComponent2D* comp) {
            if (comp && registeredDynamic_.insert(comp).second) {
                dynamicColliders.push_back(comp);
            }
        }

        // 静的コライダーコンポーネント追加
        void RegisterStatic(ColliderComponent2D* comp) {
            if (comp && registeredStatic_.insert(comp).second) {
                staticColliders.push_back(comp);
            }
        }

        // 全削除（次フレームから再登録）
        void Clear() {
            dynamicColliders.clear();
            staticColliders.clear();
            registeredDynamic_.clear();
            registeredStatic_.clear();
        }

        // 衝突チェック：全てのColliderComponentのペアをチェック
        void CheckAll() {
            size_t n = dynamicColliders.size();
            for (size_t i = 0; i < n; ++i) {
                for (size_t j = i + 1; j < n; ++j) {
                    CheckByLayer(*dynamicColliders[i], *dynamicColliders[j]);
                }
            }
        }

        // レイヤーとマスクに基づいて衝突を行う
        void CheckByLayer(ColliderComponent2D& a, ColliderComponent2D& b) {
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

    private:
        std::vector<ColliderComponent2D*> dynamicColliders; // 動的コライダー
        std::vector<ColliderComponent2D*> staticColliders;  // 静的コライダー

        std::unordered_set<ColliderComponent2D*> registeredDynamic_;
        std::unordered_set<ColliderComponent2D*> registeredStatic_;

        GlobalVariables* globalVariables = nullptr;      // 保存
    public:

        CollisionManager2d() = default;
    };
}
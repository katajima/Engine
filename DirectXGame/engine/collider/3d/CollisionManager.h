#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include "CollisionFancion.h"
#include "ColliderComponent.h"
#include"list"
#include"Collider.h"
#include <unordered_set>

/// <summary>
/// 衝突マネージャ
/// </summary>
class GlobalVariables;
class CollisionManager {
public:
    // 初期化
    // 初期化
    void Initialize(GlobalVariables* globalVariables) {
        globalVariables_ = globalVariables;
    }

    // 動的コライダーコンポーネント追加
    void Register(ColliderComponent* comp) {
        if (comp && registeredDynamic_.insert(comp).second) {
            dynamicColliders.push_back(comp);
        }
    }

    // 静的コライダーコンポーネント追加
    void RegisterStatic(ColliderComponent* comp) {
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

        // 動的 vs 静的
        for (auto* dyn : dynamicColliders) {
            for (auto* stat : staticColliders) {
                CheckByLayer(*dyn, *stat);
            }
        }
    }

    // レイヤーとマスクに基づいて衝突を行う
    void CheckByLayer(ColliderComponent& a, ColliderComponent& b) {
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
    std::vector<ColliderComponent*> dynamicColliders;
    std::vector<ColliderComponent*> staticColliders;

    std::unordered_set<ColliderComponent*> registeredDynamic_;
    std::unordered_set<ColliderComponent*> registeredStatic_;

    GlobalVariables* globalVariables_ = nullptr;
public:

    CollisionManager() = default;
};
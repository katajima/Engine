#pragma once

#include <functional>
#include "Collider.h"  // Colliderの基底クラス
#include "DirectXGame/engine/collider/ContactRecord.h"
#include "DirectXGame/engine/line/lineCommon.h"
#include "DirectXGame/engine/Collider/ColliderData.h"
#include "DirectXGame/engine/Utility/VectorUtility.h"

/// <summary>
/// コライダコンポーネント
/// </summary>
class ColliderComponent {
public:
    // Colliderを保持
    struct ColliderEntry {
        uint32_t id;
        std::unique_ptr<Collider> collider;
    };

    std::vector<ColliderEntry> colliders;

    // 所有オブジェクト（通知用）
    void* owner = nullptr;

    // 衝突時に呼ばれる関数（任意）
    std::function<void(Collider* self, Collider* other)> onHitCallback;

	// 衝突時に受け取るインターフェース（任意）
    IHitReceiver* hitReceiver = nullptr;

	// 履歴情報（衝突履歴）
    ContactRecord contactRecord_;
private:
    uint32_t nextId_ = 1; // IDは1から開始

    uint32_t uniqueId_ = 0; // 外部から一意な番号を割り当て

	LineCommon* lineCommon; // ライン描画用の共通オブジェクト
public: // 更新or判定
   
    // ワールド変換に基づいて各Colliderの座標を更新
    void UpdateAll(const WorldTransform& worldTransform) {
        for (auto& entry : colliders) {
            entry.collider->Update(worldTransform,lineCommon);
        }
    }
    // 特定のタグだけ更新
    void UpdateByTag(const WorldTransform& worldTransform, CollisionTag tag) {
        for (auto& entry : colliders) {
            if (entry.collider->tag == tag) {
                entry.collider->Update(worldTransform, lineCommon);
            }
        }
    }

	// 特定のIDのコライダーだけ更新
    void UpdateByID(const WorldTransform& worldTransform, uint32_t id) {
        for (auto& entry : colliders) {
            if (entry.id == id) {
                entry.collider->Update(worldTransform, lineCommon);
            }
        }
	}

    // 衝突チェック＋コールバック通知（このComponent vs 他のComponent）
    void CheckAndNotify(ColliderComponent& other) {
        for (const auto& c1 : colliders) {
            for (const auto& c2 : other.colliders) {
                if (c1.collider->CheckHit(*c2.collider)) {
                    if (onHitCallback) {
                        onHitCallback(c1.collider.get(), c2.collider.get());
                    }
                    if (other.onHitCallback) {
                        other.onHitCallback(c2.collider.get(), c1.collider.get());
                    }
                }
            }
        }
    }

    

public: // 削除

    // すべてのコライダーを削除
    void ClearColliders() {
        colliders.clear();
    }

    // 指定したコライダーを削除
    void RemoveCollider(Collider* target) {
        auto it = std::remove_if(colliders.begin(), colliders.end(),
            [target](const ColliderEntry& entry) {
                return entry.collider.get() == target;
            });
        colliders.erase(it, colliders.end());
    }

    // IDでのコライダー削除
    void RemoveColliderById(uint32_t id) {
        auto it = std::remove_if(colliders.begin(), colliders.end(),
            [id](const ColliderEntry& entry) {
                return entry.id == id;
            });
        colliders.erase(it, colliders.end());
    }

public: // 設定or追加
    // ライン共通クラス設定
    void SetLineCommon(LineCommon* line) { lineCommon = line; };

    // コライダー追加
    uint32_t AddCollider(std::unique_ptr<Collider> collider) {
        collider->owner = owner;
        collider->id = nextId_;
        uint32_t id = nextId_++;
        colliders.push_back({ id, std::move(collider) });
        return id;
    }

    // コライダー再設定
    void SetOwner(void* newOwner) {
        owner = newOwner;
        for (auto& entry : colliders) {
            entry.collider->owner = newOwner;
        }
    }

	// 衝突インターフェースをセット(※IHitReceiverを継承必須)
    void SetHitReceiver(IHitReceiver* receiver) {
        hitReceiver = receiver;
    }

    // タグでの有効or無効設定
    void SetEnableByTag(CollisionTag tag, bool enable) {
        for (auto& entry : colliders) {
            if (entry.collider->tag == tag) {
                entry.collider->enabled = enable;
            }
        }
    }

	// IDでの有効or無効設定
    void SetEnableById(uint32_t id, bool enable) {
        for (auto& entry : colliders) {
            if (entry.id == id) {
                entry.collider->enabled = enable;
            }
        }
	}

    // 初期化時に必ずセット
    void SetUniqueId(uint32_t id) { uniqueId_ = id; }

public: // 取得

    // インターフェース取得
    IHitReceiver* GetHitReceiver() const { return hitReceiver; }

    // コライダーコンポーネントID取得
    uint32_t GetUniqueId() const { return uniqueId_; }

	// コライダーID取得
    uint32_t GetNextId() const {
        return nextId_;
	}

    // ID検索でコライダー取得
    Collider* FindColliderById(uint32_t id) {
        for (auto& entry : colliders) {
            if (entry.id == id) {
                return entry.collider.get();
            }
        }
        return nullptr;
    }

    // コライダをIDで取得
    template <typename T>
    T* FindColliderById(uint32_t id) {
        for (auto& entry : colliders) {
            if (entry.id == id) {
                return  dynamic_cast<T*>(entry.collider.get());
            }
        }
        return nullptr;
        //return dynamic_cast<T*>(it->second.get());
    }


    // コライダ数取得
    size_t GetColliderCount() const {
        return colliders.size();
    }

    // 全コライダー取得
    std::vector<Collider*> GetAllColliders() const {
        std::vector<Collider*> results;
        for (const auto& entry : colliders) {
            results.push_back(entry.collider.get());
        }
        return results;
    }

    // 指定タグのコライダーを取得
    std::vector<Collider*> FindByTag(CollisionTag tag) const {
        std::vector<Collider*> results;
        for (const auto& entry : colliders) {
            if (entry.collider->tag == tag) {
                results.push_back(entry.collider.get());
            }
        }
        return results;
    }
};


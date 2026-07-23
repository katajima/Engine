#pragma once
#include <functional>
#include "Collider2D.h"  // Colliderの基底クラス
#include "DirectXGame/engine/collider/ContactRecord.h"
#include "DirectXGame/engine/line/lineCommon.h"
#include "DirectXGame/engine/Collider/ColliderData.h"
#include "DirectXGame/engine/Utility/VectorUtility.h"

namespace Engine {
    /// <summary>
    /// コライダ2dコンポーネント
    /// </summary>
    class ColliderComponent2D {
    public:
        // Colliderを保持
        struct ColliderEntry {
            uint32_t id;
            std::unique_ptr<Collider2D> collider;
        };

        VectorContainer<ColliderEntry> colliders;


        // 所有オブジェクト（通知用）
        void* owner = nullptr;

        /// <summary>
        /// 衝突時に呼ばれる関数（任意）
        /// </summary>
        std::function<void(Collider2D* self, Collider2D* other)> onHitCallback;

        // 衝突時に受け取るインターフェース（任意）
        IHitReceiver* hitReceiver = nullptr;

        // 履歴情報（衝突履歴）
        ContactRecord contactRecord_;
    private:
        uint32_t nextId_ = 1; // IDは1から開始

        uint32_t uniqueId_ = 0; // 外部から一意な番号を割り当て

    public: // 更新or判定

        /// <summary>
        /// ワールド変換に基づいて各Colliderの座標を更新
        /// </summary>
        void UpdateAll(const WorldTransform2d& worldTransform) {
            for (auto& entry : colliders) {
                entry.collider->Update(worldTransform);
            }
        }
        /// <summary>
        /// 特定のタグだけ更新
        /// </summary>
        void UpdateByTag(const WorldTransform2d& worldTransform, CollisionTag tag) {
            for (auto& entry : colliders) {
                if (entry.collider->tag == tag) {
                    entry.collider->Update(worldTransform);
                }
            }
        }

        /// <summary>
        /// 特定のIDのコライダーだけ更新
        /// </summary>
        void UpdateByID(const WorldTransform2d& worldTransform, uint32_t id) {
            for (auto& entry : colliders) {
                if (entry.id == id) {
                    entry.collider->Update(worldTransform);
                }
            }
        }

        /// <summary>
        /// 衝突チェック＋コールバック通知（このComponent vs 他のComponent）
        /// </summary>
        void CheckAndNotify(ColliderComponent2D& other) {
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

        /// <summary>
        /// すべてのコライダーを削除
        /// </summary>
        void ClearColliders() {
            colliders.Clear();
        }

        /// <summary>
        /// 指定したコライダーを削除
        /// </summary>
        void RemoveCollider(Collider2D* target) {
            colliders.RemoveIf([target](const ColliderEntry& entry) {
                return entry.collider.get() == target;
                });
        }

        /// <summary>
        /// IDでのコライダー削除
        /// </summary>
        void RemoveColliderById(uint32_t id) {
            colliders.RemoveIf([id](const ColliderEntry& entry) {
                return entry.id == id;
                });
        }

    public: // 設定or追加


        /// <summary>
        /// コライダー追加
        /// </summary>
        uint32_t AddCollider(std::unique_ptr<Collider2D> collider) {
            collider->owner = owner;
            collider->id = nextId_;
            uint32_t id = nextId_++;
            colliders.Add({ id, std::move(collider) });
            return id;
        }

        /// <summary>
        /// コライダー再設定
        /// </summary>
        void SetOwner(void* newOwner) {
            owner = newOwner;
            for (auto& entry : colliders) {
                entry.collider->owner = newOwner;
            }
        }

        /// <summary>
        /// 衝突インターフェースをセット(※IHitReceiverを継承必須)
        /// </summary>
        void SetHitReceiver(IHitReceiver* receiver) {
            hitReceiver = receiver;
        }

        /// <summary>
        /// タグでの有効or無効設定
        /// </summary>
        void SetEnableByTag(CollisionTag tag, bool enable) {
            for (auto& entry : colliders) {
                if (entry.collider->tag == tag) {
                    entry.collider->enabled = enable;
                }
            }
        }

        /// <summary>
        /// IDでの有効or無効設定
        /// </summary>
        void SetEnableById(uint32_t id, bool enable) {
            for (auto& entry : colliders) {
                if (entry.id == id) {
                    entry.collider->enabled = enable;
                }
            }
        }

        /// <summary>
        /// 初期化時に必ずセット
        /// </summary>
        void SetUniqueId(uint32_t id) { uniqueId_ = id; }

    public: // 取得

        /// <summary>
        /// インターフェース取得
        /// </summary>
        IHitReceiver* GetHitReceiver() const { return hitReceiver; }

        /// <summary>
        /// コライダーコンポーネントID取得
        /// </summary>
        uint32_t GetUniqueId() const { return uniqueId_; }

        /// <summary>
        /// コライダーID取得
        /// </summary>
        uint32_t GetNextId() const {
            return nextId_;
        }

        /// <summary>
        /// ID検索でコライダー取得
        /// </summary>
        Collider2D* FindColliderById(uint32_t id) {
            for (auto& entry : colliders) {
                if (entry.id == id) {
                    return entry.collider.get();
                }
            }
            return nullptr;
        }

        /// <summary>
        /// コライダ数取得
        /// </summary>
        size_t GetColliderCount() const {
            return colliders.Size();
        }

        /// <summary>
        /// 全コライダー取得
        /// </summary>
        std::vector<Collider2D*> GetAllColliders() const {
            std::vector<Collider2D*> results;
            for (const auto& entry : colliders) {
                results.push_back(entry.collider.get());
            }
            return results;
        }

        /// <summary>
        /// 指定タグのコライダーを取得
        /// </summary>
        std::vector<Collider2D*> FindByTag(CollisionTag tag) const {
            std::vector<Collider2D*> results;
            for (const auto& entry : colliders) {
                if (entry.collider->tag == tag) {
                    results.push_back(entry.collider.get());
                }
            }
            return results;
        }
    };
}
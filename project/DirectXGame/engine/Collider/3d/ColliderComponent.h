#pragma once

#include <functional>
#include "Collider.h"  // Colliderの基底クラス
#include "DirectXGame/engine/collider/ContactRecord.h"
#include "DirectXGame/engine/line/lineCommon.h"
#include "DirectXGame/engine/Collider/ColliderData.h"


/// <summary>
/// コライダコンポーネント
/// </summary>
namespace Engine {
/// <summary>
/// ColliderComponentを管理・実装するクラス。
/// </summary>
    class ColliderComponent {
    public:
        // Colliderを保持
        struct ColliderEntry {
            uint32_t id = 0;
            std::unique_ptr<Collider> collider = nullptr;
        };

        std::vector<ColliderEntry> colliders;

        // 所有オブジェクト（通知用）
        void* owner = nullptr;

        /// <summary>
        /// 衝突時に呼ばれる関数（任意）
        /// </summary>
        std::function<void(Collider* self, Collider* other)> onHitCallback;

        // 衝突時に受け取るインターフェース（任意）
        IHitReceiver* hitReceiver = nullptr;

        // 履歴情報（衝突履歴）
        ContactRecord contactRecord_{};
    private:
        uint32_t nextId_ = 1; // IDは1から開始

        uint32_t uniqueId_ = 0; // 外部から一意な番号を割り当て

        LineCommon* lineCommon = nullptr; // ライン描画用の共通オブジェクト
    public: // 更新or判定

        /// <summary>
        /// ワールド変換に基づいて各Colliderの座標を更新
        /// </summary>
        void UpdateAll(const WorldTransform& worldTransform);
        /// <summary>
        /// 特定のタグだけ更新
        /// </summary>
        void UpdateByTag(const WorldTransform& worldTransform, CollisionTag tag);

        /// <summary>
        /// 特定のIDのコライダーだけ更新
        /// </summary>
        void UpdateByID(const WorldTransform& worldTransform, uint32_t id);


        /// <summary>
        /// 衝突チェック＋コールバック通知（このComponent vs 他のComponent）
        /// </summary>
        void CheckAndNotify(ColliderComponent& other);



    public: // 削除

        /// <summary>
        /// すべてのコライダーを削除
        /// </summary>
        void ClearColliders() {
            colliders.clear();
        }

        /// <summary>
        /// 指定したコライダーを削除
        /// </summary>
        void RemoveCollider(Collider* target) {
            auto it = std::remove_if(colliders.begin(), colliders.end(),
                [target](const ColliderEntry& entry) {
                    return entry.collider.get() == target;
                });
            colliders.erase(it, colliders.end());
        }

        /// <summary>
        /// IDでのコライダー削除
        /// </summary>
        void RemoveColliderById(uint32_t id) {
            auto it = std::remove_if(colliders.begin(), colliders.end(),
                [id](const ColliderEntry& entry) {
                    return entry.id == id;
                });
            colliders.erase(it, colliders.end());
        }

    public: // 設定or追加
        /// <summary>
        /// ライン共通クラス設定
        /// </summary>
        void SetLineCommon(LineCommon* line) { lineCommon = line; };

        /// <summary>
        /// コライダー追加
        /// </summary>
        uint32_t AddCollider(std::unique_ptr<Collider> collider);

        /// <summary>
        /// コライダー再設定
        /// </summary>
        void SetOwner(ColliderComponent* newOwner);

        /// <summary>
        /// 衝突インターフェースをセット(※IHitReceiverを継承必須)
        /// </summary>
        void SetHitReceiver(IHitReceiver* receiver) {
            hitReceiver = receiver;
        }

        /// <summary>
        /// タグでの有効or無効設定
        /// </summary>
        void SetEnableByTag(CollisionTag tag, bool enable);

        /// <summary>
        /// IDでの有効or無効設定
        /// </summary>
        void SetEnableById(uint32_t id, bool enable);

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

        }


        /// <summary>
        /// コライダ数取得
        /// </summary>
        size_t GetColliderCount() const {
            return colliders.size();
        }

        /// <summary>
        /// 全コライダー取得
        /// </summary>
        std::vector<Collider*> GetAllColliders() const {
            std::vector<Collider*> results;
            for (const auto& entry : colliders) {
                results.push_back(entry.collider.get());
            }
            return results;
        }

        /// <summary>
        /// 指定タグのコライダーを取得
        /// </summary>
        std::vector<Collider*> FindByTag(CollisionTag tag) const {
            std::vector<Collider*> results;
            for (const auto& entry : colliders) {
                if (entry.collider->GetTag() == tag) {
                    results.push_back(entry.collider.get());
                }
            }
            return results;
        }
    };
}

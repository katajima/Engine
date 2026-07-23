#pragma once
#include <vector>
#include <iostream>

#include "DirectXGame/engine/math/MathFunctions.h"
#include "DirectXGame/engine/collider/3d/ColliderFunction3D.h"
#include "DirectXGame/engine/Base/pch.h"

namespace Engine {
   
    // 前方宣言
    class Collider;
    class BaseMesh;
    class LineCommon;
    // =========================
    // Collider 用 OctreeNode
    // =========================
    struct OctreeColliderNode {
        AABB bounds;
        std::vector<Collider*> colliders;
        std::vector<std::unique_ptr<OctreeColliderNode>> children;
        int depth;

        // コンストラクタ
        OctreeColliderNode(const AABB& bounds, int depth)
            : bounds(bounds), depth(depth) {
        }

        /// <summary>
        /// 小分け
        /// </summary>
        void Subdivide(int divX, int divY, int divZ, int maxDepth) {
            if (depth >= maxDepth) return;

            Vector3 size = (bounds.max - bounds.min);
            Vector3 childSize = size / Vector3(float(divX), float(divY), float(divZ));

            for (int x = 0; x < divX; ++x) {
                for (int y = 0; y < divY; ++y) {
                    for (int z = 0; z < divZ; ++z) {
                        Vector3 childMin = bounds.min + Vector3(float(x), float(y), float(z)) * childSize;
                        Vector3 childMax = childMin + childSize;
                        children.push_back(std::make_unique<OctreeColliderNode>(
                            AABB{ childMin, childMax }, depth + 1));
                    }
                }
            }
        }

        /// <summary>
        /// クリア
        /// </summary>
        void Clear() {
            colliders.clear();
            children.clear();   // unique_ptr の破棄で子孫ノードも全部消える
        }
    };

    // =========================
    // Collider 用 Octree
    // =========================
    class Collider;
/// <summary>
/// OctreeColliderを管理・実装するクラス。
/// </summary>
    class OctreeCollider {
    public:
        // コンストラクタ
        OctreeCollider(const AABB& bounds, int maxDepth = 4, int divX = 2, int divY = 2, int divZ = 2)
            : maxDepth(maxDepth), divX(divX), divY(divY), divZ(divZ) {
            root = std::make_unique<OctreeColliderNode>(bounds, 0);
        }

        ~OctreeCollider() {
            Clear();
        }

        /// <summary>
        /// クリア
        /// </summary>
        void Clear() const { root->Clear(); }

        /// <summary>
        /// 追加
        /// </summary>
        void Insert(Collider* collider);

        /// <summary>
        /// 挿入
        /// </summary>
        void Query(const AABB& area, std::vector<Collider*>& results) { QueryNode(root.get(), area, results); }

        /// <summary>
        /// Octree クラス内で呼び出し用の関数を追加
        /// </summary>
        void Draw(LineCommon& lineDrawer, Vector3 offset = Vector3(0, 0, 0));

    private:
        /// <summary>
        /// オクツリー描画
        /// </summary>
        void DrawOctree(OctreeColliderNode* node, LineCommon& lineDrawer, Vector3 offset = Vector3(0, 0, 0));
        /// <summary>
        /// 挿入
        /// </summary>
        void InsertCollider(OctreeColliderNode* node, Collider* collider, const AABB& aabb);
        /// <summary>
        /// 挿入
        /// </summary>
        void QueryNode(OctreeColliderNode* node, const AABB& area, std::vector<Collider*>& results);
    private:
        std::unique_ptr<OctreeColliderNode> root = nullptr;
        int maxDepth = 4;
        int divX = 2, divY = 2, divZ = 2;
    };
}
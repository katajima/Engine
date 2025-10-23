#pragma once
#include <vector>
#include <iostream>

#include "DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/collider/3d/ColliderFanction3D.h"
#include "DirectXGame/engine/Base/pch.h"
//#include "DirectXGame/engine/collider/3d/Collider.h"

// オクツリーのノード
struct OctreeNode {
    AABB bounds;  // ノードの境界
    std::vector<Triangle> triangles;  // このノードに含まれる三角形
    std::vector<Capsule> capsules;    // このノードに含まれるカプセル
    std::vector<OctreeNode*> children; // 動的に子ノードの数を変更可能

    int depth; // 現在のノードの深さ

    // コンストラクタ
    OctreeNode(const AABB& bounds, int depth)
        : bounds(bounds), depth(depth) {}

    // オクツリーの子ノードを作成する関数
    void subdivide(int divX, int divY, int divZ, int maxDepth);

    // クリア
    void clear();
};

// 前方宣言
class Collider;
class BaseMesh;
class LineCommon;
// オクツリーの管理クラス
class Octree {
public:
    std::unique_ptr<OctreeNode> root;
    int maxDepth; // 分割の最大深度
    int divX, divY, divZ; // X, Y, Z方向の分割数

    // コンストラクタで最大深度と分割数を指定できるようにする
    Octree(const AABB& bounds, int maxDepth = 4, int divX = 2, int divY = 2, int divZ = 2)
        : maxDepth(maxDepth), divX(divX), divY(divY), divZ(divZ) {
        root = std::make_unique<OctreeNode>(bounds, 0);
    }

    // 挿入
    void insert(const BaseMesh& mesh);

    // オクツリーに三角形を挿入
    void insert(const Triangle& triangle) {
        insertTriangle(root.get(), triangle);
    }

    // オクツリーにカプセルを挿入
    void insert(const Capsule& capsule) {
        insertCapsule(root.get(), capsule);
    }

    // Octree クラス内で呼び出し用の関数を追加
    void draw(LineCommon& lineDrawer, Vector3 offset = Vector3(0, 0, 0));


    // オクツリー全体でカプセルと三角形の衝突判定を行う
    bool checkCollisions(const Capsule& capsule) {
        // ルートノードから衝突判定を開始
        return checkCollisionWithNode(capsule, root.get());
    }

    // query: 指定AABBと交差する全てのCollider（ここではCapsuleとTriangle）をresultsに追加する
    void query(const AABB& area, std::vector<Collider*>& results) {
        queryNode(root.get(), area, results);
    }

private:
    // オクツリー描画
    void drawOctree(OctreeNode* node, LineCommon& lineDrawer, Vector3 offset = Vector3(0, 0, 0));

    // 三角形を挿入
    void insertTriangle(OctreeNode* node, const Triangle& triangle) {
        // 三角形がノードの範囲内に収まっているかをチェック
        if (!node->bounds.intersects(triangle.bounds)) return;

        // 子ノードが存在しない場合は、まず分割を試みる
        if (node->children.empty()) {
            if (node->depth < maxDepth) {
                node->subdivide(divX, divY, divZ, maxDepth);
            }
        }

        // 子ノードがない場合はこのノードに格納
        if (node->children.empty()) {
            node->triangles.push_back(triangle);
        }
        else {
            // 子ノードに三角形を挿入
            for (OctreeNode* child : node->children) {
                insertTriangle(child, triangle);
            }
        }
    }

    // カプセルを挿入
    void insertCapsule(OctreeNode* node, const Capsule& capsule) {
        if (!node->bounds.intersects(capsule.computeAABB())) return;

        // 子ノードが存在しない場合は、まず分割を試みる
        if (node->children.empty()) {
            if (node->depth < maxDepth) {
                node->subdivide(divX, divY, divZ, maxDepth);
            }
        }

        // 子ノードがない場合はこのノードに格納
        if (node->children.empty()) {
            node->capsules.push_back(capsule);
        }
        else {
            // 子ノードにカプセルを挿入
            for (OctreeNode* child : node->children) {
                insertCapsule(child, capsule);
            }
        }
    }

    // 書いていない
    void queryNode(OctreeNode* node, const AABB& area, std::vector<Collider*>& results);

    // OctreeNode 内でカプセルと三角形の衝突判定を行う
    bool checkCollisionWithNode(const Capsule& capsule, OctreeNode* node);

};




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

    // 小分け
    void subdivide(int divX, int divY, int divZ, int maxDepth) {
        if (depth >= maxDepth) return;

        Vector3 size = (bounds.max_ - bounds.min_);
        Vector3 childSize = size / Vector3(float(divX), float(divY), float(divZ));

        for (int x = 0; x < divX; ++x) {
            for (int y = 0; y < divY; ++y) {
                for (int z = 0; z < divZ; ++z) {
                    Vector3 childMin = bounds.min_ + Vector3(float(x), float(y), float(z)) * childSize;
                    Vector3 childMax = childMin + childSize;
                    children.push_back(std::make_unique<OctreeColliderNode>(
                        AABB{ childMin, childMax }, depth + 1));
                }
            }
        }
    }

    // クリア
    void clear() {
        colliders.clear();
        for (auto& c : children) {
            c->clear();
        }
    }
};

// =========================
// Collider 用 Octree
// =========================
class Collider;
class OctreeCollider {
public:
    std::unique_ptr<OctreeColliderNode> root;
    int maxDepth;
    int divX, divY, divZ;

    // コンストラクタ
    OctreeCollider(const AABB& bounds, int maxDepth = 4, int divX = 2, int divY = 2, int divZ = 2)
        : maxDepth(maxDepth), divX(divX), divY(divY), divZ(divZ) {
        root = std::make_unique<OctreeColliderNode>(bounds, 0);
    }

    // クリア
    void Clear() {
        root->clear();
    }

    // 追加
    void Insert(Collider* collider);

    //　挿入
    void Query(const AABB& area, std::vector<Collider*>& results) {
        queryNode(root.get(), area, results);
    }

private:
    // 挿入
    void insertCollider(OctreeColliderNode* node, Collider* collider, const AABB& aabb);
    // 挿入
    void queryNode(OctreeColliderNode* node, const AABB& area, std::vector<Collider*>& results);
};
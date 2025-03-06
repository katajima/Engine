#pragma once
#include <vector>
#include <iostream>
#include "DirectXGame/engine/math/MathFanctions.h"

// オクツリーのノード
struct OctreeNode {
    AABB bounds;  // ノードの境界
    std::vector<Triangle> triangles;  // このノードに含まれる三角形
    std::vector<Capsule> capsules;    // このノードに含まれるカプセル
    OctreeNode* children[8];  // 子ノードへのポインタ（最大8個）

    // オクツリーの子ノードを作成する関数
    void subdivide() {
        Vector3 center = (bounds.min_ + bounds.max_) * 0.5f;
        Vector3 halfSize = (bounds.max_ - bounds.min_) * 0.5f;

        for (int i = 0; i < 8; ++i) {
            // 子ノードの境界を設定
            Vector3 minChild = center;
            Vector3 maxChild = center;

            if (i & 1) minChild.x = bounds.min_.x;
            else maxChild.x = bounds.max_.x;

            if (i & 2) minChild.y = bounds.min_.y;
            else maxChild.y = bounds.max_.y;

            if (i & 4) minChild.z = bounds.min_.z;
            else maxChild.z = bounds.max_.z;

            // 子ノードを作成
            children[i] = new OctreeNode();
            children[i]->bounds = AABB(minChild, maxChild);
        }
    }
};

// オクツリーの管理クラス
class Octree {
public:
    OctreeNode* root;

    Octree(const AABB& bounds) {
        root = new OctreeNode();
        root->bounds = bounds;
    }

    // オクツリーに三角形を挿入
    void insert(const Triangle& triangle) {
        insertTriangle(root, triangle);
    }

    // オクツリーにカプセルを挿入
    void insert(const Capsule& capsule) {
        insertCapsule(root, capsule);
    }

private:
    // 三角形を挿入
    void insertTriangle(OctreeNode* node, const Triangle& triangle) {
        // 三角形がノードの範囲内に収まっているかをチェック
        if (!node->bounds.intersects(triangle.bounds)) return;

        if (node->children[0] == nullptr) {
            node->triangles.push_back(triangle);
        }
        else {
            for (int i = 0; i < 8; ++i) {
                insertTriangle(node->children[i], triangle);
            }
        }
    }

    // カプセルを挿入
    void insertCapsule(OctreeNode* node, const Capsule& capsule) {
        if (!node->bounds.intersects(capsule.computeAABB())) return;

        if (node->children[0] == nullptr) {
            node->capsules.push_back(capsule);
        }
        else {
            for (int i = 0; i < 8; ++i) {
                insertCapsule(node->children[i], capsule);
            }
        }
    }
};


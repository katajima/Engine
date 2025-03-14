#pragma once
#include <vector>
#include <iostream>
#include "DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/collider/3d/ColliderFanction3D.h"

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


class Mesh;
class LineCommon;
// オクツリーの管理クラス
class Octree {
public:
    OctreeNode* root;
    int maxDepth; // 分割の最大深度
    int divX, divY, divZ; // X, Y, Z方向の分割数

    // コンストラクタで最大深度と分割数を指定できるようにする
    Octree(const AABB& bounds, int maxDepth = 4, int divX = 2, int divY = 2, int divZ = 2)
        : maxDepth(maxDepth), divX(divX), divY(divY), divZ(divZ) {
        root = new OctreeNode(bounds, 0);
    }


    void insert(const Mesh& mesh);

    // オクツリーに三角形を挿入
    void insert(const Triangle& triangle) {
        insertTriangle(root, triangle);
    }

    // オクツリーにカプセルを挿入
    void insert(const Capsule& capsule) {
        insertCapsule(root, capsule);
    }

    // Octree クラス内で呼び出し用の関数を追加
    void draw(LineCommon& lineDrawer, Vector3 offset = Vector3(0, 0, 0));


    // オクツリー全体でカプセルと三角形の衝突判定を行う
    bool checkCollisions(const Capsule& capsule) {
        // ルートノードから衝突判定を開始
        return checkCollisionWithNode(capsule, root);
    }
private:
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

    // OctreeNode 内でカプセルと三角形の衝突判定を行う
    bool checkCollisionWithNode(const Capsule& capsule, OctreeNode* node);
};

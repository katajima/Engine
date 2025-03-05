#pragma once
#include <vector>
#include <iostream>
#include "DirectXGame/engine/math/MathFanctions.h"



class OctreeNode {
public:
    AABB bounds;
    std::vector<Triangle> triangles;
    OctreeNode* children[8] = { nullptr };
    int maxTriangles = 4;
    int maxDepth = 5;
    int depth;

    OctreeNode(const AABB& bounds, int depth = 0) : bounds(bounds), depth(depth) {}

    ~OctreeNode() {
        for (int i = 0; i < 8; i++) {
            delete children[i];
        }
    }

    void insert(const Triangle& tri) {
        if (!bounds.intersects(tri.bounds)) return;

        if (triangles.size() < maxTriangles || depth >= maxDepth) {
            triangles.push_back(tri);
            return;
        }

        if (children[0] == nullptr) subdivide();

        for (int i = 0; i < 8; i++) {
            if (children[i]->bounds.intersects(tri.bounds)) {
                children[i]->insert(tri);
            }
        }
    }

    void subdivide() {
        Vector3 min = bounds.min_;
        Vector3 max = bounds.max_;
        Vector3 center = Vector3((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);

        children[0] = new OctreeNode(AABB(min, center), depth + 1);
        children[1] = new OctreeNode(AABB(Vector3(center.x, min.y, min.z), Vector3(max.x, center.y, center.z)), depth + 1);
        children[2] = new OctreeNode(AABB(Vector3(min.x, center.y, min.z), Vector3(center.x, max.y, center.z)), depth + 1);
        children[3] = new OctreeNode(AABB(Vector3(center.x, center.y, min.z), Vector3(max.x, max.y, center.z)), depth + 1);
        children[4] = new OctreeNode(AABB(Vector3(min.x, min.y, center.z), Vector3(center.x, center.y, max.z)), depth + 1);
        children[5] = new OctreeNode(AABB(Vector3(center.x, min.y, center.z), Vector3(max.x, center.y, max.z)), depth + 1);
        children[6] = new OctreeNode(AABB(Vector3(min.x, center.y, center.z), Vector3(center.x, max.y, max.z)), depth + 1);
        children[7] = new OctreeNode(AABB(center, max), depth + 1);
    }
};

class Octree {
public:
    OctreeNode* root;

    Octree(const AABB& bounds) {
        root = new OctreeNode(bounds);
    }

    ~Octree() {
        delete root;
    }

    void insert(const Triangle& tri) {
        root->insert(tri);
    }
};

// テスト用
int main() {
    Octree tree(AABB(Vector3(0, 0, 0), Vector3(10, 10, 10)));

    tree.insert(Triangle(Vector3(1, 1, 1), Vector3(2, 2, 2), Vector3(3, 1, 2)));
    tree.insert(Triangle(Vector3(5, 5, 5), Vector3(6, 6, 6), Vector3(7, 5, 6)));

    std::cout << "オクツリーに三角形を追加しました。\n";
    return 0;
}


#include "Octree.h"
#include "DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Mesh/Mesh.h"


/// <summary>
/// Octree
/// </summary>

void Octree::drawOctree(OctreeNode* node, LineCommon& lineDrawer, Vector3 offset)
{
    if (!node) return;

    // AABB を描画

    //LineCommon::GetInstance()->AddLineAABB(node->bounds, offset);
    lineDrawer.AddLineAABB(node->bounds, offset);

    // 子ノードがある場合は再帰的に描画
    for (OctreeNode* child : node->children) {
        if (child) {  // nullptr チェック
            drawOctree(child, lineDrawer, offset);
        }
    }
}

void Octree::draw(LineCommon& lineDrawer, Vector3 offset)
{
  
    drawOctree(root, lineDrawer, offset);
}






/// <summary>
/// メッシュの三角形を入れる
/// </summary>
/// <param name="mesh"></param>
void Octree::insert(const Mesh& mesh) {
   for (auto& triangle : mesh.triangle) {
       insertTriangle(root, triangle);
   }
}




/// <summary>
/// OctreeNode
/// </summary>

void OctreeNode::subdivide(int divX, int divY, int divZ, int maxDepth)
{
    clear();

  


    if (depth >= maxDepth) return;  // 最大深度に達したら分割しない

    float stepX = (bounds.max_.x - bounds.min_.x) / divX;
    float stepY = (bounds.max_.y - bounds.min_.y) / divY;
    float stepZ = (bounds.max_.z - bounds.min_.z) / divZ;

    for (int x = 0; x < divX; ++x) {
        for (int y = 0; y < divY; ++y) {
            for (int z = 0; z < divZ; ++z) {
                Vector3 minChild = {
                    bounds.min_.x + x * stepX,
                    bounds.min_.y + y * stepY,
                    bounds.min_.z + z * stepZ
                };
                Vector3 maxChild = minChild + Vector3(stepX, stepY, stepZ);

                children.push_back(new OctreeNode(AABB(minChild, maxChild), depth + 1));
            }
        }
    }
}


void OctreeNode::clear()
{
    for (OctreeNode* child : children) {
        if (child) {
            child->clear(); // 再帰的に子ノードを削除
            delete child;
        }
    }
    children.clear(); // 子ノードのポインタをすべて削除
    triangles.clear(); // 三角形のリストをクリア
    capsules.clear();  // カプセルのリストをクリア
}


// OctreeNode 内でカプセルと三角形の衝突判定を行う
bool Octree::checkCollisionWithNode(const Capsule& capsule, OctreeNode* node) {
    if (!node->bounds.intersects(capsule.computeAABB())) {
        return false;  // AABBが交差していなければ衝突しない
    }

    // ノード内に含まれている三角形との衝突判定
    for (const Triangle& triangle : node->triangles) {
        if (IsCollision(triangle,capsule)) {
            return true;
        }
    }

    // ノード内に含まれているカプセルとの衝突判定
    for (const Capsule& otherCapsule : node->capsules) {
        if (IsCollision(capsule, otherCapsule)) {
            return true;
        }
    }

    // 子ノードがある場合、再帰的に衝突判定を行う
    for (OctreeNode* child : node->children) {
        if (checkCollisionWithNode(capsule, child)) {
            return true;
        }
    }

    return false;  // 衝突しなかった場合
}

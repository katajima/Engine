#include "Octree.h"
#include "DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"
#include "DirectXGame/engine/collider/3d/Collider.h"

/// <summary>
/// Octree
/// </summary>

void Engine::Octree::drawOctree(OctreeNode* node, LineCommon& lineDrawer, Vector3 offset)
{
	if (!node) return;

	// AABB を描画

	//LineCommon::GetInstance()->AddLineAABB(node->bounds, offset);
	lineDrawer.GetDebugLineMeshData().AddLineAABB(node->bounds, offset);

	// 子ノードがある場合は再帰的に描画
	for (OctreeNode* child : node->children) {
		if (child) {  // nullptr チェック
			drawOctree(child, lineDrawer, offset);
		}
	}
}

void Engine::Octree::draw(LineCommon& lineDrawer, Vector3 offset)
{

	drawOctree(root.get(), lineDrawer, offset);
}






/// <summary>
/// メッシュの三角形を入れる
/// </summary>
/// <param name="mesh"></param>
void Engine::Octree::insert(const BaseMesh& mesh) {
	for (auto& triangle : mesh.triangle) {
		insertTriangle(root.get(), triangle);
	}
}




/// <summary>
/// OctreeNode
/// </summary>

void Engine::OctreeNode::subdivide(int divX, int divY, int divZ, int maxDepth)
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


void Engine::OctreeNode::clear()
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


void Engine::Octree::queryNode(OctreeNode* node, const AABB& area, std::vector<Collider*>& results)
{


}

// OctreeNode 内でカプセルと三角形の衝突判定を行う
bool Engine::Octree::checkCollisionWithNode(const Capsule& capsule, OctreeNode* node) {
	if (!node->bounds.intersects(capsule.computeAABB())) {
		return false;  // AABBが交差していなければ衝突しない
	}

	// ノード内に含まれている三角形との衝突判定
	for (const Triangle& triangle : node->triangles) {
		if (Collision::Detection::Check(triangle, capsule)) {
			return true;
		}
	}

	// ノード内に含まれているカプセルとの衝突判定
	for (const Capsule& otherCapsule : node->capsules) {
		if (Collision::Detection::Check(capsule, otherCapsule)) {
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








void Engine::OctreeCollider::Insert(Collider* collider)
{
	if (!collider || !collider->enabled) return;
	AABB aabb = collider->GetAABB();
	insertCollider(root.get(), collider, aabb);
}


void Engine::OctreeCollider::insertCollider(OctreeColliderNode* node, Collider* collider, const AABB& aabb) {
	if (!node->bounds.intersects(aabb)) return;

	// 子ノードを持たない場合、または最大深度に到達している場合はこのノードに保持
	if (node->depth >= maxDepth) {
		node->colliders.push_back(collider);
		return;
	}

	if (node->children.empty()) {
		node->subdivide(divX, divY, divZ, maxDepth);
	}

	bool insertedToChild = false;

	for (auto& child : node->children) {
		if (child->bounds.Contains(aabb)) {
			insertCollider(child.get(), collider, aabb);
			insertedToChild = true;
			break;
		}
	}

	// どの子にも完全には入らなかった場合、このノードに保持
	if (!insertedToChild) {
		node->colliders.push_back(collider);
	}
}

void Engine::OctreeCollider::queryNode(OctreeColliderNode* node, const AABB& area, std::vector<Collider*>& results)
{
	// ノードAABBとクエリアABBが交差しない場合はスキップ
	if (!node->bounds.intersects(area)) return;

	// このノードに含まれるコライダーを追加
	for (auto* c : node->colliders) {
		if (c->GetAABB().intersects(area)) {
			results.push_back(c);
		}
	}

	// 子ノードを持たない場合、ここで終了
	if (node->children.empty()) return;

	// 子ノードに含まれるものだけ探索
	for (auto& child : node->children) {
		// 完全に内包される場合のみ再帰する
		if (child->bounds.intersects(area)) {
			queryNode(child.get(), area, results);
		}
	}
}

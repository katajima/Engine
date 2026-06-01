#include "Octree.h"
#include "DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"
#include "DirectXGame/engine/collider/3d/Collider.h"


void Engine::OctreeCollider::Insert(Collider* collider)
{
	if (!collider || !collider->IsEnabled()) return;
	AABB aabb = collider->GetAABB();
	InsertCollider(root.get(), collider, aabb);
}

void Engine::OctreeCollider::InsertCollider(OctreeColliderNode* node, Collider* collider, const AABB& aabb) {
	if (!node->bounds.intersects(aabb)) return;

	// 子ノードを持たない場合、または最大深度に到達している場合はこのノードに保持
	if (node->depth >= maxDepth) {
		node->colliders.push_back(collider);
		return;
	}

	if (node->children.empty()) {
		node->Subdivide(divX, divY, divZ, maxDepth);
	}

	bool insertedToChild = false;

	for (auto& child : node->children) {
		if (child->bounds.Contains(aabb)) {
			InsertCollider(child.get(), collider, aabb);
			insertedToChild = true;
			break;
		}
	}

	// どの子にも完全には入らなかった場合、このノードに保持
	if (!insertedToChild) {
		node->colliders.push_back(collider);
	}
}

void Engine::OctreeCollider::QueryNode(OctreeColliderNode* node, const AABB& area, std::vector<Collider*>& results)
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
			QueryNode(child.get(), area, results);
		}
	}
}

#pragma region Draw

void Engine::OctreeCollider::Draw(LineCommon& lineDrawer, Vector3 offset)
{
	DrawOctree(root.get(), lineDrawer, offset);
}

void Engine::OctreeCollider::DrawOctree(OctreeColliderNode* node, LineCommon& lineDrawer, Vector3 offset)
{
	if (!node) return;

	// AABB を描画
	lineDrawer.GetDebugLineMeshData().AddLineAABB(node->bounds, offset, { 0.933f, 0.51f, 0.933f ,1 });

	// 子ノードがある場合は再帰的に描画
	for (auto& child : node->children) {
		if (child) {  // nullptr チェック
			DrawOctree(child.get(), lineDrawer, offset);
		}
	}
}
#pragma endregion // 描画

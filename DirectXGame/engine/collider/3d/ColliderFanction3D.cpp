#include "ColliderFanction3D.h"


bool CapsuleIntersectsWithOctree(const Capsule& capsule, OctreeNode* node) {
	// ノード内のカプセルと三角形を全てチェック
	for (const auto& tri : node->triangles) {
		if (CapsuleIntersectsTriangle(capsule, tri)) {
			return true;
		}
	}

	for (const auto& cap : node->capsules) {
		if (IsCollision(capsule, cap)) {
			return true;
		}
	}

	// 子ノードも含めて調べる
	for (int i = 0; i < 8; ++i) {
		if (node->children[i]) {
			if (CapsuleIntersectsWithOctree(capsule, node->children[i])) {
				return true;
			}
		}
	}

	return false;
}
#include "AStarAlgorithm.h"

#include "DirectXGame/engine/Line/LineCommon.h"

bool AStarPathfinder::FindPath(const Vector2& startWorld,
	const Vector2& endWorld,
	std::vector<Vector2>& path) {
	int startX = static_cast<int>(startWorld.x / m_map->GetCellSize());
	int startZ = static_cast<int>(startWorld.y / m_map->GetCellSize());

	int endX = static_cast<int>(endWorld.x / m_map->GetCellSize());
	int endZ = static_cast<int>(endWorld.y / m_map->GetCellSize());

	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
	std::unordered_map<int, Node> allNodes;

	Node startNode = { startX, startZ, 0, Heuristic(startX, startZ, endX, endZ) };
	openList.push(startNode);
	allNodes[startX + startZ * m_map->GetWidth()] = startNode;

	std::unordered_map<int, Node> cameFrom;

	m_path.clear();
	path.clear();

	while (!openList.empty()) {
		Node current = openList.top();
		openList.pop();

		if (current.x == endX && current.z == endZ) {
			// ゴールに到達した場合、経路を逆順で復元
			while (cameFrom.find(current.x + current.z * m_map->GetWidth()) != cameFrom.end()) {
				path.push_back(Vector2(current.x * m_map->GetCellSize(), current.z * m_map->GetCellSize()));

				// 進行方向を計算（現在のノードから前のノードへ）
				Node previous = cameFrom[current.x + current.z * m_map->GetWidth()];
				Vector2 direction = Vector2(static_cast<float>(previous.x - current.x),
					static_cast<float>(previous.z - current.z));

				// 進行方向を表示（または別の処理）
				// direction は移動ベクトルです（方向）
				// direction.x と direction.y を使って進行方向を利用できます。

				current = previous;
			}
			path.push_back(Vector2(startX * m_map->GetCellSize(), startZ * m_map->GetCellSize()));
			std::reverse(path.begin(), path.end());

			// m_path に経路を保存
			m_path = path;  // 最短経路を m_path に保存

			return true;
		}

		for (const auto& neighbor : GetNeighbors(current.x, current.z)) {
			int neighborX = neighbor.first;
			int neighborZ = neighbor.second;

			if (m_map->IsBlocked(neighborX, neighborZ)) continue;

			float tentativeGCost = current.gCost + 1.0f;

			int neighborKey = neighborX + neighborZ * m_map->GetWidth();

			if (allNodes.find(neighborKey) == allNodes.end() || tentativeGCost < allNodes[neighborKey].gCost) {
				Node neighborNode = { neighborX, neighborZ, tentativeGCost, Heuristic(neighborX, neighborZ, endX, endZ) };
				openList.push(neighborNode);
				allNodes[neighborKey] = neighborNode;
				cameFrom[neighborKey] = current;
			}
		}
	}

	return false;
}



float AStarPathfinder::Heuristic(int x1, int z1, int x2, int z2)
{
	return static_cast<float>(std::abs(x1 - x2) + std::abs(z1 - z2)); // マンハッタン距離
}

std::vector<std::pair<int, int>> AStarPathfinder::GetNeighbors(int x, int z)
{
	std::vector<std::pair<int, int>> result;
	const int dirs[4][2] = {
		{ 1, 0 }, {-1, 0}, { 0, 1 }, { 0, -1 }
	};

	for (auto& dir : dirs) {
		int nx = x + dir[0];
		int nz = z + dir[1];
		if (nx >= 0 && nx < m_map->GetWidth() && nz >= 0 && nz < m_map->GetHeight()) {
			result.emplace_back(nx, nz);
		}
	}

	return result;
}


Vector2 AStarPathfinder::GetDirectionToNextNode() {
	if (m_path.size() < 2) {
		return Vector2(0, 0);  // 終了した場合、進行方向はゼロベクトル
	}

	// 現在位置の更新
	Vector2 current = m_path[0];

	// 次のノードを現在位置から計算
	Vector2 next = m_path[1];

	// 進行方向を計算（次のノード - 現在のノード）
	Vector2 direction = next - current;

	if (direction.LengthSq() > 0) {
		direction.Normalize(); // 単位ベクトルにして進行方向を決定
	}
	else {
		direction = Vector2(0, 0);  // ゼロベクトルの場合、進行方向なし
	}

	// もし現在位置が次のノードに達した場合、次のノードに進む
	if ((current - next).LengthSq() < m_map->GetCellSize() * m_map->GetCellSize()) {
		m_path.erase(m_path.begin()); // 先頭ノードを削除して次に進む
	}

	return direction;
}

void AStarPathfinder::DrawPath(LineCommon* line,float yPos) const
{
	// m_pathが2つ以上のノードを含んでいる場合に描画を行う
	if (m_path.size() < 2) {
		return;  // 経路が無い場合は何も描画しない
	}

	Vector4 pathColor = Vector4(0.0f, 1.0f, 0.0f, 1.0f); // 緑色のライン

	// 経路の各ノード間でラインを描画
	for (size_t i = 0; i < m_path.size() - 1; ++i) {
		Vector2 current = m_path[i];
		Vector2 next = m_path[i + 1];

		// 現在のノードから次のノードに向かってラインを描画
		line->AddLine(
			Vector3(current.x, yPos, current.y),  // 現在のノード
			Vector3(next.x, yPos, next.y),        // 次のノード
			pathColor                             // ラインの色
		);
	}
}



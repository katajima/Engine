#include "AStarAlgorithm.h"
#include "DirectXGame/engine/Line/LineCommon.h"

//----------------------------------------
// A*アルゴリズムによる経路探索
//----------------------------------------
bool AStarPathfinder::FindPath(const Vector2& startWorld,
	const Vector2& endWorld,
	std::vector<Vector2>& path) {

	// ワールド座標をグリッド座標に変換
	int startX = static_cast<int>(startWorld.x / m_map->GetCellSize());
	int startZ = static_cast<int>(startWorld.y / m_map->GetCellSize());
	int endX = static_cast<int>(endWorld.x / m_map->GetCellSize());
	int endZ = static_cast<int>(endWorld.y / m_map->GetCellSize());

	// オープンリスト（優先度付きキュー）
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
	// すべてのノードを格納（探索済み含む）
	std::unordered_map<int, Node> allNodes;

	// スタートノードを初期化して登録
	Node startNode = { startX, startZ, 0, Heuristic(startX, startZ, endX, endZ) };
	openList.push(startNode);
	allNodes[startX + startZ * m_map->GetWidth()] = startNode;

	// 経路復元用マップ
	std::unordered_map<int, Node> cameFrom;

	// 経路初期化
	m_path.clear();
	path.clear();

	//----------------------------------------
	// メイン探索ループ
	//----------------------------------------
	while (!openList.empty()) {
		// 最小f値（g + h）のノードを取り出す
		Node current = openList.top();
		openList.pop();

		// ゴールに到達した場合
		if (current.x == endX && current.z == endZ) {

			// 経路を逆順にたどって復元
			while (cameFrom.find(current.x + current.z * m_map->GetWidth()) != cameFrom.end()) {
				path.push_back(Vector2(current.x * m_map->GetCellSize(),
					current.z * m_map->GetCellSize()));

				// 一つ前のノードを取得
				Node previous = cameFrom[current.x + current.z * m_map->GetWidth()];

				// 進行方向を算出（デバッグ・利用用）
				Vector2 direction = Vector2(static_cast<float>(previous.x - current.x),
					static_cast<float>(previous.z - current.z));

				// 次のノードへ
				current = previous;
			}

			// スタート位置を追加し、順序を正しい方向に反転
			path.push_back(Vector2(startX * m_map->GetCellSize(), startZ * m_map->GetCellSize()));
			std::reverse(path.begin(), path.end());

			// 結果をメンバに保持
			m_path = path;

			return true; // 経路探索成功
		}

		//----------------------------------------
		// 隣接ノードの探索
		//----------------------------------------
		for (const auto& neighbor : GetNeighbors(current.x, current.z)) {
			int neighborX = neighbor.first;
			int neighborZ = neighbor.second;

			// 通行不可セルはスキップ
			if (m_map->IsBlocked(neighborX, neighborZ)) continue;

			// 現ノードからの仮コスト（g値）
			float tentativeGCost = current.gCost + 1.0f;
			int neighborKey = neighborX + neighborZ * m_map->GetWidth();

			// 新規ノードまたはより短い経路が見つかった場合
			if (allNodes.find(neighborKey) == allNodes.end() ||
				tentativeGCost < allNodes[neighborKey].gCost) {

				Node neighborNode = {
					neighborX, neighborZ,
					tentativeGCost,
					Heuristic(neighborX, neighborZ, endX, endZ)
				};

				openList.push(neighborNode);
				allNodes[neighborKey] = neighborNode;
				cameFrom[neighborKey] = current;
			}
		}
	}

	// ゴールに到達できなかった場合
	return false;
}

//----------------------------------------
// ヒューリスティック関数（マンハッタン距離）
//----------------------------------------
float AStarPathfinder::Heuristic(int x1, int z1, int x2, int z2)
{
	return static_cast<float>(std::abs(x1 - x2) + std::abs(z1 - z2));
}

//----------------------------------------
// 指定ノードの隣接セルを取得（上下左右4方向）
//----------------------------------------
std::vector<std::pair<int, int>> AStarPathfinder::GetNeighbors(int x, int z)
{
	std::vector<std::pair<int, int>> result;

	// 4方向（上下左右）
	const int dirs[4][2] = {
		{ 1, 0 }, {-1, 0}, { 0, 1 }, { 0, -1 }
	};

	for (auto& dir : dirs) {
		int nx = x + dir[0];
		int nz = z + dir[1];

		// マップ範囲内かチェック
		if (nx >= 0 && nx < m_map->GetWidth() &&
			nz >= 0 && nz < m_map->GetHeight()) {
			result.emplace_back(nx, nz);
		}
	}

	return result;
}

//----------------------------------------
// 現在の経路上で次に進むべき方向ベクトルを返す
//----------------------------------------
Vector2 AStarPathfinder::GetDirectionToNextNode() {
	// 経路が2点未満の場合は進行方向なし
	if (m_path.size() < 2) {
		return Vector2(0, 0);
	}

	// 現在位置と次ノードの取得
	Vector2 current = m_path[0];
	Vector2 next = m_path[1];

	// 進行方向ベクトルを算出
	Vector2 direction = next - current;

	if (direction.LengthSq() > 0) {
		direction.Normalize(); // 単位ベクトル化
	}
	else {
		direction = Vector2(0, 0); // 無効方向
	}

	// 次ノードに到達したら先頭を削除して進行
	if ((current - next).LengthSq() < m_map->GetCellSize() * m_map->GetCellSize()) {
		m_path.erase(m_path.begin());
	}

	return direction;
}

//----------------------------------------
// 経路を可視化（デバッグライン描画）
//----------------------------------------
void AStarPathfinder::DrawPath(Engine::LineCommon* line, float yPos) const
{
	// 経路が有効でない場合は描画しない
	if (m_path.size() < 2) return;

	// 線の色（緑）
	Vector4 pathColor = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

	// 経路を順にラインで接続
	for (size_t i = 0; i < m_path.size() - 1; ++i) {
		Vector2 current = m_path[i];
		Vector2 next = m_path[i + 1];

		line->GetDebugLineMeshData().AddLine(
			Vector3(current.x, yPos, current.y), // 始点
			Vector3(next.x, yPos, next.y),       // 終点
			pathColor                            // 色
		);
	}
}

#pragma once

// engine
#include"DirectXGame/engine/struct/Vector2.h"
#include"DirectXGame/engine/struct/Vector3.h"
#include"DirectXGame/engine/struct/Vector4.h"
#include "DirectXGame/engine/math/MapChip.h"


// c++
#include <queue>
#include <unordered_map>


class LineCommon;
class AStarPathfinder {
public:
    struct Node {
        int x, z;
        float gCost;  // スタートから現在までのコスト
        float hCost;  // ヒューリスティック（終点までの推定）
        float fCost() const { return gCost + hCost; }

        bool operator>(const Node& other) const {
            return fCost() > other.fCost();
        }
    };

   // AStarPathfinder(const MapChip& map) : m_map(map) {}

    void SetMap(const MapChip& map) {
        m_map = &map;
    }
    // パスを計算。成功すれば path に結果を格納
    bool FindPath(const Vector2& startWorld,
        const Vector2& endWorld,
        std::vector<Vector2>& path);

    // 進行方向を計算する関数
    Vector2 GetDirectionToNextNode();

    void DrawPath(LineCommon* line,float yPos) const;

private:
    const MapChip* m_map;
    std::vector<Vector2> m_path;  // 最短経路を格納する

    float Heuristic(int x1, int z1, int x2, int z2);
    std::vector<std::pair<int, int>> GetNeighbors(int x, int z);
};


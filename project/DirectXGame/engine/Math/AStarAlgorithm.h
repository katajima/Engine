#pragma once

// engine
#include"DirectXGame/engine/struct/Vector2.h"
#include"DirectXGame/engine/struct/Vector3.h"
#include"DirectXGame/engine/struct/Vector4.h"
#include "DirectXGame/engine/math/MapChip.h"


// c++
#include <queue>
#include <unordered_map>

// 前方宣言
class LineCommon;

/// <summary>
/// A＊アルゴリズムクラス
/// </summary>
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

    /// <summary>
    /// マップチップ設定
    /// </summary>
    void SetMap(const MapChip& map) {
        m_map = &map;
    }
    // パスを計算。成功すれば path に結果を格納
    bool FindPath(const Vector2& startWorld,
        const Vector2& endWorld,
        std::vector<Vector2>& path);

    /// <summary>
    /// 進行方向を計算する関数
    /// </summary>
    Vector2 GetDirectionToNextNode();
    /// <summary>
    /// 描画
    /// </summary>
    void DrawPath(Engine::LineCommon* line,float yPos) const;

private:
    const MapChip* m_map;
    std::vector<Vector2> m_path;  // 最短経路を格納する
    /// <summary>
    /// ヒューリスティック
    /// </summary>
    float Heuristic(int x1, int z1, int x2, int z2);
    /// <summary>
    /// 近所
    /// </summary>
    std::vector<std::pair<int, int>> GetNeighbors(int x, int z);
};


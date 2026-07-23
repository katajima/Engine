#pragma once

/// engine
#include "DirectXGame/engine/struct/Structs2D.h"
#include "DirectXGame/engine/struct/Structs3D.h"
#include "DirectXGame/engine/struct/Vector2.h"
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/struct/Vector4.h"

/// C++
#include <vector>

enum class MapCellType : uint8_t {
    Empty = 0, // 空
    Obstacle = 1, // 障害物
    // 必要に応じて拡張
};
// 前方宣言
namespace Engine {
    class LineCommon;
}

/// <summary>
/// マップチップ
/// </summary>
class MapChip {
public:
    MapChip(int width, int height, float cellSize);
    /// <summary>
    /// クリア
    /// </summary>
    void Clear(MapCellType value = MapCellType::Empty);

    /// <summary>
    /// AABBに当たる範囲をObstacleに変換
    /// </summary>
    void AddObstacleFromBox(const Box& box, MapCellType type = MapCellType::Obstacle);
    /// <summary>
    /// OBBに当たる範囲をObstacleに変換
    /// </summary>
    void AddObstacleFromOBB2D(const OBB2D& obb, MapCellType type = MapCellType::Obstacle);
    /// <summary>
    /// SATベースのOBB vs AABB
    /// </summary>
    bool CheckOBBvsAABB2D(const OBB2D& obb, const Vector2& aabbCenter, float halfExtent) const;

    /// <summary>
    /// マップチップの種類取得
    /// </summary>
    MapCellType GetCell(int x, int z) const;
    /// <summary>
    /// マップチップの種類設定
    /// </summary>
    void SetCell(int x, int z, MapCellType type);

    /// <summary>
    /// マップチップ1マスの大きさ
    /// </summary>
    float GetCellSize() const { return m_cellSize; }

    /// <summary>
    /// ワールド座標→マップ座標変換
    /// </summary>
    bool WorldToMap(float worldX, float worldZ, int& mapX, int& mapZ) const;

    /// <summary>
    /// 横幅取得
    /// </summary>
    int GetWidth() const { return m_width; }
    /// <summary>
    /// 縦幅取得
    /// </summary>
    int GetHeight() const { return m_height; }
    /// <summary>
    /// 描画
    /// </summary>
    void DrawMapChip(Engine::LineCommon* line,float yPos) const;

    /// <summary>
    /// セルが障害物かどうかを返す
    /// </summary>
    bool IsBlocked(int x, int z) const {
        if (x < 0 || z < 0 || x >= m_width || z >= m_height) {
            return true;  // 範囲外は障害物とみなす
        }
        return m_map[z][x] == MapCellType::Obstacle;  // m_map[z][x] が Obstacle の場合は障害物
    }

private:
    int m_width;
    int m_height;
    float m_cellSize;

    std::vector<std::vector<MapCellType>> m_map;
};






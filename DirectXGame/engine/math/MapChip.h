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

class LineCommon;
class MapChip {
public:
    MapChip(int width, int height, float cellSize);

    void Clear(MapCellType value = MapCellType::Empty);

    // AABBに当たる範囲をObstacleに変換
    void AddObstacleFromBox(const Box& box, MapCellType type = MapCellType::Obstacle);
    // OBBに当たる範囲をObstacleに変換
    void AddObstacleFromOBB2D(const OBB2D& obb, MapCellType type = MapCellType::Obstacle);
    // SATベースのOBB vs AABB
    bool CheckOBBvsAABB2D(const OBB2D& obb, const Vector2& aabbCenter, float halfExtent) const;

    // マップチップの種類取得/設定
    MapCellType GetCell(int x, int z) const;
    void SetCell(int x, int z, MapCellType type);

    // マップチップ1マスの大きさ
    float GetCellSize() const { return m_cellSize; }

    // ワールド座標→マップ座標変換
    bool WorldToMap(float worldX, float worldZ, int& mapX, int& mapZ) const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    void DrawMapChip(LineCommon* line,float yPos) const;

    // セルが障害物かどうかを返す
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






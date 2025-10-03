#include "MapChip.h"

#include "DirectXGame/engine/Line/LineCommon.h"

MapChip::MapChip(int width, int height, float cellSize)
    : m_width(width), m_height(height), m_cellSize(cellSize),
    m_map(height, std::vector<MapCellType>(width, MapCellType::Empty)) {}

void MapChip::Clear(MapCellType value) {
    for (auto& row : m_map) {
        std::fill(row.begin(), row.end(), value);
    }
}

void MapChip::AddObstacleFromBox(const Box& box, MapCellType type)
{
    int xStart = static_cast<int>(std::floor(box.min_.x / m_cellSize));
    int xEnd = static_cast<int>(std::floor(box.max_.x / m_cellSize));
    int zStart = static_cast<int>(std::floor(box.min_.y / m_cellSize));
    int zEnd = static_cast<int>(std::floor(box.max_.y / m_cellSize));

    for (int z = zStart; z <= zEnd; ++z) {
        for (int x = xStart; x <= xEnd; ++x) {
            if (x >= 0 && x < m_width && z >= 0 && z < m_height) {
                m_map[z][x] = type;
            }
        }
    }
}

void MapChip::AddObstacleFromOBB2D(const OBB2D& obb, MapCellType type) {
    // OBBの境界を含むグリッド範囲を計算
    float extentX = std::abs(obb.axisX.x * obb.halfSize.x) + std::abs(obb.axisZ.x * obb.halfSize.y);
    float extentZ = std::abs(obb.axisX.y * obb.halfSize.x) + std::abs(obb.axisZ.y * obb.halfSize.y);

    float minX = obb.center.x - extentX;
    float maxX = obb.center.x + extentX;
    float minZ = obb.center.y - extentZ;
    float maxZ = obb.center.y + extentZ;

    int xStart = static_cast<int>(std::floor(minX / m_cellSize));
    int xEnd = static_cast<int>(std::floor(maxX / m_cellSize));
    int zStart = static_cast<int>(std::floor(minZ / m_cellSize));
    int zEnd = static_cast<int>(std::floor(maxZ / m_cellSize));

    for (int z = zStart; z <= zEnd; ++z) {
        for (int x = xStart; x <= xEnd; ++x) {
            if (x < 0 || x >= m_width || z < 0 || z >= m_height) continue;

            // セル中心のAABBを作る
            Vector2 cellCenter = {
                (x + 0.5f) * m_cellSize,
                (z + 0.5f) * m_cellSize
            };

            // SATによる交差判定
            if (CheckOBBvsAABB2D(obb, cellCenter, m_cellSize * 0.5f)) {
                m_map[z][x] = type;
            }
        }
    }
}

bool MapChip::CheckOBBvsAABB2D(const OBB2D& obb, const Vector2& aabbCenter, float halfExtent) const
{
    Vector2 axes[2] = { obb.axisX, obb.axisZ };

    Vector2 diff = {
        aabbCenter.x - obb.center.x,
        aabbCenter.y - obb.center.y
    };

    // AABB軸：X, Z軸
    Vector2 aabbAxes[2] = {
        {1.0f, 0.0f},
        {0.0f, 1.0f}
    };

    for (int i = 0; i < 2; ++i) {
        Vector2 axis = axes[i];

        float projOBB = obb.halfSize.x * std::abs(axis.x * obb.axisX.x + axis.y * obb.axisX.y)
            + obb.halfSize.y * std::abs(axis.x * obb.axisZ.x + axis.y * obb.axisZ.y);

        float projAABB = halfExtent * (std::abs(axis.x * aabbAxes[0].x + axis.y * aabbAxes[0].y) +
            std::abs(axis.x * aabbAxes[1].x + axis.y * aabbAxes[1].y));

        float centerProj = std::abs(diff.x * axis.x + diff.y * axis.y);

        if (centerProj > projOBB + projAABB)
            return false;
    }

    return true;
}


MapCellType MapChip::GetCell(int x, int z) const {
    if (x < 0 || x >= m_width || z < 0 || z >= m_height) return MapCellType::Empty;


    return m_map[z][x];
}

void MapChip::SetCell(int x, int z, MapCellType type) {
    if (x < 0 || x >= m_width || z < 0 || z >= m_height) return;
    m_map[z][x] = type;
}

bool MapChip::WorldToMap(float worldX, float worldZ, int& mapX, int& mapZ) const {
    mapX = static_cast<int>(std::floor(worldX / m_cellSize));
    mapZ = static_cast<int>(std::floor(worldZ / m_cellSize));
    return (mapX >= 0 && mapX < m_width && mapZ >= 0 && mapZ < m_height);
}

void MapChip::DrawMapChip(LineCommon* line,float yPos) const
{

    float size = m_cellSize;
    int width = m_width;
    int height = m_height;

    Vector4 gridColor = Vector4{ 0.3f, 0.3f, 0.3f, 1.0f }; // 薄いグレー
    Vector4 obstacleColor = Vector4{ 1.0f, 0.0f, 0.0f, 1.0f }; // 赤色（障害物用）

    // 横方向のライン（Z軸方向）
    for (int z = 0; z <= height; ++z)
    {
        float fz = z * size;
        for (int x = 0; x <= width; ++x)
        {
            // 障害物がある場合は色を変える
            Vector4 lineColor = IsBlocked(x, z) ? obstacleColor : gridColor;

            float fx = x * size;
            line->AddLine(
                Vector3{ fx, yPos, fz },
                Vector3{ fx, yPos, fz + size },
                lineColor
            );
        }
    }

    // 縦方向のライン（X軸方向）
    for (int x = 0; x <= width; ++x)
    {
        float fx = x * size;
        for (int z = 0; z <= height; ++z)
        {
            // 障害物がある場合は色を変える
            Vector4 lineColor = IsBlocked(x, z) ? obstacleColor : gridColor;

            float fz = z * size;
            line->AddLine(
                Vector3{ fx, yPos, fz },
                Vector3{ fx + size, yPos, fz },
                lineColor
            );
        }
    }
}


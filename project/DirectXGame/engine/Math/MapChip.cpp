#include "MapChip.h"
#include "DirectXGame/engine/Line/LineCommon.h"

// コンストラクタ：マップサイズとセルサイズを指定して初期化
MapChip::MapChip(int width, int height, float cellSize)
    : m_width(width), m_height(height), m_cellSize(cellSize),
    m_map(height, std::vector<MapCellType>(width, MapCellType::Empty)) {
}

// 全セルを指定した値で初期化
void MapChip::Clear(MapCellType value) {
    for (auto& row : m_map) {
        std::fill(row.begin(), row.end(), value);
    }
}

// AABB(Box)から障害物をマップに追加
void MapChip::AddObstacleFromBox(const Box& box, MapCellType type)
{
    // ボックスの最小・最大座標をセルインデックスに変換
    int xStart = static_cast<int>(std::floor(box.min_.x / m_cellSize));
    int xEnd = static_cast<int>(std::floor(box.max_.x / m_cellSize));
    int zStart = static_cast<int>(std::floor(box.min_.y / m_cellSize));
    int zEnd = static_cast<int>(std::floor(box.max_.y / m_cellSize));

    // ボックス範囲内のセルを指定タイプに設定
    for (int z = zStart; z <= zEnd; ++z) {
        for (int x = xStart; x <= xEnd; ++x) {
            if (x >= 0 && x < m_width && z >= 0 && z < m_height) {
                m_map[z][x] = type;
            }
        }
    }
}

// OBB（回転矩形）をもとに障害物セルを設定
void MapChip::AddObstacleFromOBB2D(const OBB2D& obb, MapCellType type) {
    // OBBの外接AABBを求める（範囲走査用）
    float extentX = std::abs(obb.axisX.x * obb.halfSize.x) + std::abs(obb.axisZ.x * obb.halfSize.y);
    float extentZ = std::abs(obb.axisX.y * obb.halfSize.x) + std::abs(obb.axisZ.y * obb.halfSize.y);

    float minX = obb.center.x - extentX;
    float maxX = obb.center.x + extentX;
    float minZ = obb.center.y - extentZ;
    float maxZ = obb.center.y + extentZ;

    // 対応するセル範囲を計算
    int xStart = static_cast<int>(std::floor(minX / m_cellSize));
    int xEnd = static_cast<int>(std::floor(maxX / m_cellSize));
    int zStart = static_cast<int>(std::floor(minZ / m_cellSize));
    int zEnd = static_cast<int>(std::floor(maxZ / m_cellSize));

    // セルごとにOBBとの交差判定
    for (int z = zStart; z <= zEnd; ++z) {
        for (int x = xStart; x <= xEnd; ++x) {
            if (x < 0 || x >= m_width || z < 0 || z >= m_height) continue;

            // セルの中心座標を計算
            Vector2 cellCenter = {
                (x + 0.5f) * m_cellSize,
                (z + 0.5f) * m_cellSize
            };

            // SAT（分離軸定理）でOBBとAABBが重なるか確認
            if (CheckOBBvsAABB2D(obb, cellCenter, m_cellSize * 0.5f)) {
                m_map[z][x] = type;
            }
        }
    }
}

// OBBとAABB（セル）の2D交差判定（SAT法）
bool MapChip::CheckOBBvsAABB2D(const OBB2D& obb, const Vector2& aabbCenter, float halfExtent) const
{
    // OBBのローカル軸
    Vector2 axes[2] = { obb.axisX, obb.axisZ };

    // AABB中心との差分ベクトル
    Vector2 diff = {
        aabbCenter.x - obb.center.x,
        aabbCenter.y - obb.center.y
    };

    // AABBの軸（ワールド座標系）
    Vector2 aabbAxes[2] = {
        {1.0f, 0.0f},
        {0.0f, 1.0f}
    };

    // 各軸で投影して分離があるか確認
    for (int i = 0; i < 2; ++i) {
        Vector2 axis = axes[i];

        // OBBとAABBそれぞれの投影範囲を計算
        float projOBB = obb.halfSize.x * std::abs(axis.x * obb.axisX.x + axis.y * obb.axisX.y)
            + obb.halfSize.y * std::abs(axis.x * obb.axisZ.x + axis.y * obb.axisZ.y);

        float projAABB = halfExtent * (std::abs(axis.x * aabbAxes[0].x + axis.y * aabbAxes[0].y) +
            std::abs(axis.x * aabbAxes[1].x + axis.y * aabbAxes[1].y));

        // 中心間距離の投影
        float centerProj = std::abs(diff.x * axis.x + diff.y * axis.y);

        // 投影範囲が重ならなければ衝突なし
        if (centerProj > projOBB + projAABB)
            return false;
    }

    // 全軸で分離が見つからなければ交差あり
    return true;
}

// セルの内容を取得
MapCellType MapChip::GetCell(int x, int z) const {
    if (x < 0 || x >= m_width || z < 0 || z >= m_height) return MapCellType::Empty;
    return m_map[z][x];
}

// セルの内容を設定
void MapChip::SetCell(int x, int z, MapCellType type) {
    if (x < 0 || x >= m_width || z < 0 || z >= m_height) return;
    m_map[z][x] = type;
}

// ワールド座標をマップのセルインデックスに変換
bool MapChip::WorldToMap(float worldX, float worldZ, int& mapX, int& mapZ) const {
    mapX = static_cast<int>(std::floor(worldX / m_cellSize));
    mapZ = static_cast<int>(std::floor(worldZ / m_cellSize));
    return (mapX >= 0 && mapX < m_width && mapZ >= 0 && mapZ < m_height);
}

// マップをデバッグ描画（ラインでグリッド表示）
void MapChip::DrawMapChip(Engine::LineCommon* line, float yPos) const
{
    float size = m_cellSize;
    int width = m_width;
    int height = m_height;

    // グリッド線と障害物セルの色
    Vector4 gridColor = Vector4{ 0.3f, 0.3f, 0.3f, 1.0f }; // 薄いグレー
    Vector4 obstacleColor = Vector4{ 1.0f, 0.0f, 0.0f, 1.0f }; // 赤色（障害物用）

    // 横方向（Z軸方向）にグリッドラインを描画
    for (int z = 0; z <= height; ++z)
    {
        float fz = z * size;
        for (int x = 0; x <= width; ++x)
        {
            // 障害物セルなら赤色で描画
            Vector4 lineColor = IsBlocked(x, z) ? obstacleColor : gridColor;

            float fx = x * size;
            line->GetDebugLineMeshData().AddLine(
                Vector3{ fx, yPos, fz },
                Vector3{ fx, yPos, fz + size },
                lineColor
            );
        }
    }

    // 縦方向（X軸方向）にグリッドラインを描画
    for (int x = 0; x <= width; ++x)
    {
        float fx = x * size;
        for (int z = 0; z <= height; ++z)
        {
            // 障害物セルなら赤色で描画
            Vector4 lineColor = IsBlocked(x, z) ? obstacleColor : gridColor;

            float fz = z * size;
            line->GetDebugLineMeshData().AddLine(
                Vector3{ fx, yPos, fz },
                Vector3{ fx + size, yPos, fz },
                lineColor
            );
        }
    }
}

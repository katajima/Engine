#include "VisionComponent.h"
#include <DirectXGame/engine/Math/MathFunctions.h>
#include "DirectXGame/engine/line/lineCommon.h"

// オブジェクトが入っているか
inline bool IsObjectVisible(
    const Vector3& enemyPos,
    const Vector3& enemyDir, // Z+ forward方向
    const Vector3& playerPos,
    float viewRadius,
    float viewAngleDegree, // 例: 90°
    std::function<bool(Vector3 origin, Vector3 dir, float maxDist)> raycastFunc // 障害物チェック
)
{
    // プレイヤーとの距離
    Vector3 toPlayer = playerPos - enemyPos;
    float distance = Length(toPlayer);
    if (distance > viewRadius) return false;

    // プレイヤーの方向
    Vector3 toPlayerDir = Normalize(toPlayer);
    float dot = Dot(enemyDir, toPlayerDir);
    float angleCos = cosf(Math::DegreesToRadians(viewAngleDegree * 0.5f));
    if (dot < angleCos) return false;

    // 視界内 → Raycastで壁をチェック
    if (raycastFunc(enemyPos, toPlayerDir, distance)) {
        return false; // 壁で遮られている
    }

    return true;
}

void VisionComponent::DrawDireLine(Vector3 selfPos, Vector3 selfDir, Vector3 targetPos, float viewDistance)
{
    if (gLineDrawer) {
        Vector3 end = selfPos + Normalize(targetPos - selfPos) * viewDistance;
        gLineDrawer->GetDebugLineMeshData().AddLine(selfPos, end, { 1.0f, 1.0f, 0.0f,1.0f }); // 黄色線
    }
}

#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"

#include"functional"


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

// 外部から利用できるよう公開
enum class VisionMode {
    Alert,
    Combat
};

// 前方宣言
namespace Engine {
    class LineCommon;
}
/// <summary>
/// 視界コンポーネントクラス
/// </summary>
class VisionComponent
{
public:
    std::function<bool(Vector3, Vector3, float)> raycastFunc;
private:
    VisionMode currentMode = VisionMode::Alert;
    float alertViewAngle = 90.0f;
    float combatViewAngle = 120.0f;
    float alertViewDistance = 10.0f;
    float combatViewDistance = 20.0f;

   
    float lastSeenTime = -1.0f; // 最後に視界に入った時間
    const float maxLostTime = 2.0f; // 見失われてから最大待機時間（秒）

    bool canSeePlayer = false;


    Engine::LineCommon* gLineDrawer = nullptr;
public:
    //ライン共通クラス設定
    void SetLineCommon(Engine::LineCommon* line) { gLineDrawer = line; }

    // アングル設定
    void SetAlertView(float angle,float distance) { 
        alertViewAngle = angle; 
        alertViewDistance = distance;
    }

    // アングル設定
    void SetCombatView(float angle,float distance) {
        combatViewAngle = angle;
        combatViewDistance = distance;
    }
    
    // ビューモード設定
    void SetMode(VisionMode mode) { currentMode = mode; }
    // ビューモード取得
    VisionMode GetMode() const { return currentMode; }
    
    // プレイヤー視界
    bool IsPlayerVisible() const { return canSeePlayer; }

    // 更新
    void Update(float deltaTime, Vector3 selfPos, Vector3 selfDir, Vector3 targetPos)
    {
        float viewAngle = (currentMode == VisionMode::Combat) ? combatViewAngle : alertViewAngle;
        float viewDistance = (currentMode == VisionMode::Combat) ? combatViewDistance : alertViewDistance;

        bool visible = IsObjectVisible(selfPos, selfDir, targetPos, viewDistance, viewAngle, raycastFunc);

        // 入っているなら
        if (visible)
        {
            canSeePlayer = true;
            lastSeenTime = 0.0f;
        }
        else if (canSeePlayer)
        {
            lastSeenTime += deltaTime;
            if (lastSeenTime > maxLostTime)
                canSeePlayer = false;
        }       
    }
private:
    // ライン描画
    void DrawDireLine(Vector3 selfPos, Vector3 selfDir, Vector3 targetPos, float viewDistance);

};


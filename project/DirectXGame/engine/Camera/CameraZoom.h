#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"

// カメラズーム
/// <summary>
/// CameraZoomを管理・実装するクラス。
/// </summary>
class CameraZoom {
public:

    /// <summary>
    /// 更新
    /// </summary>
    void Update(float& posZ,float dt);
    /// <summary>
    /// デフォルトZ設定
    /// </summary>
    void SetDefultZ(float z){ defultPosZ = z; }

    /// <summary>
    /// リクエスト(ズーム位置、速度、時間)
    /// </summary>
    void Request(const CameraZoomData& data);

private:
    // データ
    CameraZoomData data;
    // デフォルト位置
    float defultPosZ = 0.0f; 
    // 補間
    float t = 0.0f;
    // ズーム中か
    bool isZoom = false;
};
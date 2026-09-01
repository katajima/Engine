#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
/// <summary>
/// 移動コマンド
/// </summary>
struct MoveCommand {
    // 最終的な速度
    Vector3 finalVelocity{};
    // 最終的な方向
    Vector3 finalDirection{};
	// final回転
    Vector3 finalRotation{};
    // 着地しているか
    bool isLanding = false;
    // 地面高さ
    float groundHeight = 0.0f;
};

enum class MoveLayer : uint8_t { 
    kBase, 
    kAdditive, 
    kOverride 
};

/// <summary>
/// 移動リクエスト
/// </summary>
struct MoveRequest {
    // 
    MoveLayer layer = MoveLayer::kBase;
    // 速度倍率(Base)
    float speedMultiplier = 1.0f;
    // Additive/Override
    Vector3 velocity{0,0,0};
	Vector3 direction{};
	Vector3 rotation{};
    int priority = 999;     // Override競合用
    bool invincible = false;

    bool isLanding = false;

    float groundHeight = 0.0f;
};
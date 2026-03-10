#pragma once
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/application/base/State/BaseMainState.h"

#include "DirectXGame/application/base/Character/Base/CharacterContext.h"

namespace Engine {
    class Camera;
}

/// <summary>
/// 移動コマンド
/// </summary>
struct MoveCommand {
    // 最終的な速度
    Vector3 finalVelocity;
    // 最終的な方向
    Vector3 finalDirection;
	// final回転
	Vector3 finalRotation;
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
    Vector3 velocity{};
	Vector3 direction{};
	Vector3 rotation{};
    int priority = 999;     // Override競合用
    bool invincible = false;
};
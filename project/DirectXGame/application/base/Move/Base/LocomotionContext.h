#pragma once
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/application/base/State/BaseMainState.h"

namespace Engine {
    class Camera;
}

/// <summary>
/// 現在の状態
/// </summary>
struct LocomotionContext
{
    float dt = 0.0f;

    // ===== 入力 =====
    InputSystem input;

    //  ===== カメラ =====
    const Engine::Camera* camera = nullptr;
    Vector3 cameraDirection{};

    // ===== 現在の状態 =====
    bool onGround = false;
    bool isAttacking = false;
    bool isHitStun = false;
	Character::CharacterMainState state = Character::CharacterMainState::Idle;

    // ===== 物理情報 =====
    Vector3 position{};
    Vector3 velocity{};
    Vector3 forward{}; // Z+
    Vector3 right{};   // X+
    Vector3 up{};      // Y+

    // 上昇時の重力係数
    float upGravity = 15.0f;
    // 落下時の重力係数
    float fallGravity = 30.0f;
    // 攻撃中の重力係数
    float attackingGravity = 20.0f;

    // ===== パラメータ =====
    float moveSpeed = 6.0f;
    float dashSpeed = 10.0f;
    float jumpPower = 12.0f;

    // ===== スタミナ =====
    float stamina = 100.0f;
    float maxStamina = 100.0f;

    // ===== ロックオン =====
    bool isLockOn = false;
    Vector3 lockOnTargetDir{}; // 正規化済み
};

/// <summary>
/// 移動コマンド
/// </summary>
struct MoveCommand {
    // 最終的な速度
    Vector3 finalVelocity;
    // 最終的な方向
    Vector3 finalDirection;
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
    int priority = 0;     // Override競合用
    bool invincible = false;
};
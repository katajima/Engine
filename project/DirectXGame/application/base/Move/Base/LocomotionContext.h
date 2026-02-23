#pragma once
#include "DirectXGame/application/base/Input/InputSystem.h"

/// <summary>
/// 現在の状態
/// </summary>
struct LocomotionContext
{
    float dt = 0.0f;

    // ===== 入力 =====
    InputSystem input;

    // ===== 現在の状態 =====
    bool onGround = false;
    bool isAttacking = false;
    bool isHitStun = false;

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
};
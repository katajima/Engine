#pragma once
#include<cstdint>
#include<string>
#include<vector>



// 攻撃関係データ
struct AttackData {
    std::string name;        // 技名
    float damage;            // ダメージ
    int startupFrames;       // 発生フレーム
    int activeFrames;        // 持続フレーム
    int recoveryFrames;      // 硬直フレーム
    bool canCancelOnHit;     // ヒット時にキャンセル可能か
    bool canCancelOnWhiff;   // 空振り時でもキャンセル可能か
};

// 攻撃技
enum class AttackState {
    Idle,
    LightAttack1,
    LightAttack2,
    HeavyAttack,
    SpecialAttack
};

struct ComboTransition {
    AttackState current;
    AttackState next;
    bool canCancel;  // キャンセル可能か
};

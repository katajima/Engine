#pragma once
#include<cstdint>
#include<string>
#include<vector>
#include"DirectXGame/engine/struct/Vector2.h"
#include"DirectXGame/engine/struct/Vector3.h"
#include"DirectXGame/engine/struct/Vector4.h"



// 攻撃キーInput
struct AttackKeyFlag
{
    bool IsAttack; // 攻撃するか

    bool IsNormalAttack; // B
    bool IsDashAttack;   // X
    bool IsJampAttack;   // Y
    bool IsSpecialAttack;// RT
};

// 攻撃インプット
struct AttackInput
{
    int comboNum; // 攻撃回数
    bool comboNext = false;
    AttackKeyFlag keyFlag;
};


// 攻撃内部データ
struct AttackData {
    std::string name;        // 技名
    float damage;            // ダメージ
    float hitNum;            // ヒット回数
    float startupFrames;     // 発生フレーム
    float activeFrames;      // 持続フレーム
    float recoveryFrames;    // 硬直フレーム
    bool canCancelOnHit;     // ヒット時にキャンセル可能か
    bool canCancelOnWhiff;   // 空振り時でもキャンセル可能か
    AttackMotion motion;     // 攻撃モーション
};

// 攻撃モーション
struct AttackMotion
{
    Vector3 rotate;            // 回転
    Vector3 scale;             // 拡縮 
    Vector3 translate;         // 位置 

    Vector3 roll; // 回転量(フレーム)
};


// 攻撃技
enum class AttackState {
    Idle,
    LightAttack1,
    LightAttack2,
    HeavyAttack,
    SpecialAttack
};

// 攻撃タイプ(一振りの攻撃に対して)
enum class AttackType
{
    Blow,               // 一撃
    ConsecutiveHits,    // 連撃
    Duration,           // 持続
    LastBlow,           // 後に一撃
    LastConsecutiveHits,// 後に連撃
};



struct ComboTransition {
    AttackState current;
    AttackState next;
    bool canCancel;  // キャンセル可能か
};

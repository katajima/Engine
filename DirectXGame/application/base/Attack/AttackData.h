#pragma once
#include<cstdint>
#include<string>
#include<vector>
#include <functional>
#include"DirectXGame/engine/struct/Vector2.h"
#include"DirectXGame/engine/struct/Vector3.h"
#include"DirectXGame/engine/struct/Vector4.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

// 攻撃キーInput
struct AttackKeyFlag
{
    bool IsAttack; // 攻撃するか

    bool IsNormalAttack; // B
};

// 攻撃インプット
struct AttackInput
{
    int comboNum; // 攻撃回数
    bool comboNext = false;
    AttackKeyFlag keyFlag;
};


// 攻撃モーション
struct AttackMotion
{
    Vector3 rotate;            // 回転
    Vector3 scale;             // 拡縮 
    Vector3 translate;         // 位置 

    Vector3 roll; // 回転量(フレーム)
};

// 攻撃タイプ(一振りの攻撃に対して)
enum class AttackType
{
    Blow,               // 一撃
    ConsecutiveHits,    // 連撃
    Duration,           // 持続
    LastBlow,           // 後に一撃
    LastConsecutiveHits,// 後に連撃
    Charge              // 溜め攻撃
};

// 攻撃内部データ
struct AttackData {
    std::string name;        // 技名
    std::string transformId;
    float damage = 0;        // ダメージ
    float hitNum = 0;        // ヒット回数
    float startupFrames  = 0;// 発生フレーム
    float activeFrames   = 0;// 持続フレーム
    float recoveryFrames = 0;// 硬直フレーム
    bool canCancelOnHit;     // ヒット時にキャンセル可能か
    bool canCancelOnWhiff;   // 空振り時でもキャンセル可能か
    AttackMotion motion;     // 攻撃モーション
    AttackType attackType;   // 攻撃タイプ
};



struct AttackNode {
    std::string id;                        // ノード識別ID
    AttackData data;                      // 攻撃データ
    std::vector<std::string> nextNodeIds; // 遷移可能なノードID（複数）
    std::function<bool()> canCancelFunc;  // キャンセル条件（任意）

    AttackNode() = default;
    AttackNode(const std::string& id_, const AttackData& data_)
        : id(id_), data(data_) {}
};















//// 攻撃用定数
    //struct ConstAttack {
    //	// 振りかぶり時間
    //	uint32_t anticipationTime;
    //	// ための時間
    //	uint32_t chargeTime;
    //	// 攻撃振り時間
    //	uint32_t swingTime;
    //	// 硬直時間
    //	uint32_t recoveryTime;
    //	// 振りかぶり移動速さ
    //	float anticipationSpeed;
    //	// ための移動速さ
    //	float chargeSpeed;
    //	// 攻撃振りの移動速さ
    //	float swingSpeed;
    //};
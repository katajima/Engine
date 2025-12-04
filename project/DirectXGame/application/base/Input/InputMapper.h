#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/Input/Input.h"

enum class GameAction {
    Move,
    Jump,
    Attack,
    Dash,
    Skill,
    Special
};


class InputMapper {
public:

    // 初期化
    void Initialize(Input* input) { input_ = input; };

    // アクションとキーコードを紐づける
    void Bind(GameAction action, int deviceKey)
    {
        table_[action] = deviceKey;
    }

    // 押した瞬間
    bool IsTrigger(GameAction action)
    {
        if (table_.contains(action)) {
            return input_->IsTriggerKey(table_[action]);
        }
        return false;
    }

    // 押しっぱなし
    bool IsPress(GameAction action)
    {
        if (table_.contains(action)) {
            return input_->IsPushKey(table_[action]);
        }
        return false;
    }

    // 離した瞬間
    bool IsRelease(GameAction action)
    {
        if (table_.contains(action)) {
            return input_->IsKeyReleased(table_[action]);
        }
        return false;
    }
private:
    Input* input_ = nullptr;
    std::unordered_map<GameAction, int> table_;
};

struct ActionInputState
{
    // ----------- 攻撃入力（複数ボタン） -----------
    bool attackLightTrigger = false;   // 弱攻撃（押した瞬間）
    bool attackHeavyTrigger = false;   // 強攻撃（押した瞬間）
    bool attackSpecialTrigger = false;  // 必殺技

    bool attackLightHold = false;   // 長押し中
    bool attackHeavyHold = false;

    float attackLightHoldTime = 0.0f;
    float attackHeavyHoldTime = 0.0f;

    // ----------- 移動 -----------
    Vector3 moveDir{ 0,0,0 };
    float moveAmount = 0.0f;

    // ----------- ジャンプ -----------
    bool jumpTrigger = false;
    bool bufferedJump = false; // ジャンプバッファ

    // ----------- ダッシュ -----------
    bool dashTrigger = false;
    bool dashHold = false;

    // ----------- コンボ向け履歴 -----------
    int attackSequence = 0;  // 1段目/2段目/3段目
};

class ActionInputBuilder
{
public:

    void Initialize(InputMapper* mapper) { mapper_ = mapper;};

    void Update(float dt, ActionInputState& outState);

private:
    InputMapper* mapper_ = nullptr;

    float attackHoldTimer_ = 0.0f;

    int attackSequence_ = 0;
    float comboTimer_ = 0.0f;

    float lastDirInputTime_ = 0.0f;
    int dashTapCount_ = 0;
};
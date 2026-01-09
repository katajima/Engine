#pragma once
#include<string>
#include <memory>
#include <functional>
#include"DirectXGame/engine/math/MathFunctions.h"






// 攻撃用サブステート
enum class AttackSubState {
    Ready,  // 準備
    Swing, // 攻撃1
    End,    // 後隙
    Invalid // 無効値
};

// 攻撃ステート
template<class SubStateEnum, class BaseSubStateType>
class SubStateMachine;

class BaseCharacter;

/// <summary>
/// 攻撃サブステートクラス
/// </summary>
class BaseAttackSubState {
public:
    virtual ~BaseAttackSubState() = default;

    BaseAttackSubState(const AttackSubState& state, BaseCharacter* character,
        SubStateMachine<AttackSubState, BaseAttackSubState>* fsm)
        : attackSubstate_(state), character_(character), fsm_(fsm) {
    }

    // 開始
    virtual void Enter() {}
    // 更新
    virtual void Update(float deltaTime) {}
    // 終了
    virtual void Exit() {}


public:
    // サブステート取得
    AttackSubState GetAttackSubState() const { return attackSubstate_; }
protected:
    AttackSubState attackSubstate_;
    // テンプレート指定付きで宣言
    SubStateMachine<AttackSubState, BaseAttackSubState>* fsm_ = nullptr;
    BaseCharacter* character_ = nullptr;
};



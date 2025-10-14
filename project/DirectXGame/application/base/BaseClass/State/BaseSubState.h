#pragma once
#include<string>
#include <memory>
#include <functional>
#include"DirectXGame/engine/math/MathFanctions.h"






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
class BaseAttackSubState {
public:
    virtual ~BaseAttackSubState() = default;

    BaseAttackSubState(const AttackSubState& state, BaseCharacter* character,
        SubStateMachine<AttackSubState, BaseAttackSubState>* fsm)
        : attackSubstate_(state), character_(character), fsm_(fsm) {
    }
    virtual void Enter() {}
    virtual void Update(float deltaTime) {}
    virtual void Exit() {}


public:
    AttackSubState GetAttackSubState() const { return attackSubstate_; }
protected:
    AttackSubState attackSubstate_;
    // テンプレート指定付きで宣言
    SubStateMachine<AttackSubState, BaseAttackSubState>* fsm_ = nullptr;
    BaseCharacter* character_ = nullptr;
};



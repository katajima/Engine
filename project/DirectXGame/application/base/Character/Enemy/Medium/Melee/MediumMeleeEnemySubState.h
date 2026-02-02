#pragma once
#include <DirectXGame/application/base/State/SubStateMachine.h>


template<class SubStateEnum, class BaseSubStateType>
class SubStateMachine;


/// <summary>
/// 攻撃準備
/// </summary>
class MediumMeleeEnemyAttackReadySubState :public BaseAttackSubState{
public:
    MediumMeleeEnemyAttackReadySubState(BaseCharacter* c, SubStateMachine<AttackSubState, BaseAttackSubState>* fsm)
        : BaseAttackSubState(AttackSubState::Ready, c, fsm) {
    }

    // 開始
    void Enter() override {
        timer_ = 0.0f;
    }
    // 更新
    void Update(float deltaTime) override;

    // 終了
    void Exit() override {}

private:
    float timer_ = 0.0f;
    const float readyTime_ = 4.75f;
    Vector3 dire = {};
};

// 攻撃本体
class MediumMeleeEnemyAttackSwingSubState : public BaseAttackSubState {
public:
    MediumMeleeEnemyAttackSwingSubState(BaseCharacter* c, SubStateMachine<AttackSubState, BaseAttackSubState>* fsm)
        : BaseAttackSubState(AttackSubState::Swing, c, fsm) {
    }
    // 開始
    void Enter() override;
    // 更新
    void Update(float deltaTime) override;
    // 終了
    void Exit() override;

private:
    float timer_ = 0.0f;
    const float swingTime_ = 1.25f;

    Vector3 dire_{};

};




// 終了状態（後隙）
class MediumMeleeEnemyAttackEndSubState : public BaseAttackSubState {
public:
    MediumMeleeEnemyAttackEndSubState(BaseCharacter* c, SubStateMachine<AttackSubState, BaseAttackSubState>* fsm)
        : BaseAttackSubState(AttackSubState::End, c, fsm) {
    }
    // 開始
    void Enter() override;
    // 更新
    void Update(float deltaTime) override;
    // 終了
    void Exit() override {
        printf("[AttackEnd] Exit\n");
    }

private:
    Vector3 rotate_{};
    float targetRotateY_ = 0;
    float timer_ = 0.0f;
    const float endTime_ = 0.5f;
};

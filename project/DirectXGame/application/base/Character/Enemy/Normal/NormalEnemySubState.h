#pragma once
#include <DirectXGame/application/base/BaseClass/State/SubStateMachine.h>


template<class SubStateEnum, class BaseSubStateType>
class SubStateMachine;

class NormalEnemyAttackReadySubState :public BaseAttackSubState{
public:
    NormalEnemyAttackReadySubState(BaseCharacter* c, SubStateMachine<AttackSubState, BaseAttackSubState>* fsm)
        : BaseAttackSubState(AttackSubState::Ready, c, fsm) {
    }

    void Enter() override {
        timer_ = 0.0f;
    }

    void Update(float deltaTime) override;

    void Exit() override {}

private:
    float timer_ = 0.0f;
    const float readyTime_ = 1.0f;
    Vector3 dire = {};
};

// 攻撃本体
class NormalEnemyAttackSwingSubState : public BaseAttackSubState {
public:
    NormalEnemyAttackSwingSubState(BaseCharacter* c, SubStateMachine<AttackSubState, BaseAttackSubState>* fsm)
        : BaseAttackSubState(AttackSubState::Swing, c, fsm) {
    }

    void Enter() override;

    void Update(float deltaTime) override;
    void Exit() override;

private:
    float timer_ = 0.0f;
    const float swingTime_ = 2.0f;
};




// 終了状態（後隙）
class NormalEnemyAttackEndSubState : public BaseAttackSubState {
public:
    NormalEnemyAttackEndSubState(BaseCharacter* c, SubStateMachine<AttackSubState, BaseAttackSubState>* fsm)
        : BaseAttackSubState(AttackSubState::End, c, fsm) {
    }

    void Enter() override;

    void Update(float deltaTime) override;

    void Exit() override {
        printf("[AttackEnd] Exit\n");
    }

private:
    Vector3 rotate_{};
    float targetRotateY_ = 0;
    float timer_ = 0.0f;
    const float endTime_ = 0.5f;
};

#pragma once
#include <DirectXGame/application/base/Character/State/SubStateMachine.h>
#include <DirectXGame/engine/struct/Vector3.h>

namespace Character {
    template<class SubStateEnum, class BaseSubStateType>
    class SubStateMachine;


    /// <summary>
    /// 攻撃準備
    /// </summary>
    class MediumMeleeEnemyAttackReadySubState :public BaseAttackSubState {
    public:
        MediumMeleeEnemyAttackReadySubState(BaseCharacter* c, SubStateMachine<AttackSubState, BaseAttackSubState>* fsm)
            : BaseAttackSubState(AttackSubState::Ready, c, fsm) {
        }

        // 開始
        void Enter() override;
        // 更新
        void Update(float deltaTime) override;

        // 終了
        void Exit() override {}

    private:
        float timer_ = 0.0f;
        const float readyTime_ = 2.75f;
        Vector3 dire_ = {};
        float size = 1.0f;
        int clock = 1;
    };

    // 攻撃本体
/// <summary>
/// MediumMeleeEnemyAttackSwingSubStateを管理・実装するクラス。
/// </summary>
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
/// <summary>
/// MediumMeleeEnemyAttackEndSubStateを管理・実装するクラス。
/// </summary>
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
        float startRotateY_ = 0.0f;
        float rotateDiffY_ = 0.0f;
        float timer_ = 0.0f;
        const float endTime_ = 0.5f;
    };
}
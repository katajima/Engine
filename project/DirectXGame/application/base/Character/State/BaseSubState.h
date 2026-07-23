#pragma once
#include <string>
#include <memory>
#include <functional>

namespace Character {
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

        /// <summary>
        /// 開始
        /// </summary>
        virtual void Enter() {}
        /// <summary>
        /// 更新
        /// </summary>
        virtual void Update(float deltaTime) {}
        /// <summary>
        /// 終了
        /// </summary>
        virtual void Exit() {}


    public:
        /// <summary>
        /// サブステート取得
        /// </summary>
        AttackSubState GetAttackSubState() const { return attackSubstate_; }
    protected:
        AttackSubState attackSubstate_;
        // テンプレート指定付きで宣言
        SubStateMachine<AttackSubState, BaseAttackSubState>* fsm_ = nullptr;
        BaseCharacter* character_ = nullptr;
    };
}


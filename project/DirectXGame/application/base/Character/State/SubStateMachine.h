#pragma once
#include "BaseSubState.h"
#include <unordered_map>
#include <functional>
#include <cassert>
#include "DirectXGame/engine/Utility/ConvertUtility.h"

namespace Character {
    /// <summary>
    /// サブステートマシーン
    /// </summary>
    /// <typeparam name="SubStateEnum"></typeparam>
    /// <typeparam name="BaseSubStateType"></typeparam>
    template<class SubStateEnum, class BaseSubStateType>
/// <summary>
/// SubStateMachineを管理・実装するクラス。
/// </summary>
    class SubStateMachine
    {
    public:
        using FactoryType = std::function<std::unique_ptr<BaseSubStateType>(BaseCharacter*, SubStateMachine*)>;


        explicit SubStateMachine(BaseCharacter* character)
            : character_(character) {
        }

        /// <summary>
        /// 登録
        /// </summary>
        void RegisterState(SubStateEnum type, FactoryType factory) {
            factories_[type] = factory;
        }
        /// <summary>
        /// 変更
        /// </summary>
        void ChangeState(const AttackSubState& name) {
            auto it = factories_.find(name);
            if (it != factories_.end()) {
                nowState_ = name;
                ChangeState(it->second(character_, this));
            }
            else {
                assert(false && "SubState not registered!");
            }
        }

        /// <summary>
        /// 更新
        /// </summary>
        void Update(float deltaTime = ConvertUtility::kDefaultDeltaTime) {
            if (state_) state_->Update(deltaTime);
        }
        /// <summary>
        /// 現在のステート取得
        /// </summary>
        AttackSubState GetAttackSubState() const {
            if (state_) {
                return state_->GetAttackSubState();
            }
            // 状態が無い場合は無効値を返す
            return SubStateEnum::Invalid;
        }
        /// <summary>
        /// 終了したか取得
        /// </summary>
        bool IsFinished() const { return isFinished_; }
        /// <summary>
        /// 終了したか設定
        /// </summary>
        void SetFinished(bool flag) { isFinished_ = flag; }
    private:
        /// <summary>
        /// ステート変更
        /// </summary>
        void ChangeState(std::unique_ptr<BaseAttackSubState> newState) {
            if (state_) {
                state_->Exit(); // 終了
            }
            state_ = std::move(newState);
            if (state_) {
                state_->Enter();// 開始
            }
        }
    private:
        bool isFinished_ = false;
    private:
        std::unordered_map<SubStateEnum, FactoryType> factories_;
        std::unique_ptr<BaseSubStateType> state_;
        SubStateEnum nowState_ = SubStateEnum::Invalid;
        BaseCharacter* character_ = nullptr;
    };
}

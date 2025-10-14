#pragma once
#include "BaseSubState.h"
#include <unordered_map>
#include <functional>
#include <memory>
#include <cassert>

template<class SubStateEnum, class BaseSubStateType>
class SubStateMachine
{
public:
    using FactoryType = std::function<std::unique_ptr<BaseSubStateType>(BaseCharacter*, SubStateMachine*)>;


    explicit SubStateMachine(BaseCharacter* character)
        : character_(character) {
    }
    // 登録
    void RegisterState(SubStateEnum type, FactoryType factory) {
        factories_[type] = factory;
    }

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

    // 更新
    void Update(float deltaTime = 1.0f / 60.0f) {
        if (state_) state_->Update(deltaTime);
    }

    AttackSubState GetAttackSubState() const {
        if (state_) {
            return state_->GetAttackSubState();
        }
        // 状態が無い場合は無効値を返す
        return SubStateEnum::Invalid;
    }

    bool IsFinished() const { return isFinished_; }
    void SetFinished(bool flag) { isFinished_ = flag; }
private:
    // ステート変更
    void ChangeState(std::unique_ptr<BaseAttackSubState> newState) {
        if (state_) {
            state_->Exit();
        }
        state_ = std::move(newState);
        if (state_) {
            state_->Enter();
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

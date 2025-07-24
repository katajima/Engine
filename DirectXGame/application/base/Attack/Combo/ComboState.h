#pragma once
#include "DirectXGame/application/base/BaseClass/Character/BaseCharacter.h"
#include "DirectXGame/application/base/BaseClass/Attack/AttackData.h"
#include "DirectXGame/application/base/Attack/Input/AttackInputHander.h"




class ComboState {
public:
    virtual ~ComboState() = default;

    virtual void Enter(BaseCharacter* owner) = 0;
    virtual void Update(BaseCharacter* owner, float deltaTime) = 0;
    virtual void Exit(BaseCharacter* owner) = 0;

    // 次のステートに遷移するかを判断する
    virtual std::shared_ptr<ComboState> HandleInput(BaseCharacter* owner, AttackInput input) = 0;

    // 入力受付時間の範囲チェック
    bool IsInputAcceptable(float timeInState) const {
        return timeInState >= inputWindowStart && timeInState <= inputWindowEnd;
    }

    virtual float GetTimeInState() const = 0;

protected:
    float inputWindowStart = 0.1f;      // 入力受付スタート
    float inputWindowEnd = 0.5f;        // 入力受付エンド
};

class ComboNodeState : public ComboState {
public:
    ComboNodeState(std::string anim, ComboData combo)
        : animation(anim), comboData_(combo) {
    }

    void Enter(BaseCharacter* owner) override;

    void Update(BaseCharacter* owner, float dt) override;

    void Exit(BaseCharacter* owner) override;

    std::shared_ptr<ComboState> HandleInput(BaseCharacter* owner, AttackInput input) override {
        if (!IsInputAcceptable(timeInState)) return nullptr;

        auto it = nextStates.find(input);
        if (it != nextStates.end()) {
            return it->second;
        }
        return nullptr;
    }

    void SetNextState(AttackInput input, std::shared_ptr<ComboNodeState> next) {
        nextStates[input] = next;
    }

    bool HasNextState() const {
        return !nextStates.empty();
    }

    float GetTimeInState() const { return timeInState; }

private:
    std::string animation;
    ComboData comboData_;


    float timeInState = 0.0f;

    std::map<AttackInput, std::shared_ptr<ComboNodeState>> nextStates;
};


class ComboStateMachine {
public:
    ComboStateMachine(BaseCharacter* entity) : owner(entity) {}

    void SetState(std::shared_ptr<ComboState> state) {
        if (currentState) currentState->Exit(owner);
        currentState = state;
        if (currentState) currentState->Enter(owner);
        bufferedInput.reset(); // 状態遷移したら入力リセット
    }

    void Update(float dt) {
        if (!currentState) return;

        currentState->Update(owner, dt);

        // 入力がバッファされていて、入力受付時間内なら状態遷移
        if (bufferedInput) {
            auto next = currentState->HandleInput(owner, *bufferedInput);
            // もし次のステートがあれば、遷移
            if (next && currentState->IsInputAcceptable(currentState->GetTimeInState())) {
                SetState(next);
            }
            bufferedInput.reset();
        }
    }

    // 入力はバッファに保存のみ
    void HandleInput(AttackInput input) {
        bufferedInput = input;
    }

    void Reset() {
        SetState(rootState);
    }

    void SetRoot(std::shared_ptr<ComboState> state) {
        rootState = state;
        if (rootState) {
            SetState(rootState);
        }
    }

    bool IsComboFinished() const {
        auto node = std::dynamic_pointer_cast<ComboNodeState>(currentState);
        if (!node) return true;
        return !node->HasNextState();
    }

private:
    BaseCharacter* owner;
    std::shared_ptr<ComboState> currentState;
    std::shared_ptr<ComboState> rootState;

    std::optional<AttackInput> bufferedInput;  // 入力バッファ

};

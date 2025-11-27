#pragma once
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Attack/Combo/ComboData.h"
#include "DirectXGame/application/base/Attack/Input/AttackInputHander.h"



/// <summary>
/// コンボステートクラス
/// </summary>
class ComboState {
public:
    virtual ~ComboState() = default;
    // 開始
    virtual void Enter(BaseCharacter* owner) = 0;
    // 更新
    virtual void Update(BaseCharacter* owner, float deltaTime) = 0;
    // 終了
    virtual void Exit(BaseCharacter* owner) = 0;

    // 次のステートに遷移するかを判断する
    virtual std::shared_ptr<ComboState> HandleInput(BaseCharacter* owner, AttackInput input) = 0;

    // 入力受付時間の範囲チェック
    bool IsInputAcceptable(float timeInState) const {
        return timeInState >= inputWindowStart && timeInState <= inputWindowEnd;
    }
    // 時間
    virtual float GetTimeInState() const = 0;

protected:
    float inputWindowStart = 0.1f;      // 入力受付スタート
    float inputWindowEnd = 0.5f;        // 入力受付エンド
};


/// <summary>
/// コンボステートノード
/// </summary>
class ComboNodeState : public ComboState {
public:
    ComboNodeState(std::string anim, ComboData combo)
        : animation(anim), comboData_(combo) {
    }
    // 開始
    void Enter(BaseCharacter* owner) override;
    // 更新
    void Update(BaseCharacter* owner, float dt) override;
    // 終了
    void Exit(BaseCharacter* owner) override;

    // 入力があったら
    std::shared_ptr<ComboState> HandleInput(BaseCharacter* owner, AttackInput input) override {
        if (!IsInputAcceptable(timeInState)) return nullptr;

        auto it = nextStates.find(input);
        if (it != nextStates.end()) {
            return it->second;
        }
        return nullptr;
    }

    // 次のステート
    void SetNextState(AttackInput input, std::shared_ptr<ComboNodeState> next) {
        nextStates[input] = next;
    }

    // 次のステートは存在するか
    bool HasNextState() const {
        return !nextStates.empty();
    }
    // 時間内にステート
    float GetTimeInState() const { return timeInState; }

private:
    std::string animation;
    ComboData comboData_;


    float timeInState = 0.0f;
    Vector3 dire_{};

    std::map<AttackInput, std::shared_ptr<ComboNodeState>> nextStates;
};

/// <summary>
/// コンボステートマシーン
/// </summary>
class ComboStateMachine {
public:
    ComboStateMachine(BaseCharacter* entity) : owner(entity) {}

    // ステート設定
    void SetState(std::shared_ptr<ComboState> state) {
        if (currentState) currentState->Exit(owner);
        currentState = state;
        if (currentState) currentState->Enter(owner);
        bufferedInput.reset(); // 状態遷移したら入力リセット
    }
    // 更新
    void Update(float dt) {
        // ステートが無いなら早期リターン
        if (!currentState) return;

        // 現在のステート更新
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
    // リセット
    void Reset() {
        SetState(rootState);
    }
    // 設定
    void SetRoot(std::shared_ptr<ComboState> state) {
        rootState = state;
        if (rootState) {
            SetState(rootState);
        }
    }
    // コンボが終了したか
    bool IsComboFinished() const {
        auto node = std::dynamic_pointer_cast<ComboNodeState>(currentState);
        if (!node) return true;
        return !node->HasNextState();
    }

private:
    BaseCharacter* owner;                       // 
    std::shared_ptr<ComboState> currentState;   // 
    std::shared_ptr<ComboState> rootState;      // 

    std::optional<AttackInput> bufferedInput;   // 入力バッファ
};

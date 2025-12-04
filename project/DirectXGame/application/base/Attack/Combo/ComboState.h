#pragma once
#include "DirectXGame/application/base/Attack/Combo/ComboData.h"
#include "DirectXGame/application/base/Attack/Input/AttackInputHander.h"

#include <optional>
#include <memory>
#include <map>

// 前方宣言
class BaseCharacter;

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
    bool IsInputAcceptable() const {
        return timeInState >= inputWindowStart && timeInState <= inputWindowEnd;
    }
   
    // 時間
    float GetTimeInState() const { return timeInState; }
    // 次のステートえ移行受付する時間
    float GetNextStateTime() const { return timeInState > timeNextState; }
    // ステート終了時間
    float GetEndStateTime() const { return timeInState > stateEndTime; }

protected:
    float inputWindowStart = 0.1f;      // 入力受付スタート
    float inputWindowEnd = 0.5f;        // 入力受付エンド
    float timeNextState = 0.5f;         // ステート移行時間
    float stateEndTime = 0.5f;          // ステート終了時間
    float timeInState = 0.0f;           // 時間


    bool isGravity = true;              // 重力はあるか？
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
    
private:
    std::string animation;
    ComboData comboData_;


   
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
    void SetState(std::shared_ptr<ComboState> state);
    // 更新
    void Update(float dt);

    // 入力はバッファに保存のみ
    void HandleInput(AttackInput input) { 
        bufferedInput = input;
    }
    // リセット
    void Reset() { SetState(rootState); }
    // 設定
    void SetRoot(std::shared_ptr<ComboState> state);
    // コンボが終了したか
    bool IsComboFinished() const {
        auto node = std::dynamic_pointer_cast<ComboNodeState>(currentState);
        if (!node) return true;
        return !node->HasNextState();
    }

private:
    BaseCharacter* owner;                       // 使用者
private:
    std::shared_ptr<ComboState> currentState;   // 現在のステート
    std::shared_ptr<ComboState> rootState;      // 初期ステート

    std::optional<AttackInput> bufferedInput;   // 入力バッファ

    // 次のステートに移行するか
    bool isNextState = false;
};

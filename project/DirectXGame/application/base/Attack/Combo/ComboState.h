#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboData.h"
#include "DirectXGame/application/base/Attack/Input/AttackInputHandler.h"

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

public:
    // 次のステートに遷移するかを判断する
    virtual std::shared_ptr<ComboState> HandleInput(BaseCharacter* owner, AttackInput input) = 0;
    // 入力受付時間の範囲チェック
    virtual bool IsInputAcceptable() const = 0;
    // 次のステートえ移行受付する時間
    virtual float GetNextStateTime() const = 0;
    // ステート終了時間
    virtual bool GetEndStateTime() const = 0;
    // 次のステートへ移行可能か
    virtual bool GetIsNextState() const = 0;

    // 時間
    float GetTimeInState() const { return timeInState; }

    void Set(const Vector3& dire) { direction_ = dire; };

protected:
    float timeInState = 0.0f;           // 時間
    Vector3 direction_{ 0,0,1 };
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

    // 終了処理
    void End(BaseCharacter* owner);

public:

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
    // 入力受付可能か
    bool IsInputAcceptable() const override{
        return comboData_.comboCondition.IsComdoInputWindow(timeInState);
    }
    // 次のステートえ移行受付する時間
    float GetNextStateTime() const override{ 
        return timeInState > comboData_.comboCondition.GetComboNextTime(); 
    }
    // ステート終了時間
    bool GetEndStateTime() const override{
        return timeInState > comboData_.comboCondition.GetComboEndTime();
    }
    // 次のステートへ移行可能か
    bool GetIsNextState() const override {
        return comboData_.comboCondition.IsNextCombo();
    };
    // キャンセルするか
    bool GetIsCansel() const {
        return comboData_.comboCondition.IsComboCansel();
    }

private:
    std::string animation;
    ComboData comboData_;
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

    Vector3 direction_{ 0,0,1 };
};

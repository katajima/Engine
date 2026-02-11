#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboData.h"
#include "DirectXGame/application/base/Attack/Input/AttackInputHandler.h"

#include <optional>
#include <memory>
#include <map>

// 前方宣言
namespace Character {
    class BaseCharacter;
}
namespace Combo {
    /// <summary>
    /// コンボステートクラス
    /// </summary>
    class State {
    public:
        virtual ~State() = default;
        // 開始
        virtual void Enter(Character::BaseCharacter* owner) = 0;
        // 更新
        virtual void Update(Character::BaseCharacter* owner, float deltaTime) = 0;
        // 終了
        virtual void Exit(Character::BaseCharacter* owner) = 0;

    public:
        // 次のステートに遷移するかを判断する
        virtual std::shared_ptr<State> HandleInput(Character::BaseCharacter* owner, AttackInput input) = 0;
        // 入力受付時間の範囲チェック
        virtual bool IsInputAcceptable() = 0;
        // 次のステートえ移行受付する時間
        virtual bool GetNextStateTime() = 0;
        // ステート終了時間
        virtual bool GetEndStateTime() = 0;
        // 次のステートへ移行可能か
        virtual bool GetIsNextState() = 0;
    public:
        // 時間
        float GetTimeInState() const { return timeInState; }
        // 時間設定
        void SetTimeInState(float time) { timeInState = time; }
        // 方向取得
        void Set(const Vector3& dire) { direction_ = dire; };



    protected:
        float timeInState = 0.0f;           // 時間
        Vector3 direction_{ 0,0,1 };
    };


    /// <summary>
    /// コンボステートノード
    /// </summary>
    class NodeState : public State {
    public:
        NodeState(std::string anim, ComboData combo)
            : animation(anim), comboData_(combo) {
        }
        // 開始
        void Enter(Character::BaseCharacter* owner) override;
        // 更新
        void Update(Character::BaseCharacter* owner, float dt) override;
        // 終了
        void Exit(Character::BaseCharacter* owner) override;

        // 終了処理
        void End(Character::BaseCharacter* owner);

    public:

        // 入力があったら
        std::shared_ptr<State> HandleInput(Character::BaseCharacter* owner, AttackInput input) override {
            auto it = nextStates.find(input);
            if (it != nextStates.end()) {
                return it->second;
            }
            return nullptr;
        }

        // 次のステート
        void SetNextState(AttackInput input, std::shared_ptr<NodeState> next) {
            nextStates[input] = next;
        }

        // 次のステートは存在するか
        bool HasNextState() const {
            return !nextStates.empty();
        }
        // 入力受付可能か
        bool IsInputAcceptable() override {
            return comboData_.GetComboCondition().IsComdoNextInputWindow(timeInState);
        }
        // 次のステートえ移行受付する時間
        bool GetNextStateTime() override {
            return timeInState > comboData_.GetComboCondition().GetComboNextTime();
        }
        // ステート終了時間
        bool GetEndStateTime() override {
            return timeInState > comboData_.GetComboCondition().GetComboEndTime();
        }
        // 次のステートへ移行可能か
        bool GetIsNextState() override {
            return comboData_.GetComboCondition().GetNextReceiver().GetIsNext();
        };
        // キャンセルするか
        bool GetIsCansel() {
            return comboData_.GetComboCondition().GetCancelReceiver().GetIsCancel();
        }
        // コンボ名取得
        std::string GetName() const { return name; }
        // アニメーション名取得
        std::string GetAnimationName() const { return animation; }
        // コンボ名設定
        void SetName(const std::string& comboName) { name = comboName; }

        // コンボデータ取得
        ComboData& GetComboData() { return comboData_; }

    private:
        // コンボ名
        std::string name;
        // アニメーション名
        std::string animation;
        // コンボデータ
        ComboData comboData_;
        // 次のステートマップ
        std::map<AttackInput, std::shared_ptr<NodeState>> nextStates;
    };

    /// <summary>
    /// コンボステートマシーン
    /// </summary>
    class StateMachine {
    public:
        StateMachine(Character::BaseCharacter* entity) : owner(entity) {}

        // ステート設定
        void SetState(std::shared_ptr<State> state);
        // 更新
        void Update(float dt);

        // 入力はバッファに保存のみ
        void HandleInput(AttackInput input) {
            bufferedInput = input;
        }
        // リセット
        void Reset() { SetState(rootState); }
        // 設定
        void SetRoot(std::shared_ptr<State> state);
        // コンボが終了したか
        bool IsComboFinished() const {
            auto node = std::dynamic_pointer_cast<NodeState>(currentState);
            if (!node) return true;
            return !node->HasNextState();
        }

    private:
        Character::BaseCharacter* owner;                       // 使用者
    private:
        std::shared_ptr<State> currentState;   // 現在のステート
        std::shared_ptr<State> rootState;      // 初期ステート

        std::optional<AttackInput> bufferedInput;   // 入力バッファ
    };
}

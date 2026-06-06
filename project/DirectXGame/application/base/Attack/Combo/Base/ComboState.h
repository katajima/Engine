#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboData.h"
#include "DirectXGame/application/base/Attack/Input/AttackInputHandler.h"
#include "DirectXGame/application/base/Character/Base/CharacterContext.h"

namespace Combo {
	enum class TransitionCondition {
		Default,
		GroundMiss,
		GroundHit,
		AirMiss,
		AirHit,
	};

    /// <summary>
    /// コンボステートクラス
    /// </summary>
    class State {
    public:
        virtual ~State() = default;
        // 開始
        virtual void Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) = 0;
        // 更新
        virtual void Update(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) = 0;
        // 終了
        virtual void Exit(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) = 0;

    public:
        // 次のステートに遷移するかを判断する
        virtual std::shared_ptr<State> HandleInput(Character::BaseCharacter* owner, ActionInput input) = 0;
        // 入力受付時間の範囲チェック
        virtual bool IsInputAcceptable() = 0;
        // 次のステートえ移行受付する時間
        virtual bool GetNextStateTime() = 0;
        // ステート終了時間
        virtual bool GetEndStateTime() = 0;
        // 次のステートへ移行可能か
        virtual bool GetIsNextState() = 0;
        // 次のステートへ強制移行するか
        virtual bool GetIsCompulsionNext() = 0;
    public:
        // 時間
        float GetTimeInState() const { return timeInState; }
        // 時間設定
        void SetTimeInState(float time) { timeInState = time; }
        // 方向取得
        void Set(const Vector3& dire) { direction_ = dire; };



    protected:
        bool isDebug = false;
        float timeInState = 0.0f;           // 時間
        Vector3 direction_{ 0,0,1 };
    };


    /// <summary>
    /// コンボステートノード
    /// </summary>
    class NodeState : public State {
    public:
        NodeState(std::string anim, ComboData combo)
            : animation(anim), comboData(combo) {
        }
        // 開始
        void Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) override;
        // 更新
        void Update(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) override;
        // 終了
        void Exit(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) override;

        // 終了処理
        void End(Character::BaseCharacter* owner, const Character::CharacterContext& ctx);

    public:

        // 入力があったら
        std::shared_ptr<State> HandleInput(Character::BaseCharacter* owner, ActionInput input) override;

        // 次のステート
        void SetNextState(ActionInput input, TransitionCondition condition, std::shared_ptr<NodeState> next);
        void NotifyHit() { hasHit_ = true; }
        bool HasHit() const { return hasHit_; }
        std::shared_ptr<NodeState> ResolveNextState(Character::BaseCharacter* owner, ActionInput input);

        // 次のステートは存在するか
        bool HasNextState() const;
        bool HasNextState(ActionInput input) const;
        // 入力受付可能か
        bool IsInputAcceptable() override {
            return comboData.GetComboCondition().IsComdoNextInputWindow(timeInState);
        }
        // 次のステートえ移行受付する時間
        bool GetNextStateTime() override {
            return timeInState > comboData.GetComboCondition().GetComboNextTime();
        }
        // ステート終了時間
        bool GetEndStateTime() override {
            return timeInState > comboData.GetComboCondition().GetComboEndTime();
        }
        // 次のステートへ移行可能か
        bool GetIsNextState() override {
            return comboData.GetComboCondition().GetNextReceiver().GetIsNext();
        };
        // 次のステートへ強制移行するか
        bool GetIsCompulsionNext() override {
            return comboData.GetComboCondition().GetData().isCompulsionNext;
        };

        // キャンセルするか
        bool GetIsCansel() {
            return comboData.GetComboCondition().GetCancelReceiver().GetIsCancel();
        }


        // コンボ名取得
        std::string GetName() const { return name; }
        // アニメーション名取得
        std::string GetAnimationName() const { return animation; }
        // コンボ名設定
        void SetName(const std::string& comboName) { name = comboName; }

        // コンボデータ取得
        ComboData& Data() { return comboData; }

        ComboData GetData() const { return comboData; }

    private:
        // コンボ名
        std::string name;
        // アニメーション名
        std::string animation;
        // コンボデータ
        ComboData comboData;
        struct TransitionTargets {
            std::weak_ptr<NodeState> defaultTarget;
            std::weak_ptr<NodeState> groundMiss;
            std::weak_ptr<NodeState> groundHit;
            std::weak_ptr<NodeState> airMiss;
            std::weak_ptr<NodeState> airHit;
        };
        // 次のステートマップ
        std::map<ActionInput, TransitionTargets> nextStates;
        bool hasHit_ = false;
    };

    /// <summary>
    /// コンボステートマシーン
    /// </summary>
    class StateMachine {
    public:
        StateMachine(Character::BaseCharacter* entity) : owner(entity) {}

        // ステート設定
        void SetState(std::shared_ptr<State> state, const Character::CharacterContext& ctx);
        // 更新
        void Update(const Character::CharacterContext& ctx);

        // 入力はバッファに保存のみ
        void HandleInput(ActionInput input) {
            bufferedInput = input;
            bufferedInputAge_ = 0.0f;
            isBufferedInputAccepted_ = false;
        }
        bool CanTransition(ActionInput input) const;
        std::shared_ptr<NodeState> ResolveTransitionTarget(ActionInput input) const;
        std::optional<ActionInput> ConsumeTransitionedInput();
        void NotifyCurrentStateHit();
        // リセット
        void Reset() { SetState(rootState,{}); }
        // 設定
        void SetRoot(std::shared_ptr<State> state);
        // コンボが終了したか
        bool IsComboFinished() const {
            auto node = std::dynamic_pointer_cast<NodeState>(currentState);
            if (!node) return true;
            return !node->HasNextState();
        }

        // 現在のステートを取得
        std::shared_ptr<NodeState> GetCurrentState() const { 
            if (currentState) {
                return std::dynamic_pointer_cast<NodeState>(currentState);
            }
            else {
                return nullptr;
            }
        }
        // デバッグか設定
        void SetIsDebug(bool is) { isDebug = is; }
    private:
        Character::BaseCharacter* owner;                       // 使用者
    private:
        std::shared_ptr<State> currentState;   // 現在のステート
        std::shared_ptr<State> rootState;      // 初期ステート

        std::optional<ActionInput> bufferedInput;   // 入力バッファ
        float bufferedInputAge_ = 0.0f;
        std::optional<ActionInput> transitionedInput_;
        bool isBufferedInputAccepted_ = false;
        bool isDebug = false;
    };
}

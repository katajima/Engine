#pragma once
#include "DirectXGame/application/base/BaseClass/Character/BaseCharacter.h"
#include "DirectXGame/application/base/BaseClass/Attack/AttackData.h"

//
//enum class AttackInput {
//    Light,
//    Heavy
//};
//
//
//class ComboData {
//
//};
//
//
//class ComboState {
//public:
//    virtual ~ComboState() = default;
//
//    virtual void Enter(BaseCharacter* owner) = 0;
//    virtual void Update(BaseCharacter* owner, float deltaTime) = 0;
//    virtual void Exit(BaseCharacter* owner) = 0;
//
//    // 次のステートに遷移するかを判断する
//    virtual ComboState* HandleInput(BaseCharacter* owner, AttackInput input) = 0;
//
//    // 入力受付時間の範囲チェック
//    bool IsInputAcceptable(float timeInState) const {
//        return timeInState >= inputWindowStart && timeInState <= inputWindowEnd;
//    }
//
//protected:
//    float inputWindowStart = 0.1f;      // 入力受付スタート
//    float inputWindowEnd = 0.5f;        // 入力受付エンド
//};
//
//class ComboNodeState : public ComboState {
//public:
//    ComboNodeState(std::string anim, float dmg)
//        : animation(anim), damage(dmg) {
//    }
//
//    void Enter(BaseCharacter* owner) override {
//        owner->GetObject3D()->SetAnimetion("",0.1f);
//        //PlayAnimation(owner, animation);
//        timeInState = 0.0f;
//    }
//
//    void Update(BaseCharacter* owner, float dt) override {
//        timeInState += dt;
//    }
//
//    void Exit(BaseCharacter* owner) override {
//        // アニメ終了時の処理など
//    }
//
//    ComboState* HandleInput(BaseCharacter* owner, AttackInput input) override {
//        if (!IsInputAcceptable(timeInState)) return nullptr;
//
//        auto it = nextStates.find(input);
//        if (it != nextStates.end()) {
//            return it->second;
//        }
//        return nullptr;
//    }
//
//    void SetNextState(AttackInput input, ComboNodeState* next) {
//        nextStates[input] = next;
//    }
//
//private:
//    std::string animation;
//    float damage;
//    float staminaCost = 0;					// スタミナ消費量
//    float mpCost = 0;                       // MP消費
//    float movementSpeedMultiplier = 1.0f;	// 攻撃中の移動速度倍率
//    KnockbackData knockbackData{};			// ノックバックデータ
//
//
//    float timeInState = 0.0f;
//
//    std::map<AttackInput, ComboNodeState*> nextStates;
//};
//
//
//class ComboStateMachine {
//public:
//    ComboStateMachine(BaseCharacter* entity) : owner(entity) {}
//
//    void SetState(ComboState* state) {
//        if (currentState) currentState->Exit(owner);
//        currentState = state;
//        if (currentState) currentState->Enter(owner);
//    }
//
//    void Update(float dt) {
//        if (currentState) currentState->Update(owner, dt);
//    }
//
//    void HandleInput(AttackInput input) {
//        if (!currentState) return;
//
//        ComboState* next = currentState->HandleInput(owner, input);
//        if (next) {
//            SetState(next);
//        }
//    }
//
//    void Reset() {
//        SetState(rootState);
//    }
//
//    void SetRoot(ComboState* state) {
//        rootState = state;
//        SetState(rootState);
//    }
//
//private:
//    BaseCharacter* owner;
//    ComboState* currentState = nullptr;
//    ComboState* rootState = nullptr;
//};

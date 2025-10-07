#pragma once
#include "BaseState.h"



using CharacterMainStateFactory = std::function<std::unique_ptr<BaseCharacterState>(BaseCharacter*)>;


class CharacterStateMachine {
public:
    CharacterStateMachine() {
    
    }


    // 初期化
    void Init(BaseCharacter* character, CharacterMainState initialState = CharacterMainState::Move);

    // 更新
    void Update();


    void ChangeState(const CharacterMainState& name)  {
        auto it = stateFactoryMap_.find(name);
        if (it != stateFactoryMap_.end()) {
            nowState_ = name;
            ChangeState(it->second(character_));
        }
        else {
            assert(true);
        }
    }
    
    // ---- 現在ステートの取得 ----
    BaseCharacterState* GetCurrentState() const { return state_.get(); }


    CharacterMainState GetCurrentMainState() const {
        return state_ ? state_->GetCharacterMainState() : CharacterMainState::Move;
    }

    // 登録
    void RegisterState(const CharacterMainState& name, CharacterMainStateFactory factory) {
        stateFactoryMap_[name] = factory;
    }
private:
    // ステート変更
    void ChangeState(std::unique_ptr<BaseCharacterState> newState) {
        if (state_) {
            state_->Exit();
        }
        state_ = std::move(newState);
        if (state_) {
            state_->Enter();
        }
    }
   
private:
    std::unique_ptr<BaseCharacterState> state_;// ステート
    // 状態管理用Map
    std::unordered_map<CharacterMainState, CharacterMainStateFactory> stateFactoryMap_;
    //
    CharacterMainState nowState_;
    // 操作状態
    BaseCharacter* character_ = nullptr;
};
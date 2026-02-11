#pragma once
#include "BaseMainState.h"


namespace Character {
    using CharacterMainStateFactory = std::function<std::unique_ptr<BaseCharacterState>(BaseCharacter*)>;

    /// <summary>
    /// キャラクターステートマシーンクラス
    /// </summary>
    class CharacterStateMachine {
    public:
        CharacterStateMachine() {}


        // 初期化
        void Init(BaseCharacter* character, CharacterMainState initialState = CharacterMainState::Move);

        // 更新
        void Update();

        // 変更
        void ChangeState(const CharacterMainState& name) {
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

        // 現在のステート取得
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
            if (!isOneFrameOneState_)
                if (state_) {
                    state_->Exit(); // 終了
                }
            state_ = std::move(newState);
            if (state_) {
                state_->Enter(); // 開始
            }
            isOneFrameOneState_ = true;
        }

    private:
        std::unique_ptr<BaseCharacterState> state_;// ステート
        // 状態管理用Map
        std::unordered_map<CharacterMainState, CharacterMainStateFactory> stateFactoryMap_;
        //
        CharacterMainState nowState_;
        // 操作状態
        BaseCharacter* character_ = nullptr;
        // ステート変更が1フレーム中に何度も起らないようにするフラグ
        bool isOneFrameOneState_ = false;
    };
}
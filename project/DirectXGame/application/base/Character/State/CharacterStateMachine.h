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


        /// <summary>
        /// 初期化
        /// </summary>
        void Init(BaseCharacter* character, CharacterMainState initialState = CharacterMainState::Move);

        /// <summary>
        /// 更新
        /// </summary>
        void Update(const CharacterContext& ctx);

        /// <summary>
        /// 変更
        /// </summary>
        void ChangeState(const CharacterMainState& name) {
            auto it = stateFactoryMap_.find(name);
            if (it != stateFactoryMap_.end()) {
                prevState_ = nowState_;
                nowState_ = name;
                ChangeState(it->second(character));
            }
            else {
                assert(true);
            }
        }

        /// <summary>
        /// ---- 現在ステートの取得 ----
        /// </summary>
        BaseCharacterState* GetCurrentState() const { return state_.get(); }

        /// <summary>
        /// 現在のステート取得
        /// </summary>
        CharacterMainState GetCurrentMainState() const {
            return nowState_;
        }
        CharacterMainState GetPrevState() const { return prevState_; }

        /// <summary>
        /// 登録
        /// </summary>
        void RegisterState(const CharacterMainState& name, CharacterMainStateFactory factory) {
            stateFactoryMap_[name] = factory;
        }
    private:
        /// <summary>
        /// ステート変更
        /// </summary>
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
        std::unique_ptr<BaseCharacterState> state_ = nullptr;// ステート
        // 状態管理用Map
        std::unordered_map<CharacterMainState, CharacterMainStateFactory> stateFactoryMap_;
        //
        CharacterMainState nowState_{};
        // 
        CharacterMainState prevState_{};
        // 操作状態
        BaseCharacter* character = nullptr;
        // ステート変更が1フレーム中に何度も起らないようにするフラグ
        bool isOneFrameOneState_ = false;
    };
}
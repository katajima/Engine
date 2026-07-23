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
		LockOn,
		NoLockOn,
	};

    /// <summary>
    /// コンボ1段分の共通インターフェースです。
    /// </summary>
    class State {
    public:
        virtual ~State() = default;

        /// <summary>
        /// ステート開始時にアニメーション、移動、攻撃判定などの初期化を行います。
        /// </summary>
        /// <param name="owner">このコンボを実行するキャラクターです。nullptrは想定していません。</param>
        /// <param name="ctx">キャラクター周辺システムへの参照をまとめたコンテキストです。</param>
        virtual void Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) = 0;

        /// <summary>
        /// ステート中の移動、判定、演出、遷移条件を更新します。
        /// </summary>
        /// <param name="owner">このコンボを実行中のキャラクターです。nullptrは想定していません。</param>
        /// <param name="ctx">更新に必要な外部システム参照をまとめたコンテキストです。</param>
        virtual void Update(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) = 0;

        /// <summary>
        /// ステート終了時に、一時的な判定や演出状態を解除します。
        /// </summary>
        /// <param name="owner">このコンボを終了するキャラクターです。nullptrは想定していません。</param>
        /// <param name="ctx">終了処理に必要な外部システム参照をまとめたコンテキストです。</param>
        virtual void Exit(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) = 0;

    public:
        /// <summary>
        /// 入力受付時間と遷移条件を評価し、入力に応じた次のコンボステートを解決します。
        /// </summary>
        /// <param name="owner">入力を処理するキャラクターです。</param>
        /// <param name="input">評価する攻撃入力です。</param>
        /// <returns>遷移先がある場合は次のステート、遷移しない場合はnullptrを返します。</returns>
        virtual std::shared_ptr<State> HandleInput(Character::BaseCharacter* owner, ActionInput input) = 0;

        /// <summary>
        /// 現在のステート経過時間が入力受付ウィンドウ内にあり、入力を受理できる状態か確認します。
        /// </summary>
        /// <returns>入力受付中ならtrue、それ以外はfalseです。</returns>
        virtual bool IsInputAcceptable() = 0;

        /// <summary>
        /// 次のステートへ遷移可能な時刻に到達しているか確認します。
        /// </summary>
        /// <returns>遷移可能時刻に到達していればtrue、それ以外はfalseです。</returns>
        virtual bool GetNextStateTime() = 0;

        /// <summary>
        /// このステートの終了時刻に到達しているか確認します。
        /// </summary>
        /// <returns>終了時刻を過ぎていればtrue、それ以外はfalseです。</returns>
        virtual bool GetEndStateTime() = 0;

        /// <summary>
        /// コンボ条件側が次ステートへの遷移を許可しているか取得します。
        /// </summary>
        /// <returns>遷移許可中ならtrue、それ以外はfalseです。</returns>
        virtual bool GetIsNextState() = 0;

        /// <summary>
        /// 入力に関係なく次ステートへ強制遷移する設定か取得します。
        /// </summary>
        /// <returns>強制遷移を行う場合はtrue、それ以外はfalseです。</returns>
        virtual bool GetIsCompulsionNext() = 0;
    public:
        /// <summary>
        /// ステート開始からの経過時間を取得します。
        /// </summary>
        /// <returns>経過時間を秒単位で返します。</returns>
        float GetTimeInState() const { return timeInState; }

        /// <summary>
        /// ステート開始からの経過時間を設定します。
        /// </summary>
        /// <param name="time">設定する経過時間です。単位は秒です。</param>
        void SetTimeInState(float time) { timeInState = time; }

        /// <summary>
        /// コンボ移動や攻撃方向に使う基準方向を設定します。
        /// </summary>
        /// <param name="dire">設定する方向ベクトルです。呼び出し側で必要に応じて正規化してください。</param>
        void Set(const Vector3& dire) { direction_ = dire; };



    protected:
        bool isDebug = false;               // デバッグ表示や編集用の挙動を有効にするか
        float timeInState = 0.0f;           // 現在ステートに入ってからの経過時間（秒）
        Vector3 direction_{ 0,0,1 };        // コンボ移動や攻撃に使う基準方向
    };


    /// <summary>
    /// コンボステートノード
    /// </summary>
    class NodeState : public State {
    public:
        /// <summary>
        /// コンボノードを作成します。
        /// </summary>
        /// <param name="anim">このノードで再生するアニメーション名です。</param>
        /// <param name="combo">移動、判定、演出、遷移条件をまとめたコンボデータです。</param>
        NodeState(std::string anim, ComboData combo)
            : animation(anim), comboData(combo) {
        }

        /// <inheritdoc/>
        void Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) override;

        /// <inheritdoc/>
        void Update(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) override;

        /// <inheritdoc/>
        void Exit(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) override;

        /// <summary>
        /// コンボノード固有の終了処理を実行します。
        /// </summary>
        /// <param name="owner">終了処理を行うキャラクターです。</param>
        /// <param name="ctx">終了処理で利用する外部システム参照です。</param>
        void End(Character::BaseCharacter* owner, const Character::CharacterContext& ctx);

    public:

        /// <summary>
        /// 入力受付時間と遷移条件を評価し、入力に対応する次のステートを解決します。
        /// </summary>
        /// <param name="owner">入力を処理するキャラクターです。</param>
        /// <param name="input">評価する攻撃入力です。</param>
        /// <returns>遷移先ステートがあれば共有ポインタ、なければnullptrです。</returns>
        std::shared_ptr<State> HandleInput(Character::BaseCharacter* owner, ActionInput input) override;

        /// <summary>
        /// 入力と遷移条件に対応する次ノードを登録します。
        /// </summary>
        /// <param name="input">次ノードへ進むための攻撃入力です。</param>
        /// <param name="condition">命中、空中、ロックオンなどの追加遷移条件です。</param>
        /// <param name="next">遷移先ノードです。weak_ptrとして保持されるため所有権は受け取りません。</param>
        void SetNextState(ActionInput input, TransitionCondition condition, std::shared_ptr<NodeState> next);

        /// <summary>
        /// このノードの攻撃が命中したことを、キャンセル条件・遠距離攻撃・カメラ・音へ通知します。
        /// </summary>
        void NotifyHit() {
            // ヒット発生をキャンセル条件、遠距離投擲物、カメラ演出へ伝える
            hasHit_ = true;
            comboData.GetComboRange().NotifyHit();
            comboData.GetComboCamera().OnHit();
			// 命中確定時だけ、このコンボに設定されたヒット音を再生する。
			comboData.GetComboAudio().OnHit();
        }
		/// <summary>
		/// このノードでヒットカウント加算を一度実行済みか取得します。
		/// </summary>
		/// <returns>一度でも加算済みならtrue、それ以外はfalseです。</returns>
		bool HasIncrementedHitCount() const { return hasIncrementedHitCount_; }
		/// <summary>
		/// このノードのヒットカウント加算済み状態を設定します。
		/// </summary>
		/// <param name="value">加算済みとして扱う場合はtrueです。</param>
		void SetHasIncrementedHitCount(bool value) { hasIncrementedHitCount_ = value; }
        /// <summary>
        /// このノード中に命中通知が発生したか取得します。
        /// </summary>
        /// <returns>命中済みならtrue、それ以外はfalseです。</returns>
        bool HasHit() const { return hasHit_; }

        /// <summary>
        /// 入力と現在条件から実際に遷移する次ノードを解決します。
        /// </summary>
        /// <param name="owner">遷移条件の評価に使うキャラクターです。</param>
        /// <param name="input">評価する攻撃入力です。</param>
        /// <returns>遷移可能なノードがあればその共有ポインタ、存在しなければnullptrです。</returns>
        std::shared_ptr<NodeState> ResolveNextState(Character::BaseCharacter* owner, ActionInput input);

        /// <summary>
        /// いずれかの入力に対する次ノードが登録されているか確認します。
        /// </summary>
        /// <returns>次ノード候補が1つ以上あればtrue、それ以外はfalseです。</returns>
        bool HasNextState() const;

        /// <summary>
        /// 指定入力に対する次ノードが登録されているか確認します。
        /// </summary>
        /// <param name="input">確認する攻撃入力です。</param>
        /// <returns>指定入力に次ノード候補があればtrue、それ以外はfalseです。</returns>
        bool HasNextState(ActionInput input) const;

        /// <summary>
        /// 現在のステート経過時間が入力受付ウィンドウ内か確認します。
        /// </summary>
        /// <returns>入力を受け付けられる時間ならtrue、それ以外はfalseです。</returns>
        bool IsInputAcceptable() override {
            return comboData.GetComboCondition().IsComdoNextInputWindow(timeInState);
        }

        /// <inheritdoc/>
        bool GetNextStateTime() override {
            return timeInState > comboData.GetComboCondition().GetComboNextTime();
        }

        /// <inheritdoc/>
        bool GetEndStateTime() override {
            return timeInState > comboData.GetComboCondition().GetComboEndTime();
        }

        /// <inheritdoc/>
        bool GetIsNextState() override {
            return comboData.GetComboCondition().GetNextReceiver().GetIsNext();
        };

        /// <inheritdoc/>
        bool GetIsCompulsionNext() override {
            return comboData.GetComboCondition().GetData().isCompulsionNext;
        };

        /// <summary>
        /// 現在ノードがキャンセル遷移可能な状態か取得します。
        /// </summary>
        /// <returns>キャンセル可能ならtrue、それ以外はfalseです。</returns>
        bool GetIsCansel() {
            return comboData.GetComboCondition().GetCancelReceiver().GetIsCancel();
        }


        /// <summary>
        /// コンボノード名を取得します。
        /// </summary>
        /// <returns>エディターや保存で使うノード名を返します。</returns>
        std::string GetName() const { return name; }

        /// <summary>
        /// 再生するアニメーション名を取得します。
        /// </summary>
        /// <returns>アニメーション名を返します。</returns>
        std::string GetAnimationName() const { return animation; }

        /// <summary>
        /// コンボノード名を設定します。
        /// </summary>
        /// <param name="comboName">保存や表示に使う新しいノード名です。</param>
        void SetName(const std::string& comboName) { name = comboName; }

        /// <summary>
        /// 編集可能なコンボデータ参照を取得します。
        /// </summary>
        /// <returns>このノードが保持するコンボデータへの参照を返します。</returns>
        ComboData& Data() { return comboData; }

        /// <summary>
        /// コンボデータのコピーを取得します。
        /// </summary>
        /// <returns>このノードが保持するコンボデータのコピーを返します。</returns>
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
            std::weak_ptr<NodeState> lockOn;
            std::weak_ptr<NodeState> noLockOn;
        };
        // 次のステートマップ
        std::map<ActionInput, TransitionTargets> nextStates;
        bool hasHit_ = false; // このノードの実行中に攻撃が命中したか
		// 一度だけ加算設定のヒットカウントを既に実行したか
		bool hasIncrementedHitCount_ = false;
    };

    /// <summary>
    /// コンボステートマシーン
    /// </summary>
    class StateMachine {
    public:
        /// <summary>
        /// コンボステートマシンを作成します。
        /// </summary>
        /// <param name="entity">コンボを実行するキャラクターです。所有権は受け取りません。</param>
        StateMachine(Character::BaseCharacter* entity) : owner(entity) {}

        /// <summary>
        /// 現在ステートを差し替え、必要に応じてEnter/Exitを呼びます。
        /// </summary>
        /// <param name="state">新しく実行するステートです。nullptrの場合は現在ステートを解除します。</param>
        /// <param name="ctx">ステート切り替えに使う外部システム参照です。</param>
        void SetState(std::shared_ptr<State> state, const Character::CharacterContext& ctx);

        /// <summary>
        /// 現在ステートと入力バッファを更新します。
        /// </summary>
        /// <param name="ctx">ステート更新に必要な外部システム参照です。</param>
        void Update(const Character::CharacterContext& ctx);

        /// <summary>
        /// 入力を即時遷移させず、次の受付タイミング用にバッファします。
        /// </summary>
        /// <param name="input">保存する攻撃入力です。</param>
        void HandleInput(ActionInput input) {
            bufferedInput = input;
            bufferedInputAge_ = 0.0f;
            isBufferedInputAccepted_ = false;
        }

        /// <summary>
        /// 指定入力で現在ステートから遷移できるか確認します。
        /// </summary>
        /// <param name="input">評価する攻撃入力です。</param>
        /// <returns>遷移可能ならtrue、それ以外はfalseです。</returns>
        bool CanTransition(ActionInput input) const;

        /// <summary>
        /// 指定入力で遷移するノードを取得します。
        /// </summary>
        /// <param name="input">評価する攻撃入力です。</param>
        /// <returns>遷移先ノードがあれば共有ポインタ、なければnullptrを返します。</returns>
        std::shared_ptr<NodeState> ResolveTransitionTarget(ActionInput input) const;

        /// <summary>
        /// 実際に遷移へ使われた入力を取り出してクリアします。
        /// </summary>
        /// <returns>遷移に使用済みの入力があればその値、なければstd::nulloptです。</returns>
        std::optional<ActionInput> ConsumeTransitionedInput();

        /// <summary>
        /// 現在ノードへ命中通知を転送します。
        /// </summary>
        void NotifyCurrentStateHit();

        /// <summary>
        /// ルートステートへ戻します。
        /// </summary>
        void Reset() { SetState(rootState,{}); }

        /// <summary>
        /// コンボ開始時に使うルートステートを設定します。
        /// </summary>
        /// <param name="state">ルートにするステートです。共有所有として保持します。</param>
        void SetRoot(std::shared_ptr<State> state);

        /// <summary>
        /// 現在のコンボが終端ノードに到達しているか確認します。
        /// </summary>
        /// <returns>現在ステートがない、または次ノードがない場合はtrueです。</returns>
        bool IsComboFinished() const {
            auto node = std::dynamic_pointer_cast<NodeState>(currentState);
            if (!node) return true;
            return !node->HasNextState();
        }

        /// <summary>
        /// 現在実行中のノードステートを取得します。
        /// </summary>
        /// <returns>現在ステートがNodeStateならその共有ポインタ、違う場合はnullptrです。</returns>
        std::shared_ptr<NodeState> GetCurrentState() const { 
            if (currentState) {
                return std::dynamic_pointer_cast<NodeState>(currentState);
            }
            else {
                return nullptr;
            }
        }
        /// <summary>
        /// デバッグ用の挙動を有効化するか設定します。
        /// </summary>
        /// <param name="is">有効にする場合はtrue、無効にする場合はfalseです。</param>
        void SetIsDebug(bool is) { isDebug = is; }
    private:
        Character::BaseCharacter* owner;                       // 使用者
    private:
        std::shared_ptr<State> currentState;   // 現在のステート
        std::shared_ptr<State> rootState;      // 初期ステート

        std::optional<ActionInput> bufferedInput;   // 入力バッファ
        float bufferedInputAge_ = 0.0f;                    // バッファ入力を保持してからの経過時間（秒）
        std::optional<ActionInput> transitionedInput_;     // 実際に遷移へ使われた入力
        bool isBufferedInputAccepted_ = false;             // バッファ入力が現在ステートで受理済みか
        bool isDebug = false;                              // デバッグ表示や編集用の挙動を有効にするか
    };
}

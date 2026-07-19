#pragma once
#include <map>
#include <string>
#include <memory>
#include <optional>
#include "DirectXGame/application/base/Attack/Combo/Base/Debug/ComboDebug.h"
#include "DirectXGame/application/base/Character/Base/CharacterContext.h"

namespace Combo {

	const std::string kHitEffectCountKey = "ヒットエフェクト数";
	const std::string kComboEffectCountKey = "コンボエフェクト数";

	static std::string MakeHitEffectSlotKey(int index) {
		return "ヒットエフェクトスロット" + std::to_string(index);
	}

	static std::string MakeHitEffectNameKey(int index) {
		return "ヒットエフェクト名前" + std::to_string(index);
	}

	static std::string MakeComboEffectNameKey(int index) {
		return "コンボエフェクト名前" + std::to_string(index);
	}

	static std::string MakeComboEffectStartTimeKey(int index) {
		return "コンボエフェクト開始時間" + std::to_string(index);
	}

	static std::string MakeComboEffectEndTimeKey(int index) {
		return "コンボエフェクト終了時間" + std::to_string(index);
	}

	static std::string MakeComboEffectIntervalKey(int index) {
		return "コンボエフェクト頻度" + std::to_string(index);
	}

	static std::string MakeComboEffectParentKey(int index) {
		return "コンボエフェクト追従先" + std::to_string(index);
	}

	static std::string MakeComboEffectTriggerTypeKey(int index) {
		return "コンボエフェクト発生条件" + std::to_string(index);
	}

	static std::string MakeComboEffectOffsetKey(int index) {
		return "コンボエフェクトオフセット" + std::to_string(index);
	}

	static std::string MakeComboEffectSequenceName(int index) {
		return "コンボエフェクト" + std::to_string(index);
	}

	const std::string kGroundLightStartKey = "開始コンボ(地上弱攻撃)";
	const std::string kAirLightStartKey = "開始コンボ(空中弱攻撃)";
	const std::string kGroundHeavyStartKey = "開始コンボ(地上強攻撃)";
	const std::string kAirHeavyStartKey = "開始コンボ(空中強攻撃)";
	const std::string kGroundSkillStartKey = "開始コンボ(地上スキル)";
	const std::string kAirSkillStartKey = "開始コンボ(空中スキル)";
	const std::string kDodgeLightStartKey = "開始コンボ(回避中弱攻撃)";
	const std::string kDodgeHeavyStartKey = "開始コンボ(回避中強攻撃)";
	const std::string kDodgeSkillStartKey = "開始コンボ(回避中スキル)";
	const std::string kDodgeSuccessLightStartKey = "開始コンボ(回避成功後弱攻撃)";
	const std::string kDodgeSuccessHeavyStartKey = "開始コンボ(回避成功後強攻撃)";
	const std::string kDodgeSuccessSkillStartKey = "開始コンボ(回避成功後スキル)";

	struct StartComboRoutes {
		std::string groundLight;
		std::string airLight;
		std::string groundHeavy;
		std::string airHeavy;
		std::string groundSkill;
		std::string airSkill;
		std::string dodgeLight;
		std::string dodgeHeavy;
		std::string dodgeSkill;
		std::string dodgeSuccessLight;
		std::string dodgeSuccessHeavy;
		std::string dodgeSuccessSkill;
	};


	/// <summary>
	/// コンボシステム
	/// </summary>
	class System
	{
	public:
		~System() {
			ClearNode();
			comboStateMachine_.reset();
		}

		/// <summary>キャラクターのコンボ状態、保存データ、デバッグ編集機能を初期化する。</summary>
		/// <param name="character">コンボを実行する所有者。非所有ポインターで利用中は有効であること。</param>
		/// <param name="lineCommon">当たり判定などのデバッグ描画に使用するライン基盤。</param>
		/// <param name="globalVariables">コンボ定義の登録・保存・読込先。</param>
		/// <param name="audioManager">攻撃音の再生とプレビューに使用する音声管理。非所有ポインター。</param>
		void Initialize(Character::BaseCharacter* character, Engine::LineCommon* lineCommon,
			Engine::GlobalVariables* globalVariables, Engine::AudioManager* audioManager);

		/// <summary>現在のコンボノード、入力受付、スタミナ、クールダウンを更新する。</summary>
		/// <param name="ctx">所有者の入力、移動、状態をまとめたフレームコンテキスト。</param>
		void Update(const Character::CharacterContext& ctx);
		/// <summary>攻撃入力から開始または派生可能なコンボを要求する。</summary>
		/// <param name="input">弱攻撃、強攻撃、スキルなどのアクション入力。</param>
		/// <returns>コンボ開始または遷移要求を受理した場合はtrue。</returns>
		bool RequestAttack(ActionInput input);
		// 現在のコンボ攻撃が命中したことを通知
		// 命中を通知し、コンボ設定に応じてヒットカウントを加算したか返す
		bool NotifyAttackHit();
		// クリア
		void ClearNode();
		// 名前設定
		void Create(const std::string name);
	public: // 保存や適応に関しての関数

		/// <summary>コンボデータの全項目をGlobalVariablesへ登録または上書きする。</summary>
		/// <param name="name">保存グループ名。</param>
		/// <param name="data">登録するコンボデータ。</param>
		void ApplyGlobalComboData(const std::string& name, Combo::GlobalData& data);

		/// <summary>保存済み項目を指定コンボデータへ読み込む。</summary>
		/// <param name="name">読込元の保存グループ名。</param>
		/// <param name="data">読込結果を格納するデータ。</param>
		void GetGlobalComboData(const std::string& name, Combo::GlobalData& data);

		/// <summary>指定コンボデータを保存対象へ設定する。</summary>
		/// <param name="name">保存グループ名。</param>
		/// <param name="data">保存するコンボデータ。</param>
		void SetGlobalComboData(const std::string& name, Combo::GlobalData& data);

		// 全保存項目の設定
		void SetGlobalComboDatas();
	public:
		/// <summary>エフェクトなどが追従できる親ワールド変換を名前付きで登録する。</summary>
		/// <param name="name">参照に使用する一意な名前。</param>
		/// <param name="transform">呼び出し側が所有するワールド変換。登録中は有効であること。</param>
		void SetParentTransform(const std::string& name, Engine::WorldTransform* transform) {
			parentTransforms_[name] = transform;
		}

		/// <summary>登録名から親ワールド変換を検索する。</summary>
		/// <param name="name">SetParentTransformで登録した名前。</param>
		/// <returns>非所有ポインター。未登録の場合はnullptr。</returns>
		Engine::WorldTransform* GetParentTransform(const std::string& name) {
			auto it = parentTransforms_.find(name);
			if (it != parentTransforms_.end()) {
				return it->second;
			}
			return nullptr;
		}

		/// <summary>指定コンボノードに対応する登録名を取得する。</summary>
		/// <param name="comboNodeName">検索するコンボノード名。</param>
		/// <returns>存在する場合はノード名、存在しない場合は空文字。</returns>
		std::string GetParentName(const std::string& comboNodeName) {
			auto it = comboNodes_.find(comboNodeName);
			if (it != comboNodes_.end()) {
				return it->first;
			}
			return "";
		}

		//	トランスフォーム取得
		std::map<std::string, Engine::WorldTransform*> GetParentTransforms() { return parentTransforms_; };


	public:
		/// <summary>コンボ状態機械を取得する。</summary>
		/// <returns>Systemが所有する状態機械への非所有ポインター。</returns>
		StateMachine* GetComboStateMachine() { return comboStateMachine_.get(); }

		// コンボノードステートマップ取得
		std::map<std::string, std::shared_ptr<NodeState>>  GetComboNodeStates() { return comboNodes_; };
		// コンボノード名前マップ取得
		std::map<std::string, std::string> GetComboNodeNames() { return comboNodenames_; }
		// 名前取得
		std::string GetName() const { return name; }
		// 開始コンボ取得
		StartComboRoutes GetStartComboRoutes() const;
		// 開始コンボ設定
		void SetStartComboRoutes(const StartComboRoutes& routes);
		/// <summary>名前に対応するコンボノードを取得する。</summary>
		/// <param name="name">検索するノード名。</param>
		/// <returns>共有所有権を持つノード。存在しない場合はnullptr。</returns>
		std::shared_ptr<NodeState> GetComboNodeState(const std::string& name) {
			auto it = comboNodes_.find(name);
			if (it != comboNodes_.end()) {
				return it->second;
			}
			return nullptr;
		}
	public:
		/// <summary>生成済みコンボノードを名前付きで登録する。</summary>
		/// <param name="name">一意なノード名。</param>
		/// <param name="node">登録する共有ノード。nullptrは登録しないこと。</param>
		void AddComboNode(const std::string& name, std::shared_ptr<NodeState> node);
		/// <summary>アニメーション名とデータからコンボノードを生成して登録する。</summary>
		/// <param name="nodeName">一意なノード名。</param>
		/// <param name="animationName">再生するアニメーション名。</param>
		/// <param name="data">入力、移動、当たり判定、演出を含むノード設定。</param>
		void AddComboNode(const std::string& nodeName, const std::string& animationName, const ComboData& data);
		/// <summary>入力条件に応じたコンボノード間の遷移を登録する。</summary>
		/// <param name="from">遷移元ノード名。</param>
		/// <param name="input">遷移に必要なアクション入力。</param>
		/// <param name="to">遷移先ノード名。</param>
		/// <param name="condition">地上・空中などの追加遷移条件。</param>
		void ConnectCombo(const std::string& from, ActionInput input, const std::string& to,
			TransitionCondition condition = TransitionCondition::Default);
		// コンボ名を参照している接続と開始ルートを更新
		void RenameComboReferences(const std::string& oldName, const std::string& newName);
		/// <summary>指定ノードを先頭としてコンボを開始する。</summary>
		/// <param name="name">開始するノード名。</param>
		/// <returns>ノードが存在し、開始できた場合はtrue。</returns>
		bool StartCombo(const std::string& name);
		// コンボが終了したか
		bool IsComboFinished() const {
			return comboStateMachine_->IsComboFinished();
		}
		void SetIsDebug(bool is) { isDebug = is; }
	public:
		// データ設定
		void SetData(ComboData& data, const GlobalData& gData);
		// コンボエディタの音一覧とプレビューで使用する音声管理を取得する。
		Engine::AudioManager* GetAudioManager() { return audioManager_; }

		// グローバルデータ取得
		GlobalData& GetComboGlobalData(const std::string& comboNodeName) {
			auto it = comboGlobalDatas_.find(comboNodeName);
			assert(it != comboGlobalDatas_.end());
			return it->second;
		}
	public:
		// コンボ条件データ
		struct ComboConditionData {
			Combo::EndConditionType type = Combo::EndConditionType::kOnTimer;
		};

		// コンボ作成
		void CreateCombo(const std::string& comboNodeName);
		// 指定データを元にコンボ作成
		void CreateCombo(const std::string& comboNodeName, const GlobalData& sourceData);
	private:
		// グローバルデータ作成
		void CreateGlobalData(const std::string& comboNodeName);
		// 登録と保存で共通のコンボ項目一覧を書き込む
		void WriteGlobalComboData(const std::string& groupName, GlobalData& data, bool overwrite);
		void ConnectSavedCombos();
		std::string ResolveStartCombo(ActionInput input, bool isLanding, bool isDodging, bool isDodgeSuccess) const;
		bool IsStartComboKey(const std::string& key) const;
		float GetStaminaCost(ActionInput input) const;
		float GetComboStaminaCost(ActionInput input, const std::shared_ptr<NodeState>& node) const;
		bool CanUseComboNode(const std::shared_ptr<NodeState>& node) const;
		bool CanPayStamina(float cost) const;
		void PayStamina(float cost);
		void UpdateCooldowns(float dt);
		void StartCooldown(const std::shared_ptr<NodeState>& node);


	private:
		// コンボステートマシーン
		std::unique_ptr<StateMachine> comboStateMachine_ = nullptr;
		//
		std::unique_ptr<ComboDebug> comboDebug_ = nullptr;
		// コンボノードステートマップ
		std::map<std::string, std::shared_ptr<NodeState>> comboNodes_;
		// コンボノード名マップ(保存用)
		std::map<std::string, std::string> comboNodenames_;
		//
		std::string name = "";

		// 保存データマップ
		std::map<std::string, GlobalData> comboGlobalDatas_;
		// 親ワールド変換マップ
		std::map<std::string, Engine::WorldTransform*> parentTransforms_;
		// Frameworkが所有する音声管理への非所有ポインター。
		Engine::AudioManager* audioManager_ = nullptr;

		bool isDebug = false;
		std::optional<ActionInput> pendingCostInput_;
		float pendingStaminaCost_ = 0.0f;
		std::shared_ptr<NodeState> pendingCooldownNode_ = nullptr;
		std::map<std::string, float> cooldownTimers_;
		std::string groundLightStart_ = "MeleeAttack1";
		std::string airLightStart_ = "JumpAttack";
		std::string groundHeavyStart_ = "Attack10";
		std::string airHeavyStart_ = "Attack10";
		std::string groundSkillStart_ = "SkillAttack01";
		std::string airSkillStart_ = "JumpSkillAttack01";
		std::string dodgeLightStart_ = "MeleeAttack1";
		std::string dodgeHeavyStart_ = "Attack10";
		std::string dodgeSkillStart_ = "SkillAttack01";
		std::string dodgeSuccessLightStart_ = "MeleeAttack1";
		std::string dodgeSuccessHeavyStart_ = "Attack10";
		std::string dodgeSuccessSkillStart_ = "SkillAttack01";

	private:
		Engine::GlobalVariables* globalVariables = nullptr;
		Character::BaseCharacter* owner = nullptr;
	};
};

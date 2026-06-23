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

		// 初期化
		void Initialize(Character::BaseCharacter* character, Engine::LineCommon* lineCommon, Engine::GlobalVariables* globalVariables);

		// コンボ更新
		void Update(const Character::CharacterContext& ctx);
		// プレイヤーからの攻撃入力要求
		bool RequestAttack(ActionInput input);
		// 現在のコンボ攻撃が命中したことを通知
		void NotifyAttackHit();
		// クリア
		void ClearNode();
		// 名前設定
		void Create(const std::string name);
	public: // 保存や適応に関しての関数

		// 保存項目の追加
		void ApplyGlobalComboData(const std::string& name, Combo::GlobalData& data);

		// 保存項目の適応
		void GetGlobalComboData(const std::string& name, Combo::GlobalData& data);

		// 保存項目に設定
		void SetGlobalComboData(const std::string& name, Combo::GlobalData& data);

		// 全保存項目の設定
		void SetGlobalComboDatas();
	public:
		// 親ワールド変換取得してコンテナに追加
		void SetParentTransform(const std::string& name, Engine::WorldTransform* transform) {
			parentTransforms_[name] = transform;
		}

		Engine::WorldTransform* GetParentTransform(const std::string& name) {
			auto it = parentTransforms_.find(name);
			if (it != parentTransforms_.end()) {
				return it->second;
			}
			return nullptr;
		}

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
		// コンボステートマシーン取得
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
		// コンボノードステート取得
		std::shared_ptr<NodeState> GetComboNodeState(const std::string& name) {
			auto it = comboNodes_.find(name);
			if (it != comboNodes_.end()) {
				return it->second;
			}
			return nullptr;
		}
	public:
		// ノード追加
		void AddComboNode(const std::string& name, std::shared_ptr<NodeState> node);
		// ノード追加(データから生成)
		void AddComboNode(const std::string& nodeName, const std::string& animationName, const ComboData& data);
		// コンボ接続
		void ConnectCombo(const std::string& from, ActionInput input, const std::string& to,
			TransitionCondition condition = TransitionCondition::Default);
		// コンボ名を参照している接続と開始ルートを更新
		void RenameComboReferences(const std::string& oldName, const std::string& newName);
		// 最初のコンボ
		bool StartCombo(const std::string& name);
		// コンボが終了したか
		bool IsComboFinished() const {
			return comboStateMachine_->IsComboFinished();
		}
		void SertIsDebug(bool is) { isDebug = is; }
	public:
		// データ設定
		void SetData(ComboData& data, const GlobalData& gData);

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

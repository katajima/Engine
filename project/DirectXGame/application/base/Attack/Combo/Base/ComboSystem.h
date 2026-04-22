#pragma once
#include <map>
#include <string>
#include <memory>
#include "ComboDebug.h"
#include "DirectXGame/application/base/Character/Base/CharacterContext.h"

namespace Combo {

	const std::string kHitEffectCountKey = "ヒットエフェクト数";

	static std::string MakeHitEffectSlotKey(int index) {
		return "ヒットエフェクトスロット" + std::to_string(index);
	}

	static std::string MakeHitEffectNameKey(int index) {
		return "ヒットエフェクト名前" + std::to_string(index);
	}


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
		// クリア
		void ClearNode();

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
		void ConnectCombo(const std::string& from, ActionInput input, const std::string& to);
		// 最初のコンボ
		void StartCombo(const std::string& name);
		// インプット
		void InputCombo(ActionInput input) {
			comboStateMachine_->HandleInput(input);
		}
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
	private:
		// グローバルデータ作成
		void CreateGlobalData(const std::string& comboNodeName);

	private:
		// コンボステートマシーン
		std::unique_ptr<StateMachine> comboStateMachine_ = nullptr;
		//
		std::unique_ptr<ComboDebug> comboDebug_ = nullptr;
		// コンボノードステートマップ
		std::map<std::string, std::shared_ptr<NodeState>> comboNodes_;
		// 保存データマップ 
		std::map<std::string, GlobalData> comboGlobalDatas_;
		// 親ワールド変換マップ
		std::map<std::string, Engine::WorldTransform*> parentTransforms_;

		bool isDebug = false;

	private:
		Engine::GlobalVariables* globalVariables = nullptr;
		Character::BaseCharacter* owner = nullptr;
	};
};

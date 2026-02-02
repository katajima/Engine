#pragma once
#include <map>
#include <string>
#include <memory>

#include "ComboState.h"
#include "DirectXGame/application/base/Attack/Combo/Base/ComboData.h"

class BaseCharacter; // 前方宣言
namespace Engine {
	class GlobalVariables;
}



/// <summary>
/// コンボシステム
/// </summary>
class ComboSystem
{
public:
	// 初期化
	void Initialize(BaseCharacter* character, Engine::GlobalVariables* globalVariables);

	// コンボ更新
	void UpdateCombo(float dt) {
		comboStateMachine_->Update(dt);
	}

	// クリア
	void ClearNode();

public: // 保存や適応に関しての関数

	// 保存項目の追加
	void ApplyGlobalComboData(const std::string& name, ComboGlovalData& data);

	// 保存項目の適応
	void GetGlobalComboData(const std::string& name, ComboGlovalData& data);

	// 保存項目に設定
	void SetGlobalComboData(const std::string& name, ComboGlovalData& data);

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
		return it->first;
	}

	//	トランスフォーム取得
	std::map<std::string, Engine::WorldTransform*> GetParentTransforms() { return parentTransforms_; };


public: 
	// コンボステートマシーン取得
	ComboStateMachine* GetComboStateMachine() { return comboStateMachine_.get(); }

	// コンボノードステートマップ取得
	std::map<std::string, std::shared_ptr<ComboNodeState>>  GetComboNodeStates() { return comboNodes_; };

	// コンボノードステート取得
	std::shared_ptr<ComboNodeState> GetComboNodeState(const std::string& name) {
		auto it = comboNodes_.find(name);
		if (it != comboNodes_.end()) {
			return it->second;
		}
		return nullptr;
	}
public:
	// ノード追加
	void AddComboNode(const std::string& name, std::shared_ptr<ComboNodeState> node);
	// ノード追加(データから生成)
	void AddComboNode(const std::string& nodeName, const std::string animationName ,const ComboData& data);
	// コンボ接続
	void ConnectCombo(const std::string& from, AttackInput input, const std::string& to);
	// 最初のコンボ
	void StartCombo(const std::string& name);
	// インプット
	void InputCombo(AttackInput input) {
		comboStateMachine_->HandleInput(input);
	}
	// コンボが終了したか
	bool IsComboFinished() const {
		return comboStateMachine_->IsComboFinished();
	}

public:
	// データ設定
	void SetData(ComboData& data, const ComboGlovalData& gData);

	// グローバルデータ取得
	ComboGlovalData& GetComboGlobalData(const std::string& comboNodeName) {
		auto it = comboGlobalDatas_.find(comboNodeName);
		assert(it != comboGlobalDatas_.end());
		return it->second;
	}
public:
	// 追加ヒットボックスデータ
	struct AddHitBoxData {
		HitBoxCollData hitBoxData;
	};

	// コンボ条件データ
	struct ComboConditionData {
		EndConditionType type = EndConditionType::kOnTimer;
	};

	// コンボ作成
	void CreateCombo(const std::string comboNodeName, const std::vector<AddHitBoxData> addHitBoxData,
		GamePadButton button = GamePadButton::GAMEPAD_B);
private:
	// グローバルデータ作成
	void CreateGlobalData(const std::string comboNodeName);

private:
	// コンボステートマシーン
	std::unique_ptr<ComboStateMachine> comboStateMachine_;
	// コンボノードステートマップ
	std::map<std::string, std::shared_ptr<ComboNodeState>> comboNodes_;
	// 保存データマップ 
	std::map<std::string, ComboGlovalData> comboGlobalDatas_;
	// 親ワールド変換マップ
	std::map<std::string, Engine::WorldTransform*> parentTransforms_;
private:
	Engine::GlobalVariables* globalVariables = nullptr;

};

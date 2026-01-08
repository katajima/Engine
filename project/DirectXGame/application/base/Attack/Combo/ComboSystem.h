#pragma once
#include <map>
#include <string>
#include <memory>

#include "ComboState.h"
#include "ComboGlobalData.h"

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
	void SetGlobalComboData(const std::string& name, ComboGlovalData& data);


public: 
	// コンボステートマシーン取得
	ComboStateMachine* GetComboStateMachine() { return comboStateMachine_.get(); }
public:
	// ノード追加
	void AddComboNode(const std::string& name, std::shared_ptr<ComboNodeState> node);
	// ノード追加(データから生成)
	void AddComboNode(const std::string& name, const ComboData& data);
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
private:
	// コンボステートマシーン
	std::unique_ptr<ComboStateMachine> comboStateMachine_;
	// コンボノードステートマップ
	std::map<std::string, std::shared_ptr<ComboNodeState>> comboNodes_;
private:
	Engine::GlobalVariables* globalVariables = nullptr;

};

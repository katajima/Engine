#pragma once
#include <map>
#include <string>
#include <memory>

#include "ComboState.h"


class BaseCharacter; // 前方宣言

/// <summary>
/// コンボシステム
/// </summary>
class ComboSystem
{
public:
	// 初期化
	void Initialize(BaseCharacter* character);

	// コンボ更新
	void UpdateCombo(float dt) {
		comboStateMachine_->Update(dt);
	}


	void ClearNode();

public: 
	// コンボデータ取得
	ComboData* GetComboData() { return comboData_; }
	// コンボデータ設定
	void SetComboData(ComboData* data) { comboData_ = data; }
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
private:
	// コンボデータ
	ComboData* comboData_ = nullptr;
	// コンボステートマシーン
	std::unique_ptr<ComboStateMachine> comboStateMachine_;
	// コンボノードステートマップ
	std::map<std::string, std::shared_ptr<ComboNodeState>> comboNodes_;
};

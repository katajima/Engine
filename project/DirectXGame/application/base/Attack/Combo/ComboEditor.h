#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboSequencer.h"
#include "ComboSystem.h"

// コンボ数に応じてシーケンサーを増やす
// 
//
// 

namespace Engine {
	class GlobalVariables;
}
// 前方宣言
class BaseCharacter;

class ComboEditorBlock {
public:
	// 初期化
	void Initialize(Engine::GlobalVariables* globalVariables, ComboSystem* comboSystem, std::shared_ptr<ComboNodeState> state,BaseCharacter* owner);
	// 更新
	void UpdateImGui(float dt);
public:
	// 開始
	void Enter();
	// 終了
	void Exit();

public:
	// エディタ上で選択中か取得
	void SetNowChoice(bool nowChoice) { nowChoice_ = nowChoice; }
private:
	// 最初のフレーム設定
	void ImGuiFirstFrame();
	// 現在のフレーム設定
	void ImGuiCurrentFrame(float dt);
	// シーケンサー設定
	void ImGuiSequenceSettings();
	// シーケンサー追加
	void AddSequencerEvent(float startFrame,float endFrame, unsigned int color ,const std::string& name);
	// シーケンサー表示
	void SequencerProgress();
private:
	// アニメーションをステートに適応
	void AnimationApplyToState();
	// 
	void ApplyComboDataToState();

private:
	// シーケンサーの状態をステートに適応
	void SequencerApplyToState();

private: // 貰いもの
	Engine::GlobalVariables* globalVariables = nullptr;

private: // もらいもの
	// ノードステート
	std::shared_ptr<ComboNodeState> state = nullptr;
	BaseCharacter* owner = nullptr;
	ComboSystem* comboSystem = nullptr;
private:

	// シーケンサー
	AttackSequence sequence_;
private:
	// 現在のフレーム
	int currentFrame = 0;

	// 再生中か
	bool isPlaying = false;
	// ループ再生するか
	bool loopPlay = false;

	// 最初のフレーム
	int firstFrame = 0;
	// マックスフレーム(二十秒)
	int maxFrame = 300;


	int selected = -1;
	bool expanded = true;
	
	// コンボ名
	std::string comboName_ = "Combo1";

	// 今選択中か
	bool nowChoice_ = false;
};



// コンボエディター
class ComboEditor {
public:

	// 初期化
	void Initialize(ComboSystem* comboSystem, Engine::GlobalVariables* globalVariables, BaseCharacter* owner);
	// 更新
	void UpdateImGui(float dt);

	// コンボエディターブロックをコンボシステムに適応
	void ApplyComboEditorToSystem();

private:

	// コンボエディターブロック作成
	void CreateComboEditorBlock(const std::string& comboName, ComboSystem* comboSystem, std::shared_ptr<ComboNodeState> state, BaseCharacter* owner);


private: // もらいもの
	// コンボシステム
	ComboSystem* comboSystem = nullptr;
	Engine::GlobalVariables* globalVariables = nullptr;
	BaseCharacter* owner = nullptr;

private:
	// コンボエディターブロックマップ
	std::map<std::string, ComboEditorBlock> comboEditorBlocks_;

	// 選択中のコンボエディターブロック名
	std::string selectedComboEditorBlockName_;
};
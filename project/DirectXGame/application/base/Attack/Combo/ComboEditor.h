#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboSequencer.h"
#include "ComboSystem.h"
#include "DirectXGame/engine/Utility/ConvertUtility.h"

// コンボ数に応じてシーケンサーを増やす
// 



namespace Engine {
	class GlobalVariables;
}
// 前方宣言
class BaseCharacter;
class BasePlayer;

class ComboEditorBlock {
public:
	struct Data {
		///  アニメーション設定  ///

		// アニメーション名
		std::string animationName_ = "no";
		// アニメーションスピード
		float animationSpeed_ = 1.0f;
		// アニメーションのブレンド時間
		float animationBlendTime_ = 0.1f;

		/// 移動設定  ///

		// 移動速度
		float moveSpeed_ = 0.0f;
		// 強制移動
		bool isCompulsionMove_ = false;

		/// 重力設定  ///

		// 重力はあるか？
		bool isGravity = true;			
		// 重力スケール
		float gravityScale = 1.0f;			


		/// ノックバック設定  ///

		// ノックバック持続時間
		float knockbackDuration_ = 0.1f;	
		// ノックバックパワー
		float knockbackPower = 30.0f;		
		// ノックバックY方向パワー
		float knockbackPowerY = 30.0f;		
		// Y方向にノックバックするか
		bool isVerticalBoost_ = false;		
		// ダメージ
		float damage = 0;					


		/// 親子付け設定  ///　 
		std::string parentName_ = "なし";
		// オフセット
		Vector3 parentOffset_ = { 0.0f,0.0f,0.0f };
		// ヒットボックスの発生条件タイプ
		HitBoxSpawnType spawnType_ = HitBoxSpawnType::kOnTime;
		// ヒットボックス依存先タイプ
		HitBoxParentType dependenceType_ = HitBoxParentType::kParent;


		///	終了条件 ///
		EndConditionType endConditionType = EndConditionType::kOnTimer;

	};

	// 初期化
	void Initialize(Engine::GlobalVariables* globalVariables, ComboSystem* comboSystem, std::shared_ptr<ComboNodeState> state,BaseCharacter* owner);
	// 更新
	void UpdateImGui(float dt);
public:
	// エディタ上で選択中か取得
	void SetNowChoice(bool nowChoice) { nowChoice_ = nowChoice; }

	// 接続可能なコンボ名リスト設定
	void SetConectComboNames(const std::vector<std::string>& names) { conectComboNames_ = names; }

	
	// シーケンサー取得
	AttackSequence GetAttackSequence() const { return sequence_; }

	// 最大フレーム取得
	float GetMaxFrame() const { return ConvertUtility::FramesToSeconds(maxFrame); }

	// データ取得
	Data GetData() const { return data_; }
private:
	// アニメーション設定
	void ImGuiApplyAnimationToState();
	// ヒットボックス設定
	void ImGuiApplyHitBox();
	// 終了条件
	void ImGuiEndConditionType();
	// 移動関係設定
	void ImGuiMove();
	// リアクション設定
	void ImGuiReaction();

	// 親子付け設定
	void ImGuiApplyParentToState();

	// 最初のフレーム設定
	void ImGuiFirstFrame();
	// 現在のフレーム設定
	void ImGuiCurrentFrame(float dt);
	// HitBoxSpawnType
	void ImGuiHitBoxSpawnType();
	// HitBoxParentType
	void ImGuiHitBoxParentType();

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
	// 所有者
	BaseCharacter* owner = nullptr;
	// コンボシステム
	ComboSystem* comboSystem = nullptr;
	// 接続可能なコンボ名リスト
	std::vector<std::string> conectComboNames_;
private:
	// ノードステート
	std::shared_ptr<ComboNodeState> state = nullptr;
	// シーケンサー
	AttackSequence sequence_;
private:
	// 現在のフレーム
	int currentFrame = 0;
	// 再生中か
	bool isPlaying = true;
	// ループ再生するか
	bool loopPlay = true;
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
private:
	// データ
	Data data_;
};



// コンボエディター
class ComboEditor {
public:

	// 初期化
	void Initialize(ComboSystem* comboSystem, Engine::GlobalVariables* globalVariables, BaseCharacter* owner);
	// 更新 
	void Update(float dt);
private:
	// 更新
	void UpdateImGui(float dt);
	// コンボエディターブロックをコンボシステムに適応
	void ApplyComboEditorToSystem();
	// グローバルデータ設定
	void SetGlobalData();
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
	// コンボエディターブロック名リスト
	std::vector<std::string> comboEditorBlockNames_;


	// 選択中のコンボエディターブロック名
	std::string selectedComboEditorBlockName_;

	ComboGlovalData data;

	// コンボエディターがアクティブか
	bool isComboEditorActive_ = true;
};
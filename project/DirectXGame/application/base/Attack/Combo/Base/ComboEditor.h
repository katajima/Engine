#pragma once
#include  "ComboImGui.h"


namespace Engine {
	class GlobalVariables;
	class LineCommon;
}
// 前方宣言
namespace Character {
	class BaseCharacter;
	class BasePlayer;
}


namespace Combo {
	class EditorBlock {
	public:
		// 初期化
		void Initialize(Engine::LineCommon* lineCommon,Engine::GlobalVariables* globalVariables, Combo::System* comboSystem, const std::string& stateName, Character::BaseCharacter* owner);
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
		GlobalData GetData() const { return data_; }
	private:
		// ヒットボックス設定
		void ImGuiApplyHitBox();
		// 終了条件
		void ImGuiEndConditionType();
		// 移動関係設定
		void ImGuiMove();
		// ロックオン関係
		void ImGuiLockOn();
		// リアクション設定
		void ImGuiReaction();
	private:
		// シーケンサーの状態をステートに適応
		void SequencerApplyToState();

	private: // 貰いもの
		Engine::GlobalVariables* globalVariables = nullptr;
		Engine::LineCommon* lineCommon = nullptr;
	private: // もらいもの
		// 所有者
		Character::BaseCharacter* owner = nullptr;
		// コンボシステム
		Combo::System* comboSystem = nullptr;
		// 接続可能なコンボ名リスト
		std::vector<std::string> conectComboNames_;
	private:
		// ノードステート
		std::string stateName = "";
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
		GlobalData data_;
	};



	// コンボエディター
	class Editor {
	public:

		// 初期化
		void Initialize(Engine::LineCommon* lineCommon,Combo::System* comboSystem, Engine::GlobalVariables* globalVariables, Character::BaseCharacter* owner);
		// 更新 
		void Update(float dt);

		// コンボエディターがアクティブか取得
		bool IsActive() const { return isComboEditorActive_; }

	private:
		// 更新
		void UpdateImGui(float dt);
		// コンボエディターブロックをコンボシステムに適応
		void ApplyComboEditorToSystem();
		// グローバルデータ設定
		void SetGlobalData();
	private:
		// コンボエディターブロック作成
		void CreateComboEditorBlock(const std::string& comboName, Combo::System* comboSystem, const std::string& stateName, Character::BaseCharacter* owner);
	private: // もらいもの
		// コンボシステム
		Combo::System* comboSystem = nullptr;
		Engine::GlobalVariables* globalVariables = nullptr;
		Engine::LineCommon* lineCommon = nullptr;
		Character::BaseCharacter* owner = nullptr;

	private:
		// コンボエディターブロックマップ
		std::map<std::string, EditorBlock> comboEditorBlocks_;
		// コンボエディターブロック名リスト
		std::vector<std::string> comboEditorBlockNames_;


		// 選択中のコンボエディターブロック名
		std::string selectedComboEditorBlockName_;

		GlobalData data;

		// コンボエディターがアクティブか
		bool isComboEditorActive_ = true;
	};
}
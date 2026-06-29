#pragma once
#include "ComboEditorBlock.h"
#include <set>

namespace Combo {
	// コンボエディター
	class Editor {
	public:

		// 初期化
		void Initialize(Engine::LineCommon* lineCommon, Engine::GlobalVariables* globalVariables, 
			Character::CharacterManager* characterManager,Character::BaseCharacter* owner, EffectSystem* effectSystem);
		// 更新 
		void Update(float dt);

		// コンボエディターがアクティブか取得
		bool IsActive() const { return isComboEditorActive_; }

		// 所有者設定
		void SetOwner(Character::BaseCharacter* owner);

	private:
		// 更新
		void UpdateImGui(float dt);
		
		// コンボエディターブロックをコンボシステムに適応
		void ApplyComboEditorToSystem();
		
		// グローバルデータ設定
		void SetGlobalData();
		
		// コンボノードの管理
		void DrawNodeManagement();
		
		// コンボ開始設定のデバッグ描画
		void DrawStartComboSettings();
		
		// コンボ全体の流れをノードとして表示、接続を編集
		void DrawComboNodeGraph();
		
		// コンボ追加
		void AddComboNode();

		// コンボ追加時にコピーするか
		void AddCopiedComboNode();

		// 新しいコンボ名のバリデーション
		bool ValidateNewComboName(const std::string& comboName);

		// コンボ名前変更
		void RenameComboNode(const std::string& oldName, const std::string& newName);

		// コンボ削除
		void DeleteComboNode(const std::string& comboName);

		// セーブ
		void DrawSaveComboNode();
	private:
		// コンボエディターブロック作成
		void CreateComboEditorBlock(const std::string& comboName, Combo::System* comboSystem, const std::string& stateName, Character::BaseCharacter* owner);
	private: // もらいもの
		// コンボシステム
		Combo::System* comboSystem = nullptr;
		// 保存項目
		Engine::GlobalVariables* globalVariables = nullptr;
		// ライン描画共通
		Engine::LineCommon* lineCommon = nullptr;
		// 所有者
		Character::BaseCharacter* owner = nullptr;
		// キャラクター管理
		Character::CharacterManager* characterManager = nullptr;
		// エフェクト管理
		EffectSystem* effectSystem = nullptr;
	private:
		// コンボエディターブロックマップ
		std::map<std::string, EditorBlock> comboEditorBlocks_;
		// コンボエディターブロック名リスト
		std::vector<std::string> comboEditorBlockNames_;


		// 選択中のコンボエディターブロック名
		std::string selectedComboEditorBlockName_;
		std::array<char, 128> newComboNameBuffer_{};
		std::array<char, 128> renameComboNameBuffer_{};
		std::string copySourceComboName_;
		bool copyConnections_ = false;
		std::string pendingRenameComboName_;
		std::string pendingDeleteComboName_;
		std::string nodeManagementMessage_;
		bool showAllNodeLinks_ = false;			// ノードグラフで全接続線を表示するか
		std::set<int> initializedNodeGraphNodeIds_;	// imnodes上で初期配置済みのノードID

		GlobalData data;

		// コンボエディターがアクティブか
		bool isComboEditorActive_ = true;
	};
}

#pragma once
#include "ComboEditorBlock.h"

namespace Combo {
	// コンボエディター
	class Editor {
	public:

		// 初期化
		void Initialize(Engine::LineCommon* lineCommon,Combo::System* comboSystem,
			Engine::GlobalVariables* globalVariables, Character::BaseCharacter* owner,
			EffectSystem* effectSystem);
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
		// コンボノードの管理
		void DrawNodeManagement();
		void AddComboNode();
		void RenameComboNode(const std::string& oldName, const std::string& newName);
		void DeleteComboNode(const std::string& comboName);
	private:
		// コンボエディターブロック作成
		void CreateComboEditorBlock(const std::string& comboName, Combo::System* comboSystem, const std::string& stateName, Character::BaseCharacter* owner);
	private: // もらいもの
		// コンボシステム
		Combo::System* comboSystem = nullptr;
		Engine::GlobalVariables* globalVariables = nullptr;
		Engine::LineCommon* lineCommon = nullptr;
		Character::BaseCharacter* owner = nullptr;
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
		std::string pendingRenameComboName_;
		std::string pendingDeleteComboName_;
		std::string nodeManagementMessage_;

		GlobalData data;

		// コンボエディターがアクティブか
		bool isComboEditorActive_ = true;
	};
}

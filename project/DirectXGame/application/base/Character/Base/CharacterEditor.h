#pragma once
#include "CharacterGlobalData.h"
#include "map"

namespace Engine {
	class GlobalVariables;
}

namespace Character {



	class EditorBlock {
	public:



	private:


	};



	/// <summary>
	/// キャラクター用エディター
	/// </summary>
	class Editor {
	public:
		// 初期化
		void Initialize(Engine::GlobalVariables* globalVariables);
		// 更新 
		void Update(float dt);
	private:
		// 更新
		void UpdateImGui(float dt);

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
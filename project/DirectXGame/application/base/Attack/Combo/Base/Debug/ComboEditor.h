#pragma once
#include "ComboEditorBlock.h"
#include <set>

namespace Combo {
	/// <summary>
	/// コンボデータの編集、ノード管理、デバッグ表示を行うエディター。
	/// </summary>
	class Editor {
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="lineCommon">ライン描画共通</param>
		/// <param name="globalVariables">外部保存用クラス</param>
		/// <param name="characterManager">キャラクター管理</param>
		/// <param name="owner">所有者</param>
		/// <param name="effectSystem">エフェクトシステム</param>
		void Initialize(Engine::LineCommon* lineCommon, Engine::GlobalVariables* globalVariables, 
			Character::CharacterManager* characterManager,Character::BaseCharacter* owner, EffectSystem* effectSystem);
		/// <summary>
		/// 更新
		/// </summary>
		void Update(float dt);

		/// <summary>
		/// コンボエディターがアクティブか取得
		/// </summary>
		bool IsActive() const { return isComboEditorActive_; }
		/// <summary>
		/// コンボエディターがマウスやキーボード入力を使用中か取得
		/// </summary>
		bool IsInputCaptured() const { return isInputCaptured_; }

		/// <summary>
		/// 所有者設定
		/// </summary>
		void SetOwner(Character::BaseCharacter* owner);
	/// 選択中コンボの軌道を現在のキャラクター位置へ追従させて描画する。
	void DrawSelectedTrajectoryDebug();

	private:
		/// <summary>
		/// 更新
		/// </summary>
		void UpdateImGui(float dt);
		
		/// <summary>
		/// コンボエディターブロックをコンボシステムに適応
		/// </summary>
		void ApplyComboEditorToSystem();
		
		/// <summary>
		/// グローバルデータ設定
		/// </summary>
		void SetGlobalData();
		
		/// <summary>
		/// コンボノードの管理
		/// </summary>
		void DrawNodeManagement();
		
		/// <summary>
		/// コンボ開始設定のデバッグ描画
		/// </summary>
		void DrawStartComboSettings();
		
		/// <summary>
		/// コンボ全体の流れをノードとして表示、接続を編集
		/// </summary>
		void DrawComboNodeGraph();
		
		/// <summary>
		/// コンボ追加
		/// </summary>
		void AddComboNode();

		/// <summary>
		/// コンボ追加時にコピーするか
		/// </summary>
		void AddCopiedComboNode();

		/// <summary>
		/// 新しいコンボ名のバリデーション
		/// </summary>
		bool ValidateNewComboName(const std::string& comboName);

		/// <summary>
		/// コンボ名前変更
		/// </summary>
		void RenameComboNode(const std::string& oldName, const std::string& newName);

		/// <summary>
		/// コンボ削除
		/// </summary>
		void DeleteComboNode(const std::string& comboName);

		/// <summary>
		/// セーブ
		/// </summary>
		void DrawSaveComboNode();
	private:
		/// <summary>
		/// コンボエディターブロック作成
		/// </summary>
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
		// Combo欄からコンボ関連のデバッグ描画を一括で切り替える。
		bool showComboDebugDraw_ = true;
		bool showAllNodeLinks_ = false;			// ノードグラフで全接続線を表示するか
		std::set<int> initializedNodeGraphNodeIds_;	// imnodes上で初期配置済みのノードID

		GlobalData data;

		// コンボエディターがアクティブか
		bool isComboEditorActive_ = true;
		// エディター操作中にプレイヤー入力を止めるための入力取得状態
		bool isInputCaptured_ = false;
	};
}

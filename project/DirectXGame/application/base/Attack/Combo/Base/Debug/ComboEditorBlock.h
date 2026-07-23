#pragma once
#include  "ComboImGui.h"
#include <array>
#include "map"
#include "DirectXGame/engine/Effect/EffectGlovalData.h"

namespace Engine {
	class GlobalVariables;
	class LineCommon;
}
// 前方宣言
namespace Character {
	class CharacterManager;	// キャラクターマネージャー
	class BaseCharacter;	// キャラクター基底クラス
	class BasePlayer;		// プレイヤー
}
class EffectSystem;

namespace Combo {
	/// <summary>
	/// 1つのコンボノードの設定と編集用UIを管理するエディターブロック。
	/// </summary>
	class EditorBlock {
	public:
		// 初期化
		void Initialize(Engine::LineCommon* lineCommon, Engine::GlobalVariables* globalVariables,
			Combo::System* comboSystem, const std::string& stateName,
			Character::BaseCharacter* owner, EffectSystem* effectSystem);
		/// <summary>
		/// 更新
		/// </summary>
		void UpdateImGui(float dt, bool isActive);
	public:
		/// <summary>
		/// エディタ上で選択中か設定
		/// </summary>
		void SetNowChoice(bool nowChoice) { nowChoice_ = nowChoice; }
		/// <summary>
		/// コエディタ上で選択中か取得
		/// </summary>
		bool GetNowChoice() const { return nowChoice_; }

		/// <summary>
		/// 接続可能なコンボ名リスト設定
		/// </summary>
		void SetConectComboNames(const std::vector<std::string>& names) { conectComboNames_ = names; }


		/// <summary>
		/// シーケンサー取得
		/// </summary>
		AttackSequence GetAttackSequence() const { return sequence_; }

		/// <summary>
		/// 最大フレーム取得
		/// </summary>
		float GetMaxFrame() const { return ConvertUtility::FramesToSeconds(maxFrame); }

		/// <summary>
		/// データ取得
		/// </summary>
		GlobalData GetData() const { return data_; }
		/// <summary>
		/// エディタ上で直接編集するためのデータ参照を取得
		/// </summary>
		GlobalData& GetMutableData() { return data_; }
	private:
		/// <summary>
		/// ヒットボックス設定
		/// </summary>
		void ImGuiApplyHitBox();
		/// <summary>
		/// 終了条件
		/// </summary>
		void ImGuiEndConditionType();
		/// <summary>
		/// 攻撃種別と遠距離設定
		/// </summary>
		void ImGuiAttackType();
		/// <summary>
		/// 遠距離攻撃設定
		/// </summary>
		void ImGuiRange();
		/// <summary>
		/// 演出設定
		/// </summary>
		void ImGuiEffect();
		/// <summary>
		/// 攻撃、命中、終了時の音声設定
		/// </summary>
		void ImGuiAudio();
		/// <summary>
		/// 移動関係設定
		/// </summary>
		void ImGuiMove();
		/// <summary>
		/// リアクション設定
		/// </summary>
		void ImGuiReaction();
		/// <summary>
		/// カメラ設定
		/// </summary>
		void ImGuiCamera();
		/// <summary>
		/// コンボ接続設定
		/// </summary>
		void ImGuiConnection();
	private:
		/// <summary>
		/// シーケンサーの状態をステートに適応
		/// </summary>
		void SequencerApplyToState();

		void DrawHitEffectEditor(HitReactionData& reaction,
			const std::map<std::string, EffectGlobalData>& effectDatas);
		void DrawComboEffectEditor(GloblEffectData& effect,
			const std::map<std::string, EffectGlobalData>& effectDatas);
		/// <summary>
		/// コンボエフェクトの設定をシーケンサー表示へ反映する
		/// </summary>
		void SyncComboEffectSequenceEvents();
		/// <summary>
		/// Soundディレクトリの一覧から音源を選択し、試聴できるUIを描画する。
		/// </summary>
		void DrawSoundSelector(const char* label, std::string& soundId, float volume);

	private: // 貰いもの
		Engine::GlobalVariables* globalVariables = nullptr;
		Engine::LineCommon* lineCommon = nullptr;
		EffectSystem* effectSystem = nullptr;
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
		// コンボエディターがアクティブか
		bool isComboEditorActive_ = true;
	private:
		// データ
		GlobalData data_;
	};
}

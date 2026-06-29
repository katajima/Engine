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
	class EditorBlock {
	public:
		// 初期化
		void Initialize(Engine::LineCommon* lineCommon, Engine::GlobalVariables* globalVariables,
			Combo::System* comboSystem, const std::string& stateName,
			Character::BaseCharacter* owner, EffectSystem* effectSystem);
		// 更新
		void UpdateImGui(float dt, bool isActive);
	public:
		// エディタ上で選択中か設定
		void SetNowChoice(bool nowChoice) { nowChoice_ = nowChoice; }
		// コエディタ上で選択中か取得
		bool GetNowChoice() const { return nowChoice_; }

		// 接続可能なコンボ名リスト設定
		void SetConectComboNames(const std::vector<std::string>& names) { conectComboNames_ = names; }


		// シーケンサー取得
		AttackSequence GetAttackSequence() const { return sequence_; }

		// 最大フレーム取得
		float GetMaxFrame() const { return ConvertUtility::FramesToSeconds(maxFrame); }

		// データ取得
		GlobalData GetData() const { return data_; }
		// エディタ上で直接編集するためのデータ参照を取得
		GlobalData& GetMutableData() { return data_; }
	private:
		// ヒットボックス設定
		void ImGuiApplyHitBox();
		// 終了条件
		void ImGuiEndConditionType();
		// 攻撃種別と遠距離設定
		void ImGuiAttackType();
		// 遠距離攻撃設定
		void ImGuiRange();
		// 演出設定
		void ImGuiEffect();
		// 移動関係設定
		void ImGuiMove();
		// リアクション設定
		void ImGuiReaction();
		// カメラ設定
		void ImGuiCamera();
		// コンボ接続設定
		void ImGuiConnection();
	private:
		// シーケンサーの状態をステートに適応
		void SequencerApplyToState();

		void DrawHitEffectEditor(HitReactionData& reaction,
			const std::map<std::string, EffectGlobalData>& effectDatas);
		void DrawComboEffectEditor(GloblEffectData& effect,
			const std::map<std::string, EffectGlobalData>& effectDatas);
		// コンボエフェクトの設定をシーケンサー表示へ反映する
		void SyncComboEffectSequenceEvents();

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

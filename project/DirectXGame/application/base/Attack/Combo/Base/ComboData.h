#pragma once
#include "DirectXGame/application/base/Attack/Combo/Receiver/ComboCondition.h"
#include "DirectXGame/application/base/Attack/Combo/Motion/ComboMotion.h"
#include "DirectXGame/application/base/Attack/Combo/HitBox/ComboHitBox.h"
#include "DirectXGame/application/base/Attack/Combo/Effect/ComboEffect.h"
#include "DirectXGame/application/base/Attack/Combo/Range/ComboRange.h"


namespace Combo {
	// コンボデータ
	class ComboData {
	public:

		/// <summary>
		/// コンボデータを開始し、条件、モーション、判定、演出を初期化します。
		/// </summary>
		/// <param name="owner">コンボを実行するキャラクターです。</param>
		/// <param name="ctx">キャラクターと周辺システムへの参照をまとめた実行コンテキストです。</param>

		void Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx);

		/// <summary>
		/// コンボの経過時間に応じて移動、判定、演出、遠距離攻撃を更新します。
		/// </summary>
        /// <param name="ctx">更新対象キャラクターと周辺システムへの参照をまとめた実行コンテキストです。</param>

		void Update(const Character::CharacterContext& ctx);

		/// <summary>
		/// コンボを終了し、一時的に有効化した判定や演出を解除します。
		/// </summary>
        /// <param name="owner">コンボを終了するキャラクターです。</param>

		void Exit(Character::BaseCharacter* owner);
	public:
		/// <summary>
		/// コンボ条件データへの参照を取得します。
		/// </summary>
		/// <returns>内部で管理しているコンボ条件への参照です.</returns>

		ComboCondition& GetComboCondition() { return comboCondition; }
		/// <summary>
		/// コンボモーションへの参照を取得します。
		/// </summary>
		/// <returns>内部で管理しているモーションへの参照です.</returns>

		ComboMotion& GetComboMotion() { return motion; }
		/// <summary>
		/// コンボカメラ演出への参照を取得します。
		/// </summary>
		/// <returns>内部で管理しているカメラ演出への参照です.</returns>

		ComboCamera& GetComboCamera() { return camera; }
		/// <summary>
		/// コンボのヒットボックス設定への参照を取得します。
		/// </summary>
		/// <returns>内部で管理しているヒットボックスへの参照です.</returns>

		ComboHitBox& GetComboHitBox() { return hitBox; }
		/// <summary>
		/// コンボエフェクトへの参照を取得します。
		/// </summary>
		/// <returns>内部で管理しているエフェクトへの参照です.</returns>

		ComboEffect& GetComboEffect() { return effect; }
		/// <summary>
		/// コンボオーディオへの参照を取得します。
		/// </summary>
		/// <returns>内部で管理しているオーディオへの参照です.</returns>

		ComboAudio& GetComboAudio() { return audio; }
		/// <summary>
		/// コンボの遠距離攻撃処理への参照を取得します。
		/// </summary>
		/// <returns>内部で管理している遠距離攻撃処理への参照です.</returns>

		ComboRange& GetComboRange() { return range; }
		/// <summary>
		/// コンボの攻撃タイプを取得します。
		/// </summary>
		/// <returns>近距離、遠距離などのコンボタイプです.</returns>

		Type GetType() const { return type; }
		/// <summary>
		/// 攻撃ノード単位の調整データを編集可能な参照で取得します。
		/// </summary>
		/// <returns>攻撃ノード調整データへの参照です.</returns>

		GlobalAction& GetActionData() { return action; }
		/// <summary>
		/// 攻撃ノード単位の調整データを読み取り専用参照で取得します。
		/// </summary>
		/// <returns>攻撃ノード調整データへの読み取り専用参照です。</returns>
		const GlobalAction& GetActionData() const { return action; }
	public:	// 時間
		/// <summary>
		/// コンボ開始からの経過時間を取得します。
		/// </summary>
		/// <returns>経過時間を秒単位で返します.</returns>

		float GetTimer() const { return timer_; }
		/// <summary>
		/// コンボの経過時間を0秒へリセットします。
		/// </summary>
		void ResetTimer() { timer_ = 0.0f; }
		/// <summary>
		/// コンボの経過時間を設定します。
		/// </summary>
        /// <param name="time">設定する経過時間です。単位は秒です。</param>

		void SetTimer(float time) { timer_ = time; } 
		/// <summary>
		/// デバッグ用の挙動を有効にするか設定します。
		/// </summary>
        /// <param name="is">デバッグ中として扱う場合はtrueです。</param>

		void SetIsDebug(bool is) { isDebug = is; }
		/// <summary>
		/// コンボの攻撃タイプを設定します。
		/// </summary>
        /// <param name="type">設定するコンボタイプです。</param>

		void SetType(Type type) { this->type = type; }
	private:
		ComboCondition comboCondition{};		// コンボ条件クラス
		ComboMotion motion{};					// コンボ用モーションクラス
		ComboCamera camera{};					// コンボ用カメラクラス
		ComboHitBox hitBox{};					// コンボ用ヒットボックスクラス
		ComboEffect effect{};					// コンボ用エフェクト
		ComboAudio audio{};						// コンボ用オーディオクラス
		ComboRange range{};						// コンボ用遠距離クラス（弾など）
		GlobalAction action{};					// 攻撃ノード単位の調整項目
		float timer_ = 0.0f;					// 時間
		bool isDebug = false;					// エディターを使用したデバック中か
		bool isHitStop = false;					// ヒットストップ中か
		bool useCamera = false;					// カメラを使用するか
		Type type = Type::kMelle;// 攻撃タイプ
	};
};





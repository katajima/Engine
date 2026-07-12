#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include <map>
#include <vector>

namespace Engine {
	class WorldTransform;
	class AudioManager;
}


class CameraManager;		// カメラ管理
class BaseCamera;			// カメラ
namespace Character {
	class BaseCharacter;		// キャラクター
	struct CharacterContext;	// キャラクター状態
}
class BaseWeapon;			// 武器
class EffectSystem;			// エフェクト管理

namespace Combo {
	/// <summary>
	/// コンボ時のカメラ
	/// </summary>
	class ComboCamera {
	public:
		/// <summary>
		/// コンボカメラ演出を開始し、所有者から必要なカメラ参照を取得します。
		/// </summary>
		/// <param name="owner">コンボを実行するキャラクターです。所有権は受け取りません。</param>
		void Enter(Character::BaseCharacter* owner);

		/// <summary>
		/// コンボ中のカメラ切り替え、ズーム、シェイクなどを時間に応じて更新します。
		/// </summary>
		/// <param name="timer">コンボステート開始からの経過時間です。単位は秒です。</param>
		/// <param name="dt">前フレームからの経過時間です。単位は秒です。</param>
		void Update(float timer, float dt);

		/// <summary>
		/// 攻撃が命中した瞬間に設定されているカメラ演出を再生します。
		/// </summary>
		void OnHit();

		/// <summary>
		/// コンボカメラ演出を終了し、必要に応じて元のカメラ状態へ戻します。
		/// </summary>
		void Exit();

		/// <summary>
		/// カメラ演出で注視するターゲットTransformを設定します。
		/// </summary>
		/// <param name="target">注視対象Transformです。所有権は受け取りません。nullptrの場合はターゲットなしとして扱います。</param>
		void SetTarget(const Engine::WorldTransform* target) { this->target = target; }

		/// <summary>
		/// コンボカメラ演出の調整データを取得します。
		/// </summary>
		/// <returns>編集可能なカメラ演出データ参照を返します。</returns>
		GlobalCameraData& GetData() { return data_; }
	private:
		const Engine::WorldTransform* target;	

		BaseCamera* camera = nullptr;
		CameraManager* cameraManager = nullptr;	// カメラ管理
		// カメラデータ
		GlobalCameraData data_;
		bool isCameraChanged_ = false;			// カメラ切り替えを実行済みか
		bool isZoomRequested_ = false;			// ズーム演出を実行済みか
		bool isShakeRequested_ = false;			// シェイク演出を実行済みか
		bool isTargetOffsetRequested_ = false;	// 注視点オフセットを実行済みか
		bool isLookAheadRequested_ = false;		// 先読み演出を実行済みか
		bool isSpeedZoomRequested_ = false;		// 速度ズーム演出を実行済みか
		bool isLockOnReleased_ = false;			// ロックオン解除を実行済みか
	};

	/// <summary>
	/// コンボ用エフェクト
	/// </summary>
	class ComboEffect {
	public:
		/// <summary>
		/// コンボエフェクトを開始し、所有者や武器、エフェクトシステムへの参照を取得します。
		/// </summary>
		/// <param name="owner">コンボを実行するキャラクターです。所有権は受け取りません。</param>
		void Enter(Character::BaseCharacter* owner);

		/// <summary>
		/// コンボ時間、接地状態、発生条件に応じてエフェクトを更新・発生させます。
		/// </summary>
		/// <param name="ctx">接地状態や外部システム参照をまとめたコンテキストです。</param>
		/// <param name="timer">コンボステート開始からの経過時間です。単位は秒です。</param>
		/// <param name="dt">前フレームからの経過時間です。単位は秒です。</param>
		void Update(const Character::CharacterContext& ctx, float timer, float dt);

		/// <summary>
		/// コンボエフェクトを終了し、発生済みフラグなどの一時状態を整理します。
		/// </summary>
		/// <param name="owner">コンボを終了するキャラクターです。</param>
		void Exit(Character::BaseCharacter* owner);

		/// <summary>
		/// コンボシステムが持つ追従先Transform一覧を設定します。
		/// </summary>
		/// <param name="parentTransforms">追従先名をキーにしたTransform一覧です。ポインタの所有権は受け取りません。</param>
		void SetParentTransforms(const std::map<std::string, Engine::WorldTransform*>& parentTransforms) { parentTransforms_ = parentTransforms; }

		/// <summary>
		/// 指定時刻がトレイルエフェクトの発生時間内か確認します。
		/// </summary>
		/// <param name="timer">コンボステート開始からの経過時間です。単位は秒です。</param>
		/// <returns>トレイル発生時間内ならtrue、それ以外はfalseです。</returns>
		bool IsEffectTrail(float timer) const {
			return data_.trailEffectStartTime <= timer && 
				timer <= (data_.trailEffectLifeTime + data_.trailEffectStartTime);
		}

		/// <summary>
		/// コンボエフェクトの調整データを取得します。
		/// </summary>
		/// <returns>編集可能なエフェクトデータ参照を返します。</returns>
		GloblEffectData& GetData() { return data_; }
	private:
		/// <summary>
		/// 指定時間内のコンボエフェクトを頻度に応じて発生させます。
		/// </summary>
		/// <param name="ctx">発生条件の判定に使うキャラクターコンテキストです。</param>
		/// <param name="timer">コンボステート開始からの経過時間です。単位は秒です。</param>
		void EmitComboEffects(const Character::CharacterContext& ctx, float timer);

		/// <summary>
		/// 演出条件の時間範囲を満たしているか確認します。
		/// </summary>
		/// <param name="entry">判定するエフェクト発生設定です。</param>
		/// <param name="timer">コンボステート開始からの経過時間です。単位は秒です。</param>
		/// <returns>発生可能な時間範囲内ならtrue、それ以外はfalseです。</returns>
		bool IsTriggerTimeValid(const ComboEffectEntry& entry, float timer) const;

		/// <summary>
		/// 指定演出を現在の基準位置へ発生させます。
		/// </summary>
		/// <param name="entry">発生させるエフェクト設定です。</param>
		void EmitEntry(const ComboEffectEntry& entry);

		/// <summary>
		/// エフェクトの発生基準位置を取得します。
		/// </summary>
		/// <param name="entry">位置タイプや追従先名を含むエフェクト設定です。</param>
		/// <returns>ワールド座標での発生基準位置を返します。</returns>
		Vector3 GetEffectBasePosition(const ComboEffectEntry& entry) const;
	private:
		GloblEffectData data_;


		Character::BaseCharacter* owner = nullptr;	// 使用者
		BaseWeapon* weapon = nullptr;
		EffectSystem* effectSystem = nullptr;		// エフェクト発生先
		std::map<std::string, Engine::WorldTransform*> parentTransforms_;	// 追従先Transform一覧
		std::vector<float> nextEmitTimes_;			// 各コンボエフェクトの次回発生時間
		std::vector<bool> emittedFlags_;				// 一回発生条件が発生済みか
		bool wasOnGround_ = false;					// 前フレームの接地状態
	};


	/// <summary>
	/// コンボ用音
	/// </summary>
	class ComboAudio {
	public:
		/// <summary>
		/// ゲーム全体で共有する音声管理を設定します。
		/// </summary>
		/// <param name="audioManager">音声再生に使うAudioManagerです。所有権は受け取りません。</param>
		void Initialize(Engine::AudioManager* audioManager);

		/// <summary>
		/// コンボ音声状態を開始し、再生済みフラグをリセットします。
		/// </summary>
		/// <param name="owner">コンボを実行するキャラクターです。必要な位置情報などを参照します。</param>
		void Enter(Character::BaseCharacter* owner);

		/// <summary>
		/// 時間条件に応じた攻撃音などを更新します。
		/// </summary>
		/// <param name="ctx">音声条件の判定に使うキャラクターコンテキストです。</param>
		/// <param name="timer">コンボステート開始からの経過時間です。単位は秒です。</param>
		/// <param name="dt">前フレームからの経過時間です。単位は秒です。</param>
		void Update(const Character::CharacterContext& ctx, float timer, float dt);

		/// <summary>
		/// コンボ音声状態を終了します。
		/// </summary>
		/// <param name="owner">コンボを終了するキャラクターです。</param>
		void Exit(Character::BaseCharacter* owner);

		/// <summary>
		/// 攻撃が相手へ命中した瞬間の音を再生します。
		/// </summary>
		void OnHit();

		/// <summary>
		/// 保存・エディター編集対象となる音声設定を取得します。
		/// </summary>
		/// <returns>編集可能な音声設定参照を返します。</returns>
		GlobalAudio& GetData() { return data_; }


	private:
		GlobalAudio data_{};                         // コンボノードごとの音声設定。
		Engine::AudioManager* audioManager_ = nullptr; // ゲーム全体の音声管理（非所有）。
		bool isAttackSoundPlayed_ = false;            // 攻撃音の多重発火を防ぐフラグ。
	};


}

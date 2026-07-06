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
		// 開始
		void Enter(Character::BaseCharacter* owner);

		// 更新
		void Update(float timer, float dt);
		// 攻撃が命中した瞬間のカメラ演出を再生する
		void OnHit();

		// 終了
		void Exit();

		void SetTarget(const Engine::WorldTransform* target) { this->target = target; }

		// データ構造体取得
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
		// 開始
		void Enter(Character::BaseCharacter* owner);

		// 更新
		void Update(const Character::CharacterContext& ctx, float timer, float dt);

		// 終了
		void Exit(Character::BaseCharacter* owner);
		// コンボシステムが持つ追従先Transform一覧を設定する
		void SetParentTransforms(const std::map<std::string, Engine::WorldTransform*>& parentTransforms) { parentTransforms_ = parentTransforms; }

		// トレイルするか
		bool IsEffectTrail(float timer) const {
			return data_.trailEffectStartTime <= timer && 
				timer <= (data_.trailEffectLifeTime + data_.trailEffectStartTime);
		}
		GloblEffectData& GetData() { return data_; }
	private:
		// 指定時間内のコンボエフェクトを頻度に応じて発生させる
		void EmitComboEffects(const Character::CharacterContext& ctx, float timer);
		// 演出条件の時間範囲を満たしているか確認する
		bool IsTriggerTimeValid(const ComboEffectEntry& entry, float timer) const;
		// 指定演出を現在位置へ発生させる
		void EmitEntry(const ComboEffectEntry& entry);
		// エフェクトの発生基準位置を取得する
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
		// ゲーム全体で共有する音声管理と、このコンボの音声設定を適用する。
		void Initialize(Engine::AudioManager* audioManager);

		// 開始
		void Enter(Character::BaseCharacter* owner);

		// 更新
		void Update(const Character::CharacterContext& ctx, float timer, float dt);

		// 終了
		void Exit(Character::BaseCharacter* owner);

		// 攻撃が相手へ命中した瞬間の音を再生する。
		void OnHit();

		// 保存・エディタ編集対象となる音声設定を取得する。
		GlobalAudio& GetData() { return data_; }


	private:
		GlobalAudio data_{};                         // コンボノードごとの音声設定。
		Engine::AudioManager* audioManager_ = nullptr; // ゲーム全体の音声管理（非所有）。
		bool isAttackSoundPlayed_ = false;            // 攻撃音の多重発火を防ぐフラグ。
	};


}

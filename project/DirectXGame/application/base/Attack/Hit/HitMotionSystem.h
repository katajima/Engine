#pragma once
#include <DirectXGame/engine/Transform/WorldTransform/WorldTransform.h>
#include "HitMotion.h"
#include <list>
namespace Character {
	class BaseCharacter;
	class ParameterComponent;
}
namespace Engine {
	class PostEffectPass;
}
class MoveRequestSystem;
class EffectSystem;

/// <summary>
/// ヒットコンポーネント
/// </summary>
class HitMotionSystem
{
public:
	enum class HitMotionState
	{
		None,
		HitStop,
		Knockback,
		BlowAway,
		Launch,
		WallBounce,
		Suction,
		Down,
	};

public:
	~HitMotionSystem();

	/// <summary>キャラクターの被弾リアクション管理を初期化する。</summary>
	/// <param name="owner">リアクションを適用するキャラクター。非所有ポインターで、利用中は有効であること。</param>
	/// <param name="effectSystem">命中エフェクトの出力先。演出不要の場合はnullptrを許容する。</param>
	void Initialize(Character::BaseCharacter* owner, EffectSystem* effectSystem, Engine::Camera*  camera);
	/// <summary>ヒットストップ、ノックバック、スタン、ダウンなどの残り時間と移動を更新する。</summary>
	/// <param name="dt">秒単位のフレーム時間。</param>
	void Update(float dt);

	// ヒットエフェクト
	void EmitHitEffect();

	/// <summary>次に適用する被弾リアクション設定を開始する。</summary>
	/// <param name="data">リアクション種別、時間、移動量、演出を含む設定。</param>
	void SetReactionData(const HitReactionData& data);
	/// <summary>所有者自身へ適用するヒットストップ時間を設定する。</summary>
	/// <param name="time">停止時間（秒）。0以下で停止を解除する。</param>
	void SetSelfHitStopTime(float time) { selfHitStopTime_ = time; }
	/// <summary>いずれかの被弾リアクションが進行中か取得する。</summary>
	/// <returns>リアクション中ならtrue。</returns>
	bool IsHitMotion() const;
	/// <summary>操作不能のヒットスタン中か取得する。</summary>
	/// <returns>残りスタン時間がある場合はtrue。</returns>
	bool IsHitStun() const;
	/// <summary>ダウン状態が継続中か取得する。</summary>
	/// <returns>残りダウン時間がある場合はtrue。</returns>
	bool IsDown() const;
	/// <summary>打ち上げ滞空状態か取得する。</summary>
	/// <returns>残り滞空時間がある場合はtrue。</returns>
	bool IsLaunch() const;
	/// <summary>相手由来のヒットストップ中か取得する。</summary>
	/// <returns>残り停止時間がある場合はtrue。</returns>
	bool IsHitStop() const;
	/// <summary>自分由来のヒットストップ中か取得する。</summary>
	/// <returns>残り停止時間がある場合はtrue。</returns>
	bool IsSelfHitStop() const;
	/// <summary>現在のリアクション中に通常重力を適用できるか取得する。</summary>
	/// <returns>重力を適用する場合はtrue。</returns>
	bool IsGravityEnabled() const;
	/// <summary>現在のリアクションで使用する重力倍率を取得する。</summary>
	/// <returns>重力加速度へ乗算する倍率。</returns>
	float GetGravityScale() const;
	// スタン時間
	float GetHitStunTimer() const { return hitStunTimer_; }
	// ダウン時間
	float GetDownTimer() const { return downTimer_; }
	// セルフヒットストップ時間
	float GetSelfHitStopTimer() const { return selfHitStopTime_; }
	// 現在のヒットリアクション状態取得
	HitMotionState GetHitMotionState() const { return hitMotionState_; }


	// 終了処理
	bool IsFinished() const;
	// デバッグ用に現在のリアクションを即終了する
	void ForceFinishReaction();

private:
	/// <summary>パラメータへダメージを反映し、時間依存処理を更新する。</summary>
	/// <param name="dt">秒単位のフレーム時間。</param>
	/// <param name="parameter">ダメージを反映するパラメータ。nullptrの場合は反映しない。</param>
	void DamageProcess(float dt, Character::ParameterComponent* parameter);

	// ビネットのパラメータを調整する
	void ConfigureVignette();


	// 移動速度算出
	Vector3 BuildMoveVelocity() const;
	/// <summary>算出したリアクション速度を移動システムへ要求する。</summary>
	/// <param name="velocity">ワールド空間の速度。</param>
	void SendReactionMoveRequest(const Vector3& velocity);
	// リアクション終了
	void FinishReaction();

	/// <summary>残り時間を減算して継続中か判定する。</summary>
	/// <param name="dt">減算する秒単位の時間。</param>
	/// <param name="time">更新対象の残り時間。</param>
	/// <returns>減算後も時間が残っている場合はtrue。</returns>
	bool GetIsTime(float dt, float& time);

private:
	// モーションステート
	HitMotionState hitMotionState_ = HitMotionState::None;
	// リアクションデータ
	HitReactionData data_{};
	// ダメージモーションリスト
	std::list<DamageMotion> damageMotions_;
	// 時間
	float timer_ = 0.0f;
	// スタン時間
	float hitStunTimer_ = 0.0f;
	// ダウン時間
	float downTimer_ = 0.0f;
	// 打ち上げ時間
	float launchFloatTime_ = 0.0f;
	// 打ち上げ開始時の高さ
	float launchStartHeight_ = 0.0f;
	// ヒットストップ時間
	float hitStopTime_ = 0.0f;
	// セルフヒットストップ時間
	float selfHitStopTime_ = 0.0f;
	// レンダーテクスチャエフェクト時間
	float renderTargetEffectTime_ = 0.0f;
	// アクション中か
	bool isAction_ = false;

	// エフェクト発生位置
	Engine::WorldTransform worldEffect_;
	
private:
	// 所有者
	Character::BaseCharacter* owner = nullptr;
	// リアクション移動システム
	MoveRequestSystem* moveRequestSystem = nullptr;
	// エフェクト
	EffectSystem* effectSystem = nullptr;
	// カメラ
	Engine::Camera* camera = nullptr;

private:
	static constexpr const char* kVignetteEffectName_ = "DodgeSuccessVignette";			// ビネット登録名
	static constexpr const char* kVignettePassName_ = "Vignette_DodgeSuccessVignette";	// ビネットパス検索名
	// 被弾時のビネットパス
	Engine::PostEffectPass* vignettePass_ = nullptr;
};

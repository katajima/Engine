#pragma once
#include <DirectXGame/engine/Transform/WorldTransform/WorldTransform.h>
#include "HitMotion.h"
#include <list>
namespace Character {
	class BaseCharacter;
	class ParameterComponent;
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
		Down,
	};

public:

	// 初期化
	void Initialize(Character::BaseCharacter* owner, EffectSystem* effectSystem);
	// 更新
	void Update(float dt);

	// ヒットエフェクト
	void EmitHitEffect();
	
	// リアクションデータ設定
	void SetReactionData(const HitReactionData& data);
	// ヒットストップ時間設定(自分)
	void SetSelfHitStopTime(float time) { selfHitStopTime_ = time; }
	// ヒットモーション中か
	bool IsHitMotion() const;
	// スタン中か
	bool IsHitStun() const;
	// ダウン中か
	bool IsDown() const;
	// 打ち上げ中か
	bool IsLaunch() const;
	// ヒットストップ中か
	bool IsHitStop() const;
	// セルフヒットストップ中か
	bool IsSelfHitStop() const;
	// 重力は有効か
	bool IsGravityEnabled() const;
	// 重力倍率取得
	float GetGravityScale() const;
	// スタン時間
	float GetHitStunTimer() const { return hitStunTimer_; }
	// ダウン時間
	float GetDownTimer() const { return downTimer_; }
	// セルフヒットストップ時間
	float GetSelfHitStopTimer() const { return selfHitStopTime_; }


	// 終了処理
	bool IsFinished() const;

private:
	// ダメージ処理
	void DamageProcess(float dt, Character::ParameterComponent* parameter);
	// 移動速度算出
	Vector3 BuildMoveVelocity() const;
	// 方向指定
	Vector3 NormalizeSafe(const Vector3& v) const;
	// 移動システムにリクエストする
	void SendReactionMoveRequest(const Vector3& velocity);
	// リアクション終了
	void FinishReaction();

	// 時間内か
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
	// ヒットストップ時間
	float hitStopTime_ = 0.0f;
	// セルフヒットストップ時間
	float selfHitStopTime_ = 0.0f;

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
};
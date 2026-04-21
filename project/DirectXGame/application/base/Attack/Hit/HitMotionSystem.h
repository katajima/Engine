#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "DirectXGame/engine/math/random.h"
#include "DirectXGame/application/base/Attack/AttackData.h"
#include "HitMotion.h"


class ObjectComponent;	// 前方宣言
namespace Character {
	class BaseCharacter;
	class ParameterComponent;
}
class ReactionMoveSystem;
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
	// ヒットモーション中か
	bool IsHitMotion() const;
	// スタン中か
	bool IsHitStun() const;
	// ダウン中か
	bool IsDown() const;
	// 重力は有効か
	bool IsGravityEnabled() const;
	// 重力倍率取得
	float GetGravityScale() const;
	// スタン時間
	float GetHitStunTimer() const { return hitStunTimer_; }
	// ダウン時間
	float GetDownTimer() const { return downTimer_; }
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
	// アクション中か
	bool isAction_ = false;

	// エフェクト発生位置
	Engine::WorldTransform worldEffect_;
private:
	// 所有者
	Character::BaseCharacter* owner = nullptr;
	// リアクション移動システム
	ReactionMoveSystem* reactionMoveSystem = nullptr;
	// エフェクト
	EffectSystem* effectSystem = nullptr;
};
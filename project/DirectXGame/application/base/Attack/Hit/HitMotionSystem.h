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
	void Initialize(Character::BaseCharacter* owner);
	void Update(float dt);

	void SetReactionData(const HitReactionData& data);

	bool IsHitMotion() const;
	bool IsHitStun() const;
	bool IsDown() const;

	bool IsGravityEnabled() const;

	float GetGravityScale() const;

	float GetHitStunTimer() const { return hitStunTimer_; }
	float GetDownTimer() const { return downTimer_; }

	bool IsFinished() const;

private:
	void DamageProcess(float dt, Character::ParameterComponent* parameter);

	Vector3 BuildMoveVelocity() const;
	Vector3 NormalizeSafe(const Vector3& v) const;
	void SendReactionMoveRequest(const Vector3& velocity);
	void FinishReaction();

private:
	HitMotionState hitMotionState_ = HitMotionState::None;

	HitReactionData data_{};

	std::list<DamageMotion> damageMotions_;

	float timer_ = 0.0f;
	float hitStunTimer_ = 0.0f;
	float downTimer_ = 0.0f;

	bool isAction_ = false;

private:
	Character::BaseCharacter* owner_ = nullptr;
	ReactionMoveSystem* reactionMoveSystem_ = nullptr;
};
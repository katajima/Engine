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
		None,		// 無し
		HitStop,	// ヒットストップ
		Knockback,	// ノックバック
		AirStick,	// 吸い付き
	};

	// 初期化
	void Initialize(Character::BaseCharacter* owner);

	// 更新
	void Update(float dt);

	// リアクションデータ
	void SetReactionData(const HitReactionData& data);

	// ヒットモーション中か
	bool IsHitMotion();
	//
	float GetGravity() { return 1.0f; }
	// 重力があるか設定
	void UseGravity(ObjectComponent* object);

private:


	void DamageProcess(float dt, Character::ParameterComponent* parameter);

private:
	HitMotionState hitMotionState_ = HitMotionState::None;
	// ヒットストップモーション
	std::unique_ptr<HitStopMotion> hitStopMotion_;
	// ノックバックモーション
	std::unique_ptr <KnockbackMotion> knockbackMotion_;
	// 吸い付きモーション
	std::unique_ptr <AirStickMotion> airStickMotion_;

	// ダメージモーション
	std::list<DamageMotion> damageMotions_;

private:
	// 所有者
	Character::BaseCharacter* owner = nullptr;
	// リアクション移動システム
	ReactionMoveSystem* reactionMoveSystem = nullptr;
};

#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "DirectXGame/engine/math/random.h"
#include "DirectXGame/application/base/Attack/AttackData.h"
#include "HitMotion.h"


class ObjectComponent;	// 前方宣言
namespace Character {
	class ParameterComponent;
}
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


	// 更新
	void Update(float deltaTime, ObjectComponent* object, Character::ParameterComponent* parameter);

	// リアクションデータ
	void SetReactionData(const AttackReactionData& data);

	// ヒットモーション中か
	bool IsHitMotion();
	// 重力があるか設定
	void UseGravity(ObjectComponent* object);

private:


	void DamageProcess(float dt, Character::ParameterComponent* parameter);

private:
	HitMotionState hitMotionState_ = HitMotionState::None;
	// ヒットストップモーション
	HitStopMotion hitStopMotion_;
	// ノックバックモーション
	KnockbackMotion knockbackMotion_;
	// 吸い付きモーション
	AirStickMotion airStickMotion_;
	
	// ダメージモーション
	std::list<DamageMotion> damageMotions_;



};

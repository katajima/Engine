#include "HitMotionSystem.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include <DirectXGame/application/base/Character/Base/CharacterData.h>

void HitMotionSystem::Update(float dt, ObjectComponent* object, CharacterParameterComponent* parameter)
{
	// ダメージモーション
	DamageProcess(dt, parameter);

	// 各ヒットモーションが再生中なら更新する
	if (IsHitMotion()) {

		hitStopMotion_.Update(dt);			// ヒットストップモーション更新
		knockbackMotion_.Update(dt, object);// ノックバックモーション更新
		airStickMotion_.Update(dt, object);	// エアスティックモーション更新

		// 重力を設定 
		UseGravity(object);
	}
	else {	// 何もヒットモーションが発生していない場合は重力をオンにする
		object->GetRigidBodyComponent()->SetIsGravity(true);
	}
}

void HitMotionSystem::SetReactionData(const AttackReactionData& data)
{
	//hitStopMotion_.SetData(data.GetHitStopData());
	//airStickMotion_.SetData(data.GetAirStickData());
	knockbackMotion_.SetData(data.GetKnockbackData());

	DamageMotion dama;
	dama.SetData(data.GetDamageData());
	damageMotions_.push_back(dama);
}

bool HitMotionSystem::IsHitMotion()
{
	return hitStopMotion_.IsPlaying()
		|| knockbackMotion_.IsPlaying()
		|| airStickMotion_.IsPlaying();
}

void HitMotionSystem::UseGravity(ObjectComponent* object)
{
	bool hitStopGravity = hitStopMotion_.GetData().GetData().gravityEnabled_;
	bool knockbackGravity = knockbackMotion_.GetData().GetData().gravityEnabled_;
	bool airStickGravity = airStickMotion_.GetData().GetData().gravityEnabled_;

	bool isGravity = hitStopGravity && knockbackGravity && airStickGravity;

	object->GetRigidBodyComponent()->SetIsGravity(isGravity);
}

void HitMotionSystem::DamageProcess(float dt, CharacterParameterComponent* parameter) {

	for (auto& damage : damageMotions_) {
		damage.Update(dt);

		if (damage.GetDamageData().IsAttack()) {
			parameter->parameters_.HP.value -= damage.GetDamageData().GetDamage();
		}

	}

	// 終了していたら消す
	damageMotions_.remove_if([](const DamageMotion& damage) { if (!damage.IsPlaying()) {
		return true;
	}
	return false;
		});
}
#include "HitMotionSystem.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include <DirectXGame/application/base/Character/Base/CharacterData.h>
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
void HitMotionSystem::Initialize(Character::BaseCharacter* owner){
	this->owner = owner;
	reactionMoveSystem = owner->GetMoveComponent()->GetReactionMoveSystem();
	hitStopMotion_ = std::make_unique<HitStopMotion>();
	knockbackMotion_ = std::make_unique<KnockbackMotion>();
	airStickMotion_ = std::make_unique<AirStickMotion>();

};



void HitMotionSystem::Update(float dt)
{
	// ダメージモーション
	DamageProcess(dt, owner->GetCharacterParameterComponent());

	// 各ヒットモーションが再生中なら更新する
	if (IsHitMotion()) {

		hitStopMotion_->Update(dt);			// ヒットストップモーション更新
		knockbackMotion_->Update(dt, owner->GetObjectComponent());// ノックバックモーション更新
		airStickMotion_->Update(dt, owner->GetObjectComponent());	// エアスティックモーション更新

		// 重力を設定 
		UseGravity(owner->GetObjectComponent());
	
		//reactionMoveSystem->SetRequest();
	}
}

void HitMotionSystem::SetReactionData(const HitReactionData& data)
{
	//hitStopMotion_.SetData(data.GetHitStopData());
	//airStickMotion_.SetData(data.GetAirStickData());
	knockbackMotion_->SetData(data.GetKnockbackData());

	DamageMotion dama;
	dama.SetData(data.GetDamageData());
	damageMotions_.push_back(dama);
}

bool HitMotionSystem::IsHitMotion()
{
	return hitStopMotion_->IsPlaying()
		|| knockbackMotion_->IsPlaying()
		|| airStickMotion_->IsPlaying();
}

void HitMotionSystem::UseGravity(ObjectComponent* object)
{
	bool hitStopGravity = hitStopMotion_->GetData().GetData().gravityEnabled;
	bool knockbackGravity = knockbackMotion_->GetData().GetData().gravityEnabled;
	bool airStickGravity = airStickMotion_->GetData().GetData().gravityEnabled;

	bool isGravity = hitStopGravity && knockbackGravity && airStickGravity;

	object->GetRigidBodyComponent()->SetIsGravity(isGravity);
}

void HitMotionSystem::DamageProcess(float dt, Character::ParameterComponent* parameter) {

	for (auto& damage : damageMotions_) {
		damage.Update(dt);

		if (damage.GetDamageData().IsAttack()) {
			parameter->parameters->HP.value -= damage.GetDamageData().GetDamage();
		}

	}

	// 終了していたら消す
	damageMotions_.remove_if([](const DamageMotion& damage) { if (!damage.IsPlaying()) {
		return true;
	}
	return false;
		});
}
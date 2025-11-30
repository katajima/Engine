#include "HitMotionSystem.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"

void HitMotionSystem::Update(float dt, ObjectComponent* object)
{
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

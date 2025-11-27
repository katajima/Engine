#include "MoveComponent.h"


void MovementComponent::Initialize(ControlType type) {
	// 移動システムの生成
	moveSystem_ = std::make_unique<MoveSystem>();
	moveSystem_->Initialize();

	// ジャンプシステムの生成
	jumpSystem_ = std::make_unique<JumpSystem>();
	jumpSystem_->Initialize();

	// 移動制限の生成
	movementRestrictions_ = std::make_unique<MovementRestrictions>();
	movementRestrictions_->Initialize({ Vector3::Set(-200.0f) }, { Vector3::Set(200.0f) });

	// 状態遷移機械の生成
	movementStateMachine_ = std::make_unique<MovementStateMachine>();
	movementStateMachine_->Initialize();

	// 操作タイプの設定
	controlType_ = type;
};

void MovementComponent::Update(float dt, WorldTransform& object, RigidBodyComponent& rigid, Input* input) {


	// 移動システムの更新
	if (controlType_ == ControlType::Manual) {	// 手動操作なら入力を渡す
		moveSystem_->Update(dt, object, input);
	}
	else {
		moveSystem_->Update(dt, object);
	}
	// ジャンプシステムの更新
	jumpSystem_->Update(dt, object,rigid);
	// 移動制限の更新
	movementRestrictions_->Update(object);
}





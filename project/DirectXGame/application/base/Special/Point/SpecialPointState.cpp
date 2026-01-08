#include "SpecialPointState.h"

#include "SpecialPoint.h"

#pragma region Pop

// 開始
void SpecialPointPopState::Enter() {
	timer = 0.0f;
	object_->GetObjectComponent()->GetWorldTransform().translate_.y = 2.0f;
	object_->GetObjectComponent()->GetRigidBodyComponent()->AddForce(upVelocity_);
	object_->GetObjectComponent()->GetRigidBodyComponent()->SetGravityScale(glavityScale_);
	object_->GetObjectComponent()->GetRigidBodyComponent()->SetIsGravity(true);
	object_->GetObjectComponent()->GetWorldTransform().Update();
};
// 更新
void SpecialPointPopState::Update(float dt) {
	timer += dt; // 時間加算

	if (groundY >= object_->GetObjectComponent()->GetWorldPosition().y) {
		object_->GetObjectComponent()->GetWorldTransform().translate_.y = groundY;
		object_->GetObjectComponent()->GetRigidBodyComponent()->SetIsGravity(false);
		// 地面に着地したら待機状態へ
		object_->GetStateMachine()->ChangeState(SpecialPointState::kIdle);
		return;
	}
	else {
		object_->GetObjectComponent()->GetRigidBodyComponent()->SetIsGravity(true);
	}

	// 範囲内なら移動状態へ
	if (object_->IsMove() && IsNextStateTime()) {
		object_->GetObjectComponent()->GetRigidBodyComponent()->SetIsGravity(false);
		object_->GetStateMachine()->ChangeState(SpecialPointState::kMove);
		return;
	}

	object_->GetObjectComponent()->GetWorldTransform().Update();
};
// 終了
void SpecialPointPopState::Exit() {
	timer = 0.0f;
	object_->GetObjectComponent()->GetRigidBodyComponent()->SetIsGravity(false);
	object_->GetObjectComponent()->GetRigidBodyComponent()->ResetAcceleration();
	object_->GetObjectComponent()->GetRigidBodyComponent()->ResetVelocity();
};

#pragma endregion // 出現


#pragma region Idle

// 開始
void SpecialPointIdleState::Enter() {
	timer = 0.0f;
};
// 更新
void SpecialPointIdleState::Update(float dt) {
	timer += dt;

	object_->GetObjectComponent()->GetWorldTransform().rotate_.y += rotateSpeed_ * dt;

	// 一定時間経過で終了状態へ
	if (endStateTimer_ <= timer) {
		object_->GetStateMachine()->ChangeState(SpecialPointState::kEnd);
		return;
	}

	// ターゲットが範囲内に入ったら移動状態へ
	if (object_->IsMove()) {
		object_->GetStateMachine()->ChangeState(SpecialPointState::kMove);
		return;
	}

};
// 終了
void SpecialPointIdleState::Exit() {
	timer = 0.0f;
};

#pragma endregion // 待機


#pragma region Move

// 開始
void SpecialPointMoveState::Enter() {
	timer = 0.0f;
};
// 更新
void SpecialPointMoveState::Update(float dt) {
	timer += dt;

	Vector3 dire = Normalize(object_->GetTargetPos() - object_->GetObjectComponent()->GetWorldPosition());

	object_->GetObjectComponent()->GetWorldTransform().translate_ += dire * speed_ * dt;
};
// 終了
void SpecialPointMoveState::Exit() {
	timer = 0.0f;
};

#pragma endregion // 移動


#pragma region End

// 開始
void SpecialPointEndState::Enter() {
	timer = 0.0f;
};
// 更新
void SpecialPointEndState::Update(float dt) {
	timer += dt;

	object_->Die();
	object_->GetObjectComponent()->Delete();
	object_->GetObjectComponent()->GetWorldTransform().scale_ = {0,0,0};
};
// 終了
void SpecialPointEndState::Exit() {
	timer = 0.0f;
};

#pragma endregion // 終了

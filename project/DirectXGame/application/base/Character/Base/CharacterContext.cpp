#include "CharacterContext.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Attack/AttackController.h"

void Character::CharacterContextSystem::Initialize(BaseCharacter* owner, const InputSystem* input) {
	this->owner = owner;	// 所有者
	this->input = input;	// 入力データ
	this->worldTransform = &owner->GetObjectComponent()->GetWorldTransform();	// ワールドトランスフォーム
	this->rigidBody = owner->GetObjectComponent()->GetRigidBodyComponent();		// リジットボディー
	this->movementComponent = owner->GetMoveComponent();						// 移動
	this->jumpSystem = owner->GetMoveComponent()->GetJumpSystem();				// ジャンプシステム
	this->moveSystem = owner->GetMoveComponent()->GetMoveSystem();				// 移動システム
	this->lockOnSystem = owner->GetAttackController()->GeyLockOnSysutem();		// ロックオンシステム
	this->comboStateMachine = owner->GetAttackController()->GetComboSystem()->GetComboStateMachine();	// コンボステートマシン
}

Character::CharacterContext Character::CharacterContextSystem::CreateContext(float dt) {
	CharacterContext ctx{};
	// 時間
	ctx.dt = dt;
	// 入力データ
	if (input) {
		ctx.input = input;
		ctx.inputData = input->GetPlayerInputData();
	}
	// パラメータ;
	ctx.parameters = owner->GetBasicParameters();
	// カメラ
	if (owner->GetCamera() && input) {
		ctx.camera = owner->GetCamera();
		// カメラ方向
		ctx.cameraDirection = owner->GetCamera()->GetForward();
		// カメラ行列取得
		Matrix4x4 cameraWorldMatrix = Inverse(ctx.camera->GetViewMatrix());
		// スティック方向
		Vector2 stick = ctx.inputData.moveShick;

		// カメラの向きに基づいて移動方向をワールド座標系に変換
		Vector2 worldDirection = {
			stick.x * cameraWorldMatrix.m[0][0] + stick.y * cameraWorldMatrix.m[2][0],
			stick.x * cameraWorldMatrix.m[0][2] + stick.y * cameraWorldMatrix.m[2][2]
		};
		// スティック入力方向をワールド座標系で
		ctx.worldStickDirection = worldDirection;
	}


	// 方向
	ctx.direction = worldTransform->GetForward();
	// 位置
	ctx.position = worldTransform->GetWorldPosition();
	// 現在のステート
	ctx.state = owner->GetCurrentMainState();
	
	ctx.isStop = !owner->GetIsMove();

	// 重力
	ctx.isGravity = rigidBody->IsGravity();

	// 着地状態か
	ctx.onGround = movementComponent->GetIsLanding();
	// 攻撃中なら
	if (ctx.state == CharacterMainState::Attack) {
		ctx.isAttacking = true;
		if (!ctx.onGround) {
			ctx.isJumpAttacking = true;
		}
	}
	if (ctx.state == CharacterMainState::Jump) {
		ctx.isJumping = true;
	}
	if (ctx.state == CharacterMainState::Damage || ctx.state == CharacterMainState::Die || ctx.state == CharacterMainState::Fainting) {
		ctx.isCanMove = false;
	}
	if (ctx.state == CharacterMainState::Idle || ctx.state == CharacterMainState::Move) {
		ctx.isCanJump = true;
	}
	// 攻撃時の重力
	if (comboStateMachine->GetCurrentState()) {
		ctx.attackingGravity = comboStateMachine->GetCurrentState()->GetData().GetComboMotion().GetComboMove().GetData().gravityScale_;
	}
	// 上昇時の重力
	ctx.upGravity = jumpSystem->GetData().upGravity;
	// 落下時の重力
	ctx.fallGravity = jumpSystem->GetData().fallGravity;
	// ターゲット取得
	ctx.target = lockOnSystem->GetTarget();
	// 移動速度
	ctx.moveSpeed = moveSystem->GetData().maxSpeed;
	
	return ctx;
}

#include "CharacterContext.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Attack/AttackController.h"

void Character::CharacterContextSystem::Initialize(BaseCharacter* owner, const InputSystem* input) {
	this->input = input;	// 入力データ
	this->worldTransform = &owner->GetObjectComponent()->GetWorldTransform();	// ワールドトランスフォーム
	this->rigidBody = owner->GetObjectComponent()->GetRigidBodyComponent();		// リジットボディー
	this->movementComponent = owner->GetMoveComponent();						// 移動
	this->jumpSystem = owner->GetMoveComponent()->GetJumpSystem();				// ジャンプシステム
	this->moveSystem = owner->GetMoveComponent()->GetMoveSystem();				// 移動システム
	this->lockOnSystem = owner->GetAttackController()->GeyLockOnSysutem();		// ロックオンシステム
	this->hitMotionSystem = owner->GetHitMotionSystem();							// レスポンスシステム
	this->parameters = owner->GetCharacterParameterComponent();								// パラメータ
}

Character::CharacterContext Character::CharacterContextSystem::CreateContext(BaseCharacter* owner,float dt) {
	CharacterContext ctx{};
	if (!owner) return ctx;
	// 時間
	ctx.dt = dt;
	// パラメータ;
	ctx.parameters = owner->GetBasicParameters();

	// キャラクターの入力コンテキストを作成
	CreateContextInput(owner, ctx);
	// キャラクターの状態コンテキストを作成
	CreateContextState(owner, ctx);
	// キャラクターの重力コンテキストを作成
	CreateContextGravity(owner, ctx);
	// キャラクターの移動コンテキストを作成
	CreateContextMovement(owner, ctx);

	return ctx;
}

void Character::CharacterContextSystem::CreateContextState(BaseCharacter* owner, CharacterContext& ctx){
	// 現在のステート
	ctx.state = owner->GetCurrentMainState();

	// ダッシュ状態か
	if (ctx.inputData.dashHeld && parameters->GetStamina() > 1.0f) {
		parameters->Stamina().value -= 10.0f * ctx.dt;	// スタミナを減らす
		parameters->Stamina().useRate = false;	// スタミナ回復を止める
		ctx.isDashing = true;
	}
	else {
		parameters->Stamina().useRate = true;	// スタミナ回復
	}

	// ヒットストップ中か
	ctx.isHitStop = hitMotionSystem->IsHitStop();
	ctx.isSelfHitStop = hitMotionSystem->IsSelfHitStop();
	// 着地状態か
	ctx.onGround = movementComponent->GetIsLanding();
	// 攻撃中なら
	if (ctx.state == CharacterMainState::Attack) {
		ctx.isAttacking = true;
		if (!ctx.onGround) {
			ctx.isJumpAttacking = true;
		}
	}
	// 必殺技中
	if (ctx.state == CharacterMainState::Special) {
		ctx.isSpecialAttacking = true;	
	}
	// ジャンプ中
	if (ctx.state == CharacterMainState::Jump) {
		ctx.isJumping = true;	
	}
	// ダメージ状態なら
	if (ctx.state == CharacterMainState::Damage || ctx.state == CharacterMainState::Die || ctx.state == CharacterMainState::Fainting) {
		ctx.isCanMove = false;	// 動けないようにする
		if (ctx.state == CharacterMainState::Damage || ctx.state == CharacterMainState::Fainting)
			ctx.isDamage = true;	// ダメージを受けている
		ctx.isGravity = hitMotionSystem->IsGravityEnabled();
	}
	// ジャンプ可能か
	if (ctx.state == CharacterMainState::Idle || ctx.state == CharacterMainState::Move) {
		ctx.isCanJump = true;	// ジャンプ可能にする
	}
}

void Character::CharacterContextSystem::CreateContextGravity(BaseCharacter* owner, CharacterContext& ctx){
	// 攻撃時の重力
	comboStateMachine = owner->GetAttackController()->GetComboSystem()->GetComboStateMachine();	// コンボステートマシン
	if (comboStateMachine->GetCurrentState()) {
		ctx.attackingGravity = comboStateMachine->GetCurrentState()->GetData().GetComboMotion().GetComboMove().GetData().gravityScale;
	}
	// ダメージ中の重力
	ctx.damageGravity = hitMotionSystem->GetGravityScale();
	// 上昇時の重力
	ctx.upGravity = jumpSystem->GetData().upGravity;
	// 落下時の重力
	ctx.fallGravity = jumpSystem->GetData().fallGravity;
}

void Character::CharacterContextSystem::CreateContextMovement(BaseCharacter* owner, CharacterContext& ctx){
	// 方向
	ctx.direction = worldTransform->GetForward();
	// 位置
	ctx.position = worldTransform->GetWorldPosition();
	//　止まるか
	ctx.isStop = !owner->GetIsMove();
	// ターゲット取得
	ctx.target = lockOnSystem->GetTarget();
	// 移動速度
	ctx.moveSpeed = moveSystem->GetData().maxSpeed;
	// 高さ
	ctx.skyHeight = moveSystem->GetData().skyHeight;
}

void Character::CharacterContextSystem::CreateContextInput(BaseCharacter* owner, CharacterContext& ctx){

	// 入力データ
	if (input) {
		ctx.input = input;
		ctx.inputData = input->GetPlayerInputData();
	}
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
}

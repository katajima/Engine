#include "CharacterContext.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Attack/AttackController.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include <DirectXGame/application/base/Character/Death/DeathSystem.h>

void Character::CharacterContextSystem::Initialize(BaseCharacter* owner, const InputSystem* input) {
	this->input = input;	// 入力データ
	this->worldTransform = &owner->GetObjectComponent()->GetWorldTransform();	// ワールドトランスフォーム
	this->rigidBody = owner->GetObjectComponent()->GetRigidBodyComponent();		// リジットボディー
	this->movementComponent = owner->GetMoveComponent();						// 移動
	this->jumpSystem = owner->GetMoveComponent()->GetJumpSystem();				// ジャンプシステム
	this->moveSystem = owner->GetMoveComponent()->GetMoveSystem();				// 移動システム
	this->deathSystem = owner->GetDeathSystem();								// 死亡システム
	this->lockOnSystem = owner->GetAttackController()->GeyLockOnSysutem();		// ロックオンシステム
	this->hitMotionSystem = owner->GetHitMotionSystem();						// レスポンスシステム
	this->parameters = owner->GetCharacterParameterComponent();					// パラメータ
}

Character::CharacterContext Character::CharacterContextSystem::CreateContext(BaseCharacter* owner, float dt) {
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

void Character::CharacterContextSystem::CreateContextState(BaseCharacter* owner, CharacterContext& ctx) {
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

	// 現在のステート
	ctx.state = owner->GetCurrentMainState();



	// 攻撃時の重力
	comboStateMachine = owner->GetAttackController()->GetComboSystem()->GetComboStateMachine();	// コンボステートマシン
	

	// 状態に応じたフラグ設定
	switch (ctx.state)
	{
	case CharacterMainState::Idle: // 待機中
	case CharacterMainState::Move: // 移動中
		ctx.isCanJump = true;	// ジャンプ可能にする
		break;
	case CharacterMainState::Jump: // ジャンプ中
		ctx.isJumping = true;
		break;
	case CharacterMainState::Avoidance: // 回避中
		break;
	case CharacterMainState::Defense: // 防御中
		break;
	case CharacterMainState::Attack: // 攻撃中
		ctx.isAttacking = true;
		if (!ctx.onGround) {
			ctx.isJumpAttacking = true;
		}

		// 重力
		if(comboStateMachine->GetCurrentState())
		ctx.isGravity = comboStateMachine->GetCurrentState()->GetData().GetComboMotion().GetComboMove().GetData().isGravity;
		break;
	case CharacterMainState::Special: // 必殺技中
		ctx.isSpecialAttacking = true;
		break;
	case CharacterMainState::Die: // 死亡中
		// 動けないようにする
		ctx.isCanMove = false;	
		// 死亡中の重力
		ctx.isGravity = deathSystem->GetData().isGravity;	
		break;
	case CharacterMainState::Fainting: // 気絶中
	case CharacterMainState::Damage: // 被弾中
		// 動けないようにする
		ctx.isCanMove = false;
		// ダメージを受けている
		ctx.isDamage = true;	
		break;	
	default:
		break;
	}
}

// 重力処理
void Character::CharacterContextSystem::CreateContextGravity(BaseCharacter* owner, CharacterContext& ctx) {
	if (comboStateMachine->GetCurrentState()) {
		ctx.attackingGravity = comboStateMachine->GetCurrentState()->GetData().GetComboMotion().GetComboMove().GetData().gravityScale;
	}
	// ダメージ中の重力
	ctx.damageGravity = hitMotionSystem->GetGravityScale();
	// 上昇時の重力
	ctx.upGravity = jumpSystem->GetData().upGravity;
	// 落下時の重力
	ctx.fallGravity = jumpSystem->GetData().fallGravity;
	// 死亡中の重力
	ctx.dieGravity = deathSystem->GetData().gravityScale;
}

void Character::CharacterContextSystem::CreateContextMovement(BaseCharacter* owner, CharacterContext& ctx) {
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

void Character::CharacterContextSystem::CreateContextInput(BaseCharacter* owner, CharacterContext& ctx) {

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

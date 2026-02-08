#include "SmallMeleeEnemy.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include"DirectXGame/application/base/Effect/Effect.h"

void SmallMeleeEnemy::Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, 
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera){
	// 基盤初期化
	BaseInitialize(input, entity3DManager, entity2DManager, globalVariables, position, camera, "enemyBodySG01.obj", "enemy");
	// サイズ
	Vector3 size = { 0.9f,0.9f,0.9f };
	objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定


	objectComponentShadow_->GetWorldTransform().scale_ = {5.0f,5.0f ,5.0f };

	// パラメーター初期化
	Parameters().HP.Initiaize(50, 0, 100, 0);
	Parameters().speed = 10.0f;
	Parameters().strength = 10.0f;

	moveComponent_->GetMoveSystem()->GetData().maxSpeed = Parameters().speed;


	// 武器
	weapon_ = std::make_unique<SmallMeleeWeapon>();
	weapon_->SetCharacter(this);
	weapon_->Initialize(input_, entity3DManager_, nullptr, globalVariables_, {}, nullptr);
	weapon_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
	weapon_->GetWorldTransform().translate_ = { 0.0f,0.0f,2.0f };


	// エフェクト用のトランスフォーム初期化
	worldEffect_.Initialize();
	worldEffect_.parent_ = &objectComponent_->GetWorldTransform();
	worldEffect_.translate_ = { 0,1,0 };

	// 保存項目初期化
	InitializeBaseAddItem();
	// スプライト初期化
	Initialize2D();
	// トランスフォーム更新
	GetWorldTransform().Update();
}

void SmallMeleeEnemy::Update(){
	// 攻撃制御更新
	attackController_->Update(GetTime());
	// 基盤の更新
	BaseUpdate();
	//
	weapon_->Update(); // 武器更新
}

void SmallMeleeEnemy::Draw2D(){
}

void SmallMeleeEnemy::Emit(){
	// エフェクト座標更新
	worldEffect_.Update();

	// 各エフェクト出現
	effect_->Emit("starEmit", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("hitEmit", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("hitEffect2", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("ringHit", worldEffect_.worldMat_.GetWorldPosition());
}

void SmallMeleeEnemy::Move(){
	if (GetTargetDistance() <= 25.0f) {
		attackTimer_ += GetTime();
		if (attackTimer_ >= 3.0f) {
			GetCharacterStateMachine()->ChangeState(CharacterMainState::Attack);
			attackTimer_ = 0.0f;
			return;
		}
		if (GetTargetDistance() <= 20.0f) {
			DirectionMove(-Parameters().speed * 1.2f);
		}
	}
	else {
		attackTimer_ = 0.0f;
		// 移動
		DirectionMove(Parameters().speed);
	}
}

void SmallMeleeEnemy::InitStateMachine()
{
	// ステートマシーン初期化
	stateMachine_ = std::make_unique<CharacterStateMachine>();
	stateMachine_->RegisterState(CharacterMainState::Move, [](BaseCharacter* p) {
		return std::make_unique<SmallMeleeEnemyMoveState>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Attack, [](BaseCharacter* p) {
		return std::make_unique<SmallMeleeEnemyAttackState>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Die, [](BaseCharacter* p) {
		return std::make_unique<SmallMeleeEnemyDieState>(p);
		});
	stateMachine_->Init(this, CharacterMainState::Move);
}

#include "SmallRangeEnemy.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include"DirectXGame/application/base/Effect/Effect.h"


void SmallRangeEnemy::Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
{
	// 基盤初期化
	BaseInitialize(input, entity3DManager, entity2DManager, globalVariables, position, camera, "enemyBodySS01.obj", "enemy");
	// サイズ
	Vector3 size = { 1.0f,1.0f,1.0f };
	objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定
	objectComponent_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効化

	objectComponentPropeller_ = std::make_unique<ObjectComponent>();
	objectComponentPropeller_->InitializeInstancing(entity3DManager, globalVariables, "propeller", "enemyPropellerSS01.obj", "", 
		false, false, this, Engine::Object3dInstansManager::TransparencyType::kNo);
	objectComponentPropeller_->SetInstancingSRT({1,1,1}, {}, {});
	objectComponentPropeller_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効化


	objectComponentPropeller_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();

	bulletSpawn_ = std::make_unique<BulletSpawn>();
	bulletSpawn_->Initialize(this,entity3DManager,entity2DManager,globalVariables,nullptr,effect_);


	// 武器
	weapon_ = std::make_unique<SmallRangeWeapon>();
	weapon_->SetCharacter(this);
	weapon_->SetBulletManager(bulletManager_);
	weapon_->Initialize(input_, entity3DManager_, nullptr, globalVariables_, {}, nullptr);
	weapon_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
	weapon_->GetWorldTransform().translate_ = { 0.0f,-2.5f,1.5f };

	// パラメーター初期化
	Parameters().HP.Initiaize(40, 0, 100, 0);
	Parameters().speed = 10.0f;
	Parameters().strength = 10.0f;

	moveComponent_->GetMoveSystem()->GetData().maxSpeed = Parameters().speed;
	moveComponent_->GetJumpSystem()->SetIsUseJump(false);
	moveComponent_->GetDashSystem()->SetIsUseGravity(false);


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

void SmallRangeEnemy::Update()
{
	
	// 攻撃制御更新
	attackController_->Update(GetTime());
	// 基盤の更新
	BaseUpdate();
	
	weapon_->Update(); // 武器更新

	objectComponent_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効化


	objectComponentPropeller_->GetWorldTransform().rotate_.y += 20.0f * GetTime();

	objectComponentPropeller_->Update();
}

void SmallRangeEnemy::Draw2D()
{
}

void SmallRangeEnemy::Emit()
{
	// エフェクト座標更新
	worldEffect_.Update();

	// 各エフェクト出現
	effect_->Emit("starEmit", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("hitEmit", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("hitEffect2", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("ringHit", worldEffect_.worldMat_.GetWorldPosition());
}

void SmallRangeEnemy::Move()
{
	// 移動
	DirectionMoveVelocity(Parameters().speed);
	
	if (GetWorldTransform().GetWorldPosition().y < skyHeight_) {
		Velocity().y = 1.5f; // Y軸速度上昇
	}
	else if(GetWorldTransform().GetWorldPosition().y > skyHeight_) {
		Velocity().y = -1.5f; // Y軸速度降下
	}
	else {
		Velocity().y = 0.0f; // Y軸速度リセット
	}





	if (GetTargetDistance() <= 25.0f) {
		attackTimer_ += GetTime();
		if (attackTimer_ >= 2.0f) {
			attackTimer_ = 0.0f;
			// 攻撃ステートへ
			GetCharacterStateMachine()->ChangeState(CharacterMainState::Attack);
			return;
		}
		if (GetTargetDistance() <= 20.0f) {
			Vector3 velo = { -GetVelocity().x, GetVelocity().y, -GetVelocity().z };
			TargetMove(velo * 0.9f);
		}
	}
	else {
		attackTimer_ = 0.0f;

		TargetMove(GetVelocity());
	}
}

void SmallRangeEnemy::InitStateMachine()
{
	// ステートマシーン初期化
	stateMachine_ = std::make_unique<CharacterStateMachine>();
	stateMachine_->RegisterState(CharacterMainState::Move, [](BaseCharacter* p) {
		return std::make_unique<SmallRangeEnemyMoveState>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Attack, [](BaseCharacter* p) {
		return std::make_unique<SmallRangeEnemyAttackState>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Die, [](BaseCharacter* p) {
		return std::make_unique<SmallRangeEnemyDieState>(p);
		});
	stateMachine_->Init(this, CharacterMainState::Move);
}

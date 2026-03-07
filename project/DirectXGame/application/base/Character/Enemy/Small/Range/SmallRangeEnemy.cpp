#include "SmallRangeEnemy.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include"DirectXGame/application/base/Effect/Effect.h"

namespace Character {
	void SmallRangeEnemy::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
	{
		// 基盤初期化
		BaseInitialize(inputSystem, entityManager, globalVariables, position, camera, "enemyBodySS01.obj", "smallRangeEnemy",1.5f);
		
		
		
		// サイズ
		Vector3 size = { 1.0f,1.0f,1.0f };
		objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定
		objectComponent_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効化

		objectComponentPropeller_ = std::make_unique<ObjectComponent>();
		objectComponentPropeller_->InitializeInstancing(entityManager, globalVariables, "propeller", "enemyPropellerSS01.obj", "",
			false, false, this, Engine::Object3dInstansManager::TransparencyType::kNo);
		objectComponentPropeller_->SetInstancingSRT({ 1,1,1 }, {}, {});
		objectComponentPropeller_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効化


		objectComponentPropeller_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();

		bulletSpawn_ = std::make_unique<BulletSpawn>();
		bulletSpawn_->Initialize(this, entityManager,globalVariables, nullptr, effect);


		// 武器
		weapon_ = std::make_unique<SmallRangeWeapon>();
		weapon_->SetCharacter(this);
		weapon_->SetBulletManager(bulletManager);
		weapon_->Initialize(inputSystem, entityManager, globalVariables, {}, nullptr);
		weapon_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
		weapon_->GetWorldTransform().translate_ = { 0.0f,-0.5f,0.25f };

		
		moveComponent_->GetMoveSystem()->GetData().maxSpeed = Parameters()->speed;
		moveComponent_->GetMovementSystem()->SetUseGravity(false);
		

		// エフェクト用のトランスフォーム初期化
		worldEffect_.Initialize();
		worldEffect_.parent_ = &objectComponent_->GetWorldTransform();
		worldEffect_.translate_ = { 0,1,0 };

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
		effect->Emit("starEmit", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("hitEmit", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("hitEffect2", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("ringHit", worldEffect_.worldMat_.GetWorldPosition());
	}

	void SmallRangeEnemy::Move()
	{
		// 移動
		DirectionMoveVelocity(Parameters()->speed);

		if (GetWorldTransform().GetWorldPosition().y < skyHeight_) {
			Velocity().y = 1.2f; // Y軸速度上昇
		}
		else if (GetWorldTransform().GetWorldPosition().y > skyHeight_) {
			Velocity().y = -1.2f; // Y軸速度降下
		}
		else {
			Velocity().y = 0.0f; // Y軸速度リセット
		}





		if (GetTargetDistance() <= globalData_.attackStartRadius) {
			attackTimer_ += GetTime();
			if (attackTimer_ >= globalData_.attackTimer) {
				attackTimer_ = 0.0f;
				// 攻撃ステートへ
				GetCharacterStateMachine()->ChangeState(CharacterMainState::Attack);
				return;
			}
			if (GetTargetDistance() <= globalData_.startRetreatingRadius) {
				Vector3 velo = { -GetVelocity().x, GetVelocity().y, -GetVelocity().z };
				TargetMove(velo.Normalize() * globalData_.retreatSpeed);
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
}
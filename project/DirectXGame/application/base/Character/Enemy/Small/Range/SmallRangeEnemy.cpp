#include "SmallRangeEnemy.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include "DirectXGame/application/base/Bullet/base/BulletSpawn.h" 

namespace Character {
	void SmallRangeEnemy::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
	{
		// 基盤初期化
		BaseInitialize(inputSystem, entityManager, globalVariables, position, camera, "enemyBodySS01.obj", "SmallRangeEnemy",1.5f);
		// エフェクト初期化
		InitializeEffect({ 0,1,0 });
		//
		InitializeWeapon<SmallRangeWeapon>({ 0.0f,-0.5f,0.25f });

		bulletSpawn_ = std::make_unique<BulletSpawn>();
		bulletSpawn_->Initialize(this, entityManager, globalVariables, nullptr, effect);


		objectComponentPropeller_ = std::make_unique<ObjectComponent>();
		objectComponentPropeller_->InitializeInstancing(entityManager, globalVariables, "propeller", "enemyPropellerSS01.obj", "",
			false, false, this, Engine::ObjectInstance::TransparencyType::kNo);
		objectComponentPropeller_->SetInstancingSRT({ 1,1,1 }, {}, {});
		objectComponentPropeller_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効化
		objectComponentPropeller_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();

		

		// 武器
		weapon_ = std::make_unique<SmallRangeWeapon>();
		weapon_->SetCharacter(this);
		weapon_->SetBulletManager(bulletManager);
		weapon_->Initialize(inputSystem, entityManager, globalVariables, {}, nullptr);
		weapon_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
		weapon_->GetWorldTransform().translate_ = { 0.0f,-0.5f,0.25f };



		worldCollider_->translate_.y = 0;

		moveComponent_->GetMovementSystem()->SetUseGravity(false);
		moveComponent_->GetMoveSystem()->Data().useGravity = false;
		skyHeight_ = 3.0f;
		moveComponent_->GetMoveSystem()->Data().skyHeight = skyHeight_;

		// トランスフォーム更新
		GetWorldTransform().Update();
	}

	void SmallRangeEnemy::Update() {
		isStopping_ = false;
		// 基盤の更新
		BaseUpdate();

		objectComponent_->GetRigidBodyComponent()->SetIsGravity(false); // 重力無効化


		objectComponentPropeller_->GetWorldTransform().rotate_.y += 20.0f * GetTime();

		objectComponentPropeller_->Update();
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
		stateMachine_->RegisterState(CharacterMainState::Damage, [](BaseCharacter* p) {
			return std::make_unique<SmallRangeEnemyDamageState>(p);
			});
		stateMachine_->Init(this, CharacterMainState::Move);
	}
}

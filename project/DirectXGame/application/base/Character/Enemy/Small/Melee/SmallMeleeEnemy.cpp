#include "SmallMeleeEnemy.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"

namespace Character {
	void SmallMeleeEnemy::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
		// 基盤初期化
		BaseInitialize(inputSystem, entityManager, globalVariables, position, camera, 
			"enemyBodySG01.obj", "SmallMeleeEnemy",0.75f);
		// サイズ
		Vector3 size = { 1.0f,1.0f,1.0f };
		objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定
		objectComponentShadow_->GetWorldTransform().scale_ = { 2.0f,2.0f ,2.0f };
		moveSpeed_ = moveComponent_->GetMoveSystem()->Data().maxSpeed;
		// 武器
		weapon_ = std::make_unique<SmallMeleeWeapon>();
		weapon_->SetCharacter(this);
		weapon_->Initialize(inputSystem, entityManager, globalVariables, {}, nullptr);
		weapon_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
		weapon_->GetWorldTransform().translate_ = { 0.0f,0.0f,1.0f };


		// エフェクト用のトランスフォーム初期化
		worldEffect_ = std::make_unique<Engine::WorldTransform>();
		worldEffect_->Initialize();
		worldEffect_->parent_ = &objectComponent_->GetWorldTransform();
		worldEffect_->translate_ = { 0,1,0 };
		// トランスフォーム更新
		GetWorldTransform().Update();
	}

	void SmallMeleeEnemy::Update() {
		isStopping_ = false;
		// 基盤の更新
		BaseUpdate();
		//
		weapon_->Update(); // 武器更新
	}

	void SmallMeleeEnemy::Draw2D() {}

	void SmallMeleeEnemy::Move() {
		if (isStopping_) return;

		// 攻撃システム更新
		const AttackSlot* slot = enemAi->GetAttackSlotSystem()->FindSlot(this);

		Vector3 slotPos = GetTargetPos();

		if (slot) {
			slotPos = slot->position;
		}

		attackSystem_->Update(
			GetTime(),
			GetWorldPosition(),
			GetTargetPos(),
			slotPos,
			moveComponent_->GetMoveSystem()->Data().maxSpeed,
			GetWorldTransform().rotate_.y,
			globalData_,
			moveSpeed_);
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
		stateMachine_->RegisterState(CharacterMainState::Damage, [](BaseCharacter* p) {
			return std::make_unique<SmallMeleeEnemyDamageState>(p);
			});

		stateMachine_->Init(this, CharacterMainState::Move);
	}
}
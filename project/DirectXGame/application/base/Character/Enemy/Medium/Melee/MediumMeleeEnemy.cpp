#include "MediumMeleeEnemy.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"

namespace Character {
	void MediumMeleeEnemy::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
		// 基盤初期化
		BaseInitialize(inputSystem, entityManager, globalVariables, position, camera,
			"enemy.gltf", "MediumMeleeEnemy", 3.0f);

		objectComponentShadow_->GetWorldTransform().scale_ = { 2.0f,2.0f ,2.0f };


		// サイズ
		Vector3 size = { 1.0f,1.0f,1.0f };
		objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定


		// パラメーター初期化
		moveSpeed_ = moveComponent_->GetMoveSystem()->GetData().maxSpeed;

		// パーティクル初期化
		InitParticle();
		// トランスフォーム更新
		GetWorldTransform().Update();
	}

	void MediumMeleeEnemy::InitStateMachine() {
		// ステートマシーン初期化
		stateMachine_ = std::make_unique<CharacterStateMachine>();
		stateMachine_->RegisterState(CharacterMainState::Move, [](BaseCharacter* p) {
			return std::make_unique<MediumMeleeEnemyMoveState>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Attack, [](BaseCharacter* p) {
			return std::make_unique<MediumMeleeEnemyAttackState>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Die, [](BaseCharacter* p) {
			return std::make_unique<MediumMeleeEnemyDieState>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Damage, [](BaseCharacter* p) {
			return std::make_unique<MediumMeleeEnemyDamageState>(p);
			});
		stateMachine_->Init(this, CharacterMainState::Move);
	}

	void MediumMeleeEnemy::Update() {
		isStopping_ = false;
		// 基盤の更新
		BaseUpdate();
	}

	void MediumMeleeEnemy::Draw2D(){}

	void MediumMeleeEnemy::Move(){
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

	void MediumMeleeEnemy::InitParticle(){
		Engine::ParticleManager* particleManager = entityManager->GetEffectManager()->GetParticleManager();

		// エフェクト用のトランスフォーム初期化
		worldEffect_ = std::make_unique<Engine::WorldTransform>();
		worldEffect_->Initialize();
		worldEffect_->parent_ = &objectComponent_->GetWorldTransform();
		worldEffect_->translate_ = { 0,1,0 };
	}
}

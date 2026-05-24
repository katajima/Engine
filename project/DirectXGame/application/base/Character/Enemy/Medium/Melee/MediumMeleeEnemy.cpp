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
			"enemy.gltf", "MediumMeleeEnemy", 3.0f,2.0f);
		// エフェクト初期化
		InitializeEffect({ 0,1,0 });
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

}

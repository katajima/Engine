#include "SmallMeleeEnemy.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/engine/Entity/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/application/base/Attack/AttackController.h>

namespace Character {
	void SmallMeleeEnemy::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
		// 基盤初期化
		BaseInitialize(inputSystem, entityManager, globalVariables, position, camera, 
			"enemyBodySG01.obj", "SmallMeleeEnemy",0.75f,2.0f);
		// エフェクト初期化
		InitializeEffect({ 0,1,0 });
		// 武器
		weapon_ = std::make_unique<SmallMeleeWeapon>();
		weapon_->SetCharacter(this);
		weapon_->Initialize(inputSystem, entityManager, globalVariables, {}, nullptr);
		weapon_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
		weapon_->GetWorldTransform().translate_ = { 0.0f,0.0f,1.0f };


		//ReloadComboData();

		// トランスフォーム更新
		GetWorldTransform().Update();
	}

	void SmallMeleeEnemy::InitStateMachine() { 
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


	void SmallMeleeEnemy::ReloadComboData() {
		Combo::System* comboSystem = GetAttackController()->GetComboSystem();
		// コンボノードクリア
		comboSystem->ClearNode();
		comboSystem->SetParentTransform("Enemy", &objectComponent_->GetObject3D()->GetWorldTransform());
		// 軌道の依存先候補。軌道使用時は生成時位置だけをアンカーとして利用する。
		comboSystem->SetParentTransform("Trajectory", &objectComponent_->GetObject3D()->GetWorldTransform());
		comboSystem->SetParentTransform("Weapon", &weapon_->GetObject3D()->GetWorldTransform());
		comboSystem->SetParentTransform("NoParent", nullptr);
		comboSystem->Create("ComboSmallMeleeEnemy");
	}

}

#include "MediumMeleeEnemyState.h"
#include "MediumMeleeEnemy.h"

#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include <DirectXGame/application/base/Character/Death/DeathSystem.h>

namespace Character {
#pragma region Move

	void MediumMeleeEnemyMoveState::Update(const CharacterContext& ctx)	{
		// 時間更新
		timer_ += ctx.dt;

		// HPが0以上なら
		if (character->GetHP() > 0) {
			// 移動
			character->Move();
		}
		else {
			// 死亡状態に移行
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);
		}
	}

	void MediumMeleeEnemyMoveState::Exit() {

	}

	void MediumMeleeEnemyMoveState::Enter() {
		timer_ = 0.0f;
		character->GetMoveComponent()->GetMoveSystem()->Data().maxSpeed = 3.0f;
	}
#pragma endregion // 移動

#pragma region Attack


	void MediumMeleeEnemyAttackState::Update(const CharacterContext& ctx)
	{
		// サブステート更新
		subStateMachine_->Update(character->GetTime());

		// サブステートが完了したら Move に戻す
		if (subStateMachine_->IsFinished()) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
			return;
		}

		// HPが0以下なら死亡
		if (character->GetHP() <= 0) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);
		}
	}

	void MediumMeleeEnemyAttackState::Exit() {

	}

	void MediumMeleeEnemyAttackState::Enter() {
		subStateMachine_ = std::make_unique<SubStateMachine<AttackSubState, BaseAttackSubState>>(character);
		// 攻撃用サブステート登録
		subStateMachine_->RegisterState(AttackSubState::Ready, [](BaseCharacter* enemy, auto* fsm) {
			return std::make_unique<MediumMeleeEnemyAttackReadySubState>(enemy, fsm);
			});
		subStateMachine_->RegisterState(AttackSubState::Swing, [](BaseCharacter* enemy, auto* fsm) {
			return std::make_unique<MediumMeleeEnemyAttackSwingSubState>(enemy, fsm);
			});
		subStateMachine_->RegisterState(AttackSubState::End, [](BaseCharacter* enemy, auto* fsm) {
			return std::make_unique<MediumMeleeEnemyAttackEndSubState>(enemy, fsm);
			});
		subStateMachine_->ChangeState(AttackSubState::Ready);
	}


#pragma endregion // 攻撃

#pragma region Die

	void MediumMeleeEnemyDieState::Update(const CharacterContext& ctx) {
		character->GetObjectComponent()->GetWorldTransform().scale_ -= Vector3(1.1f, 1.1f, 1.1f) * character->GetTime(); // サイズを縮小
		if (character->GetObjectComponent()->GetWorldTransform().scale_.x <= 0) {
			character->GetObjectComponent()->GetWorldTransform().scale_ = Vector3{ 0,0,0 };	// 0に
			character->GetObjectComponentShadow()->GetWorldTransform().scale_ = {};
		}
	}

	void MediumMeleeEnemyDieState::Exit() {

	}

	void MediumMeleeEnemyDieState::Enter(){
		character->GetDeathSystem()->StartDeath(DeathType::Explode, { dieTimer_ ,false,1.0f,{} });
		character->GetSpecalPointManager()->AddPoint(character->GetWorldTransform().GetWorldPosition() + Vector3{ 0,4.0f,0 }, 1);
	}

#pragma endregion 

#pragma region Damage

	void MediumMeleeEnemyDamageState::Update(const CharacterContext& ctx){
		if (character->GetHitMotionSystem()->IsFinished()) {
			// HPが0以下なら死亡
			if (character->GetHP() <= 0) {
				character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);
				return;
			}

			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		}
	}

#pragma endregion
}

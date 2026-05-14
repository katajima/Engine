#include "SmallMeleeEnemyState.h"
#include "SmallMeleeEnemy.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include <DirectXGame/application/base/Character/Death/DeathSystem.h>

namespace Character {
#pragma region Move

	void SmallMeleeEnemyMoveState::Update(const CharacterContext& ctx){
		BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character);

		// 時間更新
		timer_ += ctx.dt;

		// HPが0以上なら
		if (character->GetHP() > 0) {
			// 移動
			character->Move();
			return;
		}
		else {
			// 死亡状態に移行
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);

		}
	}

	void SmallMeleeEnemyMoveState::Exit(){}

	void SmallMeleeEnemyMoveState::Enter(){
		timer_ = 0.0f;
		character->GetMoveComponent()->GetMoveSystem()->Data().maxSpeed = 3.0f;
	}

#pragma endregion

#pragma region Attack

	void SmallMeleeEnemyAttackState::Update(const CharacterContext& ctx) {
		// サブステート更新
		subStateMachine_->Update();

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

	void SmallMeleeEnemyAttackState::Exit() {
		timer_ = 0.0f;
	}

	void SmallMeleeEnemyAttackState::Enter() {
		subStateMachine_ = std::make_unique<SubStateMachine<AttackSubState, BaseAttackSubState>>(character);
		// 攻撃用サブステート登録
		subStateMachine_->RegisterState(AttackSubState::Ready, [](BaseCharacter* enemy, auto* fsm) {
			return std::make_unique<SmallMeleeAttackReadySubState>(enemy, fsm);
			});
		subStateMachine_->RegisterState(AttackSubState::Swing, [](BaseCharacter* enemy, auto* fsm) {
			return std::make_unique<SmallMeleeAttackSwingSubState>(enemy, fsm);
			});
		subStateMachine_->RegisterState(AttackSubState::End, [](BaseCharacter* enemy, auto* fsm) {
			return std::make_unique<SmallMeleeAttackEndSubState>(enemy, fsm);
			});
		subStateMachine_->ChangeState(AttackSubState::Ready);
		timer_ = 0.0f;
	}

#pragma endregion

#pragma region Die

	void SmallMeleeEnemyDieState::Update(const CharacterContext& ctx){
		character->GetObjectComponent()->GetWorldTransform().scale_ -= Vector3(1.1f, 1.1f, 1.1f) * character->GetTime(); // サイズを縮小
		if (character->GetObjectComponent()->GetWorldTransform().scale_.x <= 0) {
			character->GetObjectComponent()->GetWorldTransform().scale_ = Vector3{ 0,0,0 };	// 0に
			character->GetObjectComponentShadow()->GetWorldTransform().scale_ = {};
		}
	}

	void SmallMeleeEnemyDieState::Exit(){}

	void SmallMeleeEnemyDieState::Enter(){
		character->GetDeathSystem()->StartDeath(DeathType::Explode, { dieTimer_ ,false,1.0f,{} });
		character->GetSpecalPointManager()->AddPoint(
			character->GetWorldTransform().GetWorldPosition() + Vector3{ 0,4.0f,0 }, 1);
	}

#pragma endregion
	
#pragma region Damage
	
	void SmallMeleeEnemyDamageState::Update(const CharacterContext& ctx) {
		if (character->GetHitMotionSystem()->IsFinished()) {
			// HPが0以下なら死亡
			if (character->GetHP() <= 0) {
				character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);
				return;
			}

			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		}
		else {
			if (character->GetHitMotionSystem()->IsLaunch()) {

				character->GetWorldTransform().rotate_.x += 10.0f * character->GetTime(); // 回転
				//character->GetWorldTransform().rotate_.y += 10.0f * character->GetTime(); // 回転
				character->GetWorldTransform().rotate_.z += 10.0f * character->GetTime(); // 回転


			}
		}
	}

#pragma endregion

}

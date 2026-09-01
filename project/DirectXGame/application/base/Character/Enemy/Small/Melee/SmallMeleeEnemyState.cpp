#include "SmallMeleeEnemyState.h"
#include "SmallMeleeEnemy.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/engine/Entity/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include <DirectXGame/application/base/Character/Death/DeathSystem.h>
#include <DirectXGame/application/base/Effect/Effect.h>
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
		
	}

	void SmallMeleeEnemyDieState::Exit(){}

	void SmallMeleeEnemyDieState::Enter(){
		character->GetDeathSystem()->StartDeath(DeathType::Explode, { dieTimer_ ,false,1.0f,{} });
		// 必殺技で倒された場合は、BaseEnemy側でSPポイント生成を抑制する。
		static_cast<BaseEnemy*>(character)->DropSpecialPointOnDeath(
			character->GetWorldTransform().GetWorldPosition() + Vector3{ 0,4.0f,0 }, 1);
		character->GetObjectComponent()->GetWorldTransform().scale_ = Vector3{ 0,0,0 };	// 0に
		character->GetEffect()->Emit("EmitterDeathEnemyScrapScrew", character->GetWorldTransform().GetWorldPosition());
		character->GetEffect()->Emit("EmitterDeathEnemyScrapIronRod", character->GetWorldTransform().GetWorldPosition());
		character->GetEffect()->Emit("EmitterDeathEnemyScrapGear", character->GetWorldTransform().GetWorldPosition());
		character->GetEffect()->Emit("EmitterDeathEnemySmoke", character->GetWorldTransform().GetWorldPosition());
		character->GetEffect()->Emit("EmitterDeathEnemyExp", character->GetWorldTransform().GetWorldPosition());
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
				character->GetWorldTransform().rotate_.z += 10.0f * character->GetTime(); // 回転


			}
		}
	}

	void SmallMeleeEnemyDamageState::Exit(){
		character->GetWorldTransform().rotate_.x = 0.0f;
		character->GetWorldTransform().rotate_.z = 0.0f;
	}

#pragma endregion

}

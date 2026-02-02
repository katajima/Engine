#include "SmallMeleeEnemyState.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "SmallMeleeEnemy.h"
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>

#pragma region Move

void SmallMeleeEnemyMoveState::Update()
{
	BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);

	// 時間更新
	timer_ += character_->GetTime();

	// HPが0以上なら
	if (character_->GetHP() > 0) {
		// 移動
		character_->Move();
		return;
	}
	else {
		// 死亡状態に移行
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);

	}
}

void SmallMeleeEnemyMoveState::Exit()
{
}

void SmallMeleeEnemyMoveState::Enter()
{
	timer_ = 0.0f;
	character_->GetMoveComponent()->GetMoveSystem()->GetData().maxSpeed = 3.0f;
}

#pragma endregion

#pragma region Attack

void SmallMeleeEnemyAttackState::Update(){
	// サブステート更新
	subStateMachine_->Update();

	// サブステートが完了したら Move に戻す
	if (subStateMachine_->IsFinished()) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		return;
	}

	// HPが0以下なら死亡
	if (character_->GetHP() <= 0) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);
	}
}

void SmallMeleeEnemyAttackState::Exit(){
	timer_ = 0.0f;
}

void SmallMeleeEnemyAttackState::Enter(){
	subStateMachine_ = std::make_unique<SubStateMachine<AttackSubState, BaseAttackSubState>>(character_);
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

void SmallMeleeEnemyDieState::Update()
{
	// 時間更新
	timer_ -= character_->GetTime();
	if (timer_ <= 0.0f) {
		// 死亡判定に
		character_->SetAlive(false);
		timer_ = 0.0f;
		if (!character_->GetAlive()) {
			character_->Delete();	// キャラクター削除
			character_->GetObjectComponent()->IsDelete();	// オブジェクトコンポーネント削除
		}
	}
	else if (timer_ <= dieTimer_ / 2.0f) {
		character_->GetObjectComponent()->SetIsDraw(false);	// 描画しない	
	}
	else {
		character_->GetObjectComponent()->GetRigidBodyComponent()->SetIsGravity(false);		// 重力無し
		character_->GetObjectComponent()->GetWorldTransform().scale_ -= Vector3(1.1f, 1.1f, 1.1f) * character_->GetTime(); // サイズを縮小
		if (character_->GetObjectComponent()->GetWorldTransform().scale_.x <= 0) {
			character_->GetObjectComponent()->GetWorldTransform().scale_ = Vector3{ 0,0,0 };	// 0に
		}
	}
}

void SmallMeleeEnemyDieState::Exit()
{
}

void SmallMeleeEnemyDieState::Enter()
{
	timer_ = dieTimer_;
	character_->GetSpecalPointManager()->AddPoint(character_->GetWorldTransform().GetWorldPosition() + Vector3{ 0,4.0f,0 }, 1);
}

#pragma endregion

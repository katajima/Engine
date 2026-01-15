#include "NormalEnemyState.h"
#include "NormalEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>

#pragma region Move

void EnemyStateMove::Update()
{
	// 時間更新
	timer_ += character_->GetTime();

	// HPが0以上なら
	if (character_->GetHP() > 0) {
		// 移動
		character_->Move();
	}
	else {
		// 死亡状態に移行
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);
	}
}

void EnemyStateMove::Exit() {

}

void EnemyStateMove::Enter() {
	timer_ = 0.0f;
	character_->GetMoveComponent()->GetMoveSystem()->GetData().maxSpeed = 3.0f;
}
#pragma endregion // 移動

#pragma region Attack


void EnemyStateAttack::Update()
{
	// サブステート更新
	subStateMachine_->Update(character_->GetTime());

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

void EnemyStateAttack::Exit() {

}

void EnemyStateAttack::Enter() {
	subStateMachine_ = std::make_unique<SubStateMachine<AttackSubState, BaseAttackSubState>>(character_);
	// 攻撃用サブステート登録
	subStateMachine_->RegisterState(AttackSubState::Ready, [](BaseCharacter* enemy, auto* fsm) {
		return std::make_unique<NormalEnemyAttackReadySubState>(enemy, fsm);
		});
	subStateMachine_->RegisterState(AttackSubState::Swing, [](BaseCharacter* enemy, auto* fsm) {
		return std::make_unique<NormalEnemyAttackSwingSubState>(enemy, fsm);
		});
	subStateMachine_->RegisterState(AttackSubState::End, [](BaseCharacter* enemy, auto* fsm) {
		return std::make_unique<NormalEnemyAttackEndSubState>(enemy, fsm);
		});
	subStateMachine_->ChangeState(AttackSubState::Ready);
}


#pragma endregion // 攻撃

#pragma region Die

void EnemyStateDie::Update() {

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

void EnemyStateDie::Exit() {

}

void EnemyStateDie::Enter()
{
	timer_ = dieTimer_;
	character_->GetSpecalPointManager()->AddPoint(character_->GetWorldTransform().GetWorldPosition() + Vector3{0,4.0f,0}, 1);
}

#pragma endregion // 死亡

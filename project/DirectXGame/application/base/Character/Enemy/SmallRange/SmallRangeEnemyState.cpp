#include "SmallRangeEnemyState.h"
#include "SmallRangeEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include "DirectXGame/application/base/Weapon/Enemy/SmallRangeWeapon.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>


void SmallRangeEnemyMoveState::Update()
{
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

void SmallRangeEnemyMoveState::Exit()
{
}

void SmallRangeEnemyMoveState::Enter()
{
}


void SmallRangeEnemyAttackState::Update()
{
	SmallRangeWeapon* weapon = static_cast<SmallRangeWeapon*>(character_->GetWeapon());

	BaseEnemy* enemy = static_cast<BaseEnemy*>(character_);


	weapon->SetCharacter(character_);
	weapon->SetParent(enemy);
	weapon->SetTarget(enemy->GetTarget());
	weapon->Shot();

	character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
}

void SmallRangeEnemyAttackState::Exit()
{
}

void SmallRangeEnemyAttackState::Enter()
{
}


void SmallRangeEnemyDieState::Update()
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

void SmallRangeEnemyDieState::Exit()
{
}

void SmallRangeEnemyDieState::Enter()
{
	timer_ = dieTimer_;
	character_->GetSpecalPointManager()->AddPoint(character_->GetWorldTransform().GetWorldPosition() + Vector3{ 0,4.0f,0 }, 1);
}

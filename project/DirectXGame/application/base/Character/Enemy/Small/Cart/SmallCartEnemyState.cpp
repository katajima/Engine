#include "SmallCartEnemyState.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "SmallCartEnemy.h"
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>


void SmallCartEnemyMoveState::Update()
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

void SmallCartEnemyMoveState::Exit()
{
}

void SmallCartEnemyMoveState::Enter()
{
	timer_ = 0.0f;
	character_->GetMoveComponent()->GetMoveSystem()->GetData().maxSpeed = 3.0f;
}

void SmallCartEnemyDieState::Update()
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

void SmallCartEnemyDieState::Exit()
{
}

void SmallCartEnemyDieState::Enter()
{
	timer_ = dieTimer_;
	character_->GetSpecalPointManager()->AddPoint(character_->GetWorldTransform().GetWorldPosition() + Vector3{ 0,4.0f,0 }, 1);
	character_->GetSpecalPointManager()->AddPoint(character_->GetWorldTransform().GetWorldPosition() + Vector3{ 4,4.0f,0 }, 1);
	character_->GetSpecalPointManager()->AddPoint(character_->GetWorldTransform().GetWorldPosition() + Vector3{ -4,4.0f,0 }, 1);
	character_->GetSpecalPointManager()->AddPoint(character_->GetWorldTransform().GetWorldPosition() + Vector3{ 0,4.0f,4 }, 1);
	character_->GetSpecalPointManager()->AddPoint(character_->GetWorldTransform().GetWorldPosition() + Vector3{ 0,4.0f,-4 }, 1);
}

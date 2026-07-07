#include "SmallCartEnemyState.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "SmallCartEnemy.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>

namespace Character {
	void SmallCartEnemyMoveState::Update(const CharacterContext& ctx)
	{
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

	void SmallCartEnemyMoveState::Exit()
	{
	}

	void SmallCartEnemyMoveState::Enter()
	{
		timer_ = 0.0f;
		character->GetMoveComponent()->GetMoveSystem()->Data().maxSpeed = 3.0f;
	}

	void SmallCartEnemyDieState::Update(const CharacterContext& ctx)
	{
		// 時間更新
		timer_ -= ctx.dt;
		if (timer_ <= 0.0f) {
			// 死亡判定に
			character->SetAlive(false);
			timer_ = 0.0f;
			if (!character->GetAlive()) {
				character->Delete();	// キャラクター削除
				character->GetObjectComponent()->IsDelete();	// オブジェクトコンポーネント削除
			}
		}
		else if (timer_ <= dieTimer_ / 2.0f) {
			character->GetObjectComponent()->SetIsDraw(false);	// 描画しない	
		}
		else {
			character->GetObjectComponent()->GetRigidBodyComponent()->SetIsGravity(false);		// 重力無し
			character->GetObjectComponent()->GetWorldTransform().scale_ -= Vector3(1.1f, 1.1f, 1.1f) * character->GetTime(); // サイズを縮小
			if (character->GetObjectComponent()->GetWorldTransform().scale_.x <= 0) {
				character->GetObjectComponent()->GetWorldTransform().scale_ = Vector3{ 0,0,0 };	// 0に
			}
		}
	}

	void SmallCartEnemyDieState::Exit()
	{
	}

	void SmallCartEnemyDieState::Enter()
	{
		timer_ = dieTimer_;
		character->GetSpecialPointManager()->AddPoint(character->GetWorldTransform().GetWorldPosition() + Vector3{ 0,4.0f,0 }, 1);
		character->GetSpecialPointManager()->AddPoint(character->GetWorldTransform().GetWorldPosition() + Vector3{ 4,4.0f,0 }, 1);
		character->GetSpecialPointManager()->AddPoint(character->GetWorldTransform().GetWorldPosition() + Vector3{ -4,4.0f,0 }, 1);
		character->GetSpecialPointManager()->AddPoint(character->GetWorldTransform().GetWorldPosition() + Vector3{ 0,4.0f,4 }, 1);
		character->GetSpecialPointManager()->AddPoint(character->GetWorldTransform().GetWorldPosition() + Vector3{ 0,4.0f,-4 }, 1);
	}
}
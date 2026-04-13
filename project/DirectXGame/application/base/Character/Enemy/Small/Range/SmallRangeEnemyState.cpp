#include "SmallRangeEnemyState.h"
#include "SmallRangeEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include "DirectXGame/application/base/Weapon/Enemy/SmallRangeWeapon.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>

namespace Character {

#pragma region Move

	void SmallRangeEnemyMoveState::Update(const CharacterContext& ctx){
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

	void SmallRangeEnemyMoveState::Exit(){}

	void SmallRangeEnemyMoveState::Enter(){}

#pragma endregion

#pragma region Attack

	void SmallRangeEnemyAttackState::Update(const CharacterContext& ctx){
		SmallRangeWeapon* weapon = static_cast<SmallRangeWeapon*>(character->GetWeapon());

		BaseEnemy* enemy = static_cast<BaseEnemy*>(character);


		weapon->SetCharacter(character);
		weapon->SetParent(enemy);
		weapon->SetTarget(enemy->GetTarget());
		weapon->Shot();

		character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
	}

	void SmallRangeEnemyAttackState::Exit(){}

	void SmallRangeEnemyAttackState::Enter(){}

#pragma endregion

#pragma region Die

	void SmallRangeEnemyDieState::Update(const CharacterContext& ctx){
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

	void SmallRangeEnemyDieState::Exit(){}

	void SmallRangeEnemyDieState::Enter(){
		timer_ = dieTimer_;
		character->GetSpecalPointManager()->AddPoint(character->GetWorldTransform().GetWorldPosition() + Vector3{ 0,4.0f,0 }, 1);
	}

#pragma endregion

	void SmallRangeEnemyDamageState::Update(const CharacterContext& ctx){
		// 時間更新
		timer_ -= ctx.dt;
		if (timer_ <= 0.0f) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		}
	}

	void SmallRangeEnemyDamageState::Exit(){
		timer_ = damageTime_;
	}

	void SmallRangeEnemyDamageState::Enter(){
		timer_ = damageTime_;
	}

}
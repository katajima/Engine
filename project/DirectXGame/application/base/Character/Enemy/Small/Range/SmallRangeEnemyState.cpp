#include "SmallRangeEnemyState.h"
#include "SmallRangeEnemy.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include <DirectXGame/application/base/Character/Death/DeathSystem.h>

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

		if (!weapon || !enemy) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
			return;
		}

		timer_ += ctx.dt;

		weapon->SetCharacter(character);
		weapon->SetParent(enemy);
		weapon->SetTarget(enemy->GetTarget());

		// プレイヤー方向を向く
		Vector3 toTarget = Subtract(enemy->GetTargetPos(), enemy->GetWorldTransform().translate_);
		toTarget.y = 0.0f;

		if (toTarget.Length() > 0.001f) {
			toTarget = toTarget.Normalize();

			float targetRotY = std::atan2(toTarget.x, toTarget.z);
			float& currentRotY = enemy->GetWorldTransform().rotate_.y;

			float diff = targetRotY - currentRotY;
			diff = std::atan2(std::sin(diff), std::cos(diff));

			float maxTurn = turnSpeed_ * ctx.dt;
			diff = std::clamp(diff, -maxTurn, maxTurn);

			currentRotY += diff;
		}

		// 狙い時間が経過したら1回だけ撃つ
		if (!hasShot_ && timer_ >= shotTime_) {
			weapon->Shot();
			hasShot_ = true;
		}

		// 発射後の硬直が終わったら移動へ戻る
		if (timer_ >= endTime_) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		}
	}

	void SmallRangeEnemyAttackState::Exit(){}

	void SmallRangeEnemyAttackState::Enter(){
		timer_ = 0.0f;
		hasShot_ = false;
	}

#pragma endregion

#pragma region Die

	void SmallRangeEnemyDieState::Update(const CharacterContext& ctx){
		character->GetObjectComponent()->GetWorldTransform().scale_ -= Vector3(1.1f, 1.1f, 1.1f) * character->GetTime(); // サイズを縮小
		if (character->GetObjectComponent()->GetWorldTransform().scale_.x <= 0) {
			character->GetObjectComponent()->GetWorldTransform().scale_ = Vector3{ 0,0,0 };	// 0に
			character->GetObjectComponentShadow()->GetWorldTransform().scale_ = {};
		}
	}

	void SmallRangeEnemyDieState::Exit(){}

	void SmallRangeEnemyDieState::Enter(){
		character->GetDeathSystem()->StartDeath(DeathType::Explode, { dieTimer_ ,false,1.0f,{} });
		character->GetSpecalPointManager()->AddPoint(character->GetWorldTransform().GetWorldPosition() + Vector3{ 0,4.0f,0 }, 1);
	}

#pragma endregion

	void SmallRangeEnemyDamageState::Update(const CharacterContext& ctx){
		if (character->GetHitMotionSystem()->IsFinished()) {
			// HPが0以下なら死亡
			if (character->GetHP() <= 0) {
				character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);
				return;
			}

			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		}
	}
}
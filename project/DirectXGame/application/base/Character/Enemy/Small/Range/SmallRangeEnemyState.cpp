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
		if (character->GetHP() <= 0) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);
			return;
		}
		if (character->GetHitMotionSystem()->IsFinished()) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		}
	}
}
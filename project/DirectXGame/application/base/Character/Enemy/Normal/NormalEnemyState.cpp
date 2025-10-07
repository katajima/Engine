#include "NormalEnemyState.h"
#include "NormalEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

void EnemyStateMove::Update()
{

	if (character_->GetHP() > 0) {

		character_->Move();


		timer_ -= character_->GetTime();

		if (timer_ <= 0.0f) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Attack);
			return;
		}
	}
	else {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);

	}
}

void EnemyStateMove::Exit() {

}

void EnemyStateMove::Enter() {
	timer_ = rootTimer_;
}


void EnemyStateAttack::Update()
{
	timer_ -= character_->GetTime();
	/*Vector3 direct = subPos_.Normalize() * character_->GetTime() * attackSpeed_;
	direct.y = 0;
	character_->GetWorldTransform().translate_ = Add(character_->GetWorldTransform().translate_, direct);*/

	character_->Move();

	if (timer_ <= 0.0f) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Attack);
		timer_ = 0.0f;
		return;
	}
	if (character_->GetHP() <= 0) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);
	}
}

void EnemyStateAttack::Exit() {

}

void EnemyStateAttack::Enter() {
	BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);


	// ロックオン座標
	lockonPos_ = enemy->GetTargetPos();

	// 追跡対象からロックオン対象へのベクトル
	subPos_ = Subtract(lockonPos_, enemy->GetWorldTransform().translate_);
	timer_ = attackTimer_;
}




void EnemyStateSpecial::Update() {

}

void EnemyStateSpecial::Exit() {

}

void EnemyStateSpecial::Enter() {

}



void EnemyStateDie::Update() {
	timer_ -= character_->GetTime();
	if (timer_ <= 0.0f) {
		character_->SetAlive(false);
		timer_ = 0.0f;
		if (!character_->GetAlive()) {
			character_->Delete();
			character_->GetObject3D()->IsDelete();
		}
	}
	else if (timer_ <= dieTimer_ / 2.0f) {
		character_->GetObject3D()->SetIsDraw(false);
	}
	else {
		character_->GetObject3D()->GetRigidBodyComponent()->SetIsGravity(false);
		character_->GetObject3D()->GetWorldTransform().scale_ -= Vector3(1.1f, 1.1f, 1.1f) * character_->GetTime();
		if (character_->GetObject3D()->GetWorldTransform().scale_.x <= 0) {
			character_->GetObject3D()->GetWorldTransform().scale_ = Vector3{ 0,0,0 };
		}

		// 着地処理
		character_->GetMoveComponent()->Landing(*character_->GetObject3D()->GetTransformComponent(), *character_->GetObject3D()->GetRigidBodyComponent());
	}
}

void EnemyStateDie::Exit() {

}

void EnemyStateDie::Enter()
{
	timer_ = dieTimer_;
}
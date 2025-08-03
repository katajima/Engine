#include "NormalEnemyState.h"
#include "NormalEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

EnemyStateMove::EnemyStateMove(BaseEnemy* enemy)
	: BaseEnemyState("Move", enemy) {
}

void EnemyStateMove::Update()
{

	if (enemy_->GetHP() > 0) {

		enemy_->Move();


		timer_ -= enemy_->GetTime();

		if (timer_ <= 0.0f) {
			enemy_->ChangeState("Attack");
			return;
		}
	}
	else {
		enemy_->ChangeState("Die");
	}
}

void EnemyStateMove::Exit() {

}

void EnemyStateMove::Enter() {
	timer_ = rootTimer_;
}

EnemyStateAttack::EnemyStateAttack(BaseEnemy* enemy)
	: BaseEnemyState("Attack", enemy) {
}

void EnemyStateAttack::Update()
{
	timer_ -= enemy_->GetTime();
	Vector3 direct = subPos_.Normalize() * enemy_->GetTime() * attackSpeed_;
	direct.y = 0;
	enemy_->GetWorldTransform().translate_ = Add(enemy_->GetWorldTransform().translate_, direct);

	if (timer_ <= 0.0f) {
		enemy_->ChangeState("Move");
		timer_ = 0.0f;
		return;
	}
	if (enemy_->GetHP() <= 0) {
		enemy_->ChangeState("Die");
	}
}

void EnemyStateAttack::Exit() {

}

void EnemyStateAttack::Enter() {
	// ロックオン座標
	lockonPos_ = enemy_->GetTargetPos();

	// 追跡対象からロックオン対象へのベクトル
	subPos_ = Subtract(lockonPos_, enemy_->GetWorldTransform().translate_);
	timer_ = attackTimer_;
}


EnemyStateSpecial::EnemyStateSpecial(BaseEnemy* enemy)
	: BaseEnemyState("Special", enemy) {
}

void EnemyStateSpecial::Update() {

}

void EnemyStateSpecial::Exit() {

}

void EnemyStateSpecial::Enter() {

}

EnemyStateDie::EnemyStateDie(BaseEnemy* enemy)
	: BaseEnemyState("Die", enemy) {
}

void EnemyStateDie::Update() {
	timer_ -= enemy_->GetTime();
	if (timer_ <= 0.0f) {
		enemy_->SetAlive(false);
		timer_ = 0.0f;
		if (!enemy_->GetAlive()) {
			enemy_->Delete();
			enemy_->GetObject3D()->IsDelete();
		}
	}
	else if (timer_ <= dieTimer_ / 2.0f) {
		enemy_->GetObject3D()->SetIsDraw(false);
	}
	else {
		enemy_->GetObject3D()->GetRigidBodyComponent()->SetIsGravity(false);
		enemy_->GetObject3D()->GetWorldTransform().scale_ -= Vector3(1.1f, 1.1f, 1.1f) * enemy_->GetTime();
		if (enemy_->GetObject3D()->GetWorldTransform().scale_.x <= 0) {
			enemy_->GetObject3D()->GetWorldTransform().scale_ = Vector3{ 0,0,0 };
		}

		// 着地処理
		enemy_->GetMoveComponent()->Landing(*enemy_->GetObject3D()->GetTransformComponent(), *enemy_->GetObject3D()->GetRigidBodyComponent());
	}
}

void EnemyStateDie::Exit() {

}

void EnemyStateDie::Enter()
{
	timer_ = dieTimer_;
}
#include "NormalEnemyState.h"
#include "NormalEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"


#pragma region Move

void EnemyStateMove::Update()
{
	BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);
	if (character_->GetHP() > 0) {

		character_->Move();


		timer_ += character_->GetTime();

		if (timer_ >= rootTimer_ && enemy->GetTargetDistance() <= 50) {
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
	timer_ = 0.0f;
}
#pragma endregion // 移動

#pragma region Attack


void EnemyStateAttack::Update()
{
	subStateMachine_->Update(character_->GetTime());

	// サブステートが完了したら Move に戻す
	if (subStateMachine_->IsFinished()) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		return;
	}

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


#pragma endregion

#pragma region Special

void EnemyStateSpecial::Update() {

}

void EnemyStateSpecial::Exit() {

}

void EnemyStateSpecial::Enter() {

}

#pragma endregion

#pragma region Die

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

#pragma endregion

#pragma region Fainting

// 更新
void EenmyStateFainting::Update() {
	timer_ += character_->GetTime();

	if (timer_ >= faintingTimer_) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		return;
	}
};

// 終了
void EenmyStateFainting::Exit() {

};
// 初期化
void EenmyStateFainting::Enter() {
	timer_ = 0.0f;
	character_->Velocity() = {0,0,0};
};

#pragma endregion


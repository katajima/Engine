#include "ComboState.h"
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"


// 開始
void ComboNodeState::Enter(BaseCharacter* owner) {

	AnimationComponent* anima = owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	anima->SetAnimetion(animation, 0.0f);
	owner->GetWeapon()->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, true);
	timeInState = 0.0f;
	anima->SetStratAnimeTime();
	anima->SetIsLoop(false);
	anima->SetAnimationSpeed(1.0f);
	owner->GetWeapon()->SetComboData(comboData_);
	owner->GetWeapon()->GetObject3D()->isEmitTrailEffect = true;
	owner->GetMoveComponent()->Move(owner->GetObjectComponent()->GetWorldTransform(), owner->GetInput());
	if (anima->GetEndAnimeTime(animation) == 0.0f) {
		inputWindowEnd = 1.0f;
	}
	else {
		inputWindowEnd = anima->GetEndAnimeTime(animation);
	}
}

// 更新
void ComboNodeState::Update(BaseCharacter* owner, float dt)
{
	AnimationComponent* anima = owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent();

	timeInState += dt;
	
	auto weapon = owner->GetWeapon();

	// アニメが終了 or 入力受付時間が終わって、次のステートが無ければ終了
	bool isInputWindowOver = timeInState > inputWindowEnd;
	bool isMove = timeInState < comboData_.moveTime;
	bool hasNext = HasNextState();

	// 移動できるなら
	if (isMove) {
		owner->Velocity() = owner->GetMoveComponent()->GetDirection() * comboData_.movementSpeedMultiplier;
		owner->GetMoveComponent()->Move(owner->GetObjectComponent()->GetWorldTransform(), owner->GetInput());
	}
	// 入力受付がないのなら終了する
	if ((isInputWindowOver)) {

		timeInState = 0.0f;
		// コンボ終了 → 通常ステートに戻す
		owner->GetWeapon()->GetObject3D()->isEmitTrailEffect = false;
		owner->GetCharacterStateMachine()->ChangeState(CharacterMainState::Idle);  // ← BaseCharacterが持っている関数
		anima->SetIsLoop(true);
		anima->SetIsPlaying(true);
		anima->SetAnimationSpeed(1.0f);
		anima->SetStratAnimeTime();
	}

}

// 終了
void ComboNodeState::Exit(BaseCharacter* owner)
{
	AnimationComponent* anima = owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent();

	timeInState = 0.0f;
	// アニメ終了時の処理など
	owner->GetWeapon()->GetColliderComponent()->contactRecord_.Clear();
	owner->GetWeapon()->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, false);
	owner->GetWeapon()->GetObject3D()->isEmitTrailEffect = false;

	anima->SetStratAnimeTime();
	anima->SetIsLoop(true);
	anima->SetIsPlaying(true);
	anima->SetAnimationSpeed(1.0f);
}

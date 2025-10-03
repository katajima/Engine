#include "ComboState.h"
#include "DirectXGame/application/base/BaseClass/Weapon/BaseWeapon.h"



void ComboNodeState::Enter(BaseCharacter* owner) {

	AnimationComponent* anima = owner->GetObject3D()->GetAnimationComponent();
	anima->SetAnimetion(animation, 0.0f);
	owner->GetWeapon()->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, true);
	timeInState = 0.0f;
	anima->SetStratAnimeTime();
	anima->SetIsLoop(false);
	anima->SetAnimationSpeed(1.0f);
	owner->GetWeapon()->SetComboData(comboData_);
	owner->GetMoveComponent()->Move(*owner->GetObject3D()->GetTransformComponent(), owner->GetInput());
	if (anima->GetEndAnimeTime(animation) == 0.0f) {
		inputWindowEnd = 1.0f;
	}
	else {
		inputWindowEnd = anima->GetEndAnimeTime(animation);
	}
}

void ComboNodeState::Update(BaseCharacter* owner, float dt)
{
	AnimationComponent* anima = owner->GetObject3D()->GetAnimationComponent();

	timeInState += dt;
	
	auto weapon = owner->GetWeapon();

	// アニメが終了 or 入力受付時間が終わって、次のステートが無ければ終了
	//bool isAnimEnd = weapon->GetObject3D()->IsAnimationFinished();
	bool isInputWindowOver = timeInState > inputWindowEnd;
	bool isMove = timeInState < comboData_.moveTime;
	bool hasNext = HasNextState();

	//weapon->GetObject3D()->SetIsEmitTrailEffect(true);

	if (isMove) {
		owner->Velocity() = owner->GetMoveComponent()->GetDirection() * comboData_.movementSpeedMultiplier;
	}

	if ((/*isAnimEnd || */isInputWindowOver)/* && !hasNext*/) {

		timeInState = 0.0f;
		// コンボ終了 → 通常ステートに戻す
		owner->ChangeState("Move");  // ← BaseCharacterが持っている関数
		anima->SetIsLoop(true);
		anima->SetIsPlaying(true);
		anima->SetAnimationSpeed(1.0f);
		anima->SetStratAnimeTime();
		//weapon->GetObject3D()->SetIsEmitTrailEffect(false);
	}

}

void ComboNodeState::Exit(BaseCharacter* owner)
{
	AnimationComponent* anima = owner->GetObject3D()->GetAnimationComponent();

	timeInState = 0.0f;
	// アニメ終了時の処理など
	owner->GetWeapon()->ColliderHistoryClear();
	owner->GetWeapon()->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, false);

	anima->SetStratAnimeTime();
	anima->SetIsLoop(true);
	anima->SetIsPlaying(true);
	anima->SetAnimationSpeed(1.0f);
}

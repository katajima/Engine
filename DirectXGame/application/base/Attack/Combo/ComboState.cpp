#include "ComboState.h"
#include "DirectXGame/application/base/BaseClass/Weapon/BaseWeapon.h"



void ComboNodeState::Enter(BaseCharacter* owner) {
	owner->GetObject3D()->SetAnimetion(animation, 0.0f);
	//owner->GetObject3D()->SetAnimetion("Attack1", 0.0f);
	owner->GetWeapon()->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, true);
	timeInState = 0.0f;
	owner->GetObject3D()->SetStratAnimeTime();
	owner->GetObject3D()->SetIsLoop(false);
	owner->GetObject3D()->SetAnimationSpeed(1.0f);
	owner->GetWeapon()->SetComboData(comboData_);
	if (owner->GetObject3D()->GetEndAnimeTime(animation) == 0.0f) {
		inputWindowEnd = 1.0f;
	}
	else {
		inputWindowEnd = owner->GetObject3D()->GetEndAnimeTime(animation);
	}
}

void ComboNodeState::Update(BaseCharacter* owner, float dt)
{
	timeInState += dt;
	
	auto weapon = owner->GetWeapon();

	// アニメが終了 or 入力受付時間が終わって、次のステートが無ければ終了
	//bool isAnimEnd = weapon->GetObject3D()->IsAnimationFinished();
	bool isInputWindowOver = timeInState > inputWindowEnd;
	bool hasNext = HasNextState();

	if ((/*isAnimEnd || */isInputWindowOver)/* && !hasNext*/) {


		// コンボ終了 → 通常ステートに戻す
		owner->ChangeState("Move");  // ← BaseCharacterが持っている関数
		owner->GetObject3D()->SetIsLoop(true);
		owner->GetObject3D()->SetIsPlaying(true);
		owner->GetObject3D()->SetAnimationSpeed(1.0f);
		owner->GetObject3D()->SetStratAnimeTime();
	}

}

void ComboNodeState::Exit(BaseCharacter* owner)
{
	timeInState = 0.0f;
	// アニメ終了時の処理など
	owner->GetWeapon()->ColliderHistoryClear();
	owner->GetWeapon()->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, false);

	owner->GetObject3D()->SetStratAnimeTime();
	owner->GetObject3D()->SetIsLoop(true);
	owner->GetObject3D()->SetIsPlaying(true);
	owner->GetObject3D()->SetAnimationSpeed(1.0f);
}

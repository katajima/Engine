#include "ComboState.h"
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"


// 開始
void ComboNodeState::Enter(BaseCharacter* owner) {

	// アニメーションの設定
	AnimationComponent* anima = owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	comboData_.motion.SetAnimation(anima);
	comboData_.motion.SetMove(owner->GetMoveComponent());
	comboData_.motion.SetWorld(&owner->GetObjectComponent()->GetWorldTransform());
	comboData_.motion.GetData().animationName_ = animation;

	// 武器設定
	comboData_.Enter();								// コンボデータ開始
	owner->GetWeapon()->SetComboData(comboData_);	// コンボデータ設定
	owner->GetWeapon()->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, true); // タグ設定
	owner->GetWeapon()->GetObject3D()->isEmitTrailEffect = true; // トレイル開始

	timeInState = 0.0f;	// 時間初期化 
	

	// アニメーションが0.0fなら仮に1秒に設定
	if (anima->GetEndAnimeTime(animation) == 0.0f) {
		inputWindowEnd = 1.0f;
	}
	else {
		// 終わる時間の設定
		inputWindowEnd = anima->GetEndAnimeTime(animation);
	}

	

	// 移動
	//owner->GetMoveComponent()->Move(owner->GetObjectComponent()->GetWorldTransform(), owner->GetInput());
	
	

	// 座標更新
	owner->GetWorldTransform().Update();

	// 方向指定
	dire_ = owner->GetObjectComponent()->GetObject3D()->direction_;
}

// 更新
void ComboNodeState::Update(BaseCharacter* owner, float dt)
{
	// 時間更新
	timeInState += dt;

	// アニメが終了 or 入力受付時間が終わって、次のステートが無ければ終了
	bool isInputWindowOver = timeInState > inputWindowEnd;
	bool isMove = comboData_.motion.IsMove();
	bool hasNext = HasNextState();

	// 移動できるなら
	if (isMove) {
		owner->Velocity() = dire_;	// 方向設定
	}
	// 入力受付がないのなら終了する
	if ((isInputWindowOver)) {

		// 時間初期化
		timeInState = 0.0f;
		// コンボ終了 → 通常ステートに戻す
		owner->GetWeapon()->GetObject3D()->isEmitTrailEffect = false;
		owner->GetCharacterStateMachine()->ChangeState(CharacterMainState::Idle);  // ← BaseCharacterが持っている関数
	}
	// コンボデータ更新
	comboData_.Update(*owner->GetInput(), dt);
}

// 終了
void ComboNodeState::Exit(BaseCharacter* owner)
{
	// 時間初期化
	timeInState = 0.0f;
	// コンボデータ終了処理
	comboData_.Exit();

	// アニメ終了時の処理など
	owner->GetWeapon()->GetColliderComponent()->contactRecord_.Clear();
	owner->GetWeapon()->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, false);
	owner->GetWeapon()->GetObject3D()->isEmitTrailEffect = false;
}

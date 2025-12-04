#include "ComboState.h"
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>


#pragma region NodeState

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

	// ヒットボックスシステムを渡す
	comboData_.hitBox.SetHitBoxSystem(owner->GetAttackController()->GetHitBoxSystem());


	comboData_.comboCondition.GetData().inputWindowStart_;


	// コンボデータをコンボシステムに転送
	owner->GetAttackController()->GetComboSystem()->SetComboData(&comboData_);

	owner->GetWeapon()->GetObject3D()->isEmitTrailEffect = true; // トレイル開始

	timeInState = 0.0f;	// 時間初期化 

	// 終わる時間の設定
	inputWindowStart = comboData_.comboCondition.GetData().inputWindowStart_;
	inputWindowEnd = comboData_.comboCondition.GetData().inputWindowEnd_;
	stateEndTime = comboData_.comboCondition.GetData().stateEndTime;
	timeNextState = comboData_.comboCondition.GetData().stateNextTime;
	
	// 重力はあるか
	isGravity = comboData_.motion.GetData().isGravity_;

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
	bool isInputWindowOver = GetEndStateTime();
	bool isMove = comboData_.motion.IsMove();
	
	// 移動可能の場合は
	owner->GetMoveComponent()->GetMoveSystem()->SetIsAttackCanMove(isMove);

	// 入力受付がないのなら終了する
	if ((isInputWindowOver)) {

		// 時間初期化
		timeInState = 0.0f;
		// コンボ終了 → 通常ステートに戻す
		owner->GetWeapon()->GetObject3D()->isEmitTrailEffect = false;
		owner->GetCharacterStateMachine()->ChangeState(CharacterMainState::Idle);  // ← BaseCharacterが持っている関数
		owner->GetAttackController()->SetIsAttack(false);	 // 攻撃終了
	}
	else {
		if (!isGravity) {
			owner->GetObjectComponent()->GetRigidBodyComponent()->Velocity().y = 0;
		}
		owner->GetObjectComponent()->GetRigidBodyComponent()->SetIsGravity(isGravity);
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

	owner->GetWeapon()->GetObject3D()->isEmitTrailEffect = false;
}

#pragma endregion // コンボノードステート


#pragma region StateMachine

void ComboStateMachine::SetState(std::shared_ptr<ComboState> state) {

	if (currentState) currentState->Exit(owner);	// 終了処理
	currentState = state;
	if (currentState) currentState->Enter(owner);	// 開始処理
	bufferedInput.reset(); // 状態遷移したら入力リセット
}


void ComboStateMachine::Update(float dt) {
	// ステートが無いなら早期リターン
	if (!currentState) return;

	// 現在のステート更新
	currentState->Update(owner, dt);

	// 入力がバッファされていて、入力受付時間内なら状態遷移
	if (bufferedInput) {
		// 入力受付時間内に入力があれば次へ移行するのを予約
		if (currentState->IsInputAcceptable()) {
			isNextState = true;	// 次のステートに移行確定
		}

		if (currentState->GetNextStateTime() && isNextState) {
			auto next = currentState->HandleInput(owner, *bufferedInput);
			// もし次のステートがあれば、遷移
			if (next) {
				SetState(next);
			}
			bufferedInput.reset();
		}
	}
}

void ComboStateMachine::SetRoot(std::shared_ptr<ComboState> state) {
	rootState = state;
	if (rootState) {
		SetState(rootState);
	}
}

#pragma endregion // ステートマシーン


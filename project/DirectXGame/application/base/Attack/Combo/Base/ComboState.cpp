#include "ComboState.h"
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>

namespace Combo {

#pragma region NodeState

	// 開始
	void NodeState::Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
		// 時間初期化 
		timeInState = 0.0f;
		// アニメーションの設定
		comboData.GetComboMotion().GetComboAnimation().GetData().animationName_ = animation;


		comboData.GetComboMotion().GetComboMove().SetDirection(direction_);
		// コンボデータ開始
		comboData.SetIsDebug(isDebug);
		comboData.Enter(owner, ctx);
	}

	// 更新
	void NodeState::Update(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
		// 時間更新
		timeInState += ctx.dt;

		// コンボデータ更新
		comboData.Update(ctx);

		// 入力受付がないのなら終了する
		if (GetEndStateTime() || GetIsCansel()) {
			// 終了処理
			End(owner, ctx);
		}
	}

	// 終了
	void NodeState::Exit(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
		// 時間初期化
		timeInState = 0.0f;
		// コンボデータ終了処理
		comboData.Exit(owner);
	}

	void NodeState::End(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
		// 時間初期化
		timeInState = 0.0f;
		// コンボ終了 → 通常ステートに戻す
		owner->GetWeapon()->GetObject3D()->isEmitTrailEffect = false;
		if (ctx.inputData.jumpTrigger) {
			owner->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Jump); 
		}
		else {
			owner->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Idle);
		}
		owner->GetAttackController()->SetIsAttack(false);	 // 攻撃終了
	};


#pragma endregion // コンボノードステート


#pragma region StateMachine

	void StateMachine::SetState(std::shared_ptr<State> state, const Character::CharacterContext& ctx) {

		if (currentState) currentState->Exit(owner, ctx);	// 終了処理
		currentState = state;
		if (currentState) {
			currentState->Enter(owner, ctx);	// 開始処理
		}
		bufferedInput.reset(); // 状態遷移したら入力リセット
	}


	void StateMachine::Update(const Character::CharacterContext& ctx) {
		// ステートが無いなら早期リターン
		if (!currentState) return;

		// 現在のステート更新
		currentState->Update(owner, ctx);

		// 入力がバッファされていて、入力受付時間内なら状態遷移
		if (bufferedInput) {

			// コンボ移行時間に達して次のステートへ移行するなら
			if (currentState->GetNextStateTime() && currentState->GetIsNextState()) {
				auto next = currentState->HandleInput(owner, *bufferedInput);
				// もし次のステートがあれば、遷移
				if (next) {
					SetState(next, ctx);
				}
				bufferedInput.reset();
			}
		}
	}

	void StateMachine::SetRoot(std::shared_ptr<State> state) {
		rootState = state;
		if (rootState) {
			SetState(rootState,{});
		}
	}

#pragma endregion // ステートマシーン

}

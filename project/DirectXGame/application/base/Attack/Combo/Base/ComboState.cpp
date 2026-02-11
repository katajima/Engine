#include "ComboState.h"
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>

namespace Combo {

#pragma region NodeState

	// 開始
	void NodeState::Enter(Character::BaseCharacter* owner) {
		// 時間初期化 
		timeInState = 0.0f;
		// アニメーションの設定
		comboData_.GetComboMotion().GetComboAnimation().GetData().animationName_ = animation;


		comboData_.GetComboMotion().GetComboMove().SetDirection(direction_);
		// コンボデータ開始
		comboData_.Enter(owner);
	}

	// 更新
	void NodeState::Update(Character::BaseCharacter* owner, float dt) {
		// 時間更新
		timeInState += dt;

		// 入力受付がないのなら終了する
		if (GetEndStateTime() || GetIsCansel()) {
			// 終了処理
			End(owner);
		}
		// コンボデータ更新
		comboData_.Update(*owner->GetInput(), dt);
	}

	// 終了
	void NodeState::Exit(Character::BaseCharacter* owner) {
		// 時間初期化
		timeInState = 0.0f;
		// コンボデータ終了処理
		comboData_.Exit(owner);
	}

	void NodeState::End(Character::BaseCharacter* owner) {
		// 時間初期化
		timeInState = 0.0f;
		// コンボ終了 → 通常ステートに戻す
		owner->GetWeapon()->GetObject3D()->isEmitTrailEffect = false;
		owner->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Idle);  // ← BaseCharacterが持っている関数
		owner->GetAttackController()->SetIsAttack(false);	 // 攻撃終了
	};


#pragma endregion // コンボノードステート


#pragma region StateMachine

	void StateMachine::SetState(std::shared_ptr<State> state) {

		if (currentState) currentState->Exit(owner);	// 終了処理
		currentState = state;
		if (currentState) {
			currentState->Enter(owner);	// 開始処理
		}
		bufferedInput.reset(); // 状態遷移したら入力リセット
	}


	void StateMachine::Update(float dt) {
		// ステートが無いなら早期リターン
		if (!currentState) return;

		// 現在のステート更新
		currentState->Update(owner, dt);

		// 入力がバッファされていて、入力受付時間内なら状態遷移
		if (bufferedInput) {

			// コンボ移行時間に達して次のステートへ移行するなら
			if (currentState->GetNextStateTime() && currentState->GetIsNextState()) {
				auto next = currentState->HandleInput(owner, *bufferedInput);
				// もし次のステートがあれば、遷移
				if (next) {
					SetState(next);
				}
				bufferedInput.reset();
			}
		}
	}

	void StateMachine::SetRoot(std::shared_ptr<State> state) {
		rootState = state;
		if (rootState) {
			SetState(rootState);
		}
	}

#pragma endregion // ステートマシーン

}

#include "ComboState.h"
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include <DirectXGame/application/base/Attack/AttackController.h>
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include "DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"

namespace Combo {

#pragma region NodeState

	// 開始
	void NodeState::Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
		// 時間初期化
		timeInState = 0.0f;
		hasHit_ = false;
		// ノード開始時に一度だけ加算の判定状態をリセットする
		hasIncrementedHitCount_ = false;
		// アニメーションの設定
		comboData.GetComboMotion().GetComboAnimation().GetData().animationName = animation;


		comboData.GetComboMotion().GetComboMove().SetDirection(direction_);
		// コンボデータ開始
		comboData.SetIsDebug(isDebug);
		comboData.Enter(owner, ctx);
	}

	std::shared_ptr<State> NodeState::HandleInput(Character::BaseCharacter* owner, ActionInput input) {
		return ResolveNextState(owner, input);
	}

	std::shared_ptr<NodeState> NodeState::ResolveNextState(Character::BaseCharacter* owner, ActionInput input) {
		auto it = nextStates.find(input);
		if (it == nextStates.end()) {
			return nullptr;
		}

		const GlobalAction& action = comboData.GetActionData();
		if (action.cancelOnHitOnly && !hasHit_) {
			return nullptr;
		}
		if (action.cancelOnMissOnly && hasHit_) {
			return nullptr;
		}
		if (action.landingCancel && owner && owner->GetMoveComponent() && !owner->GetMoveComponent()->GetIsLanding()) {
			return nullptr;
		}

		const bool isLockOn =
			owner &&
			owner->GetAttackController() &&
			owner->GetAttackController()->GetLockOnSystem() &&
			owner->GetAttackController()->GetLockOnSystem()->IsLockOn();
		const std::weak_ptr<NodeState>& lockOnTarget = isLockOn ? it->second.lockOn : it->second.noLockOn;
		if (auto next = lockOnTarget.lock()) {
			// ロックオン用の分岐が設定されている場合は、地上/空中やヒット状態より優先する
			return next;
		}

		const bool onGround = owner && owner->GetMoveComponent() && owner->GetMoveComponent()->GetIsLanding();
		const std::weak_ptr<NodeState>& conditionalTarget =
			onGround ? (hasHit_ ? it->second.groundHit : it->second.groundMiss)
			: (hasHit_ ? it->second.airHit : it->second.airMiss);
		if (auto next = conditionalTarget.lock()) {
			return next;
		}
		return it->second.defaultTarget.lock();
	}

	void NodeState::SetNextState(ActionInput input, TransitionCondition condition, std::shared_ptr<NodeState> next) {
		TransitionTargets& targets = nextStates[input];
		switch (condition) {
		case TransitionCondition::GroundMiss:
			targets.groundMiss = next;
			break;
		case TransitionCondition::GroundHit:
			targets.groundHit = next;
			break;
		case TransitionCondition::AirMiss:
			targets.airMiss = next;
			break;
		case TransitionCondition::AirHit:
			targets.airHit = next;
			break;
		case TransitionCondition::LockOn:
			targets.lockOn = next;
			break;
		case TransitionCondition::NoLockOn:
			targets.noLockOn = next;
			break;
		default:
			targets.defaultTarget = next;
			break;
		}
	}

	bool NodeState::HasNextState() const {
		for (const auto& [input, targets] : nextStates) {
			if (!targets.defaultTarget.expired() || !targets.groundMiss.expired() ||
				!targets.groundHit.expired() || !targets.airMiss.expired() || !targets.airHit.expired() ||
				!targets.lockOn.expired() || !targets.noLockOn.expired()) {
				return true;
			}
		}
		return false;
	}

	bool NodeState::HasNextState(ActionInput input) const {
		auto it = nextStates.find(input);
		if (it == nextStates.end()) {
			return false;
		}
		const TransitionTargets& targets = it->second;
		return !targets.defaultTarget.expired() || !targets.groundMiss.expired() ||
			!targets.groundHit.expired() || !targets.airMiss.expired() || !targets.airHit.expired() ||
			!targets.lockOn.expired() || !targets.noLockOn.expired();
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
		bufferedInputAge_ = 0.0f;
		isBufferedInputAccepted_ = false;
	}

	bool StateMachine::CanTransition(ActionInput input) const {
		return ResolveTransitionTarget(input) != nullptr;
	}

	std::shared_ptr<NodeState> StateMachine::ResolveTransitionTarget(ActionInput input) const {
		auto node = std::dynamic_pointer_cast<NodeState>(currentState);
		return node ? node->ResolveNextState(owner, input) : nullptr;
	}

	std::optional<ActionInput> StateMachine::ConsumeTransitionedInput() {
		std::optional<ActionInput> result = transitionedInput_;
		transitionedInput_.reset();
		return result;
	}

	void StateMachine::NotifyCurrentStateHit() {
		auto node = std::dynamic_pointer_cast<NodeState>(currentState);
		if (node) {
			node->NotifyHit();
		}
	}

	void StateMachine::Update(const Character::CharacterContext& ctx) {
		// ステートが無いなら早期リターン
		if (!currentState) return;

		// 現在のステート更新
		currentState->Update(owner, ctx);

		if (!isDebug && owner->GetCurrentMainState() != Character::CharacterMainState::Attack) {
			bufferedInput.reset();
			bufferedInputAge_ = 0.0f;
			isBufferedInputAccepted_ = false;
			return;
		}

		// 入力種類は RequestAttack から渡された ActionInput を正とする。
		if (bufferedInput) {
			bufferedInputAge_ += ctx.dt;
			auto currentNode = std::dynamic_pointer_cast<NodeState>(currentState);
			const float bufferTime = currentNode ? currentNode->Data().GetComboCondition().GetData().inputBufferTime : 0.0f;
			if (bufferedInputAge_ > bufferTime) {
				bufferedInput.reset();
				bufferedInputAge_ = 0.0f;
				isBufferedInputAccepted_ = false;
				return;
			}

			if (currentState->IsInputAcceptable()) {
				isBufferedInputAccepted_ = true;
			}
			// 入力受付済みで、コンボ移行時間に達したら状態遷移する。
			if (isBufferedInputAccepted_ && currentState->GetNextStateTime()) {
				ActionInput transitionInput = *bufferedInput;
				auto next = currentState->HandleInput(owner, transitionInput);
				if (currentState->GetIsCompulsionNext()) {
					transitionInput = ActionInput::LightAttack;
					next = currentState->HandleInput(owner, transitionInput);
				}

				// もし次のステートがあれば、遷移
				if (next) {
					transitionedInput_ = transitionInput;
					SetState(next, ctx);
				}
				bufferedInput.reset();
				bufferedInputAge_ = 0.0f;
				isBufferedInputAccepted_ = false;
			}
		}
		else if (currentState->GetNextStateTime() && currentState->GetIsCompulsionNext()) {
			auto next = currentState->HandleInput(owner, ActionInput::LightAttack);

			// 強制移行ノードは入力なしで次の弱攻撃へ遷移する。
			if (next) {
				SetState(next, ctx);
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

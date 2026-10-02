#include "ComboState.h"
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include <DirectXGame/application/base/Attack/AttackController.h>
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include "DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/engine/Entity/ObjectComponent.h"
#include <utility>

namespace Combo {

#pragma region NodeState

	// 開始
	void NodeState::Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
		// 時間初期化
		timeInState = 0.0f;
		hasHit_ = false;
		canceled_ = false;
		missed_ = false;
		// ノード開始時に一度だけ加算の判定状態をリセットする
		hasIncrementedHitCount_ = false;
		// コンボノードに設定された防御効果をキャラクター共通状態へ反映する
		owner->SetActionDefenseFlags(
			comboData.GetActionData().superArmor,
			comboData.GetActionData().invincible,
			comboData.GetActionData().guardPoint);
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
		const bool onGround = owner && owner->GetMoveComponent() && owner->GetMoveComponent()->GetIsLanding();
		// 最も具体的な「地上/空中 + ヒット/ミス + ロックオン有無」を先に評価する。
		const std::weak_ptr<NodeState>& exactTarget = onGround
			? (hasHit_ ? (isLockOn ? it->second.groundHitLockOn : it->second.groundHitNoLockOn)
				: (isLockOn ? it->second.groundMissLockOn : it->second.groundMissNoLockOn))
			: (hasHit_ ? (isLockOn ? it->second.airHitLockOn : it->second.airHitNoLockOn)
				: (isLockOn ? it->second.airMissLockOn : it->second.airMissNoLockOn));
		if (auto next = exactTarget.lock()) {
			return next;
		}
		const std::weak_ptr<NodeState>& lockOnTarget = isLockOn ? it->second.lockOn : it->second.noLockOn;
		if (auto next = lockOnTarget.lock()) {
			// ロックオン用の分岐が設定されている場合は、地上/空中やヒット状態より優先する
			return next;
		}

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
		case TransitionCondition::GroundMissLockOn:
			targets.groundMissLockOn = next;
			break;
		case TransitionCondition::GroundHitLockOn:
			targets.groundHitLockOn = next;
			break;
		case TransitionCondition::AirMissLockOn:
			targets.airMissLockOn = next;
			break;
		case TransitionCondition::AirHitLockOn:
			targets.airHitLockOn = next;
			break;
		case TransitionCondition::GroundMissNoLockOn:
			targets.groundMissNoLockOn = next;
			break;
		case TransitionCondition::GroundHitNoLockOn:
			targets.groundHitNoLockOn = next;
			break;
		case TransitionCondition::AirMissNoLockOn:
			targets.airMissNoLockOn = next;
			break;
		case TransitionCondition::AirHitNoLockOn:
			targets.airHitNoLockOn = next;
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
				!targets.lockOn.expired() || !targets.noLockOn.expired() ||
				!targets.groundMissLockOn.expired() || !targets.groundHitLockOn.expired() ||
				!targets.airMissLockOn.expired() || !targets.airHitLockOn.expired() ||
				!targets.groundMissNoLockOn.expired() || !targets.groundHitNoLockOn.expired() ||
				!targets.airMissNoLockOn.expired() || !targets.airHitNoLockOn.expired()) {
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
			!targets.lockOn.expired() || !targets.noLockOn.expired() ||
			!targets.groundMissLockOn.expired() || !targets.groundHitLockOn.expired() ||
			!targets.airMissLockOn.expired() || !targets.airHitLockOn.expired() ||
			!targets.groundMissNoLockOn.expired() || !targets.groundHitNoLockOn.expired() ||
			!targets.airMissNoLockOn.expired() || !targets.airHitNoLockOn.expired();
	}

	std::vector<std::shared_ptr<NodeState>> NodeState::GetTransitionTargets() const {
		// 検証用に、条件付き遷移を含むすべての遷移先を解決する
		std::vector<std::shared_ptr<NodeState>> targets;
		for (const auto& [input, transition] : nextStates) {
			(void)input;
			const std::weak_ptr<NodeState>* candidates[] = {
				&transition.defaultTarget,
				&transition.groundMiss,
				&transition.groundHit,
				&transition.airMiss,
				&transition.airHit,
				&transition.lockOn,
				&transition.noLockOn,
				&transition.groundMissLockOn,
				&transition.groundHitLockOn,
				&transition.airMissLockOn,
				&transition.airHitLockOn,
				&transition.groundMissNoLockOn,
				&transition.groundHitNoLockOn,
				&transition.airMissNoLockOn,
				&transition.airHitNoLockOn,
			};
			for (const auto* candidate : candidates) {
				if (auto target = candidate->lock()) {
					targets.push_back(std::move(target));
				}
			}
		}
		return targets;
	}

	// 更新
	void NodeState::Update(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
		// 時間更新
		timeInState += ctx.dt;

		// コンボデータ更新
		comboData.Update(ctx);

		// 入力受付がないのなら終了する
		if (GetIsCansel()) {
			// キャンセル終了をミスと区別できるアクションイベントとして通知する
			canceled_ = true;
			owner->EmitActionEvent({ GameAction::EventType::Canceled, owner, nullptr, actionInput_, name });
			comboData.GetComboEffect().NotifyCancel();
			End(owner, ctx);
			return;
		}
		if (GetEndStateTime()) {
			// 終了処理
			End(owner, ctx);
		}
	}

	// 終了
	void NodeState::Exit(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
		// 時間初期化
		timeInState = 0.0f;
		if (!hasHit_ && !canceled_ && !missed_) {
			// 次段へ遷移してノードを抜ける場合も、命中しなかった攻撃をミスとして通知する
			missed_ = true;
			owner->EmitActionEvent({ GameAction::EventType::Missed, owner, nullptr, actionInput_, name });
			comboData.GetComboEffect().NotifyMiss();
		}
		// コンボデータ終了処理
		comboData.Exit(owner);
		// ノード終了時に攻撃由来の防御効果を解除する
		owner->ClearActionDefenseFlags();
	}

	void NodeState::End(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
		// 時間初期化
		timeInState = 0.0f;
		// コンボ終了 → 通常ステートに戻す
		if (!hasHit_ && !canceled_ && !missed_) {
			// 命中もキャンセルも無い場合だけミスイベントを通知する
			missed_ = true;
			owner->EmitActionEvent({ GameAction::EventType::Missed, owner, nullptr, actionInput_, name });
			comboData.GetComboEffect().NotifyMiss();
		}
		if (ctx.inputData.jumpTrigger) {
			owner->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Jump);
		}
		else {
			owner->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Idle);
		}
		owner->GetAttackController()->SetIsAttack(false);	 // 攻撃終了
		// Endから直接通常状態へ戻る場合にも防御効果を残さない
		owner->ClearActionDefenseFlags();
	};


#pragma endregion // コンボノードステート


#pragma region StateMachine

	void StateMachine::SetState(std::shared_ptr<State> state, const Character::CharacterContext& ctx) {

		if (currentState) currentState->Exit(owner, ctx);	// 終了処理
		currentState = state;
		if (currentState) {
			currentState->Enter(owner, ctx);	// 開始処理
		}
	}

	bool StateMachine::CanTransition(ActionInput input) const {
		return ResolveTransitionTarget(input) != nullptr;
	}

	std::shared_ptr<NodeState> StateMachine::ResolveTransitionTarget(ActionInput input) const {
		auto node = std::dynamic_pointer_cast<NodeState>(currentState);
		return node ? node->ResolveNextState(owner, input) : nullptr;
	}

	std::optional<ConsumedInput> StateMachine::ConsumeTransitionedInput() {
		std::optional<ConsumedInput> result = transitionedInput_;
		transitionedInput_.reset();
		return result;
	}

	std::vector<InputBufferId> StateMachine::ConsumeDiscardedInputIds() {
		// 期限切れ入力のIDを一度だけ資源予約システムへ渡す。
		std::vector<InputBufferId> result;
		result.reserve(discardedInputIds_.size());
		while (!discardedInputIds_.empty()) {
			result.push_back(discardedInputIds_.front());
			discardedInputIds_.pop_front();
		}
		return result;
	}

	void StateMachine::ClearInputBuffer() {
		// 保持中の先行入力をすべて消費済みとして破棄する
		for (const BufferedInput& bufferedInput : bufferedInputs_) {
			discardedInputIds_.push_back(bufferedInput.id);
		}
		bufferedInputs_.clear();
	}

	void StateMachine::NotifyCurrentStateHit() {
		auto node = std::dynamic_pointer_cast<NodeState>(currentState);
		if (node) {
			node->NotifyHit();
		}
	}

	void StateMachine::Update(const Character::CharacterContext& ctx) {
		// 開始要求はここで実コンテキストを使ってEnterするため、空のコンテキストを渡さない。
		ActivatePendingRoot(ctx);
		// ステートが無いなら早期リターン
		if (!currentState) return;

		// 現在のステート更新
		currentState->Update(owner, ctx);

		if (!isDebug && owner->GetCurrentMainState() != Character::CharacterMainState::Attack) {
			// 攻撃終了後の入力は次の攻撃へ持ち越さない
			ClearInputBuffer();
			return;
		}

		// 現在ノードの入力受付時間を基準に、古くなった先行入力を破棄する
		auto currentNode = std::dynamic_pointer_cast<NodeState>(currentState);
		const float bufferTime = currentNode ? currentNode->Data().GetComboCondition().GetData().inputBufferTime : 0.0f;
		for (auto it = bufferedInputs_.begin(); it != bufferedInputs_.end();) {
			it->age += ctx.dt;
			if (it->age > bufferTime) {
				discardedInputIds_.push_back(it->id);
				it = bufferedInputs_.erase(it);
			}
			else {
				++it;
			}
		}

		// 入力受付済みで、コンボ移行時間に達したら古い入力から遷移を試す
		if (!bufferedInputs_.empty() && currentState->IsInputAcceptable() && currentState->GetNextStateTime()) {
			for (size_t index = 0; index < bufferedInputs_.size(); ++index) {
				ActionInput transitionInput = bufferedInputs_[index].input;
				const InputBufferId transitionInputId = bufferedInputs_[index].id;
				auto next = currentState->HandleInput(owner, transitionInput);
				if (currentState->GetIsCompulsionNext()) {
					// 強制移行ノードは入力種別を弱攻撃として評価する
					transitionInput = ActionInput::LightAttack;
					next = currentState->HandleInput(owner, transitionInput);
				}

				if (next) {
					// 遷移に使用した入力だけを消費し、残りは次ノードへ引き継ぐ
					bufferedInputs_.erase(bufferedInputs_.begin() + static_cast<std::ptrdiff_t>(index));
					// 遷移元ノードへ分岐イベントを一度だけ通知する
					auto sourceNode = std::dynamic_pointer_cast<NodeState>(currentState);
					transitionedInput_ = ConsumedInput{ transitionInputId, transitionInput };
					if (sourceNode) {
						sourceNode->NotifyBranch();
					}
					SetState(next, ctx);
					return;
				}
			}
		}
		else if (bufferedInputs_.empty() && currentState->GetNextStateTime() && currentState->GetIsCompulsionNext()) {
			auto next = currentState->HandleInput(owner, ActionInput::LightAttack);

			// 強制移行ノードは入力なしで次の弱攻撃へ遷移する。
			if (next) {
				// 強制移行でも分岐演出は遷移元ノードへ通知する
				auto sourceNode = std::dynamic_pointer_cast<NodeState>(currentState);
				if (sourceNode) {
					// 遷移元の分岐通知を一度だけ発行する
					sourceNode->NotifyBranch();
				}
				SetState(next, ctx);
			}
		}
	}

	void StateMachine::SetRoot(std::shared_ptr<State> state) {
		// 新しいコンボ開始時は前のコンボの入力を持ち越さず、開始は次回Updateへ保留する。
		ClearInputBuffer();
		rootState = state;
		pendingRootState_ = rootState;
	}

	bool StateMachine::ActivatePendingRoot(const Character::CharacterContext& ctx) {
		// 保留がない場合は何も変更せず、呼び出し側が通常更新を続けられるようにする。
		if (!pendingRootState_ && !rootState) {
			return false;
		}
		if (!pendingRootState_) {
			return false;
		}
		SetState(pendingRootState_, ctx);
		pendingRootState_.reset();
		return true;
	}

#pragma endregion // ステートマシーン

}

#include "ComboSystem.h"
#include <DirectXGame/engine/GlobalVariables/GlobalVariables.h>
#include <DirectXGame/application/base/Attack/AttackController.h>
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include <DirectXGame/application/base/Character/Move/Base/MoveComponent.h>
#include <DirectXGame/application/base/Character/State/CharacterStateMachine.h>

namespace Combo {

#pragma region main

	void System::Initialize(Character::BaseCharacter* character, Engine::LineCommon* lineCommon,
		Engine::GlobalVariables* globalVariables, Engine::AudioManager* audioManager) {
		this->globalVariables = globalVariables;
		// 実行時再生とコンボエディタの音一覧で共有する音声管理を保持する。
		audioManager_ = audioManager;
		owner = character;	// 所有者設定

		// コンボ保存項目で使用するenumをグローバル変数エディタへ登録する
		EnumRegistry::Instance().Register("ComboType", {
			{ "移動のみ", static_cast<int64_t>(Type::kNone) },
			{ "近距離", static_cast<int64_t>(Type::kMelle) },
			{ "遠距離", static_cast<int64_t>(Type::kRange) },
			{ "近距離 + 遠距離", static_cast<int64_t>(Type::kMix) },
			});
		EnumRegistry::Instance().Register("RangeType", {
			{ "弾", static_cast<int64_t>(RangeType::kBullet) },
			{ "武器", static_cast<int64_t>(RangeType::kWeapon) },
			{ "サブ武器", static_cast<int64_t>(RangeType::kSubWeapon) },
			});
		EnumRegistry::Instance().Register("RangeLockOnType", {
			{ "前方", static_cast<int64_t>(RangeLockOnType::kNone) },
			{ "ターゲット", static_cast<int64_t>(RangeLockOnType::kTarget) },
			{ "カメラ", static_cast<int64_t>(RangeLockOnType::kCamera) },
			{ "オフセットターゲット", static_cast<int64_t>(RangeLockOnType::kOffsetTarget) },
			});
		EnumRegistry::Instance().Register("RangeOffsetTargetType", {
			{ "ワールドオフセット", static_cast<int64_t>(RangeOffsetTargetType::kWorldOffset) },
			{ "所有者ローカル", static_cast<int64_t>(RangeOffsetTargetType::kOwnerLocalOffset) },
			});
		EnumRegistry::Instance().Register("RangeThrowMoveType", {
			{ "直進", static_cast<int64_t>(RangeThrowMoveType::kStraight) },
			{ "ヒット時停止", static_cast<int64_t>(RangeThrowMoveType::kStopOnHit) },
			{ "ターゲット瞬間移動", static_cast<int64_t>(RangeThrowMoveType::kTeleportToTarget) },
			{ "ターゲット補間移動", static_cast<int64_t>(RangeThrowMoveType::kLerpToTarget) },
			});
		EnumRegistry::Instance().Register("RangeEffectTriggerType", {
			{ "なし", static_cast<int64_t>(RangeEffectTriggerType::kNone) },
			{ "ボタン", static_cast<int64_t>(RangeEffectTriggerType::kButton) },
			{ "時間経過", static_cast<int64_t>(RangeEffectTriggerType::kTimer) },
			{ "ヒット", static_cast<int64_t>(RangeEffectTriggerType::kHit) },
			});
		EnumRegistry::Instance().Register("RangeRecallTriggerType", {
			{ "なし", static_cast<int64_t>(RangeRecallTriggerType::kNone) },
			{ "ボタン", static_cast<int64_t>(RangeRecallTriggerType::kButton) },
			{ "時間経過", static_cast<int64_t>(RangeRecallTriggerType::kTimer) },
			{ "近づく", static_cast<int64_t>(RangeRecallTriggerType::kNearOwner) },
			});
		EnumRegistry::Instance().Register("MoveSpeedCurveType", {
			{ "一定", static_cast<int64_t>(MoveSpeedCurveType::kConstant) },
			{ "加速", static_cast<int64_t>(MoveSpeedCurveType::kEaseIn) },
			{ "減速", static_cast<int64_t>(MoveSpeedCurveType::kEaseOut) },
			{ "加速して減速", static_cast<int64_t>(MoveSpeedCurveType::kEaseInOut) },
			});
		EnumRegistry::Instance().Register("ComboEffectTriggerType", {
			{ "時間範囲", static_cast<int64_t>(ComboEffectTriggerType::kTimeWindow) },
			{ "時間経過", static_cast<int64_t>(ComboEffectTriggerType::kTimer) },
			{ "着地したら", static_cast<int64_t>(ComboEffectTriggerType::kLanding) },
			{ "ヒット時", static_cast<int64_t>(ComboEffectTriggerType::kHit) },
			{ "ミス時", static_cast<int64_t>(ComboEffectTriggerType::kMiss) },
			{ "ヒット数到達", static_cast<int64_t>(ComboEffectTriggerType::kHitCount) },
			{ "コンボ分岐時", static_cast<int64_t>(ComboEffectTriggerType::kBranch) },
			{ "キャンセル時", static_cast<int64_t>(ComboEffectTriggerType::kCancel) },
			{ "地上", static_cast<int64_t>(ComboEffectTriggerType::kGround) },
			{ "空中", static_cast<int64_t>(ComboEffectTriggerType::kAir) },
			{ "ボタン入力時", static_cast<int64_t>(ComboEffectTriggerType::kButton) },
			});

		EnumRegistry::Instance().Register("ComboEffectType", {
			{ "パーティクル", static_cast<int64_t>(ComboEffectType::Particle) },
			{ "トレイル", static_cast<int64_t>(ComboEffectType::Trail) },
		});
		EnumRegistry::Instance().Register("ComboEffectInputType", {
			{ "すべて", static_cast<int64_t>(ComboEffectInputType::kAny) },
			{ "ジャンプ", static_cast<int64_t>(ComboEffectInputType::kJump) },
			{ "回避", static_cast<int64_t>(ComboEffectInputType::kDodge) },
			{ "スキル", static_cast<int64_t>(ComboEffectInputType::kSkill) },
			{ "必殺技", static_cast<int64_t>(ComboEffectInputType::kSpecial) },
		});
		EnumRegistry::Instance().Register("TrailResetMode", {
			{ "履歴を維持", static_cast<int64_t>(Engine::TrailResetMode::kKeepHistory) },
			{ "発生開始時に消去", static_cast<int64_t>(Engine::TrailResetMode::kClearOnEmitStart) },
			{ "親変更時に消去", static_cast<int64_t>(Engine::TrailResetMode::kClearOnParentChange) },
		});
		comboStateMachine_ = std::make_unique<StateMachine>(character);

		comboDebug_ = std::make_unique<ComboDebug>();
		comboDebug_->Initialize(lineCommon, character);

	}

	void System::Update(const Character::CharacterContext& ctx) {
		UpdateCooldowns(ctx.dt);
		comboStateMachine_->SetIsDebug(isDebug);
		comboStateMachine_->Update(ctx);
		const auto transitionedInput = comboStateMachine_->ConsumeTransitionedInput();
		if (transitionedInput && pendingCostInput_ && *transitionedInput == *pendingCostInput_) {
			PayStamina(pendingStaminaCost_);
			StartCooldown(pendingCooldownNode_);
		}
		if (transitionedInput) {
			pendingCostInput_.reset();
			pendingStaminaCost_ = 0.0f;
			pendingCooldownNode_.reset();
		}
		comboDebug_->SetEnabled(isDebugDraw_);
		comboDebug_->Update(ctx.dt);
	}

	bool System::RequestAttack(ActionInput input) {
		if (!owner || owner->GetCurrentMainState() == Character::CharacterMainState::Special) {
			return false;
		}

		if (owner->GetCurrentMainState() == Character::CharacterMainState::Attack) {
			std::shared_ptr<NodeState> nextNode = comboStateMachine_->ResolveTransitionTarget(input);
			if (!nextNode) {
				return false;
			}
			if (!CanUseComboNode(nextNode)) {
				return false;
			}

			const float transitionCost = GetComboStaminaCost(input, nextNode);
			if (!CanPayStamina(transitionCost)) {
				return false;
			}

			comboStateMachine_->HandleInput(input);
			pendingCostInput_ = input;
			pendingStaminaCost_ = transitionCost;
			pendingCooldownNode_ = nextNode;
			return true;
		}

		const Character::CharacterMainState state = owner->GetCurrentMainState();
		const bool canStart =
			state == Character::CharacterMainState::Idle ||
			state == Character::CharacterMainState::Move ||
			state == Character::CharacterMainState::Jump ||
			state == Character::CharacterMainState::Avoidance;
		if (!canStart || !owner->GetMoveComponent()) {
			return false;
		}

		const bool isDodging = state == Character::CharacterMainState::Avoidance;	// 回避中の開始ルートを使うか
		const bool isDodgeSuccess = owner->IsDodgeSuccessComboWindow();			// 回避成功後の開始ルートを使うか
		const std::string startCombo = ResolveStartCombo(input, owner->GetMoveComponent()->GetIsLanding(), isDodging, isDodgeSuccess);
		std::shared_ptr<NodeState> startNode = GetComboNodeState(startCombo);
		if (!startNode) {
			return false;
		}
		if (!CanUseComboNode(startNode)) {
			return false;
		}
		const float startCost = GetComboStaminaCost(input, startNode);
		if (!CanPayStamina(startCost)) {
			return false;
		}
		if (!StartCombo(startCombo)) {
			return false;
		}

		pendingCostInput_.reset();
		pendingStaminaCost_ = 0.0f;
		pendingCooldownNode_.reset();
		owner->GetAttackController()->SetIsAttack(true);
		owner->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Attack);
		PayStamina(startCost);
		StartCooldown(startNode);
		return true;
	}

	bool System::NotifyAttackHit() {
		if (!comboStateMachine_) {
			return false;
		}

		const std::shared_ptr<NodeState> currentNode = comboStateMachine_->GetCurrentState();
		if (!currentNode) {
			return false;
		}

		// コンボノード設定に従い、必要な場合だけ攻撃者のヒットカウンターを更新する。
		const GlobalAction& action = currentNode->Data().GetActionData();
		const bool canIncrement = action.incrementHitCount &&
			(!action.incrementHitCountOnce || !currentNode->HasIncrementedHitCount());
		if (canIncrement && owner && owner->GetAttackController()) {
			owner->GetAttackController()->GetHitCounter().Hit();
			currentNode->SetHasIncrementedHitCount(true);
		}

		// ヒット条件、遠距離、カメラ、音の通知はカウント設定に関係なく行う。
		comboStateMachine_->NotifyCurrentStateHit();
		return true;
	}

	void System::ClearNode() {

		// StateMachineの状態を完全リセット

        // StateMachineを破棄する前に、各ノードが生成した実行中演出を解放する。
        for (auto& [nodeName, node] : comboNodes_) {
            if (node) {
                node->Data().GetComboEffect().ClearRuntimeEffects();
            }
        }
		if (comboStateMachine_) {
			comboStateMachine_.reset();
			comboStateMachine_ = std::make_unique<StateMachine>(owner);
		}

		comboNodes_.clear();
		comboGlobalDatas_.clear();
		comboNodenames_.clear();
		parentTransforms_.clear();
		pendingCostInput_.reset();
		pendingStaminaCost_ = 0.0f;
		pendingCooldownNode_.reset();
		cooldownTimers_.clear();
	}

	void System::AddComboNode(const std::string& name, std::shared_ptr<NodeState> node) {
		// 既に存在する場合は追加しない
		if (comboNodes_.find(name) != comboNodes_.end()) {
			return;
		}
		comboNodes_[name] = node;
	}

	void System::AddComboNode(const std::string& nodeName, const std::string& animationName, const ComboData& data) {
		// 既に存在する場合は追加しない
		if (comboNodes_.find(nodeName) != comboNodes_.end()) {
			return;
		}
		// ノード生成
		std::shared_ptr<NodeState> node = std::make_shared<NodeState>(animationName, data);
		comboNodes_[nodeName] = node;				// ノード追加
		comboNodes_[nodeName]->SetName(nodeName);	// ノード名設定
	}

	void System::ConnectCombo(const std::string& from, ActionInput input, const std::string& to, TransitionCondition condition) {
		auto itFrom = comboNodes_.find(from);
		auto itTo = comboNodes_.find(to);
		if (itFrom != comboNodes_.end()) {
			if (itTo != comboNodes_.end())
				itFrom->second->SetNextState(input, condition, itTo->second);
		}
	}

	void System::RenameComboReferences(const std::string& oldName, const std::string& newName) {
		for (auto& [nodeName, data] : comboGlobalDatas_) {
			bool changed = false;
			auto renameTarget = [&](std::string& target) {
				if (target == oldName) {
					target = newName;
					changed = true;
				}
			};
			auto renameConditional = [&](GlobalConditionalConnection& targets) {
				renameTarget(targets.groundMiss);
				renameTarget(targets.groundHit);
				renameTarget(targets.airMiss);
				renameTarget(targets.airHit);
				renameTarget(targets.lockOn);
				renameTarget(targets.noLockOn);
			};
			renameTarget(data.connection.lightAttack);
			renameTarget(data.connection.heavyAttack);
			renameTarget(data.connection.skill);
			renameConditional(data.connection.lightCondition);
			renameConditional(data.connection.heavyCondition);
			renameConditional(data.connection.skillCondition);
			if (changed) {
				SetGlobalComboData(nodeName == oldName ? newName : nodeName, data);
			}
		}

		auto renameStart = [&](std::string& startName, const std::string& key) {
			if (startName == oldName) {
				startName = newName;
				globalVariables->SetValue(name, key, newName);
			}
		};
		renameStart(groundLightStart_, kGroundLightStartKey);
		renameStart(airLightStart_, kAirLightStartKey);
		renameStart(groundHeavyStart_, kGroundHeavyStartKey);
		renameStart(airHeavyStart_, kAirHeavyStartKey);
		renameStart(groundSkillStart_, kGroundSkillStartKey);
		renameStart(airSkillStart_, kAirSkillStartKey);
		renameStart(dodgeLightStart_, kDodgeLightStartKey);
		renameStart(dodgeHeavyStart_, kDodgeHeavyStartKey);
		renameStart(dodgeSkillStart_, kDodgeSkillStartKey);
		renameStart(dodgeSuccessLightStart_, kDodgeSuccessLightStartKey);
		renameStart(dodgeSuccessHeavyStart_, kDodgeSuccessHeavyStartKey);
		renameStart(dodgeSuccessSkillStart_, kDodgeSuccessSkillStartKey);
	}

	bool System::StartCombo(const std::string& name) {
		auto it = comboNodes_.find(name);
		if (it != comboNodes_.end()) {
			comboStateMachine_->SetRoot(it->second);
			return true;
		}
		return false;
	}

	std::string System::ResolveStartCombo(ActionInput input, bool isLanding, bool isDodging, bool isDodgeSuccess) const {
		if (isDodgeSuccess) {
			// 回避成功後は地上/空中より専用開始ルートを優先する
			switch (input) {
			case ActionInput::LightAttack:
				return dodgeSuccessLightStart_;
			case ActionInput::HeavyAttack:
				return dodgeSuccessHeavyStart_;
			case ActionInput::Skill:
				return dodgeSuccessSkillStart_;
			default:
				return "";
			}
		}
		if (isDodging) {
			// 回避中に攻撃した場合は回避中専用開始ルートを使う
			switch (input) {
			case ActionInput::LightAttack:
				return dodgeLightStart_;
			case ActionInput::HeavyAttack:
				return dodgeHeavyStart_;
			case ActionInput::Skill:
				return dodgeSkillStart_;
			default:
				return "";
			}
		}
		switch (input) {
		case ActionInput::LightAttack:
			return isLanding ? groundLightStart_ : airLightStart_;
		case ActionInput::HeavyAttack:
			return isLanding ? groundHeavyStart_ : airHeavyStart_;
		case ActionInput::Skill:
			return isLanding ? groundSkillStart_ : airSkillStart_;
		default:
			return "";
		}
	}

	StartComboRoutes System::GetStartComboRoutes() const {
		return StartComboRoutes{
			.groundLight = groundLightStart_,
			.airLight = airLightStart_,
			.groundHeavy = groundHeavyStart_,
			.airHeavy = airHeavyStart_,
			.groundSkill = groundSkillStart_,
			.airSkill = airSkillStart_,
			.dodgeLight = dodgeLightStart_,
			.dodgeHeavy = dodgeHeavyStart_,
			.dodgeSkill = dodgeSkillStart_,
			.dodgeSuccessLight = dodgeSuccessLightStart_,
			.dodgeSuccessHeavy = dodgeSuccessHeavyStart_,
			.dodgeSuccessSkill = dodgeSuccessSkillStart_,
		};
	}

	void System::SetStartComboRoutes(const StartComboRoutes& routes) {
		groundLightStart_ = routes.groundLight;
		airLightStart_ = routes.airLight;
		groundHeavyStart_ = routes.groundHeavy;
		airHeavyStart_ = routes.airHeavy;
		groundSkillStart_ = routes.groundSkill;
		airSkillStart_ = routes.airSkill;
		dodgeLightStart_ = routes.dodgeLight;
		dodgeHeavyStart_ = routes.dodgeHeavy;
		dodgeSkillStart_ = routes.dodgeSkill;
		dodgeSuccessLightStart_ = routes.dodgeSuccessLight;
		dodgeSuccessHeavyStart_ = routes.dodgeSuccessHeavy;
		dodgeSuccessSkillStart_ = routes.dodgeSuccessSkill;

		if (globalVariables && !name.empty()) {
			globalVariables->SetValue(name, kGroundLightStartKey, groundLightStart_);
			globalVariables->SetValue(name, kAirLightStartKey, airLightStart_);
			globalVariables->SetValue(name, kGroundHeavyStartKey, groundHeavyStart_);
			globalVariables->SetValue(name, kAirHeavyStartKey, airHeavyStart_);
			globalVariables->SetValue(name, kGroundSkillStartKey, groundSkillStart_);
			globalVariables->SetValue(name, kAirSkillStartKey, airSkillStart_);
			globalVariables->SetValue(name, kDodgeLightStartKey, dodgeLightStart_);
			globalVariables->SetValue(name, kDodgeHeavyStartKey, dodgeHeavyStart_);
			globalVariables->SetValue(name, kDodgeSkillStartKey, dodgeSkillStart_);
			globalVariables->SetValue(name, kDodgeSuccessLightStartKey, dodgeSuccessLightStart_);
			globalVariables->SetValue(name, kDodgeSuccessHeavyStartKey, dodgeSuccessHeavyStart_);
			globalVariables->SetValue(name, kDodgeSuccessSkillStartKey, dodgeSuccessSkillStart_);
		}
	}

	bool System::IsStartComboKey(const std::string& key) const {
		// ComboPlayer グループ内で、コンボノード名ではなく開始ルート設定として扱うキー
		return key == kGroundLightStartKey || key == kAirLightStartKey ||
			key == kGroundHeavyStartKey || key == kAirHeavyStartKey ||
			key == kGroundSkillStartKey || key == kAirSkillStartKey ||
			key == kDodgeLightStartKey || key == kDodgeHeavyStartKey ||
			key == kDodgeSkillStartKey || key == kDodgeSuccessLightStartKey ||
			key == kDodgeSuccessHeavyStartKey || key == kDodgeSuccessSkillStartKey;
	}

	float System::GetStaminaCost(ActionInput input) const {
		return input == ActionInput::Skill ? 25.0f : 0.0f;
	}

	float System::GetComboStaminaCost(ActionInput input, const std::shared_ptr<NodeState>& node) const {
		if (!node) {
			return GetStaminaCost(input);
		}

		const GlobalAction& action = node->Data().GetActionData();
		return action.useCustomStaminaCost ? action.staminaCost : GetStaminaCost(input);
	}

	bool System::CanUseComboNode(const std::shared_ptr<NodeState>& node) const {
		if (!node || !owner) {
			return false;
		}

		const GlobalAction& action = node->Data().GetActionData();
		if (action.requiredAirRemainCount > 0) {
			if (!owner->GetMoveComponent() || owner->GetMoveComponent()->GetJumpCount() < action.requiredAirRemainCount) {
				return false;
			}
		}

		auto cooldownIt = cooldownTimers_.find(node->GetName());
		if (cooldownIt != cooldownTimers_.end() && cooldownIt->second > 0.0f) {
			return false;
		}
		return true;
	}

	bool System::CanPayStamina(float cost) const {
		return cost <= 0.0f ||
			(owner->GetCharacterParameterComponent() &&
				owner->GetCharacterParameterComponent()->GetStamina() >= cost);
	}

	void System::PayStamina(float cost) {
		if (cost > 0.0f && owner->GetCharacterParameterComponent()) {
			owner->GetCharacterParameterComponent()->Stamina().Add(-cost);
		}
	}

	void System::UpdateCooldowns(float dt) {
		for (auto it = cooldownTimers_.begin(); it != cooldownTimers_.end();) {
			it->second -= dt;
			if (it->second <= 0.0f) {
				it = cooldownTimers_.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void System::StartCooldown(const std::shared_ptr<NodeState>& node) {
		if (!node) {
			return;
		}

		const float cooldown = node->Data().GetActionData().cooldown;
		if (cooldown > 0.0f) {
			cooldownTimers_[node->GetName()] = cooldown;
		}
	}

#pragma endregion // main

#pragma region Global

	void Combo::System::ApplyGlobalComboData(const std::string& name, GlobalData& data) {
		globalVariables->SetGroupCategory(this->name, "Character/Combo");
		globalVariables->SetGroupCategory(name, "Character/Combo");
		globalVariables->CreateGroup(name);

		// 既に存在する場合は追加しない
		if (comboNodenames_.find(name) == comboNodenames_.end()) {
			comboNodenames_[name] = name;
			globalVariables->AddItem(this->name, name.c_str(), name);
		}

		// 未登録の項目だけを追加し、既存の保存値は維持する
		WriteGlobalComboData(name, data, false);
		// 登録後に保存済みの値を実行時データへ読み戻す
		GetGlobalComboData(name, data);
	}

	void System::WriteGlobalComboData(const std::string& groupName, GlobalData& data, bool overwrite) {
		// 項目一覧は一か所に集約し、writerが登録と保存の違いを吸収する
		Engine::GlobalVariableWriter writer(globalVariables, overwrite ?
			Engine::GlobalVariableWriteMode::Save : Engine::GlobalVariableWriteMode::Register);
		// 攻撃タイプと遠距離攻撃
		{
			writer.Value(groupName, "スタミナコスト個別指定", data.action.useCustomStaminaCost);
			writer.Value(groupName, "ヒットカウントを増やす", data.action.incrementHitCount);
			writer.Value(groupName, "ヒットカウントを一度だけ増やす", data.action.incrementHitCountOnce);
			writer.Value(groupName, "スタミナコスト", data.action.staminaCost);
			writer.Value(groupName, "クールダウン", data.action.cooldown);
			writer.Value(groupName, "空中残り回数要求", data.action.requiredAirRemainCount);
			writer.Value(groupName, "スーパーアーマー", data.action.superArmor);
			writer.Value(groupName, "無敵", data.action.invincible);
			writer.Value(groupName, "ガードポイント", data.action.guardPoint);
			writer.Value(groupName, "ヒット時のみキャンセル", data.action.cancelOnHitOnly);
			writer.Value(groupName, "ミス時のみキャンセル", data.action.cancelOnMissOnly);
			writer.Value(groupName, "着地時のみキャンセル", data.action.landingCancel);
			writer.Value(groupName, "ヒットポーズ倍率", data.action.hitPauseScale);
			writer.Value(groupName, "カメラシェイク量", data.action.cameraShakePower);
			writer.Value(groupName, "攻撃音", data.audio.attackSoundName);
			writer.Value(groupName, "攻撃音再生時間", data.audio.attackStartTime);
			writer.Value(groupName, "攻撃音音量", data.audio.attackVolume);
			writer.Value(groupName, "ヒット音", data.audio.hitSoundName);
			writer.Value(groupName, "ヒット音音量", data.audio.hitVolume);
			writer.Value(groupName, "攻撃終了音", data.audio.finishSoundName);
			writer.Value(groupName, "攻撃終了音音量", data.audio.finishVolume);

			writer.EnumValue(groupName, "コンボ攻撃タイプ", data.type, "ComboType");
			writer.EnumValue(groupName, "遠距離タイプ", data.range.rangeType, "RangeType");
			writer.EnumValue(groupName, "遠距離狙いタイプ", data.range.lockOnType, "RangeLockOnType");
			writer.EnumValue(groupName, "遠距離オフセットタイプ", data.range.offsetTargetType, "RangeOffsetTargetType");
			writer.EnumValue(groupName, "投擲移動タイプ", data.range.throwMoveType, "RangeThrowMoveType");
			writer.EnumValue(groupName, "投擲効果発動条件", data.range.effectTriggerType, "RangeEffectTriggerType");
			writer.EnumValue(groupName, "投擲回収条件", data.range.recallTriggerType, "RangeRecallTriggerType");
			writer.Value(groupName, "遠距離ロックオン開始半径", data.range.lockOnStartRadius);
			writer.Value(groupName, "遠距離オフセットターゲット", data.range.offsetTarget);
			writer.Value(groupName, "投擲補間時間", data.range.throwLerpTime);
			writer.Value(groupName, "投擲効果発動時間", data.range.effectTriggerTime);
			writer.Value(groupName, "投擲回収時間", data.range.recallTriggerTime);
			writer.Value(groupName, "投擲回収近接半径", data.range.recallNearRadius);
			writer.Value(groupName, "投擲停滞時間", data.range.throwStayTime);
			writer.Value(groupName, "遠距離発射開始時間", data.range.rangeWindowStart);
			writer.Value(groupName, "遠距離発射終了時間", data.range.rangeWindowEnd);
			writer.Value(groupName, "遠距離弾速", data.range.speed);
			writer.Value(groupName, "遠距離発射間隔", data.range.interval);
			writer.Value(groupName, "遠距離発射数", data.range.count);
			writer.Value(groupName, "遠距離ダメージ", data.range.damage);
			writer.Value(groupName, "サブ武器待機位置", data.range.subWeaponIdleOffset);
			writer.Value(groupName, "サブ武器投擲開始オフセット", data.range.subWeaponStartOffset);
			writer.Value(groupName, "サブ武器投擲速度", data.range.subWeaponThrowSpeed);
			writer.Value(groupName, "サブ武器投擲時間", data.range.subWeaponThrowLifeTime);
			writer.Value(groupName, "サブ武器戻り時間", data.range.subWeaponReturnTime);
			writer.Value(groupName, "サブ武器回転速度", data.range.subWeaponSpinSpeed);
			writer.Value(groupName, "サブ武器投擲方向に向ける", data.range.subWeaponAlignToDirection);
			writer.Value(groupName, "サブ武器スピン", data.range.subWeaponUseSpin);
			writer.Value(groupName, "サブ武器回転オフセット", data.range.subWeaponRotateOffset);
		}

		// エフェクト
		{
			writer.Value(groupName, "エフェクト(武器表示)", data.effect.weaponDraw);
			writer.Value(groupName, kComboEffectCountKey, static_cast<int>(data.effect.comboEffects.size()));

			for (int i = 0; i < static_cast<int>(data.effect.comboEffects.size()); ++i) {
				writer.Value(groupName, MakeComboEffectNameKey(i), data.effect.comboEffects[i].effectName);
				writer.Value(groupName, MakeComboEffectParentKey(i), data.effect.comboEffects[i].parentName);
				writer.EnumValue(groupName, MakeComboEffectTriggerTypeKey(i), data.effect.comboEffects[i].triggerType, "ComboEffectTriggerType");
				writer.Value(groupName, MakeComboEffectStartTimeKey(i), data.effect.comboEffects[i].startTime);
				writer.Value(groupName, MakeComboEffectEndTimeKey(i), data.effect.comboEffects[i].endTime);
				writer.Value(groupName, MakeComboEffectIntervalKey(i), data.effect.comboEffects[i].interval);
				writer.Value(groupName, MakeComboEffectOffsetKey(i), data.effect.comboEffects[i].offset);
				writer.Value(groupName, MakeComboEffectPositionKey(i), data.effect.comboEffects[i].transformPosition);
				writer.Value(groupName, MakeComboEffectRotationKey(i), data.effect.comboEffects[i].transformRotation);
				writer.Value(groupName, MakeComboEffectScaleKey(i), data.effect.comboEffects[i].transformScale);
				writer.Value(groupName, MakeComboEffectRequiredHitCountKey(i), data.effect.comboEffects[i].requiredHitCount);
				writer.EnumValue(groupName, MakeComboEffectInputTypeKey(i), data.effect.comboEffects[i].inputType, "ComboEffectInputType");
				writer.EnumValue(groupName, MakeComboEffectTypeKey(i), data.effect.comboEffects[i].type, "ComboEffectType");
				writer.Value(groupName, MakeComboEffectTrailTextureKey(i), data.effect.comboEffects[i].trailTexture);
				writer.Value(groupName, MakeComboEffectTrailColorKey(i), data.effect.comboEffects[i].trailColor.ToVector4());
				writer.Value(groupName, MakeComboEffectTrailOffsetStartKey(i), data.effect.comboEffects[i].trailOffsetStart);
				writer.Value(groupName, MakeComboEffectTrailOffsetEndKey(i), data.effect.comboEffects[i].trailOffsetEnd);
				writer.Value(groupName, MakeComboEffectTrailLifeTimeKey(i), data.effect.comboEffects[i].trailLifeTime);
				writer.Value(groupName, MakeComboEffectTrailMinDistanceKey(i), data.effect.comboEffects[i].trailSettings.minEmitDistance);
                writer.Value(groupName, MakeComboEffectTrailMaxSegmentsKey(i), static_cast<int>(data.effect.comboEffects[i].trailSettings.maxSegmentCount));
                writer.Value(groupName, MakeComboEffectTrailUseSplineKey(i), data.effect.comboEffects[i].trailSettings.useSpline);
                writer.Value(groupName, MakeComboEffectTrailSubdivisionKey(i), data.effect.comboEffects[i].trailSettings.splineSubdivision);
                writer.Value(groupName, MakeComboEffectTrailWidthStartKey(i), data.effect.comboEffects[i].trailSettings.widthStart);
				writer.Value(groupName, MakeComboEffectTrailWidthEndKey(i), data.effect.comboEffects[i].trailSettings.widthEnd);
				writer.Value(groupName, MakeComboEffectTrailFadeInKey(i), data.effect.comboEffects[i].trailSettings.fadeInTime);
				writer.Value(groupName, MakeComboEffectTrailFadeOutKey(i), data.effect.comboEffects[i].trailSettings.fadeOutTime);
				writer.Value(groupName, MakeComboEffectTrailAlphaCurveKey(i), data.effect.comboEffects[i].trailSettings.alphaCurve);
				writer.Value(groupName, MakeComboEffectTrailUvSpeedKey(i), data.effect.comboEffects[i].trailSettings.uvScrollSpeed);
				writer.Value(groupName, MakeComboEffectTrailEmissionKey(i), data.effect.comboEffects[i].trailSettings.emissionStrength);
				writer.Value(groupName, MakeComboEffectTrailFlipKey(i), data.effect.comboEffects[i].trailSettings.flipTexture);
				writer.EnumValue(groupName, MakeComboEffectTrailResetKey(i), data.effect.comboEffects[i].trailSettings.resetMode, "TrailResetMode");
				writer.EnumValue(groupName, MakeComboEffectTrajectoryTypeKey(i), data.effect.comboEffects[i].trajectory.type, "TrailTrajectoryType");
				writer.Value(groupName, MakeComboEffectTrajectoryDurationKey(i), data.effect.comboEffects[i].trajectory.duration);
				writer.Value(groupName, MakeComboEffectTrajectoryPoint0Key(i), data.effect.comboEffects[i].trajectory.point0);
				writer.Value(groupName, MakeComboEffectTrajectoryPoint1Key(i), data.effect.comboEffects[i].trajectory.point1);
				writer.Value(groupName, MakeComboEffectTrajectoryPoint2Key(i), data.effect.comboEffects[i].trajectory.point2);
				writer.Value(groupName, MakeComboEffectTrajectoryPoint3Key(i), data.effect.comboEffects[i].trajectory.point3);
				writer.Value(groupName, MakeComboEffectTrajectoryOrbitCenterKey(i), data.effect.comboEffects[i].trajectory.orbitCenter);
				writer.Value(groupName, MakeComboEffectTrajectoryOrbitRadiusKey(i), data.effect.comboEffects[i].trajectory.orbitRadius);
				writer.Value(groupName, MakeComboEffectTrajectoryOrbitHeightKey(i), data.effect.comboEffects[i].trajectory.orbitHeight);
				writer.Value(groupName, MakeComboEffectTrajectoryOrbitStartAngleKey(i), data.effect.comboEffects[i].trajectory.orbitStartAngle);
				writer.Value(groupName, MakeComboEffectTrajectoryOrbitEndAngleKey(i), data.effect.comboEffects[i].trajectory.orbitEndAngle);
			}
		}
		// 接続
		{
			writer.Value(groupName, "接続先(弱攻撃)", data.connection.lightAttack);
			writer.Value(groupName, "接続先(強攻撃)", data.connection.heavyAttack);
			writer.Value(groupName, "接続先(スキル)", data.connection.skill);
			writer.Value(groupName, "接続先(弱攻撃-地上-未ヒット)", data.connection.lightCondition.groundMiss);
			writer.Value(groupName, "接続先(弱攻撃-地上-ヒット)", data.connection.lightCondition.groundHit);
			writer.Value(groupName, "接続先(弱攻撃-空中-未ヒット)", data.connection.lightCondition.airMiss);
			writer.Value(groupName, "接続先(弱攻撃-空中-ヒット)", data.connection.lightCondition.airHit);
			writer.Value(groupName, "接続先(弱攻撃-ロックオン中)", data.connection.lightCondition.lockOn);
			writer.Value(groupName, "接続先(弱攻撃-ロックオンなし)", data.connection.lightCondition.noLockOn);
			writer.Value(groupName, "接続先(強攻撃-地上-未ヒット)", data.connection.heavyCondition.groundMiss);
			writer.Value(groupName, "接続先(強攻撃-地上-ヒット)", data.connection.heavyCondition.groundHit);
			writer.Value(groupName, "接続先(強攻撃-空中-未ヒット)", data.connection.heavyCondition.airMiss);
			writer.Value(groupName, "接続先(強攻撃-空中-ヒット)", data.connection.heavyCondition.airHit);
			writer.Value(groupName, "接続先(強攻撃-ロックオン中)", data.connection.heavyCondition.lockOn);
			writer.Value(groupName, "接続先(強攻撃-ロックオンなし)", data.connection.heavyCondition.noLockOn);
			writer.Value(groupName, "接続先(スキル-地上-未ヒット)", data.connection.skillCondition.groundMiss);
			writer.Value(groupName, "接続先(スキル-地上-ヒット)", data.connection.skillCondition.groundHit);
			writer.Value(groupName, "接続先(スキル-空中-未ヒット)", data.connection.skillCondition.airMiss);
			writer.Value(groupName, "接続先(スキル-空中-ヒット)", data.connection.skillCondition.airHit);
			writer.Value(groupName, "接続先(スキル-ロックオン中)", data.connection.skillCondition.lockOn);
			writer.Value(groupName, "接続先(スキル-ロックオンなし)", data.connection.skillCondition.noLockOn);
		}
		// 条件
		{
			writer.Value(groupName, "コンボ入力受付開始時間", data.condition.stateInput.startTime);
			writer.Value(groupName, "コンボ入力受付終了時間", data.condition.stateInput.endTime);
			writer.Value(groupName, "コンボ入力バッファ時間", data.condition.inputBufferTime);
			writer.Value(groupName, "コンボ終了時間", data.condition.stateEndTime);
			writer.Value(groupName, "コンボ移行時間", data.condition.stateNextTime);
			writer.Value(groupName, "コンボキャンセル受付開始時間", data.condition.stateCancel.startTime);
			writer.Value(groupName, "コンボキャンセル受付終了時間", data.condition.stateCancel.endTime);
			writer.Value(groupName, "コンボ移動キャンセル受付開始時間", data.condition.stateMoveCancel.startTime);
			writer.Value(groupName, "コンボ移動キャンセル受付終了時間", data.condition.stateMoveCancel.endTime);
			writer.EnumValue(groupName, "終了条件タイプ", data.condition.endConditionType, "EndConditionType");
			writer.Value(groupName, "コンボ強制移行", data.condition.isCompulsionNext);
			writer.Value(groupName, "コンボキャンセル可能", data.condition.isCancel);
			writer.Value(groupName, "コンボ移動キャンセル可能", data.condition.isMoveCancel);
			writer.Value(groupName, "コンボ入力遅延", data.condition.inputDelay);
		}
		// アニメーション
		{
			writer.Value(groupName, "アニメーション名前", data.animation.animationName);
			writer.Value(groupName, "アニメーション速度", data.animation.animationSpeed);
			writer.Value(groupName, "アニメーション遷移時間", data.animation.animationBlendTime);
			writer.Value(groupName, "アニメーションループ", data.animation.animationLoop);
			writer.Value(groupName, "アニメーションを一定時間で止めるか", data.animation.animationStop);
			writer.Value(groupName, "アニメーション停止時間", data.animation.animationStopTime);
			// 攻撃前移動アニメーション設定をコンボ単位で保存対象へ登録する
			writer.Value(groupName, "攻撃前移動アニメーション使用", data.animation.usePreMoveAnimation);
			writer.Value(groupName, "攻撃前移動アニメーション名前", data.animation.preMoveAnimationName);
			writer.Value(groupName, "攻撃前移動アニメーション速度", data.animation.preMoveAnimationSpeed);
			writer.Value(groupName, "攻撃前移動アニメーション終了時間", data.animation.preMoveAnimationEndTime);
			writer.Value(groupName, "攻撃前移動アニメーション遷移時間", data.animation.preMoveAnimationBlendTime);
			writer.Value(groupName, "攻撃前移動アニメーションループ", data.animation.preMoveAnimationLoop);
			// Transformアニメーション設定をコンボ単位で保存対象へ登録する
			writer.Value(groupName, "Transformアニメーション使用", data.animation.isTransformAnimation);
			writer.Value(groupName, "Transformアニメーション開始時間", data.animation.transformStartTime);
			writer.Value(groupName, "Transformアニメーション終了時間", data.animation.transformEndTime);
			writer.Value(groupName, "Transform開始スケール加算値", data.animation.transformStart.scale);
			writer.Value(groupName, "Transform開始回転加算値", data.animation.transformStart.rotate);
			writer.Value(groupName, "Transform開始位置加算値", data.animation.transformStart.translate);
			writer.Value(groupName, "Transform終了スケール加算値", data.animation.transformEnd.scale);
			writer.Value(groupName, "Transform終了回転加算値", data.animation.transformEnd.rotate);
			writer.Value(groupName, "Transform終了位置加算値", data.animation.transformEnd.translate);
			writer.Value(groupName, "Transform終了時復元", data.animation.restoreTransformOnExit);
		}
		// 移動
		{
			writer.Value(groupName, "コンボ中の移動慣性", data.move.inheritMoveInertia);
			// 移動慣性に使用する地上摩擦と空気抵抗をコンボ単位で保存する
			writer.Value(groupName, "移動慣性の地上摩擦係数", data.move.moveInertiaGroundFriction);
			writer.Value(groupName, "移動慣性の空気抵抗係数", data.move.moveInertiaAirResistance);
			writer.Value(groupName, "コンボ中の重力", data.move.isGravity);
			writer.Value(groupName, "コンボ中の重力強度", data.move.gravityScale);
			writer.Value(groupName, "コンボ中の最大落下速度", data.move.maxFallSpeed);
			writer.Value(groupName, "コンボ開始時に重力速度リセット", data.move.isResetGravity);

			writer.Value(groupName, "移動スピード", data.move.moveSpeed);
			writer.Value(groupName, "コンボ中の移動開始時間", data.move.moveWindow.startTime);
			writer.Value(groupName, "コンボ中の移動終了時間", data.move.moveWindow.endTime);
			writer.Value(groupName, "コンボ中の移動強制", data.move.isCompulsionMove);
			writer.EnumValue(groupName, "コンボ中の移動タイプ", data.move.moveType, "MoveType");
			writer.Value(groupName, "ソフトロックオン半径", data.move.lockOnData.softLockRadius);
			writer.Value(groupName, "ロックオン半径", data.move.lockOnData.lockOnRadius);
			writer.Value(groupName, "ロックオンターゲット接近半径", data.move.lockOnData.moveTargetRadius);

			writer.EnumValue(groupName, "ロックオンタイプ", data.move.lockOnData.type, "LockOnType");
			writer.EnumValue(groupName, "ターゲット移動タイプ", data.move.lockOnData.targetMoveType, "TargetMoveType");


			writer.Value(groupName, "コンボ中の移動方向", data.move.localMoveVector);
			writer.Value(groupName, "コンボ中の移動毎フレーム方向を更新", data.move.isUpdateDirectionEachFrame);
			writer.Value(groupName, "コンボ中の移動毎フレームターゲット位置を更新", data.move.isUpdateTargetPositionEachFrame);
			writer.Value(groupName, "コンボ中の移動方向を正規化してから使うか", data.move.isNormalizeLocalMove);
			writer.EnumValue(groupName, "コンボ中の移動速度カーブ", data.move.speedCurveType, "MoveSpeedCurveType");
			writer.Value(groupName, "コンボ中の移動速度カーブ強度", data.move.speedCurvePower);
			writer.Value(groupName, "コンボ中の縦方向移動", data.move.isVerticalMove);
			writer.Value(groupName, "コンボ中の移動基準前方を水平化", data.move.isFlattenTargetDirection);
			writer.Value(groupName, "移動方向とキャラクターの向く方向を一致させるか", data.move.alignCharacterToMovement);

			// ターゲット有無で移動の味付けを変えるための上書き設定
			writer.Value(groupName, "ターゲットあり移動上書き", data.move.targetMove.enabled);
			writer.Value(groupName, "ターゲットあり移動スピード", data.move.targetMove.moveSpeed);
			writer.Value(groupName, "ターゲットあり移動方向", data.move.targetMove.localMoveVector);
			writer.Value(groupName, "ターゲットあり移動方向を正規化してから使うか", data.move.targetMove.isNormalizeLocalMove);
			writer.EnumValue(groupName, "ターゲットありターゲット移動タイプ", data.move.targetMove.targetMoveType, "TargetMoveType");
			writer.Value(groupName, "ターゲットあり接近距離半径", data.move.targetMove.moveTargetRadius);
			writer.Value(groupName, "ターゲットなし移動上書き", data.move.noTargetMove.enabled);
			writer.Value(groupName, "ターゲットなし移動スピード", data.move.noTargetMove.moveSpeed);
			writer.Value(groupName, "ターゲットなし移動方向", data.move.noTargetMove.localMoveVector);
			writer.Value(groupName, "ターゲットなし移動方向を正規化してから使うか", data.move.noTargetMove.isNormalizeLocalMove);

		}
		// ヒットボックス
		{
			writer.Value(groupName, "親オブジェクト名前", data.hitBox.parentName);
			writer.Value(groupName, "ヒットボックス発生時間", data.hitBox.windowStart);
			writer.Value(groupName, "ヒットボックス生存時間", data.hitBox.lifeTime);
			writer.Value(groupName, "ヒットボックスヒット記録を使用", data.hitBox.useContactRecord);
			writer.Value(groupName, "ヒットボックスコライダー別ヒット記録", data.hitBox.recordPerCollider);
			writer.Value(groupName, "ヒットボックスコライダーサイズ", data.hitBox.colliderSize);		// new
			writer.Value(groupName, "ヒットボックスオフセット位置", data.hitBox.offset);		// new
			writer.Value(groupName, "ヒットボックスコライダー半径", data.hitBox.radius);
			writer.EnumValue(groupName, "ヒットボックス軌道方式", data.hitBox.trajectoryType, "HitBoxTrajectoryType");
			writer.Value(groupName, "ヒットボックス軌道時間", data.hitBox.trajectoryDuration);
			writer.Value(groupName, "ヒットボックス軌道点0", data.hitBox.trajectoryPoint0);
			writer.Value(groupName, "ヒットボックス軌道点1", data.hitBox.trajectoryPoint1);
			writer.Value(groupName, "ヒットボックス軌道点2", data.hitBox.trajectoryPoint2);
			writer.Value(groupName, "ヒットボックス軌道点3", data.hitBox.trajectoryPoint3);
			writer.Value(groupName, "ヒットボックス軌道中心", data.hitBox.trajectoryOrbitCenter);
			writer.Value(groupName, "ヒットボックス軌道半径", data.hitBox.trajectoryOrbitRadius);
			writer.Value(groupName, "ヒットボックス軌道高さ", data.hitBox.trajectoryOrbitHeight);
			writer.Value(groupName, "ヒットボックス軌道開始角", data.hitBox.trajectoryOrbitStartAngle);
			writer.Value(groupName, "ヒットボックス軌道終了角", data.hitBox.trajectoryOrbitEndAngle);				// new

			writer.EnumValue(groupName, "ヒットボックス発生条件タイプ", data.hitBox.spawnType, "HitBoxSpawnType");
			writer.EnumValue(groupName, "ヒットボックス依存先タイプ", data.hitBox.dependenceType, "HitBoxParentType");
			writer.EnumValue(groupName, "ヒットボックス影響タイプ", data.hitBox.hitEffectType, "HitBoxHitEffectType");
			writer.EnumValue(groupName, "ヒットボックス生存タイプ", data.hitBox.lifetimeType, "HitBoxLifetimeType");
			writer.EnumValue(groupName, "ヒットボックス形状タイプ", data.hitBox.shapeType, "HitBoxShapeType");
			writer.EnumValue(groupName, "ヒットボックス使用者タイプ", data.hitBox.useType, "HitBoxUseType");
			writer.EnumValue(groupName, "コライダー(タグ)", data.hitBox.tag, "CollisionTag");
			writer.EnumValue(groupName, "コライダー(レイヤー)", data.hitBox.layer, "CollisionLayer");
			writer.EnumValue(groupName, "コライダー(マスク)", data.hitBox.mask, "CollisionMask");
		}
		// リアクション
		{
			writer.Value(groupName, "ダメージ", data.hitReaction.damageData.GetOne().GetDamage());
			writer.Value(groupName, "Y方向ノックバック", data.hitReaction.isVerticalBoost);
			writer.Value(groupName, "ノックバック力", data.hitReaction.power);
			writer.Value(groupName, "Y方向ノックバック力", data.hitReaction.verticalBoost);
			writer.Value(groupName, "ノックバック持続時間", data.hitReaction.duration);
			writer.EnumValue(groupName, "ヒットリアクションタイプ", data.hitReaction.type, "HitReactionType");
			writer.EnumValue(groupName, "ヒットストップ方針(自分)", data.hitReaction.selfHitStopPolicy, "SelfHitStopPolicy");
			writer.EnumValue(groupName, "攻撃属性", data.hitReaction.attribute, "AttackAttribute");
			writer.Value(groupName, "ヒット優先度", data.hitReaction.hitPriority);
			writer.Value(groupName, "ヒットカメラシェイク量", data.hitReaction.cameraShakePower);
			writer.Value(groupName, "ヒットスタン持続時間", data.hitReaction.hitStunTime);
			writer.Value(groupName, "ダウン持続時間", data.hitReaction.downTime);
			writer.Value(groupName, "打ち上げ持続時間", data.hitReaction.launchFloatTime);
			writer.Value(groupName, "打ち上げ最大高度", data.hitReaction.launchMaxHeight);
			writer.Value(groupName, "ヒット重力", data.hitReaction.gravityEnabled);
			writer.Value(groupName, "ヒット重力倍率", data.hitReaction.gravityScale);

			writer.Value(groupName, "ヒットストップ(相手)", data.hitReaction.targetHitStopTime);
			writer.Value(groupName, "ヒットストップ(自分)", data.hitReaction.selfHitStopTime);
			writer.Value(groupName, "ヒットストップ(一回のみ)", overwrite ?
				data.hitReaction.selfHitStopPolicy == SelfHitStopPolicy::FirstHitOnly : data.hitReaction.isSingleHitStop);


			writer.Value(groupName, kHitEffectCountKey, static_cast<int>(data.hitReaction.hitEffectNames.size()));

			for (int i = 0; i < static_cast<int>(data.hitReaction.hitEffectNames.size()); ++i) {
				writer.Value(groupName, MakeHitEffectSlotKey(i), data.hitReaction.hitEffectNames[i].slotName);
				writer.Value(groupName, MakeHitEffectNameKey(i), data.hitReaction.hitEffectNames[i].effectName);
			}
		}
		// カメラ
		{
			writer.Value(groupName, "カメラロックオン", data.camera.isLockOn);
			writer.Value(groupName, "カメラロックオン補間速度", data.camera.lockOnInterpolation);
			writer.Value(groupName, "カメラロックオン回転引き継ぎ", data.camera.isLockOnRotate);
			writer.Value(groupName, "カメラロックオン解除時間", data.camera.lockOnEndTime);

			writer.Value(groupName, "カメラ切り替え", data.camera.isChangeCamera);
			writer.Value(groupName, "カメラ切り替え名", data.camera.cameraName);
			writer.Value(groupName, "カメラ切り替え開始時間", data.camera.changeCameraStartTime);
			writer.Value(groupName, "カメラ切り替え補間時間", data.camera.interpolation);
			
			writer.Value(groupName, "カメラズーム", data.camera.isZoom);
			writer.Value(groupName, "カメラロックオンズーム", data.camera.isLockOnZoom);
			writer.Value(groupName, "カメラズーム速度", data.camera.zoomSpeed);
			writer.Value(groupName, "カメラズーム時間", data.camera.zoomDuration);
			writer.Value(groupName, "カメラズーム開始時間", data.camera.zoomStartTime);
			writer.Value(groupName, "カメラズーム量", data.camera.zoomTargetDistance);

			writer.Value(groupName, "カメラシェイク", data.camera.isShake);
			writer.Value(groupName, "カメラシェイク量", data.camera.shakeCameraPower);
			writer.Value(groupName, "カメラシェイク開始時間", data.camera.shakeStartTime);
			writer.Value(groupName, "カメラシェイク時間", data.camera.shakeDuration);
			writer.Value(groupName, "カメラシェイク幅", data.camera.shakeOffset);

			writer.Value(groupName, "カメラ注視点オフセット", data.camera.isActionTargetOffset);
			writer.Value(groupName, "カメラ注視点オフセット開始時間", data.camera.actionTargetOffsetStartTime);
			writer.Value(groupName, "カメラ注視点オフセット時間", data.camera.actionTargetOffsetDuration);
			writer.Value(groupName, "カメラ注視点オフセット補間速度", data.camera.actionTargetOffsetBlendSpeed);
			writer.Value(groupName, "カメラ注視点オフセット量", data.camera.actionTargetOffset);

			writer.Value(groupName, "カメラ先読み", data.camera.isLookAhead);
			writer.Value(groupName, "カメラ先読み開始時間", data.camera.lookAheadStartTime);
			writer.Value(groupName, "カメラ先読み時間", data.camera.lookAheadDuration);
			writer.Value(groupName, "カメラ先読み距離", data.camera.lookAheadDistance);
			writer.Value(groupName, "カメラ先読み最小速度", data.camera.lookAheadMinSpeed);
			writer.Value(groupName, "カメラ先読み最大速度", data.camera.lookAheadMaxSpeed);
			writer.Value(groupName, "カメラ先読み補間速度", data.camera.lookAheadSmoothSpeed);

			writer.Value(groupName, "カメラ速度ズーム", data.camera.isSpeedZoom);
			writer.Value(groupName, "カメラ速度ズーム開始時間", data.camera.speedZoomStartTime);
			writer.Value(groupName, "カメラ速度ズーム時間", data.camera.speedZoomDuration);
			writer.Value(groupName, "カメラ速度ズーム最小速度", data.camera.speedZoomMinSpeed);
			writer.Value(groupName, "カメラ速度ズーム最大速度", data.camera.speedZoomMaxSpeed);
			writer.Value(groupName, "カメラ速度ズーム近距離補正", data.camera.speedZoomNearOffsetZ);
			writer.Value(groupName, "カメラ速度ズーム遠距離補正", data.camera.speedZoomFarOffsetZ);
			writer.Value(groupName, "カメラ速度ズーム補間速度", data.camera.speedZoomSmoothSpeed);

			// 命中を契機に再生するカメラ演出をコンボデータへ登録する
			writer.Value(groupName, "ヒット時カメラ演出", data.camera.isHitCameraEffect);
			writer.Value(groupName, "ヒット時カメラシェイク", data.camera.isHitShake);
			writer.Value(groupName, "ヒット時カメラシェイク時間", data.camera.hitShakeDuration);
			writer.Value(groupName, "ヒット時カメラシェイク幅", data.camera.hitShakeOffset);
			writer.Value(groupName, "ヒット時カメラズーム", data.camera.isHitZoom);
			writer.Value(groupName, "ヒット時カメラズーム距離", data.camera.hitZoomTargetDistance);
			writer.Value(groupName, "ヒット時カメラズーム速度", data.camera.hitZoomSpeed);
			writer.Value(groupName, "ヒット時カメラズーム時間", data.camera.hitZoomDuration);

		}
	}
	void Combo::System::GetGlobalComboData(const std::string& name, GlobalData& data) {
		// 攻撃タイプと遠距離攻撃
		{
			data.type = globalVariables->GetEnumValue<Combo::Type>(name, "コンボ攻撃タイプ");
			data.action.useCustomStaminaCost = globalVariables->GetValue<bool>(name, "スタミナコスト個別指定");
			data.action.incrementHitCount = globalVariables->GetValue<bool>(name, "ヒットカウントを増やす");
			data.action.incrementHitCountOnce = globalVariables->GetValue<bool>(name, "ヒットカウントを一度だけ増やす");
			data.action.staminaCost = globalVariables->GetValue<float>(name, "スタミナコスト");
			data.action.cooldown = globalVariables->GetValue<float>(name, "クールダウン");
			data.action.requiredAirRemainCount = globalVariables->GetValue<int>(name, "空中残り回数要求");
			data.action.superArmor = globalVariables->GetValue<bool>(name, "スーパーアーマー");
			data.action.invincible = globalVariables->GetValue<bool>(name, "無敵");
			data.action.guardPoint = globalVariables->GetValue<bool>(name, "ガードポイント");
			data.action.cancelOnHitOnly = globalVariables->GetValue<bool>(name, "ヒット時のみキャンセル");
			data.action.cancelOnMissOnly = globalVariables->GetValue<bool>(name, "ミス時のみキャンセル");
			data.action.landingCancel = globalVariables->GetValue<bool>(name, "着地時のみキャンセル");
			data.action.hitPauseScale = globalVariables->GetValue<float>(name, "ヒットポーズ倍率");
			data.action.cameraShakePower = globalVariables->GetValue<float>(name, "カメラシェイク量");
			data.audio.attackSoundName = globalVariables->GetValue<std::string>(name, "攻撃音");
			data.audio.attackStartTime = globalVariables->GetValue<float>(name, "攻撃音再生時間");
			data.audio.attackVolume = globalVariables->GetValue<float>(name, "攻撃音音量");
			data.audio.hitSoundName = globalVariables->GetValue<std::string>(name, "ヒット音");
			data.audio.hitVolume = globalVariables->GetValue<float>(name, "ヒット音音量");
			data.audio.finishSoundName = globalVariables->GetValue<std::string>(name, "攻撃終了音");
			data.audio.finishVolume = globalVariables->GetValue<float>(name, "攻撃終了音音量");
			data.range.rangeType = globalVariables->GetEnumValue<Combo::RangeType>(name, "遠距離タイプ");
			data.range.lockOnType = globalVariables->GetEnumValue<Combo::RangeLockOnType>(name, "遠距離狙いタイプ");
			data.range.offsetTargetType = globalVariables->GetEnumValue<Combo::RangeOffsetTargetType>(name, "遠距離オフセットタイプ");
			data.range.throwMoveType = globalVariables->GetEnumValue<Combo::RangeThrowMoveType>(name, "投擲移動タイプ");
			data.range.effectTriggerType = globalVariables->GetEnumValue<Combo::RangeEffectTriggerType>(name, "投擲効果発動条件");
			data.range.recallTriggerType = globalVariables->GetEnumValue<Combo::RangeRecallTriggerType>(name, "投擲回収条件");
			data.range.lockOnStartRadius = globalVariables->GetValue<float>(name, "遠距離ロックオン開始半径");
			data.range.offsetTarget = globalVariables->GetValue<Vector3>(name, "遠距離オフセットターゲット");
			data.range.throwLerpTime = globalVariables->GetValue<float>(name, "投擲補間時間");
			data.range.effectTriggerTime = globalVariables->GetValue<float>(name, "投擲効果発動時間");
			data.range.recallTriggerTime = globalVariables->GetValue<float>(name, "投擲回収時間");
			data.range.recallNearRadius = globalVariables->GetValue<float>(name, "投擲回収近接半径");
			data.range.throwStayTime = globalVariables->GetValue<float>(name, "投擲停滞時間");
			data.range.rangeWindowStart = globalVariables->GetValue<float>(name, "遠距離発射開始時間");
			data.range.rangeWindowEnd = globalVariables->GetValue<float>(name, "遠距離発射終了時間");
			data.range.speed = globalVariables->GetValue<float>(name, "遠距離弾速");
			data.range.interval = globalVariables->GetValue<float>(name, "遠距離発射間隔");
			data.range.count = globalVariables->GetValue<int>(name, "遠距離発射数");
			data.range.damage = globalVariables->GetValue<float>(name, "遠距離ダメージ");
			data.range.subWeaponIdleOffset = globalVariables->GetValue<Vector3>(name, "サブ武器待機位置");
			data.range.subWeaponStartOffset = globalVariables->GetValue<Vector3>(name, "サブ武器投擲開始オフセット");
			data.range.subWeaponThrowSpeed = globalVariables->GetValue<float>(name, "サブ武器投擲速度");
			data.range.subWeaponThrowLifeTime = globalVariables->GetValue<float>(name, "サブ武器投擲時間");
			data.range.subWeaponReturnTime = globalVariables->GetValue<float>(name, "サブ武器戻り時間");
			data.range.subWeaponSpinSpeed = globalVariables->GetValue<float>(name, "サブ武器回転速度");
			data.range.subWeaponAlignToDirection = globalVariables->GetValue<bool>(name, "サブ武器投擲方向に向ける");
			data.range.subWeaponUseSpin = globalVariables->GetValue<bool>(name, "サブ武器スピン");
			data.range.subWeaponRotateOffset = globalVariables->GetValue<Vector3>(name, "サブ武器回転オフセット");
		}

		// エフェクト
		{
			data.effect.weaponDraw = globalVariables->GetValue<bool>(name, "エフェクト(武器表示)");
			data.effect.comboEffects.clear();

			const int comboEffectCount = globalVariables->GetValue<int>(name, kComboEffectCountKey);
			for (int i = 0; i < comboEffectCount; ++i) {
				ComboEffectEntry entry{};
				entry.effectName = globalVariables->GetValue<std::string>(name, MakeComboEffectNameKey(i));
				entry.startTime = globalVariables->GetValue<float>(name, MakeComboEffectStartTimeKey(i));
				entry.offset = globalVariables->GetValue<Vector3>(name, MakeComboEffectOffsetKey(i));
				if (globalVariables->HasKey(name, MakeComboEffectPositionKey(i))) {
					entry.transformPosition = globalVariables->GetValue<Vector3>(name, MakeComboEffectPositionKey(i));
					entry.transformRotation = globalVariables->GetValue<Vector3>(name, MakeComboEffectRotationKey(i));
					entry.transformScale = globalVariables->GetValue<Vector3>(name, MakeComboEffectScaleKey(i));
					entry.requiredHitCount = globalVariables->GetValue<int>(name, MakeComboEffectRequiredHitCountKey(i));
					entry.inputType = globalVariables->GetEnumValue<ComboEffectInputType>(name, MakeComboEffectInputTypeKey(i));
				}
				if (globalVariables->HasKey(name, MakeComboEffectTypeKey(i))) {
					entry.type = globalVariables->GetEnumValue<ComboEffectType>(name, MakeComboEffectTypeKey(i));
					entry.trailTexture = globalVariables->GetValue<std::string>(name, MakeComboEffectTrailTextureKey(i));
					const Vector4 trailColor = globalVariables->GetValue<Vector4>(name, MakeComboEffectTrailColorKey(i));
			entry.trailColor = { trailColor.x, trailColor.y, trailColor.z, trailColor.w };
					entry.trailOffsetStart = globalVariables->GetValue<Vector3>(name, MakeComboEffectTrailOffsetStartKey(i));
					entry.trailOffsetEnd = globalVariables->GetValue<Vector3>(name, MakeComboEffectTrailOffsetEndKey(i));
					entry.trailLifeTime = globalVariables->GetValue<float>(name, MakeComboEffectTrailLifeTimeKey(i));
					if (globalVariables->HasKey(name, MakeComboEffectTrailWidthStartKey(i))) {
                        if (globalVariables->HasKey(name, MakeComboEffectTrailMaxSegmentsKey(i))) {
                                entry.trailSettings.minEmitDistance = globalVariables->GetValue<float>(name, MakeComboEffectTrailMinDistanceKey(i));
                                entry.trailSettings.maxSegmentCount = static_cast<size_t>(globalVariables->GetValue<int>(name, MakeComboEffectTrailMaxSegmentsKey(i)));
                                entry.trailSettings.useSpline = globalVariables->GetValue<bool>(name, MakeComboEffectTrailUseSplineKey(i));
                                entry.trailSettings.splineSubdivision = globalVariables->GetValue<int>(name, MakeComboEffectTrailSubdivisionKey(i));
                        }
						entry.trailSettings.widthStart = globalVariables->GetValue<float>(name, MakeComboEffectTrailWidthStartKey(i));
						entry.trailSettings.widthEnd = globalVariables->GetValue<float>(name, MakeComboEffectTrailWidthEndKey(i));
						entry.trailSettings.fadeInTime = globalVariables->GetValue<float>(name, MakeComboEffectTrailFadeInKey(i));
						entry.trailSettings.fadeOutTime = globalVariables->GetValue<float>(name, MakeComboEffectTrailFadeOutKey(i));
						entry.trailSettings.alphaCurve = globalVariables->GetValue<Vector4>(name, MakeComboEffectTrailAlphaCurveKey(i));
						entry.trailSettings.uvScrollSpeed = globalVariables->GetValue<Vector2>(name, MakeComboEffectTrailUvSpeedKey(i));
						entry.trailSettings.emissionStrength = globalVariables->GetValue<float>(name, MakeComboEffectTrailEmissionKey(i));
						entry.trailSettings.flipTexture = globalVariables->GetValue<bool>(name, MakeComboEffectTrailFlipKey(i));
						entry.trailSettings.resetMode = globalVariables->GetEnumValue<Engine::TrailResetMode>(name, MakeComboEffectTrailResetKey(i));
					}
					entry.trajectory.type = globalVariables->GetEnumValue<Engine::TrailTrajectoryType>(name, MakeComboEffectTrajectoryTypeKey(i));
					entry.trajectory.duration = globalVariables->GetValue<float>(name, MakeComboEffectTrajectoryDurationKey(i));
					entry.trajectory.point0 = globalVariables->GetValue<Vector3>(name, MakeComboEffectTrajectoryPoint0Key(i));
					entry.trajectory.point1 = globalVariables->GetValue<Vector3>(name, MakeComboEffectTrajectoryPoint1Key(i));
					entry.trajectory.point2 = globalVariables->GetValue<Vector3>(name, MakeComboEffectTrajectoryPoint2Key(i));
					entry.trajectory.point3 = globalVariables->GetValue<Vector3>(name, MakeComboEffectTrajectoryPoint3Key(i));
							entry.trajectory.orbitCenter = globalVariables->GetValue<Vector3>(name, MakeComboEffectTrajectoryOrbitCenterKey(i));
							entry.trajectory.orbitRadius = globalVariables->GetValue<float>(name, MakeComboEffectTrajectoryOrbitRadiusKey(i));
							entry.trajectory.orbitHeight = globalVariables->GetValue<float>(name, MakeComboEffectTrajectoryOrbitHeightKey(i));
							entry.trajectory.orbitStartAngle = globalVariables->GetValue<float>(name, MakeComboEffectTrajectoryOrbitStartAngleKey(i));
							entry.trajectory.orbitEndAngle = globalVariables->GetValue<float>(name, MakeComboEffectTrajectoryOrbitEndAngleKey(i));
				}
				if (globalVariables->HasKey(name, MakeComboEffectParentKey(i))) {
					entry.parentName = globalVariables->GetValue<std::string>(name, MakeComboEffectParentKey(i));
				}
				if (globalVariables->HasKey(name, MakeComboEffectTriggerTypeKey(i))) {
					entry.triggerType = globalVariables->GetEnumValue<ComboEffectTriggerType>(name, MakeComboEffectTriggerTypeKey(i));
				}
				if (globalVariables->HasKey(name, MakeComboEffectEndTimeKey(i))) {
					entry.endTime = globalVariables->GetValue<float>(name, MakeComboEffectEndTimeKey(i));
				}
				else {
					entry.endTime = entry.startTime;
				}
				if (globalVariables->HasKey(name, MakeComboEffectIntervalKey(i))) {
					entry.interval = globalVariables->GetValue<float>(name, MakeComboEffectIntervalKey(i));
				}
				data.effect.comboEffects.push_back(entry);
			}
		}
		// 接続
		{
			data.connection.lightAttack = globalVariables->GetValue<std::string>(name, "接続先(弱攻撃)");
			data.connection.heavyAttack = globalVariables->GetValue<std::string>(name, "接続先(強攻撃)");
			data.connection.skill = globalVariables->GetValue<std::string>(name, "接続先(スキル)");
			data.connection.lightCondition.groundMiss = globalVariables->GetValue<std::string>(name, "接続先(弱攻撃-地上-未ヒット)");
			data.connection.lightCondition.groundHit = globalVariables->GetValue<std::string>(name, "接続先(弱攻撃-地上-ヒット)");
			data.connection.lightCondition.airMiss = globalVariables->GetValue<std::string>(name, "接続先(弱攻撃-空中-未ヒット)");
			data.connection.lightCondition.airHit = globalVariables->GetValue<std::string>(name, "接続先(弱攻撃-空中-ヒット)");
			data.connection.lightCondition.lockOn = globalVariables->GetValue<std::string>(name, "接続先(弱攻撃-ロックオン中)");
			data.connection.lightCondition.noLockOn = globalVariables->GetValue<std::string>(name, "接続先(弱攻撃-ロックオンなし)");
			data.connection.heavyCondition.groundMiss = globalVariables->GetValue<std::string>(name, "接続先(強攻撃-地上-未ヒット)");
			data.connection.heavyCondition.groundHit = globalVariables->GetValue<std::string>(name, "接続先(強攻撃-地上-ヒット)");
			data.connection.heavyCondition.airMiss = globalVariables->GetValue<std::string>(name, "接続先(強攻撃-空中-未ヒット)");
			data.connection.heavyCondition.airHit = globalVariables->GetValue<std::string>(name, "接続先(強攻撃-空中-ヒット)");
			data.connection.heavyCondition.lockOn = globalVariables->GetValue<std::string>(name, "接続先(強攻撃-ロックオン中)");
			data.connection.heavyCondition.noLockOn = globalVariables->GetValue<std::string>(name, "接続先(強攻撃-ロックオンなし)");
			data.connection.skillCondition.groundMiss = globalVariables->GetValue<std::string>(name, "接続先(スキル-地上-未ヒット)");
			data.connection.skillCondition.groundHit = globalVariables->GetValue<std::string>(name, "接続先(スキル-地上-ヒット)");
			data.connection.skillCondition.airMiss = globalVariables->GetValue<std::string>(name, "接続先(スキル-空中-未ヒット)");
			data.connection.skillCondition.airHit = globalVariables->GetValue<std::string>(name, "接続先(スキル-空中-ヒット)");
			data.connection.skillCondition.lockOn = globalVariables->GetValue<std::string>(name, "接続先(スキル-ロックオン中)");
			data.connection.skillCondition.noLockOn = globalVariables->GetValue<std::string>(name, "接続先(スキル-ロックオンなし)");
		}
		// 条件
		{
			data.condition.stateInput.startTime = globalVariables->GetValue<float>(name, "コンボ入力受付開始時間");
			data.condition.stateInput.endTime = globalVariables->GetValue<float>(name, "コンボ入力受付終了時間");
			data.condition.inputBufferTime = globalVariables->GetValue<float>(name, "コンボ入力バッファ時間");
			data.condition.stateEndTime = globalVariables->GetValue<float>(name, "コンボ終了時間");
			data.condition.stateNextTime = globalVariables->GetValue<float>(name, "コンボ移行時間");
			data.condition.stateCancel.startTime = globalVariables->GetValue<float>(name, "コンボキャンセル受付開始時間");
			data.condition.stateCancel.endTime = globalVariables->GetValue<float>(name, "コンボキャンセル受付終了時間");
			data.condition.stateMoveCancel.startTime = globalVariables->GetValue<float>(name, "コンボ移動キャンセル受付開始時間");
			data.condition.stateMoveCancel.endTime = globalVariables->GetValue<float>(name, "コンボ移動キャンセル受付終了時間");
			data.condition.endConditionType = globalVariables->GetEnumValue<Combo::EndConditionType>(name, "終了条件タイプ");
			data.condition.isCompulsionNext = globalVariables->GetValue<bool>(name, "コンボ強制移行");
			data.condition.isCancel = globalVariables->GetValue<bool>(name, "コンボキャンセル可能");
			data.condition.isMoveCancel = globalVariables->GetValue<bool>(name, "コンボ移動キャンセル可能");
			data.condition.inputDelay = globalVariables->GetValue<float>(name, "コンボ入力遅延");
		}
		// アニメーション
		{
			data.animation.animationName = globalVariables->GetValue<std::string>(name, "アニメーション名前");
			data.animation.animationSpeed = globalVariables->GetValue<float>(name, "アニメーション速度");
			data.animation.animationBlendTime = globalVariables->GetValue<float>(name, "アニメーション遷移時間");
			data.animation.animationLoop = globalVariables->GetValue<bool>(name, "アニメーションループ");
			data.animation.animationStop = globalVariables->GetValue<bool>(name, "アニメーションを一定時間で止めるか");
			data.animation.animationStopTime = globalVariables->GetValue<float>(name, "アニメーション停止時間");
			// 旧保存データに攻撃前移動アニメーション項目が無い場合は、構造体の既定値を使う
			if (globalVariables->HasKey(name, "攻撃前移動アニメーション使用")) {
				data.animation.usePreMoveAnimation = globalVariables->GetValue<bool>(name, "攻撃前移動アニメーション使用");
			}
			if (globalVariables->HasKey(name, "攻撃前移動アニメーション名前")) {
				data.animation.preMoveAnimationName = globalVariables->GetValue<std::string>(name, "攻撃前移動アニメーション名前");
			}
			if (globalVariables->HasKey(name, "攻撃前移動アニメーション速度")) {
				data.animation.preMoveAnimationSpeed = globalVariables->GetValue<float>(name, "攻撃前移動アニメーション速度");
			}
			if (globalVariables->HasKey(name, "攻撃前移動アニメーション終了時間")) {
				data.animation.preMoveAnimationEndTime = globalVariables->GetValue<float>(name, "攻撃前移動アニメーション終了時間");
			}
			if (globalVariables->HasKey(name, "攻撃前移動アニメーション遷移時間")) {
				data.animation.preMoveAnimationBlendTime = globalVariables->GetValue<float>(name, "攻撃前移動アニメーション遷移時間");
			}
			if (globalVariables->HasKey(name, "攻撃前移動アニメーションループ")) {
				data.animation.preMoveAnimationLoop = globalVariables->GetValue<bool>(name, "攻撃前移動アニメーションループ");
			}
			// 保存済みのTransformアニメーション設定を実行データへ読み込む
			data.animation.isTransformAnimation = globalVariables->GetValue<bool>(name, "Transformアニメーション使用");
			data.animation.transformStartTime = globalVariables->GetValue<float>(name, "Transformアニメーション開始時間");
			data.animation.transformEndTime = globalVariables->GetValue<float>(name, "Transformアニメーション終了時間");
			data.animation.transformStart.scale = globalVariables->GetValue<Vector3>(name, "Transform開始スケール加算値");
			data.animation.transformStart.rotate = globalVariables->GetValue<Vector3>(name, "Transform開始回転加算値");
			data.animation.transformStart.translate = globalVariables->GetValue<Vector3>(name, "Transform開始位置加算値");
			data.animation.transformEnd.scale = globalVariables->GetValue<Vector3>(name, "Transform終了スケール加算値");
			data.animation.transformEnd.rotate = globalVariables->GetValue<Vector3>(name, "Transform終了回転加算値");
			data.animation.transformEnd.translate = globalVariables->GetValue<Vector3>(name, "Transform終了位置加算値");
			data.animation.restoreTransformOnExit = globalVariables->GetValue<bool>(name, "Transform終了時復元");
		}
		// 移動
		{
			data.move.inheritMoveInertia = globalVariables->GetValue<bool>(name, "コンボ中の移動慣性");
			// 保存された係数を読み込み、実行中の慣性減衰へ反映する
			data.move.moveInertiaGroundFriction = globalVariables->GetValue<float>(name, "移動慣性の地上摩擦係数");
			data.move.moveInertiaAirResistance = globalVariables->GetValue<float>(name, "移動慣性の空気抵抗係数");
			data.move.isGravity = globalVariables->GetValue<bool>(name, "コンボ中の重力");
			data.move.gravityScale = globalVariables->GetValue<float>(name, "コンボ中の重力強度");
			data.move.maxFallSpeed = globalVariables->GetValue<float>(name, "コンボ中の最大落下速度");
			data.move.isResetGravity = globalVariables->GetValue<bool>(name, "コンボ開始時に重力速度リセット");

			data.move.moveSpeed = globalVariables->GetValue<Vector3>(name, "移動スピード");
			data.move.moveWindow.startTime = globalVariables->GetValue<float>(name, "コンボ中の移動開始時間");
			data.move.moveWindow.endTime = globalVariables->GetValue<float>(name, "コンボ中の移動終了時間");
			data.move.isCompulsionMove = globalVariables->GetValue<bool>(name, "コンボ中の移動強制");
			data.move.moveType = globalVariables->GetEnumValue<Combo::MoveType>(name, "コンボ中の移動タイプ");
			data.move.lockOnData.softLockRadius = globalVariables->GetValue<float>(name, "ソフトロックオン半径");
			data.move.lockOnData.lockOnRadius = globalVariables->GetValue<float>(name, "ロックオン半径");
			data.move.lockOnData.moveTargetRadius = globalVariables->GetValue<float>(name, "ロックオンターゲット接近半径");
			data.move.lockOnData.type = globalVariables->GetEnumValue<LockOnType>(name, "ロックオンタイプ");
			data.move.lockOnData.targetMoveType = globalVariables->GetEnumValue<TargetMoveType>(name, "ターゲット移動タイプ");
			

			data.move.localMoveVector = globalVariables->GetValue<Vector3>(name, "コンボ中の移動方向");
			data.move.isUpdateDirectionEachFrame = globalVariables->GetValue<bool>(name, "コンボ中の移動毎フレーム方向を更新");
			data.move.isUpdateTargetPositionEachFrame = globalVariables->GetValue<bool>(name, "コンボ中の移動毎フレームターゲット位置を更新");
			data.move.isNormalizeLocalMove = globalVariables->GetValue<bool>(name, "コンボ中の移動方向を正規化してから使うか");
			data.move.speedCurveType = globalVariables->GetEnumValue<MoveSpeedCurveType>(name, "コンボ中の移動速度カーブ");
			data.move.speedCurvePower = globalVariables->GetValue<float>(name, "コンボ中の移動速度カーブ強度");
			data.move.isVerticalMove = globalVariables->GetValue<bool>(name, "コンボ中の縦方向移動");
			data.move.isFlattenTargetDirection = globalVariables->GetValue<bool>(name, "コンボ中の移動基準前方を水平化");
			data.move.alignCharacterToMovement = globalVariables->GetValue<bool>(name, "移動方向とキャラクターの向く方向を一致させるか");

			// ターゲット有無で切り替える移動上書きを読み込む
			data.move.targetMove.enabled = globalVariables->GetValue<bool>(name, "ターゲットあり移動上書き");
			data.move.targetMove.moveSpeed = globalVariables->GetValue<Vector3>(name, "ターゲットあり移動スピード");
			data.move.targetMove.localMoveVector = globalVariables->GetValue<Vector3>(name, "ターゲットあり移動方向");
			data.move.targetMove.isNormalizeLocalMove = globalVariables->GetValue<bool>(name, "ターゲットあり移動方向を正規化してから使うか");
			data.move.targetMove.targetMoveType = globalVariables->GetEnumValue<TargetMoveType>(name, "ターゲットありターゲット移動タイプ");
			data.move.targetMove.moveTargetRadius = globalVariables->GetValue<float>(name, "ターゲットあり接近距離半径");
			data.move.noTargetMove.enabled = globalVariables->GetValue<bool>(name, "ターゲットなし移動上書き");
			data.move.noTargetMove.moveSpeed = globalVariables->GetValue<Vector3>(name, "ターゲットなし移動スピード");
			data.move.noTargetMove.localMoveVector = globalVariables->GetValue<Vector3>(name, "ターゲットなし移動方向");
			data.move.noTargetMove.isNormalizeLocalMove = globalVariables->GetValue<bool>(name, "ターゲットなし移動方向を正規化してから使うか");

		}
		// リアクション
		{
			data.hitReaction.damageData.GetOne().SetDamage(globalVariables->GetValue<float>(name, "ダメージ"));
			data.hitReaction.isVerticalBoost = globalVariables->GetValue<bool>(name, "Y方向ノックバック");
			data.hitReaction.power = globalVariables->GetValue<float>(name, "ノックバック力");
			data.hitReaction.verticalBoost = globalVariables->GetValue<float>(name, "Y方向ノックバック力");
			data.hitReaction.duration = globalVariables->GetValue<float>(name, "ノックバック持続時間");
			data.hitReaction.type = globalVariables->GetEnumValue<HitReactionType>(name, "ヒットリアクションタイプ");
			data.hitReaction.selfHitStopPolicy = globalVariables->GetEnumValue<SelfHitStopPolicy>(name, "ヒットストップ方針(自分)");
			data.hitReaction.attribute = globalVariables->GetEnumValue<AttackAttribute>(name, "攻撃属性");
			data.hitReaction.hitPriority = globalVariables->GetValue<int>(name, "ヒット優先度");
			data.hitReaction.cameraShakePower = globalVariables->GetValue<float>(name, "ヒットカメラシェイク量");
			data.hitReaction.hitStunTime = globalVariables->GetValue<float>(name, "ヒットスタン持続時間");
			data.hitReaction.downTime = globalVariables->GetValue<float>(name, "ダウン持続時間");
			data.hitReaction.launchFloatTime = globalVariables->GetValue<float>(name, "打ち上げ持続時間");
			data.hitReaction.launchMaxHeight = globalVariables->GetValue<float>(name, "打ち上げ最大高度");
			data.hitReaction.gravityEnabled = globalVariables->GetValue<bool>(name, "ヒット重力");
			data.hitReaction.gravityScale = globalVariables->GetValue<float>(name, "ヒット重力倍率");


			data.hitReaction.targetHitStopTime = globalVariables->GetValue<float>(name, "ヒットストップ(相手)");
			data.hitReaction.selfHitStopTime =  globalVariables->GetValue<float>(name, "ヒットストップ(自分)");
			data.hitReaction.isSingleHitStop = globalVariables->GetValue<bool>(name, "ヒットストップ(一回のみ)");
			// 旧データ互換: 以前の false は「自分側ヒットストップなし」として扱う。
			if (!data.hitReaction.isSingleHitStop &&
				data.hitReaction.selfHitStopPolicy == SelfHitStopPolicy::FirstHitOnly) {
				data.hitReaction.selfHitStopPolicy = SelfHitStopPolicy::None;
			}


			data.hitReaction.hitEffectNames.clear();

			const int effectCount = globalVariables->GetValue<int>(name, kHitEffectCountKey);
			for (int i = 0; i < effectCount; ++i) {
				HitEffectEntry entry{};
				entry.slotName = globalVariables->GetValue<std::string>(name, MakeHitEffectSlotKey(i));
				entry.effectName = globalVariables->GetValue<std::string>(name, MakeHitEffectNameKey(i));

				data.hitReaction.hitEffectNames.push_back(entry);
			}
		}
		// ヒットボックス
		{
			data.hitBox.parentName = globalVariables->GetValue<std::string>(name, "親オブジェクト名前");
			data.hitBox.windowStart = globalVariables->GetValue<float>(name, "ヒットボックス発生時間");
			data.hitBox.lifeTime = globalVariables->GetValue<float>(name, "ヒットボックス生存時間");
			data.hitBox.useContactRecord = globalVariables->GetValue<bool>(name, "ヒットボックスヒット記録を使用");
			data.hitBox.recordPerCollider = globalVariables->GetValue<bool>(name, "ヒットボックスコライダー別ヒット記録");
			data.hitBox.colliderSize = globalVariables->GetValue<Vector3>(name, "ヒットボックスコライダーサイズ");
			data.hitBox.offset = globalVariables->GetValue<Vector3>(name, "ヒットボックスオフセット位置");
			data.hitBox.radius = globalVariables->GetValue<float>(name, "ヒットボックスコライダー半径");
			// 新しい軌道項目がない旧データは、従来動作のkNoneで補完する。
			if (globalVariables->HasKey(name, "ヒットボックス軌道方式")) {
				data.hitBox.trajectoryType = globalVariables->GetEnumValue<HitBox::TrajectoryType>(name, "ヒットボックス軌道方式");
				data.hitBox.trajectoryDuration = globalVariables->GetValue<float>(name, "ヒットボックス軌道時間");
				data.hitBox.trajectoryPoint0 = globalVariables->GetValue<Vector3>(name, "ヒットボックス軌道点0");
				data.hitBox.trajectoryPoint1 = globalVariables->GetValue<Vector3>(name, "ヒットボックス軌道点1");
				data.hitBox.trajectoryPoint2 = globalVariables->GetValue<Vector3>(name, "ヒットボックス軌道点2");
				data.hitBox.trajectoryPoint3 = globalVariables->GetValue<Vector3>(name, "ヒットボックス軌道点3");
				data.hitBox.trajectoryOrbitCenter = globalVariables->GetValue<Vector3>(name, "ヒットボックス軌道中心");
				data.hitBox.trajectoryOrbitRadius = globalVariables->GetValue<float>(name, "ヒットボックス軌道半径");
				data.hitBox.trajectoryOrbitHeight = globalVariables->GetValue<float>(name, "ヒットボックス軌道高さ");
				data.hitBox.trajectoryOrbitStartAngle = globalVariables->GetValue<float>(name, "ヒットボックス軌道開始角");
				data.hitBox.trajectoryOrbitEndAngle = globalVariables->GetValue<float>(name, "ヒットボックス軌道終了角");
			}

			data.hitBox.spawnType = globalVariables->GetEnumValue<HitBox::SpawnType>(name, "ヒットボックス発生条件タイプ");
			data.hitBox.dependenceType = globalVariables->GetEnumValue<HitBox::ParentType>(name, "ヒットボックス依存先タイプ");
			data.hitBox.hitEffectType = globalVariables->GetEnumValue<HitBox::HitEffectType>(name, "ヒットボックス影響タイプ");
			data.hitBox.lifetimeType = globalVariables->GetEnumValue<HitBox::LifetimeType>(name, "ヒットボックス生存タイプ");
			data.hitBox.shapeType = globalVariables->GetEnumValue<HitBox::ShapeType>(name, "ヒットボックス形状タイプ");
			data.hitBox.useType = globalVariables->GetEnumValue<HitBox::UseType>(name, "ヒットボックス使用者タイプ");


			data.hitBox.tag = globalVariables->GetEnumValue<CollisionTag>(name, "コライダー(タグ)");
			data.hitBox.layer = globalVariables->GetEnumValue<CollisionLayer>(name, "コライダー(レイヤー)");
			data.hitBox.mask = globalVariables->GetEnumValue<CollisionLayer>(name, "コライダー(マスク)");
		}
		// カメラ
		{
			data.camera.isLockOn = globalVariables->GetValue<bool>(name, "カメラロックオン");
			data.camera.lockOnInterpolation = globalVariables->GetValue<float>(name, "カメラロックオン補間速度");
			data.camera.isLockOnRotate = globalVariables->GetValue<bool>(name, "カメラロックオン回転引き継ぎ");
			data.camera.lockOnEndTime = globalVariables->GetValue<float>(name, "カメラロックオン解除時間");

			data.camera.isChangeCamera = globalVariables->GetValue<bool>(name, "カメラ切り替え");
			data.camera.cameraName = globalVariables->GetValue<std::string>(name, "カメラ切り替え名");
			data.camera.changeCameraStartTime = globalVariables->GetValue<float>(name, "カメラ切り替え開始時間");
			data.camera.interpolation = globalVariables->GetValue<float>(name, "カメラ切り替え補間時間");

			data.camera.isZoom = globalVariables->GetValue<bool>(name, "カメラズーム");
			data.camera.isLockOnZoom = globalVariables->GetValue<bool>(name, "カメラロックオンズーム");
			data.camera.zoomSpeed = globalVariables->GetValue<float>(name, "カメラズーム速度");
			data.camera.zoomDuration = globalVariables->GetValue<float>(name, "カメラズーム時間");
			data.camera.zoomStartTime = globalVariables->GetValue<float>(name, "カメラズーム開始時間");
			data.camera.zoomTargetDistance = globalVariables->GetValue<float>(name, "カメラズーム量");

			data.camera.isShake = globalVariables->GetValue<bool>(name, "カメラシェイク");
			data.camera.shakeCameraPower = globalVariables->GetValue<float>(name, "カメラシェイク量");
			data.camera.shakeStartTime = globalVariables->GetValue<float>(name, "カメラシェイク開始時間");
			data.camera.shakeDuration = globalVariables->GetValue<float>(name, "カメラシェイク時間");
			data.camera.shakeOffset = globalVariables->GetValue<Vector3>(name, "カメラシェイク幅");

			data.camera.isActionTargetOffset = globalVariables->GetValue<bool>(name, "カメラ注視点オフセット");
			data.camera.actionTargetOffsetStartTime = globalVariables->GetValue<float>(name, "カメラ注視点オフセット開始時間");
			data.camera.actionTargetOffsetDuration = globalVariables->GetValue<float>(name, "カメラ注視点オフセット時間");
			data.camera.actionTargetOffsetBlendSpeed = globalVariables->GetValue<float>(name, "カメラ注視点オフセット補間速度");
			data.camera.actionTargetOffset = globalVariables->GetValue<Vector3>(name, "カメラ注視点オフセット量");

			data.camera.isLookAhead = globalVariables->GetValue<bool>(name, "カメラ先読み");
			data.camera.lookAheadStartTime = globalVariables->GetValue<float>(name, "カメラ先読み開始時間");
			data.camera.lookAheadDuration = globalVariables->GetValue<float>(name, "カメラ先読み時間");
			data.camera.lookAheadDistance = globalVariables->GetValue<float>(name, "カメラ先読み距離");
			data.camera.lookAheadMinSpeed = globalVariables->GetValue<float>(name, "カメラ先読み最小速度");
			data.camera.lookAheadMaxSpeed = globalVariables->GetValue<float>(name, "カメラ先読み最大速度");
			data.camera.lookAheadSmoothSpeed = globalVariables->GetValue<float>(name, "カメラ先読み補間速度");

			data.camera.isSpeedZoom = globalVariables->GetValue<bool>(name, "カメラ速度ズーム");
			data.camera.speedZoomStartTime = globalVariables->GetValue<float>(name, "カメラ速度ズーム開始時間");
			data.camera.speedZoomDuration = globalVariables->GetValue<float>(name, "カメラ速度ズーム時間");
			data.camera.speedZoomMinSpeed = globalVariables->GetValue<float>(name, "カメラ速度ズーム最小速度");
			data.camera.speedZoomMaxSpeed = globalVariables->GetValue<float>(name, "カメラ速度ズーム最大速度");
			data.camera.speedZoomNearOffsetZ = globalVariables->GetValue<float>(name, "カメラ速度ズーム近距離補正");
			data.camera.speedZoomFarOffsetZ = globalVariables->GetValue<float>(name, "カメラ速度ズーム遠距離補正");
			data.camera.speedZoomSmoothSpeed = globalVariables->GetValue<float>(name, "カメラ速度ズーム補間速度");

			// 保存済みの命中時カメラ演出をランタイムデータへ復元する
			data.camera.isHitCameraEffect = globalVariables->GetValue<bool>(name, "ヒット時カメラ演出");
			data.camera.isHitShake = globalVariables->GetValue<bool>(name, "ヒット時カメラシェイク");
			data.camera.hitShakeDuration = globalVariables->GetValue<float>(name, "ヒット時カメラシェイク時間");
			data.camera.hitShakeOffset = globalVariables->GetValue<Vector3>(name, "ヒット時カメラシェイク幅");
			data.camera.isHitZoom = globalVariables->GetValue<bool>(name, "ヒット時カメラズーム");
			data.camera.hitZoomTargetDistance = globalVariables->GetValue<float>(name, "ヒット時カメラズーム距離");
			data.camera.hitZoomSpeed = globalVariables->GetValue<float>(name, "ヒット時カメラズーム速度");
			data.camera.hitZoomDuration = globalVariables->GetValue<float>(name, "ヒット時カメラズーム時間");

		}
	}

	void System::SetGlobalComboData(const std::string& name, GlobalData& data) {
		// 共通の項目一覧を上書きモードで保存する
		WriteGlobalComboData(name, data, true);
	}
	void System::SetGlobalComboDatas() {
		for (auto& pair : comboGlobalDatas_) {
			SetGlobalComboData(pair.first, pair.second);
		}
	};

	void System::SetData(ComboData& data, const GlobalData& gData) {
		// 攻撃タイプと遠距離攻撃
		data.GetActionData() = gData.action;
		// 音声設定と共有AudioManagerを実行用コンボへ適用する。
		data.GetComboAudio().GetData() = gData.audio;
		data.GetComboAudio().Initialize(audioManager_);
		data.SetType(gData.type);
		data.GetComboRange().GetData() = gData.range;
		// ヒットボックスとリアクションデータ 
		data.GetComboHitBox().GetCollData() = HitBox::CollData{ "",gData.hitBox ,gData.hitReaction };
		data.GetComboHitBox().SetPerent(GetParentTransform(gData.hitBox.parentName));
		// モーション
		data.GetComboMotion().GetComboMove().GetData() = gData.move;
		// アニメーションスピード
		data.GetComboMotion().GetComboAnimation().GetData() = gData.animation;
		// 条件
		data.GetComboCondition().GetData() = gData.condition;
		// エフェクト
		data.GetComboEffect().GetData() = gData.effect;
		data.GetComboEffect().SetParentTransforms(parentTransforms_);
		// カメラ
		data.GetComboCamera().GetData() = gData.camera;
		// コンボボタン設定
		ComboButton bo = ComboButton(ComboGamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed);
		ComboButton bo2 = ComboButton(ComboGamePadButton::GAMEPAD_X, ComboButtonInputType::kPressed);
		ComboButton bo3 = ComboButton(ComboGamePadButton::GAMEPAD_Y, ComboButtonInputType::kPressed);

		// 押し続ける
		std::vector<ComboButton> button;
		button.push_back(bo);
		button.push_back(bo2);
		button.push_back(bo3);
		data.GetComboCondition().GetNextReceiver().SetButton(button);
	}

	void System::CreateCombo(const std::string& comboNodeName) {
		// コンボデータ
		ComboData data{};
		// グローバルデータ作成
		CreateGlobalData(comboNodeName);
		// データ設定
		SetData(data, comboGlobalDatas_[comboNodeName]);
		// コンボノード追加
		AddComboNode(comboNodeName, data.GetComboMotion().GetComboAnimation().GetData().animationName, data);
	}

	void System::CreateCombo(const std::string& comboNodeName, const GlobalData& sourceData) {
		if (comboGlobalDatas_.find(comboNodeName) != comboGlobalDatas_.end() ||
			comboNodes_.find(comboNodeName) != comboNodes_.end()) {
			return;
		}

		comboGlobalDatas_[comboNodeName] = sourceData;
		ApplyGlobalComboData(comboNodeName, comboGlobalDatas_[comboNodeName]);
		SetGlobalComboData(comboNodeName, comboGlobalDatas_[comboNodeName]);

		ComboData data{};
		SetData(data, comboGlobalDatas_[comboNodeName]);
		AddComboNode(comboNodeName, data.GetComboMotion().GetComboAnimation().GetData().animationName, data);
		ConnectSavedCombos();
	}

	void System::CreateGlobalData(const std::string& comboNodeName) {
		// 既に存在する場合は追加しない
		if (comboGlobalDatas_.find(comboNodeName) != comboGlobalDatas_.end()) {
			return;
		}
		comboGlobalDatas_[comboNodeName] = GlobalData(); // グローバルデータ追加
		ApplyGlobalComboData(comboNodeName, comboGlobalDatas_[comboNodeName]); // グローバルデータ適応
		GetGlobalComboData(comboNodeName, comboGlobalDatas_[comboNodeName]); // グローバルデータ取得
	}

	void System::ConnectSavedCombos() {
		for (const auto& [nodeName, data] : comboGlobalDatas_) {
			auto connectConditional = [&](ActionInput input, const GlobalConditionalConnection& targets) {
				if (!targets.lockOn.empty()) {
					ConnectCombo(nodeName, input, targets.lockOn, TransitionCondition::LockOn);
				}
				if (!targets.noLockOn.empty()) {
					ConnectCombo(nodeName, input, targets.noLockOn, TransitionCondition::NoLockOn);
				}
				if (!targets.groundMiss.empty()) {
					ConnectCombo(nodeName, input, targets.groundMiss, TransitionCondition::GroundMiss);
				}
				if (!targets.groundHit.empty()) {
					ConnectCombo(nodeName, input, targets.groundHit, TransitionCondition::GroundHit);
				}
				if (!targets.airMiss.empty()) {
					ConnectCombo(nodeName, input, targets.airMiss, TransitionCondition::AirMiss);
				}
				if (!targets.airHit.empty()) {
					ConnectCombo(nodeName, input, targets.airHit, TransitionCondition::AirHit);
				}
			};
			if (!data.connection.lightAttack.empty()) {
				ConnectCombo(nodeName, ActionInput::LightAttack, data.connection.lightAttack);
			}
			if (!data.connection.heavyAttack.empty()) {
				ConnectCombo(nodeName, ActionInput::HeavyAttack, data.connection.heavyAttack);
			}
			if (!data.connection.skill.empty()) {
				ConnectCombo(nodeName, ActionInput::Skill, data.connection.skill);
			}
			connectConditional(ActionInput::LightAttack, data.connection.lightCondition);
			connectConditional(ActionInput::HeavyAttack, data.connection.heavyCondition);
			connectConditional(ActionInput::Skill, data.connection.skillCondition);
		}
	}

	void System::Create(const std::string name) {
		this->name = name;
		globalVariables->CreateGroup(name);

		globalVariables->AddItem(name, kGroundLightStartKey, groundLightStart_);
		globalVariables->AddItem(name, kAirLightStartKey, airLightStart_);
		globalVariables->AddItem(name, kGroundHeavyStartKey, groundHeavyStart_);
		globalVariables->AddItem(name, kAirHeavyStartKey, airHeavyStart_);
		globalVariables->AddItem(name, kGroundSkillStartKey, groundSkillStart_);
		globalVariables->AddItem(name, kAirSkillStartKey, airSkillStart_);
		globalVariables->AddItem(name, kDodgeLightStartKey, dodgeLightStart_);
		globalVariables->AddItem(name, kDodgeHeavyStartKey, dodgeHeavyStart_);
		globalVariables->AddItem(name, kDodgeSkillStartKey, dodgeSkillStart_);
		globalVariables->AddItem(name, kDodgeSuccessLightStartKey, dodgeSuccessLightStart_);
		globalVariables->AddItem(name, kDodgeSuccessHeavyStartKey, dodgeSuccessHeavyStart_);
		globalVariables->AddItem(name, kDodgeSuccessSkillStartKey, dodgeSuccessSkillStart_);
		groundLightStart_ = globalVariables->GetValue<std::string>(name, kGroundLightStartKey);
		airLightStart_ = globalVariables->GetValue<std::string>(name, kAirLightStartKey);
		groundHeavyStart_ = globalVariables->GetValue<std::string>(name, kGroundHeavyStartKey);
		airHeavyStart_ = globalVariables->GetValue<std::string>(name, kAirHeavyStartKey);
		groundSkillStart_ = globalVariables->GetValue<std::string>(name, kGroundSkillStartKey);
		airSkillStart_ = globalVariables->GetValue<std::string>(name, kAirSkillStartKey);
		dodgeLightStart_ = globalVariables->GetValue<std::string>(name, kDodgeLightStartKey);
		dodgeHeavyStart_ = globalVariables->GetValue<std::string>(name, kDodgeHeavyStartKey);
		dodgeSkillStart_ = globalVariables->GetValue<std::string>(name, kDodgeSkillStartKey);
		dodgeSuccessLightStart_ = globalVariables->GetValue<std::string>(name, kDodgeSuccessLightStartKey);
		dodgeSuccessHeavyStart_ = globalVariables->GetValue<std::string>(name, kDodgeSuccessHeavyStartKey);
		dodgeSuccessSkillStart_ = globalVariables->GetValue<std::string>(name, kDodgeSuccessSkillStartKey);

		for (auto& data : globalVariables->GetGroupData(name)) {
			if (IsStartComboKey(data.first)) {
				continue;
			}
			CreateCombo(globalVariables->GetValue<std::string>(name, data.first));
		}

		ConnectSavedCombos();
	}

#pragma endregion // 保存　適応

};

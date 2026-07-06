#include "ComboSystem.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>
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

	void System::NotifyAttackHit() {
		if (comboStateMachine_) {
			comboStateMachine_->NotifyCurrentStateHit();
		}
	}

	void System::ClearNode() {

		// StateMachineの状態を完全リセット
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

		// 攻撃タイプと遠距離攻撃
		{
			globalVariables->AddItem(name, "スタミナコスト個別指定", data.action.useCustomStaminaCost);
			globalVariables->AddItem(name, "スタミナコスト", data.action.staminaCost);
			globalVariables->AddItem(name, "クールダウン", data.action.cooldown);
			globalVariables->AddItem(name, "空中残り回数要求", data.action.requiredAirRemainCount);
			globalVariables->AddItem(name, "スーパーアーマー", data.action.superArmor);
			globalVariables->AddItem(name, "無敵", data.action.invincible);
			globalVariables->AddItem(name, "ガードポイント", data.action.guardPoint);
			globalVariables->AddItem(name, "ヒット時のみキャンセル", data.action.cancelOnHitOnly);
			globalVariables->AddItem(name, "ミス時のみキャンセル", data.action.cancelOnMissOnly);
			globalVariables->AddItem(name, "着地時のみキャンセル", data.action.landingCancel);
			globalVariables->AddItem(name, "ヒットポーズ倍率", data.action.hitPauseScale);
			globalVariables->AddItem(name, "カメラシェイク量", data.action.cameraShakePower);
			globalVariables->AddItem(name, "攻撃音", data.audio.attackSoundName);
			globalVariables->AddItem(name, "攻撃音再生時間", data.audio.attackStartTime);
			globalVariables->AddItem(name, "攻撃音音量", data.audio.attackVolume);
			globalVariables->AddItem(name, "ヒット音", data.audio.hitSoundName);
			globalVariables->AddItem(name, "ヒット音音量", data.audio.hitVolume);
			globalVariables->AddItem(name, "攻撃終了音", data.audio.finishSoundName);
			globalVariables->AddItem(name, "攻撃終了音音量", data.audio.finishVolume);

			globalVariables->AddEnumItem(name, "コンボ攻撃タイプ", data.type, "ComboType");
			globalVariables->AddEnumItem(name, "遠距離タイプ", data.range.rangeType, "RangeType");
			globalVariables->AddEnumItem(name, "遠距離狙いタイプ", data.range.lockOnType, "RangeLockOnType");
			globalVariables->AddEnumItem(name, "遠距離オフセットタイプ", data.range.offsetTargetType, "RangeOffsetTargetType");
			globalVariables->AddEnumItem(name, "投擲移動タイプ", data.range.throwMoveType, "RangeThrowMoveType");
			globalVariables->AddEnumItem(name, "投擲効果発動条件", data.range.effectTriggerType, "RangeEffectTriggerType");
			globalVariables->AddEnumItem(name, "投擲回収条件", data.range.recallTriggerType, "RangeRecallTriggerType");
			globalVariables->AddItem(name, "遠距離ロックオン開始半径", data.range.lockOnStartRadius);
			globalVariables->AddItem(name, "遠距離オフセットターゲット", data.range.offsetTarget);
			globalVariables->AddItem(name, "投擲補間時間", data.range.throwLerpTime);
			globalVariables->AddItem(name, "投擲効果発動時間", data.range.effectTriggerTime);
			globalVariables->AddItem(name, "投擲回収時間", data.range.recallTriggerTime);
			globalVariables->AddItem(name, "投擲回収近接半径", data.range.recallNearRadius);
			globalVariables->AddItem(name, "投擲停滞時間", data.range.throwStayTime);
			globalVariables->AddItem(name, "遠距離発射開始時間", data.range.rangeWindowStart);
			globalVariables->AddItem(name, "遠距離発射終了時間", data.range.rangeWindowEnd);
			globalVariables->AddItem(name, "遠距離弾速", data.range.speed);
			globalVariables->AddItem(name, "遠距離発射間隔", data.range.interval);
			globalVariables->AddItem(name, "遠距離発射数", data.range.count);
			globalVariables->AddItem(name, "遠距離ダメージ", data.range.damage);
			globalVariables->AddItem(name, "サブ武器待機位置", data.range.subWeaponIdleOffset);
			globalVariables->AddItem(name, "サブ武器投擲開始オフセット", data.range.subWeaponStartOffset);
			globalVariables->AddItem(name, "サブ武器投擲速度", data.range.subWeaponThrowSpeed);
			globalVariables->AddItem(name, "サブ武器投擲時間", data.range.subWeaponThrowLifeTime);
			globalVariables->AddItem(name, "サブ武器戻り時間", data.range.subWeaponReturnTime);
			globalVariables->AddItem(name, "サブ武器回転速度", data.range.subWeaponSpinSpeed);
			globalVariables->AddItem(name, "サブ武器投擲方向に向ける", data.range.subWeaponAlignToDirection);
			globalVariables->AddItem(name, "サブ武器スピン", data.range.subWeaponUseSpin);
			globalVariables->AddItem(name, "サブ武器回転オフセット", data.range.subWeaponRotateOffset);
		}

		// エフェクト
		{
			globalVariables->AddItem(name, "エフェクト(トレイル)発生時間", data.effect.trailEffectStartTime);
			globalVariables->AddItem(name, "エフェクト(トレイル)生存時間", data.effect.trailEffectLifeTime);
			globalVariables->AddItem(name, "エフェクト(武器表示)", data.effect.weaponDraw);
			globalVariables->AddItem(name, kComboEffectCountKey, static_cast<int>(data.effect.comboEffects.size()));

			for (int i = 0; i < static_cast<int>(data.effect.comboEffects.size()); ++i) {
				globalVariables->AddItem(name, MakeComboEffectNameKey(i), data.effect.comboEffects[i].effectName);
				globalVariables->AddItem(name, MakeComboEffectParentKey(i), data.effect.comboEffects[i].parentName);
				globalVariables->AddEnumItem(name, MakeComboEffectTriggerTypeKey(i), data.effect.comboEffects[i].triggerType, "ComboEffectTriggerType");
				globalVariables->AddItem(name, MakeComboEffectStartTimeKey(i), data.effect.comboEffects[i].startTime);
				globalVariables->AddItem(name, MakeComboEffectEndTimeKey(i), data.effect.comboEffects[i].endTime);
				globalVariables->AddItem(name, MakeComboEffectIntervalKey(i), data.effect.comboEffects[i].interval);
				globalVariables->AddItem(name, MakeComboEffectOffsetKey(i), data.effect.comboEffects[i].offset);
			}
		}
		// 接続
		{
			globalVariables->AddItem(name, "接続先(弱攻撃)", data.connection.lightAttack);
			globalVariables->AddItem(name, "接続先(強攻撃)", data.connection.heavyAttack);
			globalVariables->AddItem(name, "接続先(スキル)", data.connection.skill);
			globalVariables->AddItem(name, "接続先(弱攻撃-地上-未ヒット)", data.connection.lightCondition.groundMiss);
			globalVariables->AddItem(name, "接続先(弱攻撃-地上-ヒット)", data.connection.lightCondition.groundHit);
			globalVariables->AddItem(name, "接続先(弱攻撃-空中-未ヒット)", data.connection.lightCondition.airMiss);
			globalVariables->AddItem(name, "接続先(弱攻撃-空中-ヒット)", data.connection.lightCondition.airHit);
			globalVariables->AddItem(name, "接続先(弱攻撃-ロックオン中)", data.connection.lightCondition.lockOn);
			globalVariables->AddItem(name, "接続先(弱攻撃-ロックオンなし)", data.connection.lightCondition.noLockOn);
			globalVariables->AddItem(name, "接続先(強攻撃-地上-未ヒット)", data.connection.heavyCondition.groundMiss);
			globalVariables->AddItem(name, "接続先(強攻撃-地上-ヒット)", data.connection.heavyCondition.groundHit);
			globalVariables->AddItem(name, "接続先(強攻撃-空中-未ヒット)", data.connection.heavyCondition.airMiss);
			globalVariables->AddItem(name, "接続先(強攻撃-空中-ヒット)", data.connection.heavyCondition.airHit);
			globalVariables->AddItem(name, "接続先(強攻撃-ロックオン中)", data.connection.heavyCondition.lockOn);
			globalVariables->AddItem(name, "接続先(強攻撃-ロックオンなし)", data.connection.heavyCondition.noLockOn);
			globalVariables->AddItem(name, "接続先(スキル-地上-未ヒット)", data.connection.skillCondition.groundMiss);
			globalVariables->AddItem(name, "接続先(スキル-地上-ヒット)", data.connection.skillCondition.groundHit);
			globalVariables->AddItem(name, "接続先(スキル-空中-未ヒット)", data.connection.skillCondition.airMiss);
			globalVariables->AddItem(name, "接続先(スキル-空中-ヒット)", data.connection.skillCondition.airHit);
			globalVariables->AddItem(name, "接続先(スキル-ロックオン中)", data.connection.skillCondition.lockOn);
			globalVariables->AddItem(name, "接続先(スキル-ロックオンなし)", data.connection.skillCondition.noLockOn);
		}
		// 条件
		{
			globalVariables->AddItem(name, "コンボ入力受付開始時間", data.condition.stateInput.startTime);
			globalVariables->AddItem(name, "コンボ入力受付終了時間", data.condition.stateInput.endTime);
			globalVariables->AddItem(name, "コンボ入力バッファ時間", data.condition.inputBufferTime);
			globalVariables->AddItem(name, "コンボ終了時間", data.condition.stateEndTime);
			globalVariables->AddItem(name, "コンボ移行時間", data.condition.stateNextTime);
			globalVariables->AddItem(name, "コンボキャンセル受付開始時間", data.condition.stateCancel.startTime);
			globalVariables->AddItem(name, "コンボキャンセル受付終了時間", data.condition.stateCancel.endTime);
			globalVariables->AddItem(name, "コンボ移動キャンセル受付開始時間", data.condition.stateMoveCancel.startTime);
			globalVariables->AddItem(name, "コンボ移動キャンセル受付終了時間", data.condition.stateMoveCancel.endTime);
			globalVariables->AddEnumItem(name, "終了条件タイプ", data.condition.endConditionType, "EndConditionType");
			globalVariables->AddItem(name, "コンボ強制移行", data.condition.isCompulsionNext);
			globalVariables->AddItem(name, "コンボキャンセル可能", data.condition.isCancel);
			globalVariables->AddItem(name, "コンボ移動キャンセル可能", data.condition.isMoveCancel);
			globalVariables->AddItem(name, "コンボ入力遅延", data.condition.inputDelay);
		}
		// アニメーション
		{
			globalVariables->AddItem(name, "アニメーション名前", data.animation.animationName);
			globalVariables->AddItem(name, "アニメーション速度", data.animation.animationSpeed);
			globalVariables->AddItem(name, "アニメーション遷移時間", data.animation.animationBlendTime);
			globalVariables->AddItem(name, "アニメーションループ", data.animation.animationLoop);
			globalVariables->AddItem(name, "アニメーションを一定時間で止めるか", data.animation.animationStop);
			globalVariables->AddItem(name, "アニメーション停止時間", data.animation.animationStopTime);
			// Transformアニメーション設定をコンボ単位で保存対象へ登録する
			globalVariables->AddItem(name, "Transformアニメーション使用", data.animation.isTransformAnimation);
			globalVariables->AddItem(name, "Transformアニメーション開始時間", data.animation.transformStartTime);
			globalVariables->AddItem(name, "Transformアニメーション終了時間", data.animation.transformEndTime);
			globalVariables->AddItem(name, "Transform開始スケール加算値", data.animation.transformStart.scale);
			globalVariables->AddItem(name, "Transform開始回転加算値", data.animation.transformStart.rotate);
			globalVariables->AddItem(name, "Transform開始位置加算値", data.animation.transformStart.translate);
			globalVariables->AddItem(name, "Transform終了スケール加算値", data.animation.transformEnd.scale);
			globalVariables->AddItem(name, "Transform終了回転加算値", data.animation.transformEnd.rotate);
			globalVariables->AddItem(name, "Transform終了位置加算値", data.animation.transformEnd.translate);
			globalVariables->AddItem(name, "Transform終了時復元", data.animation.restoreTransformOnExit);
		}
		// 移動
		{
			globalVariables->AddItem(name, "コンボ中の移動慣性", data.move.inheritMoveInertia);
			globalVariables->AddItem(name, "コンボ中の重力", data.move.isGravity);
			globalVariables->AddItem(name, "コンボ中の重力強度", data.move.gravityScale);
			globalVariables->AddItem(name, "コンボ中の最大落下速度", data.move.maxFallSpeed);
			globalVariables->AddItem(name, "コンボ開始時に重力速度リセット", data.move.isResetGravity);

			globalVariables->AddItem(name, "移動スピード", data.move.moveSpeed);
			globalVariables->AddItem(name, "コンボ中の移動開始時間", data.move.moveWindow.startTime);
			globalVariables->AddItem(name, "コンボ中の移動終了時間", data.move.moveWindow.endTime);
			globalVariables->AddItem(name, "コンボ中の移動強制", data.move.isCompulsionMove);
			globalVariables->AddEnumItem(name, "コンボ中の移動タイプ", data.move.moveType, "MoveType");
			globalVariables->AddItem(name, "ソフトロックオン半径", data.move.lockOnData.softLockRadius);
			globalVariables->AddItem(name, "ロックオン半径", data.move.lockOnData.lockOnRadius);
			globalVariables->AddItem(name, "ロックオンターゲット接近半径", data.move.lockOnData.moveTargetRadius);

			globalVariables->AddEnumItem(name, "ロックオンタイプ", data.move.lockOnData.type, "LockOnType");
			globalVariables->AddEnumItem(name, "ターゲット移動タイプ", data.move.lockOnData.targetMoveType, "TargetMoveType");


			globalVariables->AddItem(name, "コンボ中の移動方向", data.move.localMoveVector);
			globalVariables->AddItem(name, "コンボ中の移動毎フレーム方向を更新", data.move.isUpdateDirectionEachFrame);
			globalVariables->AddItem(name, "コンボ中の移動毎フレームターゲット位置を更新", data.move.isUpdateTargetPositionEachFrame);
			globalVariables->AddItem(name, "コンボ中の移動方向を正規化してから使うか", data.move.isNormalizeLocalMove);
			globalVariables->AddEnumItem(name, "コンボ中の移動速度カーブ", data.move.speedCurveType, "MoveSpeedCurveType");
			globalVariables->AddItem(name, "コンボ中の移動速度カーブ強度", data.move.speedCurvePower);
			globalVariables->AddItem(name, "コンボ中の縦方向移動", data.move.isVerticalMove);
			globalVariables->AddItem(name, "コンボ中の移動基準前方を水平化", data.move.isFlattenTargetDirection);
			globalVariables->AddItem(name, "移動方向とキャラクターの向く方向を一致させるか", data.move.alignCharacterToMovement);

			// ターゲット有無で移動の味付けを変えるための上書き設定
			globalVariables->AddItem(name, "ターゲットあり移動上書き", data.move.targetMove.enabled);
			globalVariables->AddItem(name, "ターゲットあり移動スピード", data.move.targetMove.moveSpeed);
			globalVariables->AddItem(name, "ターゲットあり移動方向", data.move.targetMove.localMoveVector);
			globalVariables->AddItem(name, "ターゲットあり移動方向を正規化してから使うか", data.move.targetMove.isNormalizeLocalMove);
			globalVariables->AddEnumItem(name, "ターゲットありターゲット移動タイプ", data.move.targetMove.targetMoveType, "TargetMoveType");
			globalVariables->AddItem(name, "ターゲットあり接近距離半径", data.move.targetMove.moveTargetRadius);
			globalVariables->AddItem(name, "ターゲットなし移動上書き", data.move.noTargetMove.enabled);
			globalVariables->AddItem(name, "ターゲットなし移動スピード", data.move.noTargetMove.moveSpeed);
			globalVariables->AddItem(name, "ターゲットなし移動方向", data.move.noTargetMove.localMoveVector);
			globalVariables->AddItem(name, "ターゲットなし移動方向を正規化してから使うか", data.move.noTargetMove.isNormalizeLocalMove);

		}
		// ヒットボックス
		{
			globalVariables->AddItem(name, "親オブジェクト名前", data.hitBox.parentName);
			globalVariables->AddItem(name, "ヒットボックス発生時間", data.hitBox.windowStart);
			globalVariables->AddItem(name, "ヒットボックス生存時間", data.hitBox.lifeTime);
			globalVariables->AddItem(name, "ヒットボックスヒット記録を使用", data.hitBox.useContactRecord);
			globalVariables->AddItem(name, "ヒットボックスコライダー別ヒット記録", data.hitBox.recordPerCollider);
			globalVariables->AddItem(name, "ヒットボックスコライダーサイズ", data.hitBox.colliderSize);		// new
			globalVariables->AddItem(name, "ヒットボックスオフセット位置", data.hitBox.offset);		// new
			globalVariables->AddItem(name, "ヒットボックスコライダー半径", data.hitBox.radius);				// new

			globalVariables->AddEnumItem(name, "ヒットボックス発生条件タイプ", data.hitBox.spawnType, "HitBoxSpawnType");
			globalVariables->AddEnumItem(name, "ヒットボックス依存先タイプ", data.hitBox.dependenceType, "HitBoxParentType");
			globalVariables->AddEnumItem(name, "ヒットボックス影響タイプ", data.hitBox.hitEffectType, "HitBoxHitEffectType");
			globalVariables->AddEnumItem(name, "ヒットボックス生存タイプ", data.hitBox.lifetimeType, "HitBoxLifetimeType");
			globalVariables->AddEnumItem(name, "ヒットボックス形状タイプ", data.hitBox.shapeType, "HitBoxShapeType");
			globalVariables->AddEnumItem(name, "ヒットボックス使用者タイプ", data.hitBox.useType, "HitBoxUseType");
			globalVariables->AddEnumItem(name, "コライダー(タグ)", data.hitBox.tag, "CollisionTag");
			globalVariables->AddEnumItem(name, "コライダー(レイヤー)", data.hitBox.layer, "CollisionLayer");
			globalVariables->AddEnumItem(name, "コライダー(マスク)", data.hitBox.mask, "CollisionMask");
		}
		// リアクション
		{
			globalVariables->AddItem(name, "ダメージ", data.hitReaction.damageData.GetOne().GetDamage());
			globalVariables->AddItem(name, "Y方向ノックバック", data.hitReaction.isVerticalBoost);
			globalVariables->AddItem(name, "ノックバック力", data.hitReaction.power);
			globalVariables->AddItem(name, "Y方向ノックバック力", data.hitReaction.verticalBoost);
			globalVariables->AddItem(name, "ノックバック持続時間", data.hitReaction.duration);
			globalVariables->AddEnumItem(name, "ヒットリアクションタイプ", data.hitReaction.type, "HitReactionType");
			globalVariables->AddEnumItem(name, "ヒットストップ方針(自分)", data.hitReaction.selfHitStopPolicy, "SelfHitStopPolicy");
			globalVariables->AddEnumItem(name, "攻撃属性", data.hitReaction.attribute, "AttackAttribute");
			globalVariables->AddItem(name, "ヒット優先度", data.hitReaction.hitPriority);
			globalVariables->AddItem(name, "ヒットカメラシェイク量", data.hitReaction.cameraShakePower);
			globalVariables->AddItem(name, "ヒットスタン持続時間", data.hitReaction.hitStunTime);
			globalVariables->AddItem(name, "ダウン持続時間", data.hitReaction.downTime);
			globalVariables->AddItem(name, "打ち上げ持続時間", data.hitReaction.launchFloatTime);
			globalVariables->AddItem(name, "打ち上げ最大高度", data.hitReaction.launchMaxHeight);
			globalVariables->AddItem(name, "ヒット重力", data.hitReaction.gravityEnabled);
			globalVariables->AddItem(name, "ヒット重力倍率", data.hitReaction.gravityScale);

			globalVariables->AddItem(name, "ヒットストップ(相手)", data.hitReaction.targetHitStopTime);
			globalVariables->AddItem(name, "ヒットストップ(自分)", data.hitReaction.selfHitStopTime);
			globalVariables->AddItem(name, "ヒットストップ(一回のみ)", data.hitReaction.isSingleHitStop);


			globalVariables->AddItem(name, kHitEffectCountKey, static_cast<int>(data.hitReaction.hitEffectNames.size()));

			for (int i = 0; i < static_cast<int>(data.hitReaction.hitEffectNames.size()); ++i) {
				globalVariables->AddItem(name, MakeHitEffectSlotKey(i), data.hitReaction.hitEffectNames[i].slotName);
				globalVariables->AddItem(name, MakeHitEffectNameKey(i), data.hitReaction.hitEffectNames[i].effectName);
			}
		}
		// カメラ
		{
			globalVariables->AddItem(name, "カメラロックオン", data.camera.isLockOn);
			globalVariables->AddItem(name, "カメラロックオン補間速度", data.camera.lockOnInterpolation);
			globalVariables->AddItem(name, "カメラロックオン回転引き継ぎ", data.camera.isLockOnRotate);
			globalVariables->AddItem(name, "カメラロックオン解除時間", data.camera.lockOnEndTime);

			globalVariables->AddItem(name, "カメラ切り替え", data.camera.isChangeCamera);
			globalVariables->AddItem(name, "カメラ切り替え名", data.camera.cameraName);
			globalVariables->AddItem(name, "カメラ切り替え開始時間", data.camera.changeCameraStartTime);
			globalVariables->AddItem(name, "カメラ切り替え補間時間", data.camera.interpolation);
			
			globalVariables->AddItem(name, "カメラズーム", data.camera.isZoom);
			globalVariables->AddItem(name, "カメラロックオンズーム", data.camera.isLockOnZoom);
			globalVariables->AddItem(name, "カメラズーム速度", data.camera.zoomSpeed);
			globalVariables->AddItem(name, "カメラズーム時間", data.camera.zoomDuration);
			globalVariables->AddItem(name, "カメラズーム開始時間", data.camera.zoomStartTime);
			globalVariables->AddItem(name, "カメラズーム量", data.camera.zoomTargetDistance);

			globalVariables->AddItem(name, "カメラシェイク", data.camera.isShake);
			globalVariables->AddItem(name, "カメラシェイク量", data.camera.shakeCameraPower);
			globalVariables->AddItem(name, "カメラシェイク開始時間", data.camera.shakeStartTime);
			globalVariables->AddItem(name, "カメラシェイク時間", data.camera.shakeDuration);
			globalVariables->AddItem(name, "カメラシェイク幅", data.camera.shakeOffset);

			globalVariables->AddItem(name, "カメラ注視点オフセット", data.camera.isActionTargetOffset);
			globalVariables->AddItem(name, "カメラ注視点オフセット開始時間", data.camera.actionTargetOffsetStartTime);
			globalVariables->AddItem(name, "カメラ注視点オフセット時間", data.camera.actionTargetOffsetDuration);
			globalVariables->AddItem(name, "カメラ注視点オフセット補間速度", data.camera.actionTargetOffsetBlendSpeed);
			globalVariables->AddItem(name, "カメラ注視点オフセット量", data.camera.actionTargetOffset);

			globalVariables->AddItem(name, "カメラ先読み", data.camera.isLookAhead);
			globalVariables->AddItem(name, "カメラ先読み開始時間", data.camera.lookAheadStartTime);
			globalVariables->AddItem(name, "カメラ先読み時間", data.camera.lookAheadDuration);
			globalVariables->AddItem(name, "カメラ先読み距離", data.camera.lookAheadDistance);
			globalVariables->AddItem(name, "カメラ先読み最小速度", data.camera.lookAheadMinSpeed);
			globalVariables->AddItem(name, "カメラ先読み最大速度", data.camera.lookAheadMaxSpeed);
			globalVariables->AddItem(name, "カメラ先読み補間速度", data.camera.lookAheadSmoothSpeed);

			globalVariables->AddItem(name, "カメラ速度ズーム", data.camera.isSpeedZoom);
			globalVariables->AddItem(name, "カメラ速度ズーム開始時間", data.camera.speedZoomStartTime);
			globalVariables->AddItem(name, "カメラ速度ズーム時間", data.camera.speedZoomDuration);
			globalVariables->AddItem(name, "カメラ速度ズーム最小速度", data.camera.speedZoomMinSpeed);
			globalVariables->AddItem(name, "カメラ速度ズーム最大速度", data.camera.speedZoomMaxSpeed);
			globalVariables->AddItem(name, "カメラ速度ズーム近距離補正", data.camera.speedZoomNearOffsetZ);
			globalVariables->AddItem(name, "カメラ速度ズーム遠距離補正", data.camera.speedZoomFarOffsetZ);
			globalVariables->AddItem(name, "カメラ速度ズーム補間速度", data.camera.speedZoomSmoothSpeed);

			// 命中を契機に再生するカメラ演出をコンボデータへ登録する
			globalVariables->AddItem(name, "ヒット時カメラ演出", data.camera.isHitCameraEffect);
			globalVariables->AddItem(name, "ヒット時カメラシェイク", data.camera.isHitShake);
			globalVariables->AddItem(name, "ヒット時カメラシェイク時間", data.camera.hitShakeDuration);
			globalVariables->AddItem(name, "ヒット時カメラシェイク幅", data.camera.hitShakeOffset);
			globalVariables->AddItem(name, "ヒット時カメラズーム", data.camera.isHitZoom);
			globalVariables->AddItem(name, "ヒット時カメラズーム距離", data.camera.hitZoomTargetDistance);
			globalVariables->AddItem(name, "ヒット時カメラズーム速度", data.camera.hitZoomSpeed);
			globalVariables->AddItem(name, "ヒット時カメラズーム時間", data.camera.hitZoomDuration);

		}



		// 保存項目の適応
		GetGlobalComboData(name, data);
	};

	void Combo::System::GetGlobalComboData(const std::string& name, GlobalData& data) {
		// 攻撃タイプと遠距離攻撃
		{
			data.type = globalVariables->GetEnumValue<Combo::Type>(name, "コンボ攻撃タイプ");
			data.action.useCustomStaminaCost = globalVariables->GetValue<bool>(name, "スタミナコスト個別指定");
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
			data.effect.trailEffectStartTime = globalVariables->GetValue<float>(name, "エフェクト(トレイル)発生時間");
			data.effect.trailEffectLifeTime = globalVariables->GetValue<float>(name, "エフェクト(トレイル)生存時間");
			data.effect.weaponDraw = globalVariables->GetValue<bool>(name, "エフェクト(武器表示)");
			data.effect.comboEffects.clear();

			const int comboEffectCount = globalVariables->GetValue<int>(name, kComboEffectCountKey);
			for (int i = 0; i < comboEffectCount; ++i) {
				ComboEffectEntry entry{};
				entry.effectName = globalVariables->GetValue<std::string>(name, MakeComboEffectNameKey(i));
				entry.startTime = globalVariables->GetValue<float>(name, MakeComboEffectStartTimeKey(i));
				entry.offset = globalVariables->GetValue<Vector3>(name, MakeComboEffectOffsetKey(i));
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
		// 攻撃タイプと遠距離攻撃
		{
			globalVariables->SetValue(name, "スタミナコスト個別指定", data.action.useCustomStaminaCost);
			globalVariables->SetValue(name, "スタミナコスト", data.action.staminaCost);
			globalVariables->SetValue(name, "クールダウン", data.action.cooldown);
			globalVariables->SetValue(name, "空中残り回数要求", data.action.requiredAirRemainCount);
			globalVariables->SetValue(name, "スーパーアーマー", data.action.superArmor);
			globalVariables->SetValue(name, "無敵", data.action.invincible);
			globalVariables->SetValue(name, "ガードポイント", data.action.guardPoint);
			globalVariables->SetValue(name, "ヒット時のみキャンセル", data.action.cancelOnHitOnly);
			globalVariables->SetValue(name, "ミス時のみキャンセル", data.action.cancelOnMissOnly);
			globalVariables->SetValue(name, "着地時のみキャンセル", data.action.landingCancel);
			globalVariables->SetValue(name, "ヒットポーズ倍率", data.action.hitPauseScale);
			globalVariables->SetValue(name, "カメラシェイク量", data.action.cameraShakePower);
			globalVariables->SetValue(name, "攻撃音", data.audio.attackSoundName);
			globalVariables->SetValue(name, "攻撃音再生時間", data.audio.attackStartTime);
			globalVariables->SetValue(name, "攻撃音音量", data.audio.attackVolume);
			globalVariables->SetValue(name, "ヒット音", data.audio.hitSoundName);
			globalVariables->SetValue(name, "ヒット音音量", data.audio.hitVolume);
			globalVariables->SetValue(name, "攻撃終了音", data.audio.finishSoundName);
			globalVariables->SetValue(name, "攻撃終了音音量", data.audio.finishVolume);

			globalVariables->SetEnumValue(name, "コンボ攻撃タイプ", data.type, "ComboType");
			globalVariables->SetEnumValue(name, "遠距離タイプ", data.range.rangeType, "RangeType");
			globalVariables->SetEnumValue(name, "遠距離狙いタイプ", data.range.lockOnType, "RangeLockOnType");
			globalVariables->SetEnumValue(name, "遠距離オフセットタイプ", data.range.offsetTargetType, "RangeOffsetTargetType");
			globalVariables->SetEnumValue(name, "投擲移動タイプ", data.range.throwMoveType, "RangeThrowMoveType");
			globalVariables->SetEnumValue(name, "投擲効果発動条件", data.range.effectTriggerType, "RangeEffectTriggerType");
			globalVariables->SetEnumValue(name, "投擲回収条件", data.range.recallTriggerType, "RangeRecallTriggerType");
			globalVariables->SetValue(name, "遠距離ロックオン開始半径", data.range.lockOnStartRadius);
			globalVariables->SetValue(name, "遠距離オフセットターゲット", data.range.offsetTarget);
			globalVariables->SetValue(name, "投擲補間時間", data.range.throwLerpTime);
			globalVariables->SetValue(name, "投擲効果発動時間", data.range.effectTriggerTime);
			globalVariables->SetValue(name, "投擲回収時間", data.range.recallTriggerTime);
			globalVariables->SetValue(name, "投擲回収近接半径", data.range.recallNearRadius);
			globalVariables->SetValue(name, "投擲停滞時間", data.range.throwStayTime);
			globalVariables->SetValue(name, "遠距離発射開始時間", data.range.rangeWindowStart);
			globalVariables->SetValue(name, "遠距離発射終了時間", data.range.rangeWindowEnd);
			globalVariables->SetValue(name, "遠距離弾速", data.range.speed);
			globalVariables->SetValue(name, "遠距離発射間隔", data.range.interval);
			globalVariables->SetValue(name, "遠距離発射数", data.range.count);
			globalVariables->SetValue(name, "遠距離ダメージ", data.range.damage);
			globalVariables->SetValue(name, "サブ武器待機位置", data.range.subWeaponIdleOffset);
			globalVariables->SetValue(name, "サブ武器投擲開始オフセット", data.range.subWeaponStartOffset);
			globalVariables->SetValue(name, "サブ武器投擲速度", data.range.subWeaponThrowSpeed);
			globalVariables->SetValue(name, "サブ武器投擲時間", data.range.subWeaponThrowLifeTime);
			globalVariables->SetValue(name, "サブ武器戻り時間", data.range.subWeaponReturnTime);
			globalVariables->SetValue(name, "サブ武器回転速度", data.range.subWeaponSpinSpeed);
			globalVariables->SetValue(name, "サブ武器投擲方向に向ける", data.range.subWeaponAlignToDirection);
			globalVariables->SetValue(name, "サブ武器スピン", data.range.subWeaponUseSpin);
			globalVariables->SetValue(name, "サブ武器回転オフセット", data.range.subWeaponRotateOffset);
		}

		// エフェクト
		{
			globalVariables->SetValue(name, "エフェクト(トレイル)発生時間", data.effect.trailEffectStartTime);
			globalVariables->SetValue(name, "エフェクト(トレイル)生存時間", data.effect.trailEffectLifeTime);
			globalVariables->SetValue(name, "エフェクト(武器表示)", data.effect.weaponDraw);
			globalVariables->SetValue(name, kComboEffectCountKey, static_cast<int>(data.effect.comboEffects.size()));

			for (int i = 0; i < static_cast<int>(data.effect.comboEffects.size()); ++i) {
				globalVariables->SetValue(name, MakeComboEffectNameKey(i), data.effect.comboEffects[i].effectName);
				globalVariables->SetValue(name, MakeComboEffectParentKey(i), data.effect.comboEffects[i].parentName);
				globalVariables->SetEnumValue(name, MakeComboEffectTriggerTypeKey(i), data.effect.comboEffects[i].triggerType, "ComboEffectTriggerType");
				globalVariables->SetValue(name, MakeComboEffectStartTimeKey(i), data.effect.comboEffects[i].startTime);
				globalVariables->SetValue(name, MakeComboEffectEndTimeKey(i), data.effect.comboEffects[i].endTime);
				globalVariables->SetValue(name, MakeComboEffectIntervalKey(i), data.effect.comboEffects[i].interval);
				globalVariables->SetValue(name, MakeComboEffectOffsetKey(i), data.effect.comboEffects[i].offset);
			}
		}
		// 接続
		{
			globalVariables->SetValue(name, "接続先(弱攻撃)", data.connection.lightAttack);
			globalVariables->SetValue(name, "接続先(強攻撃)", data.connection.heavyAttack);
			globalVariables->SetValue(name, "接続先(スキル)", data.connection.skill);
			globalVariables->SetValue(name, "接続先(弱攻撃-地上-未ヒット)", data.connection.lightCondition.groundMiss);
			globalVariables->SetValue(name, "接続先(弱攻撃-地上-ヒット)", data.connection.lightCondition.groundHit);
			globalVariables->SetValue(name, "接続先(弱攻撃-空中-未ヒット)", data.connection.lightCondition.airMiss);
			globalVariables->SetValue(name, "接続先(弱攻撃-空中-ヒット)", data.connection.lightCondition.airHit);
			globalVariables->SetValue(name, "接続先(弱攻撃-ロックオン中)", data.connection.lightCondition.lockOn);
			globalVariables->SetValue(name, "接続先(弱攻撃-ロックオンなし)", data.connection.lightCondition.noLockOn);
			globalVariables->SetValue(name, "接続先(強攻撃-地上-未ヒット)", data.connection.heavyCondition.groundMiss);
			globalVariables->SetValue(name, "接続先(強攻撃-地上-ヒット)", data.connection.heavyCondition.groundHit);
			globalVariables->SetValue(name, "接続先(強攻撃-空中-未ヒット)", data.connection.heavyCondition.airMiss);
			globalVariables->SetValue(name, "接続先(強攻撃-空中-ヒット)", data.connection.heavyCondition.airHit);
			globalVariables->SetValue(name, "接続先(強攻撃-ロックオン中)", data.connection.heavyCondition.lockOn);
			globalVariables->SetValue(name, "接続先(強攻撃-ロックオンなし)", data.connection.heavyCondition.noLockOn);
			globalVariables->SetValue(name, "接続先(スキル-地上-未ヒット)", data.connection.skillCondition.groundMiss);
			globalVariables->SetValue(name, "接続先(スキル-地上-ヒット)", data.connection.skillCondition.groundHit);
			globalVariables->SetValue(name, "接続先(スキル-空中-未ヒット)", data.connection.skillCondition.airMiss);
			globalVariables->SetValue(name, "接続先(スキル-空中-ヒット)", data.connection.skillCondition.airHit);
			globalVariables->SetValue(name, "接続先(スキル-ロックオン中)", data.connection.skillCondition.lockOn);
			globalVariables->SetValue(name, "接続先(スキル-ロックオンなし)", data.connection.skillCondition.noLockOn);
		}
		// 条件
		{
			globalVariables->SetValue(name, "コンボ入力受付開始時間", data.condition.stateInput.startTime);
			globalVariables->SetValue(name, "コンボ入力受付終了時間", data.condition.stateInput.endTime);
			globalVariables->SetValue(name, "コンボ入力バッファ時間", data.condition.inputBufferTime);
			globalVariables->SetValue(name, "コンボ終了時間", data.condition.stateEndTime);
			globalVariables->SetValue(name, "コンボ移行時間", data.condition.stateNextTime);
			globalVariables->SetValue(name, "コンボキャンセル受付開始時間", data.condition.stateCancel.startTime);
			globalVariables->SetValue(name, "コンボキャンセル受付終了時間", data.condition.stateCancel.endTime);
			globalVariables->SetValue(name, "コンボ移動キャンセル受付開始時間", data.condition.stateMoveCancel.startTime);
			globalVariables->SetValue(name, "コンボ移動キャンセル受付終了時間", data.condition.stateMoveCancel.endTime);	
			globalVariables->SetEnumValue(name, "終了条件タイプ", data.condition.endConditionType, "EndConditionType");
			globalVariables->SetValue(name, "コンボ強制移行", data.condition.isCompulsionNext);
			globalVariables->SetValue(name, "コンボキャンセル可能", data.condition.isCancel);
			globalVariables->SetValue(name, "コンボ移動キャンセル可能", data.condition.isMoveCancel);
			globalVariables->SetValue(name, "コンボ入力遅延", data.condition.inputDelay);
		}
		// アニメーション
		{
			globalVariables->SetValue(name, "アニメーション名前", data.animation.animationName);
			globalVariables->SetValue(name, "アニメーション速度", data.animation.animationSpeed);
			globalVariables->SetValue(name, "アニメーション遷移時間", data.animation.animationBlendTime);
			globalVariables->SetValue(name, "アニメーションループ", data.animation.animationLoop);
			globalVariables->SetValue(name, "アニメーションを一定時間で止めるか", data.animation.animationStop);
			globalVariables->SetValue(name, "アニメーション停止時間", data.animation.animationStopTime);
			// エディター上のTransformアニメーション設定を保存値へ反映する
			globalVariables->SetValue(name, "Transformアニメーション使用", data.animation.isTransformAnimation);
			globalVariables->SetValue(name, "Transformアニメーション開始時間", data.animation.transformStartTime);
			globalVariables->SetValue(name, "Transformアニメーション終了時間", data.animation.transformEndTime);
			globalVariables->SetValue(name, "Transform開始スケール加算値", data.animation.transformStart.scale);
			globalVariables->SetValue(name, "Transform開始回転加算値", data.animation.transformStart.rotate);
			globalVariables->SetValue(name, "Transform開始位置加算値", data.animation.transformStart.translate);
			globalVariables->SetValue(name, "Transform終了スケール加算値", data.animation.transformEnd.scale);
			globalVariables->SetValue(name, "Transform終了回転加算値", data.animation.transformEnd.rotate);
			globalVariables->SetValue(name, "Transform終了位置加算値", data.animation.transformEnd.translate);
			globalVariables->SetValue(name, "Transform終了時復元", data.animation.restoreTransformOnExit);


		}
		// 移動
		{
			globalVariables->SetValue(name, "コンボ中の移動慣性", data.move.inheritMoveInertia);
			globalVariables->SetValue(name, "コンボ中の移動開始時間", data.move.moveWindow.startTime);
			globalVariables->SetValue(name, "コンボ中の移動終了時間", data.move.moveWindow.endTime);
			globalVariables->SetValue(name, "移動スピード", data.move.moveSpeed);
			globalVariables->SetValue(name, "コンボ中の移動強制", data.move.isCompulsionMove);
			globalVariables->SetValue(name, "コンボ中の重力", data.move.isGravity);
			globalVariables->SetValue(name, "コンボ中の重力強度", data.move.gravityScale);
			globalVariables->SetValue(name, "コンボ中の最大落下速度", data.move.maxFallSpeed);
			globalVariables->SetValue(name, "コンボ開始時に重力速度リセット", data.move.isResetGravity);

			globalVariables->SetEnumValue(name, "コンボ中の移動タイプ", data.move.moveType, "MoveType");
			globalVariables->SetValue(name, "ソフトロックオン半径", data.move.lockOnData.softLockRadius);
			globalVariables->SetValue(name, "ロックオン半径", data.move.lockOnData.lockOnRadius);
			globalVariables->SetValue(name, "ロックオンターゲット接近半径", data.move.lockOnData.moveTargetRadius);

			globalVariables->SetEnumValue(name, "ロックオンタイプ", data.move.lockOnData.type, "LockOnType");
			globalVariables->SetEnumValue(name, "ターゲット移動タイプ", data.move.lockOnData.targetMoveType, "TargetMoveType");


			globalVariables->SetValue(name, "コンボ中の移動方向", data.move.localMoveVector);
			globalVariables->SetValue(name, "コンボ中の移動毎フレーム方向を更新", data.move.isUpdateDirectionEachFrame);
			globalVariables->SetValue(name, "コンボ中の移動毎フレームターゲット位置を更新", data.move.isUpdateTargetPositionEachFrame);
			globalVariables->SetValue(name, "コンボ中の移動方向を正規化してから使うか", data.move.isNormalizeLocalMove);
			globalVariables->SetEnumValue(name, "コンボ中の移動速度カーブ", data.move.speedCurveType, "MoveSpeedCurveType");
			globalVariables->SetValue(name, "コンボ中の移動速度カーブ強度", data.move.speedCurvePower);
			globalVariables->SetValue(name, "コンボ中の縦方向移動", data.move.isVerticalMove);
			globalVariables->SetValue(name, "コンボ中の移動基準前方を水平化", data.move.isFlattenTargetDirection);
			globalVariables->SetValue(name, "移動方向とキャラクターの向く方向を一致させるか", data.move.alignCharacterToMovement);

			// ターゲット有無で移動の味付けを変えるための上書き設定
			globalVariables->SetValue(name, "ターゲットあり移動上書き", data.move.targetMove.enabled);
			globalVariables->SetValue(name, "ターゲットあり移動スピード", data.move.targetMove.moveSpeed);
			globalVariables->SetValue(name, "ターゲットあり移動方向", data.move.targetMove.localMoveVector);
			globalVariables->SetValue(name, "ターゲットあり移動方向を正規化してから使うか", data.move.targetMove.isNormalizeLocalMove);
			globalVariables->SetEnumValue(name, "ターゲットありターゲット移動タイプ", data.move.targetMove.targetMoveType, "TargetMoveType");
			globalVariables->SetValue(name, "ターゲットあり接近距離半径", data.move.targetMove.moveTargetRadius);
			globalVariables->SetValue(name, "ターゲットなし移動上書き", data.move.noTargetMove.enabled);
			globalVariables->SetValue(name, "ターゲットなし移動スピード", data.move.noTargetMove.moveSpeed);
			globalVariables->SetValue(name, "ターゲットなし移動方向", data.move.noTargetMove.localMoveVector);
			globalVariables->SetValue(name, "ターゲットなし移動方向を正規化してから使うか", data.move.noTargetMove.isNormalizeLocalMove);

		}
		// リアクション
		{
			globalVariables->SetValue(name, "ノックバック持続時間", data.hitReaction.duration);
			globalVariables->SetValue(name, "ノックバック力", data.hitReaction.power);
			globalVariables->SetValue(name, "Y方向ノックバック力", data.hitReaction.verticalBoost);
			globalVariables->SetValue(name, "Y方向ノックバック", data.hitReaction.isVerticalBoost);
			globalVariables->SetValue(name, "ダメージ", data.hitReaction.damageData.GetOne().GetDamage());
			globalVariables->SetEnumValue(name, "ヒットリアクションタイプ", data.hitReaction.type, "HitReactionType");
			globalVariables->SetEnumValue(name, "ヒットストップ方針(自分)", data.hitReaction.selfHitStopPolicy, "SelfHitStopPolicy");
			globalVariables->SetEnumValue(name, "攻撃属性", data.hitReaction.attribute, "AttackAttribute");
			globalVariables->SetValue(name, "ヒット優先度", data.hitReaction.hitPriority);
			globalVariables->SetValue(name, "ヒットカメラシェイク量", data.hitReaction.cameraShakePower);
			globalVariables->SetValue(name, "ヒットスタン持続時間", data.hitReaction.hitStunTime);
			globalVariables->SetValue(name, "ダウン持続時間", data.hitReaction.downTime);
			globalVariables->SetValue(name, "打ち上げ持続時間", data.hitReaction.launchFloatTime);
			globalVariables->SetValue(name, "打ち上げ最大高度", data.hitReaction.launchMaxHeight);
			globalVariables->SetValue(name, "ヒット重力", data.hitReaction.gravityEnabled);
			globalVariables->SetValue(name, "ヒット重力倍率", data.hitReaction.gravityScale);


			globalVariables->SetValue(name, "ヒットストップ(相手)", data.hitReaction.targetHitStopTime);
			globalVariables->SetValue(name, "ヒットストップ(自分)", data.hitReaction.selfHitStopTime);
			globalVariables->SetValue(name, "ヒットストップ(一回のみ)",
				data.hitReaction.selfHitStopPolicy == SelfHitStopPolicy::FirstHitOnly);


			globalVariables->SetValue(name, kHitEffectCountKey, static_cast<int>(data.hitReaction.hitEffectNames.size()));

			for (int i = 0; i < static_cast<int>(data.hitReaction.hitEffectNames.size()); ++i) {
				globalVariables->SetValue(name, MakeHitEffectSlotKey(i), data.hitReaction.hitEffectNames[i].slotName);
				globalVariables->SetValue(name, MakeHitEffectNameKey(i), data.hitReaction.hitEffectNames[i].effectName);
			}
		}
		// ヒットボックス
		{
			globalVariables->SetValue(name, "ヒットボックス発生時間", data.hitBox.windowStart);
			globalVariables->SetValue(name, "ヒットボックス生存時間", data.hitBox.lifeTime);
			globalVariables->SetValue(name, "親オブジェクト名前", data.hitBox.parentName);
			globalVariables->SetValue(name, "ヒットボックスヒット記録を使用", data.hitBox.useContactRecord);
			globalVariables->SetValue(name, "ヒットボックスコライダー別ヒット記録", data.hitBox.recordPerCollider);
			globalVariables->SetValue(name, "ヒットボックスコライダーサイズ", data.hitBox.colliderSize);		// new
			globalVariables->SetValue(name, "ヒットボックスオフセット位置", data.hitBox.offset);		// new
			globalVariables->SetValue(name, "ヒットボックスコライダー半径", data.hitBox.radius);				// new

			globalVariables->SetEnumValue(name, "ヒットボックス発生条件タイプ", data.hitBox.spawnType, "HitBoxSpawnType");
			globalVariables->SetEnumValue(name, "ヒットボックス依存先タイプ", data.hitBox.dependenceType, "HitBoxParentType");
			globalVariables->SetEnumValue(name, "ヒットボックス影響タイプ", data.hitBox.hitEffectType, "HitBoxHitEffectType");
			globalVariables->SetEnumValue(name, "ヒットボックス生存タイプ", data.hitBox.lifetimeType, "HitBoxLifetimeType");
			globalVariables->SetEnumValue(name, "ヒットボックス形状タイプ", data.hitBox.shapeType, "HitBoxShapeType");
			globalVariables->SetEnumValue(name, "ヒットボックス使用者タイプ", data.hitBox.useType, "HitBoxUseType");

			globalVariables->SetEnumValue(name, "コライダー(タグ)", data.hitBox.tag, "CollisionTag");
			globalVariables->SetEnumValue(name, "コライダー(レイヤー)", data.hitBox.layer, "CollisionLayer");
			globalVariables->SetEnumValue(name, "コライダー(マスク)", data.hitBox.mask, "CollisionMask");


		}
		// カメラ
		{
			globalVariables->SetValue(name, "カメラロックオン", data.camera.isLockOn);
			globalVariables->SetValue(name, "カメラロックオン補間速度", data.camera.lockOnInterpolation);
			globalVariables->SetValue(name, "カメラロックオン回転引き継ぎ",data.camera.isLockOnRotate);
			globalVariables->SetValue(name, "カメラロックオン解除時間", data.camera.lockOnEndTime);

			globalVariables->SetValue(name, "カメラ切り替え", data.camera.isChangeCamera);
			globalVariables->SetValue(name, "カメラ切り替え名", data.camera.cameraName);
			globalVariables->SetValue(name, "カメラ切り替え開始時間", data.camera.changeCameraStartTime);
			globalVariables->SetValue(name, "カメラ切り替え補間時間", data.camera.interpolation);

			globalVariables->SetValue(name, "カメラズーム", data.camera.isZoom);
			globalVariables->SetValue(name, "カメラロックオンズーム", data.camera.isLockOnZoom);
			globalVariables->SetValue(name, "カメラズーム速度", data.camera.zoomSpeed);
			globalVariables->SetValue(name, "カメラズーム時間", data.camera.zoomDuration);
			globalVariables->SetValue(name, "カメラズーム開始時間", data.camera.zoomStartTime);
			globalVariables->SetValue(name, "カメラズーム量", data.camera.zoomTargetDistance);

			globalVariables->SetValue(name, "カメラシェイク", data.camera.isShake);
			globalVariables->SetValue(name, "カメラシェイク量", data.camera.shakeCameraPower);
			globalVariables->SetValue(name, "カメラシェイク開始時間", data.camera.shakeStartTime);
			globalVariables->SetValue(name, "カメラシェイク時間", data.camera.shakeDuration);
			globalVariables->SetValue(name, "カメラシェイク幅", data.camera.shakeOffset);

			globalVariables->SetValue(name, "カメラ注視点オフセット", data.camera.isActionTargetOffset);
			globalVariables->SetValue(name, "カメラ注視点オフセット開始時間", data.camera.actionTargetOffsetStartTime);
			globalVariables->SetValue(name, "カメラ注視点オフセット時間", data.camera.actionTargetOffsetDuration);
			globalVariables->SetValue(name, "カメラ注視点オフセット補間速度", data.camera.actionTargetOffsetBlendSpeed);
			globalVariables->SetValue(name, "カメラ注視点オフセット量", data.camera.actionTargetOffset);

			globalVariables->SetValue(name, "カメラ先読み", data.camera.isLookAhead);
			globalVariables->SetValue(name, "カメラ先読み開始時間", data.camera.lookAheadStartTime);
			globalVariables->SetValue(name, "カメラ先読み時間", data.camera.lookAheadDuration);
			globalVariables->SetValue(name, "カメラ先読み距離", data.camera.lookAheadDistance);
			globalVariables->SetValue(name, "カメラ先読み最小速度", data.camera.lookAheadMinSpeed);
			globalVariables->SetValue(name, "カメラ先読み最大速度", data.camera.lookAheadMaxSpeed);
			globalVariables->SetValue(name, "カメラ先読み補間速度", data.camera.lookAheadSmoothSpeed);

			globalVariables->SetValue(name, "カメラ速度ズーム", data.camera.isSpeedZoom);
			globalVariables->SetValue(name, "カメラ速度ズーム開始時間", data.camera.speedZoomStartTime);
			globalVariables->SetValue(name, "カメラ速度ズーム時間", data.camera.speedZoomDuration);
			globalVariables->SetValue(name, "カメラ速度ズーム最小速度", data.camera.speedZoomMinSpeed);
			globalVariables->SetValue(name, "カメラ速度ズーム最大速度", data.camera.speedZoomMaxSpeed);
			globalVariables->SetValue(name, "カメラ速度ズーム近距離補正", data.camera.speedZoomNearOffsetZ);
			globalVariables->SetValue(name, "カメラ速度ズーム遠距離補正", data.camera.speedZoomFarOffsetZ);
			globalVariables->SetValue(name, "カメラ速度ズーム補間速度", data.camera.speedZoomSmoothSpeed);

			// エディターで変更した命中時カメラ演出を永続化する
			globalVariables->SetValue(name, "ヒット時カメラ演出", data.camera.isHitCameraEffect);
			globalVariables->SetValue(name, "ヒット時カメラシェイク", data.camera.isHitShake);
			globalVariables->SetValue(name, "ヒット時カメラシェイク時間", data.camera.hitShakeDuration);
			globalVariables->SetValue(name, "ヒット時カメラシェイク幅", data.camera.hitShakeOffset);
			globalVariables->SetValue(name, "ヒット時カメラズーム", data.camera.isHitZoom);
			globalVariables->SetValue(name, "ヒット時カメラズーム距離", data.camera.hitZoomTargetDistance);
			globalVariables->SetValue(name, "ヒット時カメラズーム速度", data.camera.hitZoomSpeed);
			globalVariables->SetValue(name, "ヒット時カメラズーム時間", data.camera.hitZoomDuration);

		}
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

#include "ComboSystem.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>
#include <DirectXGame/application/base/Attack/AttackController.h>
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include <DirectXGame/application/base/Character/Move/Base/MoveComponent.h>
#include <DirectXGame/application/base/Character/State/CharacterStateMachine.h>

namespace Combo {

#pragma region main

	void System::Initialize(Character::BaseCharacter* character, Engine::LineCommon* lineCommon, Engine::GlobalVariables* globalVariables) {
		this->globalVariables = globalVariables;
		owner = character;	// 所有者設定

		comboStateMachine_ = std::make_unique<StateMachine>(character);

		comboDebug_ = std::make_unique<ComboDebug>();
		comboDebug_->Initialize(lineCommon, character);

	}

	void System::Update(const Character::CharacterContext& ctx) {
		comboStateMachine_->SetIsDebug(isDebug);
		comboStateMachine_->Update(ctx);
		const auto transitionedInput = comboStateMachine_->ConsumeTransitionedInput();
		if (transitionedInput && pendingCostInput_ && *transitionedInput == *pendingCostInput_) {
			PayStamina(pendingStaminaCost_);
		}
		if (transitionedInput) {
			pendingCostInput_.reset();
			pendingStaminaCost_ = 0.0f;
		}
		comboDebug_->Update(ctx.dt);
	}

	bool System::RequestAttack(ActionInput input) {
		if (!owner || owner->GetCurrentMainState() == Character::CharacterMainState::Special) {
			return false;
		}

		const float staminaCost = GetStaminaCost(input);
		if (!CanPayStamina(staminaCost)) {
			return false;
		}

		if (owner->GetCurrentMainState() == Character::CharacterMainState::Attack) {
			if (!comboStateMachine_->CanTransition(input)) {
				return false;
			}

			comboStateMachine_->HandleInput(input);
			pendingCostInput_ = input;
			pendingStaminaCost_ = staminaCost;
			return true;
		}

		const Character::CharacterMainState state = owner->GetCurrentMainState();
		const bool canStart =
			state == Character::CharacterMainState::Idle ||
			state == Character::CharacterMainState::Move ||
			state == Character::CharacterMainState::Jump;
		if (!canStart || !owner->GetMoveComponent()) {
			return false;
		}

		const std::string startCombo = ResolveStartCombo(input, owner->GetMoveComponent()->GetIsLanding());
		if (!StartCombo(startCombo)) {
			return false;
		}

		pendingCostInput_.reset();
		pendingStaminaCost_ = 0.0f;
		owner->GetAttackController()->SetIsAttack(true);
		owner->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Attack);
		PayStamina(staminaCost);
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
	}

	bool System::StartCombo(const std::string& name) {
		auto it = comboNodes_.find(name);
		if (it != comboNodes_.end()) {
			comboStateMachine_->SetRoot(it->second);
			return true;
		}
		return false;
	}

	std::string System::ResolveStartCombo(ActionInput input, bool isLanding) const {
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

	float System::GetStaminaCost(ActionInput input) const {
		return input == ActionInput::Skill ? 25.0f : 0.0f;
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

#pragma endregion // main

#pragma region Global

	void Combo::System::ApplyGlobalComboData(const std::string& name, GlobalData& data) {
		globalVariables->CreateGroup(name);

		// 既に存在する場合は追加しない
		if (comboNodenames_.find(name) == comboNodenames_.end()) {
			comboNodenames_[name] = name;
			globalVariables->AddItem(this->name, name.c_str(), name);
		}
		

		

		// エフェクト
		{
			globalVariables->AddItem(name, "エフェクト(トレイル)発生時間", data.effect.trailEffectStartTime);
			globalVariables->AddItem(name, "エフェクト(トレイル)生存時間", data.effect.trailEffectLifeTime);
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
			globalVariables->AddItem(name, "接続先(強攻撃-地上-未ヒット)", data.connection.heavyCondition.groundMiss);
			globalVariables->AddItem(name, "接続先(強攻撃-地上-ヒット)", data.connection.heavyCondition.groundHit);
			globalVariables->AddItem(name, "接続先(強攻撃-空中-未ヒット)", data.connection.heavyCondition.airMiss);
			globalVariables->AddItem(name, "接続先(強攻撃-空中-ヒット)", data.connection.heavyCondition.airHit);
			globalVariables->AddItem(name, "接続先(スキル-地上-未ヒット)", data.connection.skillCondition.groundMiss);
			globalVariables->AddItem(name, "接続先(スキル-地上-ヒット)", data.connection.skillCondition.groundHit);
			globalVariables->AddItem(name, "接続先(スキル-空中-未ヒット)", data.connection.skillCondition.airMiss);
			globalVariables->AddItem(name, "接続先(スキル-空中-ヒット)", data.connection.skillCondition.airHit);
		}
		// 条件
		{
			globalVariables->AddItem(name, "コンボ入力受付開始時間", data.condition.stateInput.startTime);
			globalVariables->AddItem(name, "コンボ入力受付終了時間", data.condition.stateInput.endTime);
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
		}
		// 移動
		{
			globalVariables->AddItem(name, "コンボ中の重力", data.move.isGravity);
			globalVariables->AddItem(name, "コンボ中の重力強度", data.move.gravityScale);
			globalVariables->AddItem(name, "コンボ開始時に重力速度リセット", data.move.isResetGravity);

			globalVariables->AddItem(name, "移動スピード", data.move.moveSpeed);
			globalVariables->AddItem(name, "コンボ中の移動開始時間", data.move.moveWindow.startTime);
			globalVariables->AddItem(name, "コンボ中の移動終了時間", data.move.moveWindow.endTime);
			globalVariables->AddItem(name, "コンボ中の移動強制", data.move.isCompulsionMove);
			globalVariables->AddEnumItem(name, "コンボ中の移動タイプ", data.move.moveType, "MoveType");
			globalVariables->AddItem(name, "ロックオン半径", data.move.lockOnData.radius);
			globalVariables->AddItem(name, "ロックオンターゲット接近半径", data.move.lockOnData.moveTargetRadius);

			globalVariables->AddEnumItem(name, "ロックオンタイプ", data.move.lockOnData.type, "LockOnType");
			globalVariables->AddEnumItem(name, "ターゲット移動タイプ", data.move.lockOnData.targetMoveType, "TargetMoveType");


			globalVariables->AddItem(name, "コンボ中の移動方向", data.move.localMoveVector);
			globalVariables->AddItem(name, "コンボ中の移動毎フレーム方向を更新", data.move.isUpdateDirectionEachFrame);
			globalVariables->AddItem(name, "コンボ中の移動方向を正規化してから使うか", data.move.isNormalizeLocalMove);
			globalVariables->AddItem(name, "コンボ中の移動基準前方を水平化", data.move.isFlattenTargetDirection);
			globalVariables->AddItem(name, "移動方向とキャラクターの向く方向を一致させるか", data.move.alignCharacterToMovement);

		}
		// ヒットボックス
		{
			globalVariables->AddItem(name, "親オブジェクト名前", data.hitBox.parentName);
			globalVariables->AddItem(name, "ヒットボックス発生時間", data.hitBox.windowStart);
			globalVariables->AddItem(name, "ヒットボックス生存時間", data.hitBox.lifeTime);
			globalVariables->AddItem(name, "ヒットボックスヒット記録を使用", data.hitBox.useContactRecord);
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
			
			globalVariables->AddItem(name, "カメラズーム", data.camera.isZoom);
			globalVariables->AddItem(name, "カメラロックオンズーム", data.camera.isLockOnZoom);
			globalVariables->AddItem(name, "カメラズーム速度", data.camera.zoomSpeed);
			globalVariables->AddItem(name, "カメラズーム時間", data.camera.zoomDuration);
			globalVariables->AddItem(name, "カメラズーム開始時間", data.camera.zoomStartTime);
			globalVariables->AddItem(name, "カメラズーム量", data.camera.zoomTargetDistance);


		}



		// 保存項目の適応
		GetGlobalComboData(name, data);
	};

	void Combo::System::GetGlobalComboData(const std::string& name, GlobalData& data) {
		
		// エフェクト
		{
			data.effect.trailEffectStartTime = globalVariables->GetValue<float>(name, "エフェクト(トレイル)発生時間");
			data.effect.trailEffectLifeTime = globalVariables->GetValue<float>(name, "エフェクト(トレイル)生存時間");
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
			data.connection.heavyCondition.groundMiss = globalVariables->GetValue<std::string>(name, "接続先(強攻撃-地上-未ヒット)");
			data.connection.heavyCondition.groundHit = globalVariables->GetValue<std::string>(name, "接続先(強攻撃-地上-ヒット)");
			data.connection.heavyCondition.airMiss = globalVariables->GetValue<std::string>(name, "接続先(強攻撃-空中-未ヒット)");
			data.connection.heavyCondition.airHit = globalVariables->GetValue<std::string>(name, "接続先(強攻撃-空中-ヒット)");
			data.connection.skillCondition.groundMiss = globalVariables->GetValue<std::string>(name, "接続先(スキル-地上-未ヒット)");
			data.connection.skillCondition.groundHit = globalVariables->GetValue<std::string>(name, "接続先(スキル-地上-ヒット)");
			data.connection.skillCondition.airMiss = globalVariables->GetValue<std::string>(name, "接続先(スキル-空中-未ヒット)");
			data.connection.skillCondition.airHit = globalVariables->GetValue<std::string>(name, "接続先(スキル-空中-ヒット)");
		}
		// 条件
		{
			data.condition.stateInput.startTime = globalVariables->GetValue<float>(name, "コンボ入力受付開始時間");
			data.condition.stateInput.endTime = globalVariables->GetValue<float>(name, "コンボ入力受付終了時間");
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
		}
		// 移動
		{
			data.move.isGravity = globalVariables->GetValue<bool>(name, "コンボ中の重力");
			data.move.gravityScale = globalVariables->GetValue<float>(name, "コンボ中の重力強度");
			data.move.isResetGravity = globalVariables->GetValue<bool>(name, "コンボ開始時に重力速度リセット");

			data.move.moveSpeed = globalVariables->GetValue<Vector3>(name, "移動スピード");
			data.move.moveWindow.startTime = globalVariables->GetValue<float>(name, "コンボ中の移動開始時間");
			data.move.moveWindow.endTime = globalVariables->GetValue<float>(name, "コンボ中の移動終了時間");
			data.move.isCompulsionMove = globalVariables->GetValue<bool>(name, "コンボ中の移動強制");
			data.move.moveType = globalVariables->GetEnumValue<Combo::MoveType>(name, "コンボ中の移動タイプ");
			data.move.lockOnData.radius = globalVariables->GetValue<float>(name, "ロックオン半径");
			data.move.lockOnData.moveTargetRadius = globalVariables->GetValue<float>(name, "ロックオンターゲット接近半径");
			data.move.lockOnData.type = globalVariables->GetEnumValue<LockOnType>(name, "ロックオンタイプ");
			data.move.lockOnData.targetMoveType = globalVariables->GetEnumValue<TargetMoveType>(name, "ターゲット移動タイプ");
			

			data.move.localMoveVector = globalVariables->GetValue<Vector3>(name, "コンボ中の移動方向");
			data.move.isUpdateDirectionEachFrame = globalVariables->GetValue<bool>(name, "コンボ中の移動毎フレーム方向を更新");
			data.move.isNormalizeLocalMove = globalVariables->GetValue<bool>(name, "コンボ中の移動方向を正規化してから使うか");
			data.move.isFlattenTargetDirection = globalVariables->GetValue<bool>(name, "コンボ中の移動基準前方を水平化");
			data.move.alignCharacterToMovement = globalVariables->GetValue<bool>(name, "移動方向とキャラクターの向く方向を一致させるか");

		}
		// リアクション
		{
			data.hitReaction.damageData.GetOne().SetDamage(globalVariables->GetValue<float>(name, "ダメージ"));
			data.hitReaction.isVerticalBoost = globalVariables->GetValue<bool>(name, "Y方向ノックバック");
			data.hitReaction.power = globalVariables->GetValue<float>(name, "ノックバック力");
			data.hitReaction.verticalBoost = globalVariables->GetValue<float>(name, "Y方向ノックバック力");
			data.hitReaction.duration = globalVariables->GetValue<float>(name, "ノックバック持続時間");
			data.hitReaction.type = globalVariables->GetEnumValue<HitReactionType>(name, "ヒットリアクションタイプ");
			data.hitReaction.hitStunTime = globalVariables->GetValue<float>(name, "ヒットスタン持続時間");
			data.hitReaction.downTime = globalVariables->GetValue<float>(name, "ダウン持続時間");
			data.hitReaction.launchFloatTime = globalVariables->GetValue<float>(name, "打ち上げ持続時間");
			data.hitReaction.launchMaxHeight = globalVariables->GetValue<float>(name, "打ち上げ最大高度");
			data.hitReaction.gravityEnabled = globalVariables->GetValue<bool>(name, "ヒット重力");
			data.hitReaction.gravityScale = globalVariables->GetValue<float>(name, "ヒット重力倍率");


			data.hitReaction.targetHitStopTime = globalVariables->GetValue<float>(name, "ヒットストップ(相手)");
			data.hitReaction.selfHitStopTime =  globalVariables->GetValue<float>(name, "ヒットストップ(自分)");
			data.hitReaction.isSingleHitStop = globalVariables->GetValue<bool>(name, "ヒットストップ(一回のみ)");


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

			data.camera.isZoom = globalVariables->GetValue<bool>(name, "カメラズーム");
			data.camera.isLockOnZoom = globalVariables->GetValue<bool>(name, "カメラロックオンズーム");
			data.camera.zoomSpeed = globalVariables->GetValue<float>(name, "カメラズーム速度");
			data.camera.zoomDuration = globalVariables->GetValue<float>(name, "カメラズーム時間");
			data.camera.zoomStartTime = globalVariables->GetValue<float>(name, "カメラズーム開始時間");
			data.camera.zoomTargetDistance = globalVariables->GetValue<float>(name, "カメラズーム量");
			
		}
	}

	void System::SetGlobalComboData(const std::string& name, GlobalData& data) {
		
		// エフェクト
		{
			globalVariables->SetValue(name, "エフェクト(トレイル)発生時間", data.effect.trailEffectStartTime);
			globalVariables->SetValue(name, "エフェクト(トレイル)生存時間", data.effect.trailEffectLifeTime);
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
			globalVariables->SetValue(name, "接続先(強攻撃-地上-未ヒット)", data.connection.heavyCondition.groundMiss);
			globalVariables->SetValue(name, "接続先(強攻撃-地上-ヒット)", data.connection.heavyCondition.groundHit);
			globalVariables->SetValue(name, "接続先(強攻撃-空中-未ヒット)", data.connection.heavyCondition.airMiss);
			globalVariables->SetValue(name, "接続先(強攻撃-空中-ヒット)", data.connection.heavyCondition.airHit);
			globalVariables->SetValue(name, "接続先(スキル-地上-未ヒット)", data.connection.skillCondition.groundMiss);
			globalVariables->SetValue(name, "接続先(スキル-地上-ヒット)", data.connection.skillCondition.groundHit);
			globalVariables->SetValue(name, "接続先(スキル-空中-未ヒット)", data.connection.skillCondition.airMiss);
			globalVariables->SetValue(name, "接続先(スキル-空中-ヒット)", data.connection.skillCondition.airHit);
		}
		// 条件
		{
			globalVariables->SetValue(name, "コンボ入力受付開始時間", data.condition.stateInput.startTime);
			globalVariables->SetValue(name, "コンボ入力受付終了時間", data.condition.stateInput.endTime);
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


		}
		// 移動
		{
			globalVariables->SetValue(name, "コンボ中の移動開始時間", data.move.moveWindow.startTime);
			globalVariables->SetValue(name, "コンボ中の移動終了時間", data.move.moveWindow.endTime);
			globalVariables->SetValue(name, "移動スピード", data.move.moveSpeed);
			globalVariables->SetValue(name, "コンボ中の移動強制", data.move.isCompulsionMove);
			globalVariables->SetValue(name, "コンボ中の重力", data.move.isGravity);
			globalVariables->SetValue(name, "コンボ中の重力強度", data.move.gravityScale);
			globalVariables->SetValue(name, "コンボ開始時に重力速度リセット", data.move.isResetGravity);

			globalVariables->SetEnumValue(name, "コンボ中の移動タイプ", data.move.moveType, "MoveType");
			globalVariables->SetValue(name, "ロックオン半径", data.move.lockOnData.radius);
			globalVariables->SetValue(name, "ロックオンターゲット接近半径", data.move.lockOnData.moveTargetRadius);

			globalVariables->SetEnumValue(name, "ロックオンタイプ", data.move.lockOnData.type, "LockOnType");
			globalVariables->SetEnumValue(name, "ターゲット移動タイプ", data.move.lockOnData.targetMoveType, "TargetMoveType");


			globalVariables->SetValue(name, "コンボ中の移動方向", data.move.localMoveVector);
			globalVariables->SetValue(name, "コンボ中の移動毎フレーム方向を更新", data.move.isUpdateDirectionEachFrame);
			globalVariables->SetValue(name, "コンボ中の移動方向を正規化してから使うか", data.move.isNormalizeLocalMove);
			globalVariables->SetValue(name, "コンボ中の移動基準前方を水平化", data.move.isFlattenTargetDirection);
			globalVariables->SetValue(name, "移動方向とキャラクターの向く方向を一致させるか", data.move.alignCharacterToMovement);

		}
		// リアクション
		{
			globalVariables->SetValue(name, "ノックバック持続時間", data.hitReaction.duration);
			globalVariables->SetValue(name, "ノックバック力", data.hitReaction.power);
			globalVariables->SetValue(name, "Y方向ノックバック力", data.hitReaction.verticalBoost);
			globalVariables->SetValue(name, "Y方向ノックバック", data.hitReaction.isVerticalBoost);
			globalVariables->SetValue(name, "ダメージ", data.hitReaction.damageData.GetOne().GetDamage());
			globalVariables->SetEnumValue(name, "ヒットリアクションタイプ", data.hitReaction.type, "HitReactionType");
			globalVariables->SetValue(name, "ヒットスタン持続時間", data.hitReaction.hitStunTime);
			globalVariables->SetValue(name, "ダウン持続時間", data.hitReaction.downTime);
			globalVariables->SetValue(name, "打ち上げ持続時間", data.hitReaction.launchFloatTime);
			globalVariables->SetValue(name, "打ち上げ最大高度", data.hitReaction.launchMaxHeight);
			globalVariables->SetValue(name, "ヒット重力", data.hitReaction.gravityEnabled);
			globalVariables->SetValue(name, "ヒット重力倍率", data.hitReaction.gravityScale);


			globalVariables->SetValue(name, "ヒットストップ(相手)", data.hitReaction.targetHitStopTime);
			globalVariables->SetValue(name, "ヒットストップ(自分)", data.hitReaction.selfHitStopTime);
			globalVariables->SetValue(name, "ヒットストップ(一回のみ)", data.hitReaction.isSingleHitStop);


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

			globalVariables->SetValue(name, "カメラズーム", data.camera.isZoom);
			globalVariables->SetValue(name, "カメラロックオンズーム", data.camera.isLockOnZoom);
			globalVariables->SetValue(name, "カメラズーム速度", data.camera.zoomSpeed);
			globalVariables->SetValue(name, "カメラズーム時間", data.camera.zoomDuration);
			globalVariables->SetValue(name, "カメラズーム開始時間", data.camera.zoomStartTime);
			globalVariables->SetValue(name, "カメラズーム量", data.camera.zoomTargetDistance);

		}
	}

	void System::SetGlobalComboDatas() {
		for (auto& pair : comboGlobalDatas_) {
			SetGlobalComboData(pair.first, pair.second);
		}
	};

	void System::SetData(ComboData& data, const GlobalData& gData) {
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
		groundLightStart_ = globalVariables->GetValue<std::string>(name, kGroundLightStartKey);
		airLightStart_ = globalVariables->GetValue<std::string>(name, kAirLightStartKey);
		groundHeavyStart_ = globalVariables->GetValue<std::string>(name, kGroundHeavyStartKey);
		airHeavyStart_ = globalVariables->GetValue<std::string>(name, kAirHeavyStartKey);
		groundSkillStart_ = globalVariables->GetValue<std::string>(name, kGroundSkillStartKey);
		airSkillStart_ = globalVariables->GetValue<std::string>(name, kAirSkillStartKey);

		for (auto& data : globalVariables->GetGroupData(name)) {
			if (data.first == kGroundLightStartKey || data.first == kAirLightStartKey ||
				data.first == kGroundHeavyStartKey || data.first == kAirHeavyStartKey ||
				data.first == kGroundSkillStartKey || data.first == kAirSkillStartKey) {
				continue;
			}
			CreateCombo(globalVariables->GetValue<std::string>(name, data.first));
		}

		ConnectSavedCombos();
	}

#pragma endregion // 保存　適応

};

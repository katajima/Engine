#include "ComboSystem.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>

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
		comboDebug_->Update(ctx.dt);
	}

	void System::ClearNode() {

		// StateMachineの状態を完全リセット
		if (comboStateMachine_) {
			comboStateMachine_.reset();
			comboStateMachine_ = std::make_unique<StateMachine>(owner);
		}

		comboNodes_.clear();
		comboGlobalDatas_.clear();
		parentTransforms_.clear();
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

	void System::ConnectCombo(const std::string& from, ActionInput input, const std::string& to) {
		auto itFrom = comboNodes_.find(from);
		auto itTo = comboNodes_.find(to);
		if (itFrom != comboNodes_.end()) {
			if (itTo != comboNodes_.end())
				itFrom->second->SetNextState(input, itTo->second);
		}
	}

	void System::StartCombo(const std::string& name) {
		auto it = comboNodes_.find(name);
		if (it != comboNodes_.end()) {
			comboStateMachine_->SetRoot(it->second);
		}
	}

#pragma endregion // main

#pragma region Global

	void Combo::System::ApplyGlobalComboData(const std::string& name, GlobalData& data) {
		globalVariables->CreateGroup(name);

		// エフェクト
		{
			globalVariables->AddItem(name, "エフェクト(トレイル)発生時間", data.effect.trailEffectStartTime);
			globalVariables->AddItem(name, "エフェクト(トレイル)生存時間", data.effect.trailEffectLifeTime);
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
			globalVariables->AddEnumItem(name, "ロックオンタイプ", data.move.lockOnData.type, "LockOnType");

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




		// 保存項目の適応
		GetGlobalComboData(name, data);
	};

	void Combo::System::GetGlobalComboData(const std::string& name, GlobalData& data) {
		
		// エフェクト
		{
			data.effect.trailEffectStartTime = globalVariables->GetValue<float>(name, "エフェクト(トレイル)発生時間");
			data.effect.trailEffectLifeTime = globalVariables->GetValue<float>(name, "エフェクト(トレイル)生存時間");
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
			data.move.lockOnData.type = globalVariables->GetEnumValue<LockOnType>(name, "ロックオンタイプ");

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
	}

	void System::SetGlobalComboData(const std::string& name, GlobalData& data) {
		
		// エフェクト
		{
			globalVariables->SetValue(name, "エフェクト(トレイル)発生時間", data.effect.trailEffectStartTime);
			globalVariables->SetValue(name, "エフェクト(トレイル)生存時間", data.effect.trailEffectLifeTime);
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
			globalVariables->SetEnumValue(name, "ロックオンタイプ", data.move.lockOnData.type, "LockOnType");

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

#pragma endregion // 保存　適応

};

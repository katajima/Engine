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

	void System::ConnectCombo(const std::string& from, AttackInput input, const std::string& to) {
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




		globalVariables->AddItem(name, "ヒットボックス発生時間", data.hitBox.hitBoxWindowStart);
		globalVariables->AddItem(name, "ヒットボックス生存時間", data.hitBox.hitBoxLifeTime);

		// リアクション
		globalVariables->AddItem(name, "ダメージ", data.reaction.damage);
		globalVariables->AddItem(name, "Y方向ノックバック", data.reaction.isVerticalBoost);
		globalVariables->AddItem(name, "ノックバック力", data.reaction.knockbackPower);
		globalVariables->AddItem(name, "Y方向ノックバック力", data.reaction.knockbackPowerY);
		globalVariables->AddItem(name, "ノックバック持続時間", data.reaction.knockbackDuration);

		globalVariables->AddItem(name, "コンボ入力受付開始時間", data.stateInputStartTime);
		globalVariables->AddItem(name, "コンボ入力受付終了時間", data.stateInputEndTime);
		globalVariables->AddItem(name, "コンボ終了時間", data.stateEndTime);
		globalVariables->AddItem(name, "コンボ移行時間", data.stateNextTime);
		globalVariables->AddItem(name, "コンボ中の重力", data.move.isGravity);
		globalVariables->AddItem(name, "コンボ中の重力強度", data.move.gravityScale);


		globalVariables->AddItem(name, "コンボキャンセル受付開始時間", data.stateCancelStartTime);
		globalVariables->AddItem(name, "コンボキャンセル受付終了時間", data.stateCancelEndTime);
		globalVariables->AddItem(name, "コンボ移動キャンセル受付開始時間", data.stateMoveCancelStartTime);
		globalVariables->AddItem(name, "コンボ移動キャンセル受付終了時間", data.stateMoveCancelEndTime);


		globalVariables->AddItem(name, "コンボ中の移動スピード", data.move.moveSpeed);
		globalVariables->AddItem(name, "コンボ中の移動開始時間", data.move.moveWindowStart);
		globalVariables->AddItem(name, "コンボ中の移動終了時間", data.move.moveWindowEnd);
		globalVariables->AddItem(name, "コンボ中の移動強制", data.move.isCompulsionMove);
		globalVariables->AddEnumItem(name, "コンボ中の移動タイプ", data.move.moveType, "MoveType");


		globalVariables->AddItem(name, "アニメーション名前", data.animation.animationName);
		globalVariables->AddItem(name, "アニメーション速度", data.animation.animationSpeed);
		globalVariables->AddItem(name, "アニメーション遷移時間", data.animation.animationBlendTime);

		globalVariables->AddItem(name, "エフェクト(トレイル)発生時間", data.trailEffectStartTime);
		globalVariables->AddItem(name, "エフェクト(トレイル)生存時間", data.trailEffectLifeTime);


		globalVariables->AddItem(name, "親オブジェクト名前", data.hitBox.parentName);

		globalVariables->AddEnumItem(name, "ヒットボックス発生条件タイプ", data.hitBox.spawnType, "HitBoxSpawnType");
		globalVariables->AddEnumItem(name, "ヒットボックス依存先タイプ", data.hitBox.dependenceType, "HitBoxParentType");
		globalVariables->AddEnumItem(name, "ヒットボックス影響タイプ", data.hitBox.hitEffectType,"HitBoxHitEffectType");
		globalVariables->AddEnumItem(name, "ヒットボックス生存タイプ", data.hitBox.lifetimeType, "HitBoxLifetimeType");

		
		globalVariables->AddEnumItem(name, "終了条件タイプ", data.endConditionType, "EndConditionType");


		globalVariables->AddItem(name, "ロックオン半径", data.lockOn.lockOnRadius);
		globalVariables->AddEnumItem(name, "ロックオンタイプ", data.lockOn.lockOnType, "LockOnType");


		// 保存項目の適応
		GetGlobalComboData(name, data);
	};

	void Combo::System::GetGlobalComboData(const std::string& name, GlobalData& data) {
		data.hitBox.hitBoxWindowStart = globalVariables->GetValue<float>(name, "ヒットボックス発生時間");
		data.hitBox.hitBoxLifeTime = globalVariables->GetValue<float>(name, "ヒットボックス生存時間");

		// リアクション
		data.reaction.damage = globalVariables->GetValue<float>(name, "ダメージ");
		data.reaction.isVerticalBoost = globalVariables->GetValue<bool>(name, "Y方向ノックバック");
		data.reaction.knockbackPower = globalVariables->GetValue<float>(name, "ノックバック力");
		data.reaction.knockbackPowerY = globalVariables->GetValue<float>(name, "Y方向ノックバック力");
		data.reaction.knockbackDuration = globalVariables->GetValue<float>(name, "ノックバック持続時間");


		data.stateInputStartTime = globalVariables->GetValue<float>(name, "コンボ入力受付開始時間");
		data.stateInputEndTime = globalVariables->GetValue<float>(name, "コンボ入力受付終了時間");
		data.stateEndTime = globalVariables->GetValue<float>(name, "コンボ終了時間");
		data.stateNextTime = globalVariables->GetValue<float>(name, "コンボ移行時間");
		data.move.isGravity = globalVariables->GetValue<bool>(name, "コンボ中の重力");
		data.move.gravityScale = globalVariables->GetValue<float>(name, "コンボ中の重力強度");

		data.move.moveSpeed = globalVariables->GetValue<float>(name, "コンボ中の移動スピード");
		data.move.moveWindowStart = globalVariables->GetValue<float>(name, "コンボ中の移動開始時間");
		data.move.moveWindowEnd = globalVariables->GetValue<float>(name, "コンボ中の移動終了時間");
		data.move.isCompulsionMove = globalVariables->GetValue<bool>(name, "コンボ中の移動強制");
		data.move.moveType = globalVariables->GetEnumValue<Combo::MoveType>(name, "コンボ中の移動タイプ");


		data.stateCancelStartTime = globalVariables->GetValue<float>(name, "コンボキャンセル受付開始時間");
		data.stateCancelEndTime = globalVariables->GetValue<float>(name, "コンボキャンセル受付終了時間");

		data.stateMoveCancelStartTime = globalVariables->GetValue<float>(name, "コンボ移動キャンセル受付開始時間");
		data.stateMoveCancelEndTime = globalVariables->GetValue<float>(name, "コンボ移動キャンセル受付終了時間");



		data.animation.animationName = globalVariables->GetValue<std::string>(name, "アニメーション名前");
		data.animation.animationSpeed = globalVariables->GetValue<float>(name, "アニメーション速度");
		data.animation.animationBlendTime = globalVariables->GetValue<float>(name, "アニメーション遷移時間");


		data.trailEffectStartTime = globalVariables->GetValue<float>(name, "エフェクト(トレイル)発生時間");
		data.trailEffectLifeTime = globalVariables->GetValue<float>(name, "エフェクト(トレイル)生存時間");


		data.hitBox.parentName = globalVariables->GetValue<std::string>(name, "親オブジェクト名前");

		data.hitBox.spawnType = globalVariables->GetEnumValue<HitBox::SpawnType>(name, "ヒットボックス発生条件タイプ");
		data.hitBox.dependenceType = globalVariables->GetEnumValue<HitBox::ParentType>(name, "ヒットボックス依存先タイプ");
		data.hitBox.hitEffectType= globalVariables->GetEnumValue<HitBox::HitEffectType>(name,"ヒットボックス影響タイプ");
		data.hitBox.lifetimeType = globalVariables->GetEnumValue<HitBox::LifetimeType>(name, "ヒットボックス生存タイプ");



		data.endConditionType = globalVariables->GetEnumValue<Combo::EndConditionType>(name, "終了条件タイプ");

		

		data.lockOn.lockOnRadius = globalVariables->GetValue<float>(name, "ロックオン半径");
		data.lockOn.lockOnType = globalVariables->GetEnumValue<LockOnType>(name, "ロックオンタイプ");

	}

	void System::SetGlobalComboData(const std::string& name, GlobalData& data) {
		globalVariables->SetValue(name, "コンボ入力受付開始時間", data.stateInputStartTime);
		globalVariables->SetValue(name, "コンボ入力受付終了時間", data.stateInputEndTime);

		globalVariables->SetValue(name, "ヒットボックス発生時間", data.hitBox.hitBoxWindowStart);
		globalVariables->SetValue(name, "ヒットボックス生存時間", data.hitBox.hitBoxLifeTime);

		globalVariables->SetValue(name, "コンボ終了時間", data.stateEndTime);
		globalVariables->SetValue(name, "コンボ移行時間", data.stateNextTime);

		globalVariables->SetValue(name, "コンボ中の移動開始時間", data.move.moveWindowStart);
		globalVariables->SetValue(name, "コンボ中の移動終了時間", data.move.moveWindowEnd);

		globalVariables->SetValue(name, "コンボ中の移動スピード", data.move.moveSpeed);
		globalVariables->SetValue(name, "コンボ中の移動強制", data.move.isCompulsionMove);
		globalVariables->SetValue(name, "コンボ中の重力", data.move.isGravity);
		globalVariables->SetValue(name, "コンボ中の重力強度", data.move.gravityScale);
		globalVariables->SetEnumValue(name, "コンボ中の移動タイプ", data.move.moveType, "MoveType");


		globalVariables->SetValue(name, "コンボキャンセル受付開始時間", data.stateCancelStartTime);
		globalVariables->SetValue(name, "コンボキャンセル受付終了時間", data.stateCancelEndTime);
		globalVariables->SetValue(name, "コンボ移動キャンセル受付開始時間", data.stateMoveCancelStartTime);
		globalVariables->SetValue(name, "コンボ移動キャンセル受付終了時間", data.stateMoveCancelEndTime);


		globalVariables->SetValue(name, "アニメーション名前", data.animation.animationName);
		globalVariables->SetValue(name, "アニメーション速度", data.animation.animationSpeed);
		globalVariables->SetValue(name, "アニメーション遷移時間", data.animation.animationBlendTime);


		globalVariables->SetValue(name, "エフェクト(トレイル)発生時間", data.trailEffectStartTime);
		globalVariables->SetValue(name, "エフェクト(トレイル)生存時間", data.trailEffectLifeTime);

		globalVariables->SetValue(name, "ノックバック持続時間", data.reaction.knockbackDuration);
		globalVariables->SetValue(name, "ノックバック力", data.reaction.knockbackPower);
		globalVariables->SetValue(name, "Y方向ノックバック力", data.reaction.knockbackPowerY);
		globalVariables->SetValue(name, "Y方向ノックバック", data.reaction.isVerticalBoost);
		globalVariables->SetValue(name, "ダメージ", data.reaction.damage);


		globalVariables->SetValue(name, "親オブジェクト名前", data.hitBox.parentName);

		globalVariables->SetEnumValue(name, "ヒットボックス発生条件タイプ", data.hitBox.spawnType, "HitBoxSpawnType");
		globalVariables->SetEnumValue(name, "ヒットボックス依存先タイプ", data.hitBox.dependenceType, "HitBoxParentType");
		globalVariables->SetEnumValue(name, "ヒットボックス影響タイプ", data.hitBox.hitEffectType, "HitBoxHitEffectType");
		globalVariables->SetEnumValue(name, "ヒットボックス生存タイプ", data.hitBox.lifetimeType, "HitBoxLifetimeType");

		globalVariables->SetEnumValue(name, "終了条件タイプ", data.endConditionType, "EndConditionType");

		globalVariables->SetValue(name, "ロックオン半径", data.lockOn.lockOnRadius);
		globalVariables->SetEnumValue(name, "ロックオンタイプ", data.lockOn.lockOnType, "LockOnType");
	}

	void System::SetGlobalComboDatas() {
		for (auto& pair : comboGlobalDatas_) {
			SetGlobalComboData(pair.first, pair.second);
		}
	};

	void System::SetData(ComboData& data, const GlobalData& gData)
	{
		///
		/// ヒットボックス
		/// 
		data.GetComboHitBox().GetData().hitBpxWindowStart = gData.hitBox.hitBoxWindowStart;	// 発生時間
		data.GetComboHitBox().GetData().lifeTime = gData.hitBox.hitBoxLifeTime;				// 生成時間
		data.GetComboHitBox().GetData().parentName = gData.hitBox.parentName;
		data.GetComboHitBox().SetPerent(GetParentTransform(gData.hitBox.parentName));
		data.GetComboHitBox().GetData().offset = gData.hitBox.parentOffset;
		data.GetComboHitBox().GetData().dependenceType = gData.hitBox.dependenceType;
		data.GetComboHitBox().GetData().spawnType = gData.hitBox.spawnType;
		data.GetComboHitBox().GetData().lifetimeType = gData.hitBox.lifetimeType;
		data.GetComboHitBox().GetData().hitEffectType = gData.hitBox.hitEffectType;
		///
		/// 受付
		/// 


		// 入力受付時間設定
		data.GetComboCondition().GetNextReceiver().ConditionStartEnd(gData.stateInputStartTime, gData.stateInputEndTime);
		data.GetComboCondition().GetNextCondition().GetData().stateTime = gData.stateNextTime;
		data.GetComboCondition().GetEndCondition().GetData().stateTime = gData.stateEndTime;
		
		// キャンセル
		data.GetComboCondition().GetCancelReceiver().GetData().inputStart = gData.stateCancelStartTime;
		data.GetComboCondition().GetCancelReceiver().GetData().inputEnd = gData.stateCancelEndTime;

		// 移動キャンセル
		data.GetComboCondition().GetCancelReceiver().GetData().inputMoveStart = gData.stateMoveCancelStartTime;
		data.GetComboCondition().GetCancelReceiver().GetData().inputMoveEnd = gData.stateMoveCancelEndTime;


		
		// コンボボタン設定
		ComboButton bo = ComboButton(GamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed);
		// 押し続ける
		std::vector<ComboButton> button;
		button.push_back(bo);
		data.GetComboCondition().GetNextReceiver().GetData().comboSequence_.RegisterCombo(button);


		data.GetComboCondition().GetNextCondition().GetData().type = gData.endConditionType;
		data.GetComboCondition().GetEndCondition().GetData().type = gData.endConditionType;

		///
		/// モーション
		/// 

		// 重力
		data.GetComboMotion().GetComboMove().GetData().isGravity_ = gData.move.isGravity;
		data.GetComboMotion().GetComboMove().GetData().gravityScale_ = gData.move.gravityScale;
		// 移動
		data.GetComboMotion().GetComboMove().GetData().speed_ = gData.move.moveSpeed;
		data.GetComboMotion().GetComboMove().GetData().moveWindowStart_ = gData.move.moveWindowStart;
		data.GetComboMotion().GetComboMove().GetData().moveWindowEnd_ = gData.move.moveWindowEnd;
		data.GetComboMotion().GetComboMove().GetData().isCompulsionMove_ = gData.move.isCompulsionMove;
		data.GetComboMotion().GetComboMove().GetData().moveType = gData.move.moveType;

		// アニメーションスピード
		data.GetComboMotion().GetComboAnimation().GetData().animationBlendTime_ = gData.animation.animationBlendTime;
		data.GetComboMotion().GetComboAnimation().GetData().animationSpeed_ = gData.animation.animationSpeed;
		data.GetComboMotion().GetComboAnimation().GetData().animationName_ = gData.animation.animationName;


		///
		/// エフェクト
		/// 

		// トレイルエフェクト
		data.GetComboEffect().GetData().startTmer = gData.trailEffectStartTime;
		data.GetComboEffect().GetData().lifeTime = gData.trailEffectLifeTime;


		
		// ロックオン
		data.GetComboMotion().GetComboMove().GetData().lockOnData_.type = gData.lockOn.lockOnType;
		data.GetComboMotion().GetComboMove().GetData().lockOnData_.radius = gData.lockOn.lockOnRadius;
	}

	void System::CreateCombo(const std::string& comboNodeName, const std::vector<AddHitBoxData>& addHitBoxDatas,
		GamePadButton button) {
		// コンボデータ
		ComboData data{};
		// グローバルデータ作成
		CreateGlobalData(comboNodeName);
		// データ設定
		SetData(data, comboGlobalDatas_[comboNodeName]);
		// ヒットボックス追加
		for (AddHitBoxData addHitBoxData : addHitBoxDatas) {
			data.GetComboHitBox().AddCollider(addHitBoxData.hitBoxData, comboGlobalDatas_[comboNodeName]);
		}
		// 使うヒットボックスクリア
		data.GetComboHitBox().ClearUseHitBox();
		// 使うヒットボックス追加
		for (AddHitBoxData addHitBoxData : addHitBoxDatas) {
			data.GetComboHitBox().AddUseHitBox(addHitBoxData.hitBoxData.name);
		}
		// コンボノード追加
		AddComboNode(comboNodeName, data.GetComboMotion().GetComboAnimation().GetData().animationName_, data);
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

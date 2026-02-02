#include "ComboSystem.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>


#pragma region main

void ComboSystem::Initialize(BaseCharacter* character, Engine::GlobalVariables* globalVariables) {
	this->globalVariables = globalVariables;
	
	comboStateMachine_ = std::make_unique<ComboStateMachine>(character);
}

void ComboSystem::ClearNode() {
	comboNodes_.clear();
}

void ComboSystem::AddComboNode(const std::string& name, std::shared_ptr<ComboNodeState> node) {
	// 既に存在する場合は追加しない
	if (comboNodes_.find(name) != comboNodes_.end()) {
		return;
	}
	comboNodes_[name] = node;
}

void ComboSystem::AddComboNode(const std::string& nodeName, const std::string animationName , const ComboData& data){
	// 既に存在する場合は追加しない
	if (comboNodes_.find(nodeName) != comboNodes_.end()) {
		return;
	}
	// ノード生成
	std::shared_ptr<ComboNodeState> node = std::make_shared<ComboNodeState>(animationName, data);
	comboNodes_[nodeName] = node;				// ノード追加
	comboNodes_[nodeName]->SetName(nodeName);	// ノード名設定
}

void ComboSystem::ConnectCombo(const std::string& from, AttackInput input, const std::string& to){
	auto itFrom = comboNodes_.find(from);
	auto itTo = comboNodes_.find(to);
	if (itFrom != comboNodes_.end()) {
		if(itTo != comboNodes_.end())
		itFrom->second->SetNextState(input, itTo->second);
	}
}

void ComboSystem::StartCombo(const std::string& name){
	auto it = comboNodes_.find(name);
	if (it != comboNodes_.end()) {
		comboStateMachine_->SetRoot(it->second);
	}
}

#pragma endregion // main



#pragma region Global

void ComboSystem::ApplyGlobalComboData(const std::string& name, ComboGlovalData& data) {
	globalVariables->CreateGroup(name);


	

	globalVariables->AddItem(name, "ヒットボックス発生時間", data.hitBoxWindowStart_);
	globalVariables->AddItem(name, "ヒットボックス生存時間", data.hitBoxLifeTime_);

	// リアクション
	globalVariables->AddItem(name, "ダメージ", data.reaction.damage);
	globalVariables->AddItem(name, "Y方向ノックバック", data.reaction.isVerticalBoost_);
	globalVariables->AddItem(name, "ノックバック力", data.reaction.knockbackPower);
	globalVariables->AddItem(name, "Y方向ノックバック力", data.reaction.knockbackPowerY);
	globalVariables->AddItem(name, "ノックバック持続時間", data.reaction.knockbackDuration_);

	globalVariables->AddItem(name, "コンボ入力受付開始時間", data.stateInputStartTime);
	globalVariables->AddItem(name, "コンボ入力受付終了時間", data.stateInputEndTime);
	globalVariables->AddItem(name, "コンボ終了時間", data.stateEndTime);
	globalVariables->AddItem(name, "コンボ移行時間", data.stateNextTime);
	globalVariables->AddItem(name, "コンボ中の重力", data.isGravity);
	globalVariables->AddItem(name, "コンボ中の重力強度", data.gravityScale);


	globalVariables->AddItem(name, "コンボキャンセル受付開始時間", data.stateCancelStartTime);
	globalVariables->AddItem(name, "コンボキャンセル受付終了時間", data.stateCancelEndTime);


	globalVariables->AddItem(name, "コンボ中の移動スピード",data.moveSpeed_);
	globalVariables->AddItem(name, "コンボ中の移動開始時間", data.moveWindowStart_);
	globalVariables->AddItem(name, "コンボ中の移動終了時間", data.moveWindowEnd_);
	globalVariables->AddItem(name, "コンボ中の移動強制", data.isCompulsionMove_);


	globalVariables->AddItem(name, "アニメーション名前", data.animationName);
	globalVariables->AddItem(name, "アニメーション速度", data.animationSpeed_);
	globalVariables->AddItem(name, "アニメーション遷移時間", data.animationBlendTime_);

	globalVariables->AddItem(name, "エフェクト(トレイル)発生時間", data.trailEffectStartTime);
	globalVariables->AddItem(name, "エフェクト(トレイル)生存時間", data.trailEffectLifeTime);


	globalVariables->AddItem(name, "親オブジェクト名前", data.parentName_);

	globalVariables->AddEnumItem(name, "ヒットボックス発生条件タイプ", data.spawnType_, "HitBoxSpawnType");
	globalVariables->AddEnumItem(name, "ヒットボックス依存先タイプ", data.dependenceType_, "HitBoxParentType");

	globalVariables->AddEnumItem(name, "終了条件タイプ", data.endConditionType, "EndConditionType");


	// 保存項目の適応
	GetGlobalComboData(name, data);
};

void ComboSystem::GetGlobalComboData(const std::string& name, ComboGlovalData& data) {
	data.hitBoxWindowStart_ = globalVariables->GetValue<float>(name, "ヒットボックス発生時間");
	data.hitBoxLifeTime_ = globalVariables->GetValue<float>(name, "ヒットボックス生存時間");
	
	// リアクション
	data.reaction.damage = globalVariables->GetValue<float>(name, "ダメージ");
	data.reaction.isVerticalBoost_ = globalVariables->GetValue<bool>(name, "Y方向ノックバック");
	data.reaction.knockbackPower = globalVariables->GetValue<float>(name, "ノックバック力");
	data.reaction.knockbackPowerY = globalVariables->GetValue<float>(name, "Y方向ノックバック力");
	data.reaction.knockbackDuration_ = globalVariables->GetValue<float>(name, "ノックバック持続時間");


	data.stateInputStartTime = globalVariables->GetValue<float>(name, "コンボ入力受付開始時間");
	data.stateInputEndTime = globalVariables->GetValue<float>(name, "コンボ入力受付終了時間");
	data.stateEndTime = globalVariables->GetValue<float>(name, "コンボ終了時間");
	data.stateNextTime = globalVariables->GetValue<float>(name, "コンボ移行時間");
	data.isGravity = globalVariables->GetValue<bool>(name, "コンボ中の重力");
	data.gravityScale = globalVariables->GetValue<float>(name, "コンボ中の重力強度");

	data.moveSpeed_ = globalVariables->GetValue<float>(name,"コンボ中の移動スピード");
	data.moveWindowStart_ = globalVariables->GetValue<float>(name,"コンボ中の移動開始時間");
	data.moveWindowEnd_ = globalVariables->GetValue<float>(name,"コンボ中の移動終了時間");
	data.isCompulsionMove_ = globalVariables->GetValue<bool>(name,"コンボ中の移動強制");


	data.stateCancelStartTime = globalVariables->GetValue<float>(name, "コンボキャンセル受付開始時間");
	data.stateCancelEndTime = globalVariables->GetValue<float>(name, "コンボキャンセル受付終了時間");


	data.animationName = globalVariables->GetValue<std::string>(name, "アニメーション名前");
	data.animationSpeed_ = globalVariables->GetValue<float>(name, "アニメーション速度");
	data.animationBlendTime_ = globalVariables->GetValue<float>(name, "アニメーション遷移時間");


	data.trailEffectStartTime = globalVariables->GetValue<float>(name, "エフェクト(トレイル)発生時間");
	data.trailEffectLifeTime = globalVariables->GetValue<float>(name, "エフェクト(トレイル)生存時間");


	data.parentName_ = globalVariables->GetValue<std::string>(name, "親オブジェクト名前");

	data.spawnType_ = globalVariables->GetEnumValue<HitBoxSpawnType>(name, "ヒットボックス発生条件タイプ");
	data.dependenceType_ = globalVariables->GetEnumValue<HitBoxParentType>(name, "ヒットボックス依存先タイプ");

	data.endConditionType = globalVariables->GetEnumValue<EndConditionType>(name, "終了条件タイプ");
}

void ComboSystem::SetGlobalComboData(const std::string& name, ComboGlovalData& data){
	globalVariables->SetValue(name, "コンボ入力受付開始時間", data.stateInputStartTime);
	globalVariables->SetValue(name, "コンボ入力受付終了時間", data.stateInputEndTime);

	globalVariables->SetValue(name, "ヒットボックス発生時間", data.hitBoxWindowStart_);
	globalVariables->SetValue(name, "ヒットボックス生存時間", data.hitBoxLifeTime_);

	globalVariables->SetValue(name, "コンボ終了時間", data.stateEndTime);
	globalVariables->SetValue(name, "コンボ移行時間", data.stateNextTime);

	globalVariables->SetValue(name, "コンボ中の移動開始時間", data.moveWindowStart_);
	globalVariables->SetValue(name, "コンボ中の移動終了時間", data.moveWindowEnd_);
	
	globalVariables->SetValue(name, "コンボ中の移動スピード", data.moveSpeed_);
	globalVariables->SetValue(name, "コンボ中の移動強制", data.isCompulsionMove_);


	globalVariables->SetValue(name, "コンボキャンセル受付開始時間", data.stateCancelStartTime);
	globalVariables->SetValue(name, "コンボキャンセル受付終了時間", data.stateCancelEndTime);


	globalVariables->SetValue(name, "アニメーション名前", data.animationName);
	globalVariables->SetValue(name, "アニメーション速度", data.animationSpeed_);
	globalVariables->SetValue(name, "アニメーション遷移時間", data.animationBlendTime_);

	
	globalVariables->SetValue(name, "エフェクト(トレイル)発生時間", data.trailEffectStartTime);
	globalVariables->SetValue(name, "エフェクト(トレイル)生存時間", data.trailEffectLifeTime);

	globalVariables->SetValue(name, "ノックバック持続時間", data.reaction.knockbackDuration_);
	globalVariables->SetValue(name, "ノックバック力", data.reaction.knockbackPower);
	globalVariables->SetValue(name, "Y方向ノックバック力", data.reaction.knockbackPowerY);
	globalVariables->SetValue(name, "Y方向ノックバック", data.reaction.isVerticalBoost_);
	globalVariables->SetValue(name, "ダメージ", data.reaction.damage);




	globalVariables->SetValue(name, "親オブジェクト名前", data.parentName_);

	globalVariables->SetEnumValue(name, "ヒットボックス発生条件タイプ", data.spawnType_, "HitBoxSpawnType");
	globalVariables->SetEnumValue(name, "ヒットボックス依存先タイプ", data.dependenceType_, "HitBoxParentType");
	globalVariables->SetEnumValue(name, "終了条件タイプ", data.endConditionType, "EndConditionType");
}

void ComboSystem::SetGlobalComboDatas(){
	for (auto& pair : comboGlobalDatas_) {
		SetGlobalComboData(pair.first, pair.second);
	}
};

void ComboSystem::SetData(ComboData& data, const ComboGlovalData& gData)
{
	///
	/// ヒットボックス
	/// 

	data.GetComboHitBox().GetData().hitBpxWindowStart_ = gData.hitBoxWindowStart_;	// 発生時間
	data.GetComboHitBox().GetData().lifeTime_ = gData.hitBoxLifeTime_;				// 生成時間
	
	///
	/// 受付
	/// 
	
	
	// 入力受付時間設定
	data.GetComboCondition().ConditionStartEnd(gData.stateInputStartTime, gData.stateInputEndTime);
	data.GetComboCondition().GetData().stateNextTime = gData.stateNextTime;
	data.GetComboCondition().GetData().stateEndTime = gData.stateEndTime;
	// コンボボタン設定
	ComboButton bo = ComboButton(GamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed);
	// 押し続ける
	data.GetComboCondition().GetData().button_ = bo;
	std::vector<ComboButton> button;
	button.push_back(bo);
	data.GetComboCondition().GetInput().comboSequence_.RegisterCombo(button);
	
	///
	/// モーション
	/// 
	
	// 重力
	data.GetComboMotion().GetData().isGravity_ = gData.isGravity;
	data.GetComboMotion().GetData().gravityScale_ = gData.gravityScale;
	// アニメーションスピード
	data.GetComboMotion().GetData().animationBlendTime_ = gData.animationBlendTime_;
	data.GetComboMotion().GetData().animationSpeed_ = gData.animationSpeed_;
	data.GetComboMotion().GetData().animationName_ = gData.animationName;
	// 移動
	data.GetComboMotion().GetData().speed_ = gData.moveSpeed_;
	data.GetComboMotion().GetData().moveWindowStart_ = gData.moveWindowStart_;
	data.GetComboMotion().GetData().moveWindowEnd_ = gData.moveWindowEnd_;
	data.GetComboMotion().GetData().isCompulsionMove_ = gData.isCompulsionMove_;


	///
	/// エフェクト
	/// 

	// トレイルエフェクト
	data.GetComboEffect().GetData().startTmer = gData.trailEffectStartTime;
	data.GetComboEffect().GetData().lifeTime = gData.trailEffectLifeTime;


	// 親子付け関係
	data.GetComboHitBox().GetData().parentName_ = gData.parentName_;
	data.GetComboHitBox().SetPerent(GetParentTransform(gData.parentName_));
	data.GetComboHitBox().GetData().offset_ = gData.parentOffset_;
	data.GetComboHitBox().GetData().dependenceType_ = gData.dependenceType_;
	data.GetComboHitBox().GetData().spawnType_ = gData.spawnType_;

	// 終了条件
	data.GetComboCondition().GetData().type = gData.endConditionType;

}

void ComboSystem::CreateCombo(const std::string comboNodeName, const std::vector<AddHitBoxData> addHitBoxDatas,
	GamePadButton button)
{
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

	// コンボボタン設定
	ComboButton bo = ComboButton(button, ComboButtonInputType::kPressed);
	// 押し続ける
	data.GetComboCondition().GetData().button_ = bo;
	std::vector<ComboButton> buttons;
	buttons.push_back(bo);
	data.GetComboCondition().GetInput().comboSequence_.RegisterCombo(buttons);

	// コンボノード追加
	AddComboNode(comboNodeName, data.GetComboMotion().GetData().animationName_, data);
}

void ComboSystem::CreateGlobalData(const std::string comboNodeName){
	// 既に存在する場合は追加しない
	if (comboGlobalDatas_.find(comboNodeName) != comboGlobalDatas_.end()) {
		return;
	}
	comboGlobalDatas_[comboNodeName] = ComboGlovalData(); // グローバルデータ追加
	ApplyGlobalComboData(comboNodeName, comboGlobalDatas_[comboNodeName]); // グローバルデータ適応
	GetGlobalComboData(comboNodeName, comboGlobalDatas_[comboNodeName]); // グローバルデータ取得
}

#pragma endregion // 保存　適応


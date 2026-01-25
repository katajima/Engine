#include "ComboSystem.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>

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


	globalVariables->AddItem(name, "アニメーション速度", data.animationSpeed_);
	globalVariables->AddItem(name, "アニメーション遷移時間", data.animationBlendTime_);

	globalVariables->AddItem(name, "エフェクト(トレイル)発生時間", data.trailEffectStartTime);
	globalVariables->AddItem(name, "エフェクト(トレイル)生存時間", data.trailEffectLifeTime);


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

	data.animationSpeed_ = globalVariables->GetValue<float>(name, "アニメーション速度");
	data.animationBlendTime_ = globalVariables->GetValue<float>(name, "アニメーション遷移時間");


	data.trailEffectStartTime = globalVariables->GetValue<float>(name, "エフェクト(トレイル)発生時間");
	data.trailEffectLifeTime = globalVariables->GetValue<float>(name, "エフェクト(トレイル)生存時間");
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


	globalVariables->SetValue(name, "アニメーション速度", data.animationSpeed_);
	globalVariables->SetValue(name, "アニメーション遷移時間", data.animationBlendTime_);

	
	globalVariables->SetValue(name, "エフェクト(トレイル)発生時間", data.trailEffectStartTime);
	globalVariables->SetValue(name, "エフェクト(トレイル)生存時間", data.trailEffectLifeTime);



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
	data.GetComboMotion().GetData().animationSpeed_ = gData.animationSpeed_;
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
}

void ComboSystem::CreateCombo(const std::string comboNodeName, const std::string animationName,const std::vector<AddHitBoxData> addHitBoxDatas, Engine::WorldTransform* perent,
	const ComboConditionData comboConditionData, const HitBoxData hitBoxData, GamePadButton button)
{
	ComboData data{};

	// ヒットボックス追加
	for(AddHitBoxData addHitBoxData : addHitBoxDatas){
		data.GetComboHitBox().AddCollider(addHitBoxData.hitBoxData, addHitBoxData.comboGlovalData);
	}
	// 使うヒットボックスクリア
	data.GetComboHitBox().ClearUseHitBox();
	// 使うヒットボックス追加
	for (AddHitBoxData addHitBoxData : addHitBoxDatas){
		data.GetComboHitBox().AddUseHitBox(addHitBoxData.hitBoxData.name);
	}

	// データ設定
	SetData(data, addHitBoxDatas[0].comboGlovalData);

	// 親子付けの設定
	data.GetComboHitBox().SetPerent(perent);

	// コンボの終了条件設定
	data.GetComboCondition().GetData().type = comboConditionData.type;



	// コンボボタン設定
	ComboButton bo = ComboButton(button, ComboButtonInputType::kPressed);
	// 押し続ける
	data.GetComboCondition().GetData().button_ = bo;
	std::vector<ComboButton> buttons;
	buttons.push_back(bo);
	data.GetComboCondition().GetInput().comboSequence_.RegisterCombo(buttons);


	data.GetComboHitBox().GetData().dependenceType_ = hitBoxData.dependenceType_;	// 依存関係
	data.GetComboHitBox().GetData().spawnType_ = hitBoxData.spawnType_;			// 発生条件
	data.GetComboHitBox().GetData().offset_ = hitBoxData.offset_;


	// コンボノード追加
	AddComboNode(comboNodeName,animationName, data);	
}

#pragma endregion // 保存　適応


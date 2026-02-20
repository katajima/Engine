#include "MoveComponent.h"


void MovementComponent::Initialize(Engine::GlobalVariables* globalVariables, ControlType type, const std::string& name) {
	this->globalVariables = globalVariables;
	
	name_ = "MoveData" + name;
	// 移動システムの生成
	moveSystem_ = std::make_unique<MoveSystem>();
	moveSystem_->Initialize();
	// ジャンプシステムの生成
	jumpSystem_ = std::make_unique<JumpSystem>();
	jumpSystem_->Initialize();
	// ダッシュシステムの生成
	dashSystem_ = std::make_unique<DashSystem>();
	dashSystem_->Initialize();
	// 移動制限の生成
	movementRestrictions_ = std::make_unique<MovementRestrictions>();
	movementRestrictions_->Initialize({ Vector3::Set(-100.0f) }, { Vector3::Set(100.0f) });

	// 操作タイプの設定
	controlType_ = type;
	if (useGlobal_) {
		ApplyGlobalData(name_);
	}
};

void MovementComponent::Update(float dt, Engine::WorldTransform& object, Engine::RigidBodyComponent& rigid, Engine::Input* input) {
	if (useGlobal_) {
		SetGlobalData(name_);
	}
	// ダッシュ時の移動方向を移動システムから取得してダッシュシステムに渡す
	dashSystem_->SetDirection(moveSystem_->GetDirection());
	// ダッシュシステムの更新
	dashSystem_->Update(dt, object, rigid);
	// ジャンプシステムにダッシュ状態を伝える
	jumpSystem_->SetIsDash(dashSystem_->IsDash());
	// ジャンプシステムの更新
	jumpSystem_->Update(dt, object, rigid);
	// 移動システムに空中状態を伝える
	moveSystem_->SetIsAir(!jumpSystem_->GetIsLanding());
	// ダッシュしているかを移動システムに伝える
	moveSystem_->SetIsDash(dashSystem_->IsDash());
	// 移動システムの更新
	if (controlType_ == ControlType::Manual) {	// 手動操作なら入力を渡す
		

		moveSystem_->Update(dt, object, input);
	}
	else {
		moveSystem_->UpdateEnemy(dt, object);
	}
	// 移動制限の更新
	movementRestrictions_->Update(object);
}

#pragma region MyRegion

void MovementComponent::ApplyGlobalData(const std::string& name) {
	
	globalVariables->CreateGroup(name);
	
	
	// ジャンプ
	globalVariables->AddItem(name, "ジャンプ力", jumpSystem_->GetData().power_);
	globalVariables->AddItem(name, "ジャンプ上昇時の重力", jumpSystem_->GetData().upGravity_);
	globalVariables->AddItem(name, "ジャンプ下降時の重力", jumpSystem_->GetData().fallGravity_);

	// 移動
	globalVariables->AddItem(name, "移動最小速度", moveSystem_->GetData().minSpeed);
	globalVariables->AddItem(name, "移動最大速度", moveSystem_->GetData().maxSpeed);
	globalVariables->AddItem(name, "移動加速度", moveSystem_->GetData().speedAcceleration);
	globalVariables->AddItem(name, "回転速度", moveSystem_->GetData().rotationSpeed);
	globalVariables->AddItem(name, "スティック移動量変化", moveSystem_->GetData().isStickToSpeed);
	globalVariables->AddItem(name, "空中での移動量変化", moveSystem_->GetData().isLimitAirSpeed);
	globalVariables->AddItem(name, "空中移動量変化率", moveSystem_->GetData().airSpeedRate);

	// ダッシュ
	globalVariables->AddItem(name, "ダッシュ時間", dashSystem_->GetData().maxTime);
	globalVariables->AddItem(name, "ダッシュ加速度", dashSystem_->GetData().acceleration);
	globalVariables->AddItem(name, "ダッシュ減速度", dashSystem_->GetData().friction);
	globalVariables->AddItem(name, "ダッシュ初速度", dashSystem_->GetData().startSpeed);
	globalVariables->AddItem(name, "ダッシュ中の重力", dashSystem_->GetData().isDashGravity);


	SetGlobalData(name);
}

void MovementComponent::SetGlobalData(const std::string& name)
{
	// ジャンプ
	jumpSystem_->GetData().power_ = globalVariables->GetValue<float>(name, "ジャンプ力");
	jumpSystem_->GetData().upGravity_ = globalVariables->GetValue<float>(name, "ジャンプ上昇時の重力");
	jumpSystem_->GetData().fallGravity_ = globalVariables->GetValue<float>(name, "ジャンプ下降時の重力");

	// 移動
	moveSystem_->GetData().minSpeed = globalVariables->GetValue<float>(name, "移動最小速度");
	moveSystem_->GetData().maxSpeed = globalVariables->GetValue<float>(name, "移動最大速度");
	moveSystem_->GetData().speedAcceleration = globalVariables->GetValue<float>(name, "移動加速度");
	moveSystem_->GetData().rotationSpeed = globalVariables->GetValue<float>(name, "回転速度");
	moveSystem_->GetData().isStickToSpeed = globalVariables->GetValue<bool>(name, "スティック移動量変化");
	moveSystem_->GetData().isLimitAirSpeed = globalVariables->GetValue<bool>(name, "空中での移動量変化");
	moveSystem_->GetData().airSpeedRate = globalVariables->GetValue<float>(name, "空中移動量変化率");

	// ダッシュ
	dashSystem_->GetData().maxTime = globalVariables->GetValue<float>(name, "ダッシュ時間");
	dashSystem_->GetData().acceleration = globalVariables->GetValue<float>(name, "ダッシュ加速度");
	dashSystem_->GetData().friction = globalVariables->GetValue<float>(name, "ダッシュ減速度");
	dashSystem_->GetData().startSpeed = globalVariables->GetValue<float>(name, "ダッシュ初速度");
	dashSystem_->GetData().isDashGravity = globalVariables->GetValue<bool>(name, "ダッシュ中の重力");

}


#pragma endregion // 保存と適応






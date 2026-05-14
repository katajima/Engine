#include "MoveComponent.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"

void MovementComponent::Initialize(Character::BaseCharacter* owner, InputSystem* input, Engine::GlobalVariables* globalVariables, ControlType type, const std::string& name) {
	this->globalVariables = globalVariables;
	this->owner = owner;

	name_ = "MoveData" + name;
	// 移動システムの生成
	moveSystem_ = std::make_unique<MoveSystem>();
	moveSystem_->Initialize();
	// ジャンプシステムの生成
	jumpSystem_ = std::make_unique<JumpSystem>();
	jumpSystem_->Initialize();
	// 移動リクエストシステムの生成
	moveRequestSystem_ = std::make_unique<MoveRequestSystem>();
	// 移動制限の生成
	movementRestrictions_ = std::make_unique<MovementRestrictions>();
	movementRestrictions_->Initialize({ Vector3::Set(-100.0f) }, { Vector3::Set(100.0f) });

	// 移動リクエスト集約選択クラス
	locomotionCoordinator_ = std::make_unique<LocomotionCoordinator>();

	// 移動反映クラス初期化
	movementSystem_ = std::make_unique<MovementSystem>();
	movementSystem_->Initialize();

	// 操作タイプの設定
	controlType_ = type;
	if (useGlobal_) {
		ApplyGlobalData(name_);
	}
};

void MovementComponent::Update(Engine::WorldTransform& object, Engine::RigidBodyComponent& rigid,const Character::CharacterContext& ctx) {
	if (useGlobal_) {
		SetGlobalData(name_);
	}
	// リクエスト集約選択クラス開始
	locomotionCoordinator_->BeginFrame(ctx);

	// ジャンプシステムの更新
	jumpSystem_->Update(ctx, *locomotionCoordinator_.get());

	if (controlType_ == ControlType::Manual) {	// 手動操作なら入力を渡す
		// 移動システム更新
		moveSystem_->Update(ctx, *locomotionCoordinator_.get());
	}
	else {
		// 移動システム更新(非操作)
		moveSystem_->UpdateEnemy(ctx, *locomotionCoordinator_.get());
	}
	// 移動リクエストシステム更新
	moveRequestSystem_->Update(ctx, *locomotionCoordinator_.get());	
	// リクエスト集約選択クラスで移動コマンド生成
	MoveCommand cmd = locomotionCoordinator_->BuildCommand();

	// 移動反映クラス更新
	movementSystem_->Update(ctx, cmd, object, rigid);

	// 移動制限の更新
	movementRestrictions_->Update(object);
}

#pragma region MyRegion

void MovementComponent::ApplyGlobalData(const std::string& name) {

	globalVariables->CreateGroup(name);


	// ジャンプ
	globalVariables->AddItem(name, "ジャンプ力", jumpSystem_->GetData().power);
	globalVariables->AddItem(name, "ジャンプ上昇時の重力", jumpSystem_->GetData().upGravity);
	globalVariables->AddItem(name, "ジャンプ下降時の重力", jumpSystem_->GetData().fallGravity);

	// 移動
	globalVariables->AddItem(name, "移動最小速度", moveSystem_->Data().minSpeed);
	globalVariables->AddItem(name, "移動最大速度", moveSystem_->Data().maxSpeed);
	globalVariables->AddItem(name, "移動加速度", moveSystem_->Data().speedAcceleration);
	globalVariables->AddItem(name, "回転速度", moveSystem_->Data().rotationSpeed);
	globalVariables->AddItem(name, "スティック移動量変化", moveSystem_->Data().isStickToSpeed);
	globalVariables->AddItem(name, "空中での移動量変化", moveSystem_->Data().isLimitAirSpeed);
	globalVariables->AddItem(name, "空中移動量変化率", moveSystem_->Data().airSpeedRate);

	

	SetGlobalData(name);
}

void MovementComponent::SetGlobalData(const std::string& name)
{
	// ジャンプ
	jumpSystem_->Data().power = globalVariables->GetValue<float>(name, "ジャンプ力");
	jumpSystem_->Data().upGravity = globalVariables->GetValue<float>(name, "ジャンプ上昇時の重力");
	jumpSystem_->Data().fallGravity = globalVariables->GetValue<float>(name, "ジャンプ下降時の重力");

	// 移動
	moveSystem_->Data().minSpeed = globalVariables->GetValue<float>(name, "移動最小速度");
	moveSystem_->Data().maxSpeed = globalVariables->GetValue<float>(name, "移動最大速度");
	moveSystem_->Data().speedAcceleration = globalVariables->GetValue<float>(name, "移動加速度");
	moveSystem_->Data().rotationSpeed = globalVariables->GetValue<float>(name, "回転速度");
	moveSystem_->Data().isStickToSpeed = globalVariables->GetValue<bool>(name, "スティック移動量変化");
	moveSystem_->Data().isLimitAirSpeed = globalVariables->GetValue<bool>(name, "空中での移動量変化");
	moveSystem_->Data().airSpeedRate = globalVariables->GetValue<float>(name, "空中移動量変化率");
}


#pragma endregion // 保存と適応






#include "MoveComponent.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"

void MovementComponent::Initialize(Character::BaseCharacter* owner,InputSystem* input,Engine::GlobalVariables* globalVariables, ControlType type, const std::string& name) {
	this->globalVariables = globalVariables;
	this->owner = owner;

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
	// 攻撃移動システムの生成
	attackMoveSystem_ = std::make_unique<AttackMoveSystem>();
	attackMoveSystem_->Initialize();
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

void MovementComponent::Update(float dt, Engine::WorldTransform& object, Engine::RigidBodyComponent& rigid, InputSystem* input) {
	if (useGlobal_) {
		SetGlobalData(name_);
	}

	// コンテキスト
	LocomotionContext ctx;
	if (input) {
		ctx.input = *input;						// 入力状態
	}
	ctx.position = object.GetWorldPosition();	// 位置
	ctx.state = owner->GetCurrentMainState();				// 状態
	ctx.dt = dt;								// デルタタイム
	if (ctx.state == Character::CharacterMainState::Attack) {
		ctx.isAttacking = true;					// 攻撃
	}
	ctx.isHitStun = false;						// スタン
	ctx.onGround = movementSystem_->IsOnGround();// 着地しているか
	ctx.fallGravity = jumpSystem_->GetData().fallGravity_;
	ctx.upGravity = jumpSystem_->GetData().upGravity_;
	ctx.attackingGravity = attackingGravity;
	ctx.camera = camera;						// カメラ;
	ctx.cameraDirection = movementSystem_->GetDirection();	// 方向
	

	// 移動システムの更新
	if (controlType_ == ControlType::Manual) {	// 手動操作なら入力を渡す
		// リクエスト集約選択クラス開始
		locomotionCoordinator_->BeginFrame(ctx);

		// ダッシュシステムの更新
		dashSystem_->Update(ctx, *locomotionCoordinator_.get());

		// ジャンプシステムの更新
		jumpSystem_->Update(ctx, *locomotionCoordinator_.get(), object, rigid);

		// 攻撃移動システム更新
		attackMoveSystem_->Update(ctx, *locomotionCoordinator_.get(), object, input);

		// 移動システム更新
		moveSystem_->Update(ctx, *locomotionCoordinator_.get(), object, input);

		// リクエスト集約選択クラスで移動コマンド生成
		MoveCommand cmd = locomotionCoordinator_->BuildCommand();

		// 移動反映クラス更新
		movementSystem_->Update(ctx, cmd, object, rigid);
	}
	else {
		moveSystem_->UpdateEnemy(dt);

		if (object.GetWorldPosition().y <= -3.0f) {
			object.translate_.y = -3.0f;	// 地面位置に
			rigid.Velocity().y = 0.0f;			// y速度を0に
			rigid.SetIsGravity(false);			// 重力をオフ
			rigid.SetGravityScale(1.0f);		// 重力スケールリセット
		}
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






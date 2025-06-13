#include "NormalEnemy.h"

#include "DirectXGame/application/base/Player/Player.h"

void NormalEnemy::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera)
{
	Collider::Initialize(camera);
	Collider::SetColliderType(static_cast<uint32_t>(ColliderType::Sphere));
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));
	Collider::SetRadius(3.0f);
	Capsule cap = Collider::GetCapsule();
	cap.radius = 6.0f;
	//cap.segment.origin.z = 10;
	Collider::SetCapsule(cap);
	Collider::SetColor(Vector4{ 1,0,1,1 });

	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;

	transBase_.Initialize();
	transBase_.translate_ = position;

	object_->Initialize(entity3DManager_);
	object_->SetModel("enemy2.obj");
	object_->SetCamera(camera);
	object_->worldtransform_.parent_ = &transBase_;
	object_->worldtransform_.scale_ = { 1.7f,1.7f,1.7f };


	isAlive_ = true;

	parameter_.HP = 100;
	parameter_.MaxHP = 100;

	parameter_.moveSpeed = 3.0f;

	Initialize2D();


	InitParticle();

	nullChek = Matrix4x4::Identity();
}

void NormalEnemy::Update()
{
	HitStpoTime();
	if (parameter_.HP <= 0) {
		if (isAlive_ == true) {
			ductEmit_->Update();
			tireEmit_->Update();
			plankEmit_->Update();
			gearEmit_->Update();
			fenceEmit_->Update();
		}
		isLockOn = false;
		isAlive_ = false;
	}

	if (isAlive_) {
		if (!hit) {
			count = 0.0f;
			Move();
			oldPos_ = { 0,0,0 };
		}
		else {
			oldPos_ = { 0,0,0 };
			HitMotion();
		}
		

		// 煙
		/*dustEmit_->Update();
		dustEmit2_->Update();
		dustEmit3_->Update();*/
	}
	else {
		
	}

	object_->Update();
	transBase_.Update();
}

void NormalEnemy::Draw()
{
	// 生きていたら
	if (isAlive_) {
		object_->Draw();
		//objectSha_.Draw();
	}
}

void NormalEnemy::DrawP()
{
}

void NormalEnemy::Draw2D()
{
	/*if (isLockOn) {
		icon_lockOn->SetPosition(object_->GetScreenPosition());

		icon_lockOn->Update();
		icon_lockOn->Draw();
	}

	if (isAlive_) {

		backHpBer_->SetSize({ parameter_.MaxHP ,15.0f });
		backHpBer_->SetPosition(object_->GetScreenPosition() + Vector2{ 0,-30 });
		backHpBer_->Update();
		backHpBer_->Draw();

		hpBer_->SetPosition(object_->GetScreenPosition() + Vector2{ 0,-27.5f });
		hpBer_->SetSize({ (parameter_.HP * 0.95f),10.0f });
		hpBer_->Update();
		hpBer_->Draw();
	}*/
}

void NormalEnemy::SetPlayer(Player* player)
{
	player_ = player; 
}

void NormalEnemy::Emit()
{
	starEmit_->Update();

	int dirac = rand() % 2;

	if (dirac == 0) {
		hitEmit_->SetVelocityMinMax(-Vector3{ 5,5,0 }, { 5, 5, 0 });
		traiEmit_->SetVelocityMinMax(-Vector3{ 5,5,0 }, { 5, 5, 0 });
	}
	else if (dirac == 1) {
		hitEmit_->SetVelocityMinMax(-Vector3{ 0,5,5 }, { 0, 5, 5 });
		traiEmit_->SetVelocityMinMax(-Vector3{ 0,5,5 }, { 0, 5, 5 });
	}
	else {
		hitEmit_->SetVelocityMinMax(-Vector3{ 5,0,5 }, { 5, 0, 5 });
		traiEmit_->SetVelocityMinMax(-Vector3{ 5,0,5 }, { 5, 0, 5 });
	}

	effectEmit_->Update();
	hitEmit_->Update();
	traiEmit_->Update();
}

void NormalEnemy::ATest()
{
}

void NormalEnemy::Move()
{
	// 回転と移動量の設定
	const float kMoveSpeed = parameter_.moveSpeed; // 移動速度
	// worldTransformBase_.rotation_.y += 0.00f; // 一定量のY軸回転

	// 向いている方向への移動ベクトルの計算
	Vector3 moveDirection = { 0.0f, 0.0f, kMoveSpeed };
	Matrix4x4 rotationMatrix = MakeRotateYMatrix(transBase_.rotate_.y);
	moveDirection = TransformNormal(moveDirection, rotationMatrix);

	// ロックオン座標
	Vector3 lockOnPosition = player_->GetObject3D().GetWorldPosition();

	// 追跡対象からロックオン対象へのベクトル
	Vector3 sub = Subtract(lockOnPosition, transBase_.translate_);

	// Y軸周り角度
	transBase_.rotate_.y = std::atan2(sub.x, sub.z);

	if (Distance(player_->GetCenterPosition(), object_->GetWorldPosition()) >= 5) {

		// 移動
		transBase_.translate_ = Add(transBase_.translate_, moveDirection * Timer());
	}
}

void NormalEnemy::InitParticle()
{
	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();

	

	dustEmit_ = std::make_unique<ParticleEmitter>();
	dustEmit_->Initialize(particleManager, "smokePlane01", "smokePlane01_2");
	dustEmit_->GetFrequency() = 0.25f;
	dustEmit_->SetCount(3);
	dustEmit_->SetParent(object_->worldtransform_);
	dustEmit_->SetPos({ 0,1.1f,-0.45f });
	dustEmit_->SetRengeMinMax({ 0,0,0 }, { 0,0,0 });
	dustEmit_->SetVelocityMinMax({ 0,2,0 }, { 0, 5, 0 });
	dustEmit_->SetLifeTimeMinMax(2.5f, 2.7f);
	dustEmit_->SetAlphaClipping(0.15f);
	dustEmit_->SetSizeMinMax(Vector3{ 1.6f,1.6f,1.6f }, { 1.8f,1.8f,1.8f });
	dustEmit_->SetColorMinMax({ 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f });
	dustEmit_->SetRotateMinMax(-DegreesToRadians({ 180,180,180 }), DegreesToRadians({ 180,180,180 }));
	dustEmit_->SetUsebillboard(false);
	dustEmit_->SetEnableLighting(false);
	dustEmit_->SetIsAlpha(true);
	dustEmit_->SetIsLifeTimeScale(true);
	dustEmit_->SetLifeTimeScaleTopBottom(ParticleData::TopBottom::kTop);


	dustEmit2_ = std::make_unique<ParticleEmitter>();
	dustEmit2_->Initialize(particleManager, "smokePlane02", "smokePlane02_2");
	dustEmit2_->GetFrequency() = 0.25f;
	dustEmit2_->SetCount(3);
	dustEmit2_->SetParent(object_->worldtransform_);
	dustEmit2_->SetPos({ 0,1.1f,-0.45f });
	dustEmit2_->SetRengeMinMax({ 0,0,0 }, { 0,0,0 });
	dustEmit2_->SetVelocityMinMax({ 0,2,0 }, { 0, 5, 0 });
	dustEmit2_->SetLifeTimeMinMax(2.0f, 2.0f);
	dustEmit2_->SetAlphaClipping(0.10f);
	dustEmit2_->SetSizeMinMax(Vector3{ 1.0f,1.0f,1.0f }, { 1.5f,1.5f,1.5f });
	dustEmit2_->SetColorMinMax({ 0.1f, 0.1f, 0.1f }, { 0.1f, 0.1f, 0.1f });
	dustEmit2_->SetRotateMinMax(-DegreesToRadians({ 180,180,180 }), DegreesToRadians({ 180,180,180 }));
	dustEmit2_->SetUsebillboard(false);
	dustEmit2_->SetEnableLighting(false);
	dustEmit2_->SetIsAlpha(true);
	dustEmit2_->SetIsLifeTimeScale(true);
	dustEmit2_->SetLifeTimeScaleTopBottom(ParticleData::TopBottom::kTop);

	dustEmit3_ = std::make_unique<ParticleEmitter>();
	dustEmit3_->Initialize(particleManager, "smokePlane03", "smokePlane03_2");
	dustEmit3_->GetFrequency() = 0.25f;
	dustEmit3_->SetCount(3);
	dustEmit3_->SetParent(object_->worldtransform_);
	dustEmit3_->SetPos({ 0,1.1f,-0.45f });
	dustEmit3_->SetRengeMinMax({ 0,0,0 }, { 0,0,0 });
	dustEmit3_->SetVelocityMinMax({ 0,2,0 }, { 0, 5, 0 });
	dustEmit3_->SetLifeTimeMinMax(2.5f, 2.7f);
	dustEmit3_->SetAlphaClipping(0.10f);
	dustEmit3_->SetSizeMinMax(Vector3{ 1.2f,1.2f,1.2f }, { 1.5f,1.5f,1.5f });
	dustEmit3_->SetColorMinMax({ 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f });
	dustEmit3_->SetRotateMinMax(-DegreesToRadians({ 180,180,180 }), DegreesToRadians({ 180,180,180 }));
	dustEmit3_->SetUsebillboard(false);
	dustEmit3_->SetEnableLighting(false);
	dustEmit3_->SetIsAlpha(true);
	dustEmit3_->SetIsLifeTimeScale(true);
	dustEmit3_->SetLifeTimeScaleTopBottom(ParticleData::TopBottom::kTop);



	starEmit_ = std::make_unique<ParticleEmitter>();
	starEmit_->Initialize(particleManager, "dust", "hitStar");
	starEmit_->GetFrequency() = 0.0f;
	starEmit_->SetCount(1);
	starEmit_->SetParent(object_->worldtransform_);
	starEmit_->SetPos({ 0,0.0f,0.0f });
	starEmit_->SetRotateMinMax(-DegreesToRadians({ 180,180,180 }), DegreesToRadians({ 180,180,180 }));
	starEmit_->SetVelocityMinMax({ 0,0,0 }, { 0, 0, 0 });
	starEmit_->SetLifeTimeMinMax(0.2f, 0.2f);
	starEmit_->SetIsAlpha(true);
	starEmit_->SetUsebillboard(false);
	starEmit_->SetSizeMinMax(Vector3{ 1.6f,1.6f,1.6f }, { 1.8f,1.8f,1.8f });
	starEmit_->SetColorMinMax({ 0.424f, 0.404f, 0.431f }, { 0.424f, 0.404f, 0.431f });

	traiEmit_ = std::make_unique<ParticleEmitter>();
	traiEmit_->Initialize(particleManager, "dust", "hitEffect");
	traiEmit_->GetFrequency() = 0.0f;
	traiEmit_->SetCount(5);
	traiEmit_->SetParent(object_->worldtransform_);
	traiEmit_->SetPos({ 0,0.0f,0.0f });
	traiEmit_->SetRotateMinMax(-DegreesToRadians({ 180,180,180 }), DegreesToRadians({ 180,180,180 }));
	traiEmit_->SetVelocityMinMax({ 0,0,0 }, { 0, 0, 0 });
	traiEmit_->SetLifeTimeMinMax(0.2f, 0.2f);
	traiEmit_->SetIsAlpha(true);
	traiEmit_->SetUsebillboard(false);
	traiEmit_->SetSizeMinMax(Vector3{ 2.6f,2.6f,2.6f }, { 2.8f,2.8f,2.8f });
	traiEmit_->SetColorMinMax({ 1, 0, 0 }, { 1, 1, 0 });
	traiEmit_->SetRengeMinMax(Vector3{ -5,-5,-5 }, Vector3{ 5,5,5 });
	traiEmit_->SetAlphaClipping(0.15f);


	effectEmit_ = std::make_unique<ParticleEmitter>();
	effectEmit_->Initialize(particleManager, "dust", "hitEffect2", ParticleData::SpawnType::kPoint);
	effectEmit_->GetFrequency() = 0.0f;
	effectEmit_->SetCount(1);
	effectEmit_->SetParent(object_->worldtransform_);
	effectEmit_->SetPos({ 0,0.0f,0.0f });
	effectEmit_->SetRotateMinMax(-DegreesToRadians({ 180,180,180 }), DegreesToRadians({ 180,180,180 }));
	effectEmit_->SetVelocityMinMax({ 0,0,0 }, { 0, 0, 0 });
	effectEmit_->SetLifeTimeMinMax(0.2f, 0.3f);
	effectEmit_->SetIsAlpha(true);
	effectEmit_->SetUsebillboard(true);
	effectEmit_->SetSizeMinMax({ 8,8,8 }, { 8,8,8 });
	effectEmit_->SetColorMinMax({ 1, 0, 0 }, { 1, 1, 0 });
	effectEmit_->SetRengeMinMax(Vector3{ -5,-5,-5 }, Vector3{ 5,5,5 });
	effectEmit_->SetAlphaClipping(0.15f);


	hitEmit_ = std::make_unique<ParticleEmitter>();
	hitEmit_->Initialize(particleManager, "dust", "hit");
	hitEmit_->GetFrequency() = 0.0f;
	hitEmit_->SetCount(10);
	hitEmit_->SetParent(object_->worldtransform_);
	hitEmit_->SetPos({ 0,0.0f,0.0f });
	hitEmit_->SetRotateMinMax(-DegreesToRadians({ 90,90,90 }), DegreesToRadians({ 90,90,90 }));
	hitEmit_->SetLifeTimeMinMax(0.5f, 0.6f);
	hitEmit_->SetIsAlpha(true);
	hitEmit_->SetUsebillboard(false);
	hitEmit_->SetSizeMinMax(Vector3{ 3.6f,3.6f,3.6f }, { 3.8f,3.8f,3.8f });
	hitEmit_->SetColorMinMax({ 1, 0, 0 }, { 1, 1, 0 });


	Vector3 scale = Vector3{ 1.0f,1.0f,1.0f };

	// タイヤ
	tireEmit_ = std::make_unique<ParticleEmitter>();
	tireEmit_->Initialize(particleManager, "", "enemyTire");
	tireEmit_->GetFrequency() = 0.0f;
	tireEmit_->SetCount(1);
	tireEmit_->SetParent(object_->worldtransform_);
	tireEmit_->SetPos({ 0,0,0 });
	tireEmit_->SetVelocityMinMax({ -2,10,-2 }, { 2, 10, 2 });
	tireEmit_->SetRotateMinMax(-DegreesToRadians(Vector3{ 90,90,90 }), DegreesToRadians(Vector3{ 90,90,90 }));
	tireEmit_->SetRotateVelocityMinMax(-Vector3{ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f });
	tireEmit_->SetLifeTimeMinMax(2, 2);
	tireEmit_->SetIsGravity(true);
	tireEmit_->SetUsebillboard(false);
	tireEmit_->SetIsAlpha(true);
	tireEmit_->SetIsRotateVelocity(true);
	tireEmit_->SetIsBounce(true);
	tireEmit_->SetSizeMinMax(scale, scale);
	tireEmit_->SetColorMinMax({ 1,1,1,1 }, { 1,1,1,1 });


	// ダクト
	ductEmit_ = std::make_unique<ParticleEmitter>();
	ductEmit_->Initialize(particleManager, "", "enemyDuct");
	ductEmit_->GetFrequency() = 0.0f;
	ductEmit_->SetCount(1);
	ductEmit_->SetParent(object_->worldtransform_);
	ductEmit_->SetPos({ 0,0,0 });
	ductEmit_->SetVelocityMinMax({ -2,10,-2 }, { 2, 10, 2 });
	ductEmit_->SetRotateMinMax(-DegreesToRadians(Vector3{ 90,90,90 }), DegreesToRadians(Vector3{ 90,90,90 }));
	ductEmit_->SetRotateVelocityMinMax(-Vector3{ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f });
	ductEmit_->SetLifeTimeMinMax(2, 2);
	ductEmit_->SetIsGravity(true);
	ductEmit_->SetUsebillboard(false);
	ductEmit_->SetIsAlpha(true);
	ductEmit_->SetIsRotateVelocity(true);
	ductEmit_->SetIsBounce(true);
	ductEmit_->SetSizeMinMax(scale, scale);
	ductEmit_->SetColorMinMax({ 1,1,1,1 }, { 1,1,1,1 });

	// ダクト
	fenceEmit_ = std::make_unique<ParticleEmitter>();
	fenceEmit_->Initialize(particleManager, "", "enemyFence");
	fenceEmit_->GetFrequency() = 0.0f;
	fenceEmit_->SetCount(1);
	fenceEmit_->SetParent(object_->worldtransform_);
	fenceEmit_->SetPos({ 0,0,0 });
	fenceEmit_->SetVelocityMinMax({ -2,10,-2 }, { 2, 10, 2 });
	fenceEmit_->SetRotateMinMax(-DegreesToRadians(Vector3{ 90,90,90 }), DegreesToRadians(Vector3{ 90,90,90 }));
	fenceEmit_->SetRotateVelocityMinMax(-Vector3{ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f });
	fenceEmit_->SetLifeTimeMinMax(3, 4);
	fenceEmit_->SetIsGravity(true);
	fenceEmit_->SetUsebillboard(false);
	fenceEmit_->SetIsAlpha(true);
	fenceEmit_->SetIsRotateVelocity(true);
	fenceEmit_->SetIsBounce(true);
	fenceEmit_->SetSizeMinMax(scale, scale);
	fenceEmit_->SetColorMinMax({ 1,1,1,1 }, { 1,1,1,1 });


	scale = { 0.5f,0.5f,0.5f };
	gearEmit_ = std::make_unique<ParticleEmitter>();
	gearEmit_->Initialize(particleManager, "", "enemyGear");
	gearEmit_->GetFrequency() = 0.0f;
	gearEmit_->SetCount(5);
	gearEmit_->SetParent(object_->worldtransform_);
	gearEmit_->SetPos({ 0,0,0 });
	gearEmit_->SetVelocityMinMax({ -2,3,-2 }, { 2, 4, 2 });
	gearEmit_->SetRotateMinMax(-DegreesToRadians(Vector3{ 90,90,90 }), DegreesToRadians(Vector3{ 90,90,90 }));
	gearEmit_->SetRotateVelocityMinMax(-Vector3{ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f });
	gearEmit_->SetLifeTimeMinMax(3, 3.5f);
	gearEmit_->SetIsGravity(true);
	gearEmit_->SetUsebillboard(false);
	gearEmit_->SetIsAlpha(true);
	gearEmit_->SetIsRotateVelocity(true);
	gearEmit_->SetIsBounce(true);
	gearEmit_->SetSizeMinMax(scale, scale);
	gearEmit_->SetColorMinMax({ 1,1,1,1 }, { 1,1,1,1 });



	scale = { 2,2,2 };
	// 鋼板
	plankEmit_ = std::make_unique<ParticleEmitter>();
	plankEmit_->Initialize(particleManager, "", "enemyPlank");
	plankEmit_->GetFrequency() = 0.0f;
	plankEmit_->SetCount(10);
	plankEmit_->SetParent(object_->worldtransform_);
	plankEmit_->SetPos({ 0,0,0 });
	plankEmit_->SetVelocityMinMax({ -2,2,-2 }, { 2, 3, 2 });
	plankEmit_->SetRotateMinMax(-DegreesToRadians(Vector3{ 90,90,90 }), DegreesToRadians(Vector3{ 90,90,90 }));
	plankEmit_->SetRotateVelocityMinMax(-Vector3{ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f });
	plankEmit_->SetLifeTimeMinMax(3, 3.5f);
	plankEmit_->SetIsGravity(true);
	plankEmit_->SetUsebillboard(false);
	plankEmit_->SetIsAlpha(true);
	plankEmit_->SetIsRotateVelocity(true);
	plankEmit_->SetIsBounce(true);
	plankEmit_->SetSizeMinMax(scale, scale);
	plankEmit_->SetColorMinMax({ 1,1,1,1 }, { 1,1,1,1 });



}

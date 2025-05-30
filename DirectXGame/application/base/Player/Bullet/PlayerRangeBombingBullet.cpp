#include "PlayerRangeBombingBullet.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"



#include "DirectXGame/application/base/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Player/Player.h"

#include "DirectXGame/engine/math/random.h"

void PlayerRangeBombingBullet::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera)
{
	// コライダー設定
	Collider::Initialize(camera);
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon));
	Collider::SetRadius(3.0f);

	// オブジェクト設定
	object_ = std::make_unique<Object3d>();
	object_->Initialize(entity3DManager);
	object_->SetCamera(camera);
	object_->SetModel("player_bullet.obj");
	object_->worldtransform_.translate_ = position;
	object_->Update();
	// Y軸周り角度(θy)
	object_->worldtransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));
	// X軸周り角度(θx)
	object_->worldtransform_.rotate_.x = std::atan2(velocity_.y, -length);

	

	// ダメージ量
	parameter_.damege = 30;
	// 生存フラグ
	isAlive_ = true;
	// 行動フェーズ
	phase_ = 0;


	worldTransformUp_.Initialize();
	worldTransformUp_.parent_ = &object_->worldtransform_;
	worldTransformUp_.translate_.y = 1.5f;

	worldTransformBottom_.Initialize();
	worldTransformBottom_.parent_ = &object_->worldtransform_;
	worldTransformBottom_.translate_.y = -1.5f;

	//worldTransformUp_.Update();
	//worldTransformBottom_.Update();

	// 初期地点記録
	str = object_->worldtransform_.translate_;

	// ランダムな空中位置を設定
	randPosSky = str + Vector3{ -1000,0,0};

	max_y = 600.0f; // 最大Y座標

	randPosSky.y = max_y;




	// 各演出設定
	ParticleManager* particleManager = entity3DManager->GetEffectManager()->GetParticleManager();

	// ミサイル移動中の煙パーティクルエミッター
	moveSmokeEmitter_ = std::make_unique <ParticleEmitter>();
	InitMoveSmoke(moveSmokeEmitter_.get(), particleManager, "smokePlane01");

	// ミサイル移動中の煙パーティクルエミッター2
	moveSmokeEmitter2_ = std::make_unique <ParticleEmitter>();
	InitMoveSmoke(moveSmokeEmitter2_.get(), particleManager, "smokePlane04");

	// ミサイル爆発時の煙パーティクルエミッター
	moveExpEmitter_ = std::make_unique <ParticleEmitter>();
	InitExpSmoke(moveExpEmitter_.get(), particleManager, "smokePlane05");

	expFireEmitter_ = std::make_unique <ParticleEmitter>(); 
	InitExpFire(expFireEmitter_.get(), particleManager, "expPlane01");

	animatedCubeEmitter_ = std::make_unique <ParticleEmitter>();
	InitExpBre(animatedCubeEmitter_.get(), particleManager, "AnimatedCube");

	startSmokeEmitter_ = std::make_unique <ParticleEmitter>();
	InitStartSmoke(startSmokeEmitter_.get(), particleManager, "smokePlane05");


	trailEffect_ = std::make_unique<TrailEffect>();
	trailEffect_->Initialize(entity3DManager->GetEffectManager(), "resources/Texture/Image.png", 0.15f, {1.0f,1.0f,1.0f,1.0f});
	trailEffect_->SetCamera(camera);
	trailEffect_->SetObject(object_.get());


	Vector3 size = { 2.0f, 2.0f, 2.0f };
	hitEmitter_ = std::make_unique <ParticleEmitter>();
	hitEmitter_->Initialize(particleManager, "missileHit", "missileHit", ParticleData::SpawnType::kPoint);
	hitEmitter_->SetCount(2);
	hitEmitter_->SetLifeTimeMinMax(0.25f, 0.25f);
	hitEmitter_->SetColorMinMax({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	hitEmitter_->SetSizeMinMax(size, size);
	hitEmitter_->SetFrequency(0.00f);
	hitEmitter_->SetParent(object_->worldtransform_);
	hitEmitter_->SetPos({0,-50,0});
	hitEmitter_->SetUsebillboardRotZ(true);
	hitEmitter_->SetIsAlpha(true);
	hitEmitter_->SetVelocityMinMax({}, {});
	hitEmitter_->SetRotateMinMax({ 0,0,-3.14f }, { 0,0,3.14f });
	hitEmitter_->SetIsEmit(false);


	primitiveCylinder_ = std::make_unique<Primitive>();

	ShapeParameter::Cylinder cylinderParam;
	cylinderParam.height = cilnderHeight_;
	cylinderParam.innerRadius = 6.0f;
	cylinderParam.outerRadius = 12.0f;
	cylinderParam.isCover = false;
	cylinderParam.segments = 16;


	primitiveCylinder_->Initialize<ShapeParameter::Cylinder>(entity3DManager->GetPrimitiveCommon(), Primitive::ShapeType::Cylinder, cylinderParam, "resources/Texture/effect/gradationLine.png");
	primitiveCylinder_->SetPsoType(Primitive::PsoType::kNoCullRingClamp);

	hitObject_ = std::make_unique<Object3d>();
	hitObject_->Initialize(entity3DManager, Object3d::ObjectType::kPrimitive, Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_NONE);
	hitObject_->SetPrimitive(primitiveCylinder_.get());
	hitObject_->worldtransform_.translate_.z = 50.0f;
	hitObject_->worldtransform_.rotate_.y = DegreesToRadians(-90);
	hitObject_->SetName("cylinder");
	hitObject_->SetIsDraw(false);
	hitObject_->SetCamera(camera);

	

	enemyPos_.x = targetRange_.position.x = Random::RandomFloat(-targetRange_.radius,targetRange_.radius);
	enemyPos_.z = targetRange_.position.z = Random::RandomFloat(-targetRange_.radius,targetRange_.radius);
	enemyPos_.y = -2.0f;
	

	flag_ = false;
}

void PlayerRangeBombingBullet::Update()
{
	hitEmitter_->SetIsEmit(false);
	// 時間経過でデス
	if (Hit) {
		hitEmitter_->SetIsEmit(true);
		hitEmitter_->Emit();
		isAlive_ = false;
	}

	Vector3 strSmoke = object_->worldtransform_.worldMat_.GetWorldPosition();
	Vector3 endSmoke = object_->worldtransform_.worldPreMat_.GetWorldPosition();
	moveSmokeEmitter_->SetRengeMinMax(strSmoke, endSmoke);
	moveSmokeEmitter2_->SetRengeMinMax(strSmoke, endSmoke);



	if (count > 0 || phase_ > 0) {
		
	}

	if (isAlive_) {
		Vector3 norm;

		switch (phase_)
		{
		case 0:
			if (t > 0.0f) {
				flag_ = true;
			}
			else {
				startSmokeEmitter_->SetPos(object_->worldtransform_.worldMat_.GetWorldPosition() + Vector3{10.0f,-5.0f,0.0f});
				startSmokeEmitter_->Update();
			}

			count += GetTimer() * 2.0f;

			t = count;

			object_->worldtransform_.translate_ = Lerp(str, randPosSky, t);

			norm = randPosSky - str;

			velocity_ = norm * GetTimer();
			
			if (t >= 1) {
				t = 0;
				count = 0;
				phase_++;
			}
			
			
			break;
		case 1:

			flag_ = true;

			count += GetTimer();
			if (count >= max_count)
			{
				Vector3 pos = enemyPos_ - object_->GetWorldPosition();

				float posLength = Length(pos);


				if (posLength > 500.0f) {
					moveSmokeEmitter_->SetLifeTimeMinMax(0.2f, 0.4f);
					moveSmokeEmitter_->SetCount(5);
					moveSmokeEmitter2_->SetCount(7);
				}
				else if (posLength > 100.0f) {
					moveSmokeEmitter_->SetLifeTimeMinMax(0.3f, 0.5f);
					moveSmokeEmitter_->SetCount(5);
					moveSmokeEmitter2_->SetCount(7);
				}
				else {
					moveSmokeEmitter_->SetLifeTimeMinMax(0.7f, 1.0f);
					moveSmokeEmitter_->SetCount(5);
					moveSmokeEmitter2_->SetCount(5);
				}

				Vector3 pos2 = pos;

				velocity_ = pos2.Normalize() * 1000;
				velocity_.y = 0;

				object_->worldtransform_.translate_ += velocity_ * GetTimer();

				if (5 >= DistanceXZ(object_->GetWorldPosition(), enemyPos_)) {
					phase_++;
					posGround = enemyPos_;
					targetPos = posGround - object_->GetWorldPosition();
					count = 0;


				}

			}
			break;
		case 2:
			flag_ = true;

			moveSmokeEmitter_->SetCount(5);
			moveSmokeEmitter2_->SetCount(5);

			count += GetTimer();
			if (count >= max_count)
			{
				velocity_ = targetPos * 3;

				object_->worldtransform_.translate_ += velocity_ * GetTimer();
			}

			if (-0.0f >= object_->GetWorldPosition().y) {
				object_->worldtransform_.translate_.y = 0.5f;
				isAlive_ = false;
				count = 0;
				phase_ = 0;
				hitEmitter_->SetIsEmit(true);
				hitEmitter_->Emit();
				isEffectPlay_ = true;
				flag_ = false;

				moveExpEmitter_->SetPos(object_->worldtransform_.worldMat_.GetWorldPosition());
				moveExpEmitter_->Update();
				expFireEmitter_->SetPos(object_->worldtransform_.worldMat_.GetWorldPosition() +Vector3{0,5,0});
				expFireEmitter_->Update();
				animatedCubeEmitter_->SetPos(object_->worldtransform_.worldMat_.GetWorldPosition() + Vector3{ 0,0,0 });
				animatedCubeEmitter_->Update();
			}

			break;
		}
	}


	worldTransformUp_.Update();
	worldTransformBottom_.Update();
	object_->Update();
	// 
	hitObject_->worldtransform_.translate_ = object_->worldtransform_.translate_;
	hitObject_->worldtransform_.rotate_.x = DegreesToRadians(-90);

	if (isEffectPlay_) {
		time_ += GetTimer();

		object_->SetIsDraw(false);
		hitObject_->SetIsDraw(true);
		hitObject_->worldtransform_.translate_ = object_->worldtransform_.translate_ + Vector3{ 0.0f,5.0f ,0.0f };
		hitObject_->worldtransform_.translate_.y = cilnderHeight_ /3.0f;
		hitObject_->worldtransform_.rotate_.x = DegreesToRadians(-90);
		hitObject_->worldtransform_.scale_ += Vector3(0.15f, 0.15f, 0.00f);
		hitObject_->GetPrimitive()->GetMaterial()->color.a -= 0.05f;

		if (time_ >= 0.5f) {
			isEffectPlay_ = false;
		}
	}

	hitEmitter_->Update();





	Vector3 velo = velocity_.Normalize();

	moveSmokeEmitter_->SetVelocityMinMax(-velo, -velo);


	// Y軸周り角度(θy)
	object_->worldtransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));
	// X軸周り角度(θx)
	object_->worldtransform_.rotate_.x = std::atan2(velocity_.y, -length);




	
}

void PlayerRangeBombingBullet::Draw()
{

}

void PlayerRangeBombingBullet::DrawP()
{
	trailEffect_->Update(flag_, worldTransformUp_, worldTransformBottom_);
	trailEffect_->Draw();
}

void PlayerRangeBombingBullet::Draw2D()
{
}

void PlayerRangeBombingBullet::EnemyToColl()
{
	player_->AddHit();
	player_->SetHitTime();
}

void PlayerRangeBombingBullet::PlayerToColl()
{
}

void PlayerRangeBombingBullet::InitStartSmoke(ParticleEmitter* emitter, ParticleManager* particleManager, std::string emitName)
{
	emitter->Initialize(particleManager, emitName, emitName, ParticleData::SpawnType::kPoint);
	emitter->GetFrequency() = 0.00f;
	emitter->SetCount(5);
	emitter->SetLifeTimeMinMax(1.7f, 2.0f);
	emitter->SetIsAlpha(true);
	emitter->SetAlphaClipping(0.1f);
	emitter->SetIsLifeTimeScale(true);
	emitter->SetUsebillboard(true);
	emitter->SetUsebillboardRotZ(true);
	emitter->SetEnableLighting(false);
	emitter->SetColorMinMax({ 1,1,1,0.5f }, { 1,1,1,0.5f });
	emitter->SetRengeMinMax({ -0.25f,-0.25f ,-0.25f }, { 0.25f,0.25f,0.25f });
	emitter->SetSizeMinMax(Vector3{ 55.8f,55.8f,55.8f }, { 55.8f,55.8f,55.8f });
	emitter->SetVelocityMinMax(-Vector3{ 160,-10,160 }, { 160,60,160 });
	emitter->SetRotateMinMax(-DegreesToRadians(Vector3{ 180,180,180 }), DegreesToRadians(Vector3{ 180,180,180 }));
}

void PlayerRangeBombingBullet::InitMoveSmoke(ParticleEmitter* emitter, ParticleManager* particleManager, std::string emitName)
{
	emitter->Initialize(particleManager, emitName, emitName, ParticleData::SpawnType::kSegmentLine);
	emitter->GetFrequency() = 0.00f;
	emitter->SetCount(2);
	emitter->SetLifeTimeMinMax(0.5f, 0.7f);
	emitter->SetIsAlpha(true);
	emitter->SetAlphaClipping(0.23f);
	emitter->SetIsLifeTimeScale(true);
	emitter->SetUsebillboard(true);
	emitter->SetUsebillboardRotZ(true);
	emitter->SetEnableLighting(false);
	emitter->SetIsLineInterpolation(true);
	emitter->SetColorMinMax({ 1,1,1,0.5f }, { 1,1,1,0.5f });
	emitter->SetRengeMinMax({ -0.25f,-0.25f ,-0.25f }, { 0.25f,0.25f,0.25f });
	emitter->SetSizeMinMax(Vector3{ 2.8f,2.8f,2.8f }, { 2.8f,2.8f,2.8f });
	emitter->SetVelocityMinMax(-velocity_, -velocity_);
	emitter->SetRotateMinMax(-DegreesToRadians(Vector3{ 180,180,180 }), DegreesToRadians(Vector3{ 180,180,180 }));
	emitter->SetRengeMinMax(object_->worldtransform_.translate_, object_->worldtransform_.translate_);
}

void PlayerRangeBombingBullet::InitExpSmoke(ParticleEmitter* emitter, ParticleManager* particleManager, std::string emitName)
{
	emitter->Initialize(particleManager, emitName, emitName, ParticleData::SpawnType::kPoint);
	emitter->GetFrequency() = 0.00f;
	emitter->SetCount(10);
	emitter->SetLifeTimeMinMax(2.5f, 2.5f);
	emitter->SetIsAlpha(true);
	emitter->SetAlphaClipping(0.23f);
	emitter->SetIsLifeTimeScale(true);
	emitter->SetUsebillboard(true);
	emitter->SetUsebillboardRotZ(true);
	emitter->SetEnableLighting(false);
	emitter->SetColorMinMax({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	emitter->SetRengeMinMax({ -0.25f,-0.25f ,-0.25f }, { 0.25f,0.25f,0.25f });
	emitter->SetSizeMinMax(Vector3{ 15.8f,15.8f,15.8f }, { 15.8f,15.8f,15.8f });
	emitter->SetVelocityMinMax(-Vector3{30,0,30}, {30,100,30});
	emitter->SetRotateMinMax(-DegreesToRadians(Vector3{ 180,180,180 }), DegreesToRadians(Vector3{ 180,180,180 }));
	
}

void PlayerRangeBombingBullet::InitExpFire(ParticleEmitter* emitter, ParticleManager* particleManager, std::string emitName)
{
	emitter->Initialize(particleManager, emitName, emitName, ParticleData::SpawnType::kPoint);
	emitter->GetFrequency() = 0.00f;
	emitter->SetCount(2);
	emitter->SetLifeTimeMinMax(0.25f, 0.27f);
	emitter->SetIsAlpha(true);
	emitter->SetAlphaClipping(0.23f);
	//emitter->SetIsLifeTimeScale(true);
	emitter->SetUsebillboard(true);
	emitter->SetUsebillboardRotZ(true);
	emitter->SetEnableLighting(false);
//	emitter->SetColorMinMax({ 1.0f,0.1f,0.1f,1.0f }, { 1.0f,0.1f,0.1f,1.0f });
	emitter->SetColorMinMax({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });

	emitter->SetRengeMinMax({ -0.25f,-0.25f ,-0.25f }, { 0.25f,0.25f,0.25f });
	emitter->SetSizeMinMax(Vector3{ 8.8f,8.8f,8.8f }, { 8.8f,8.8f,8.8f });
	emitter->SetVelocityMinMax(-Vector3{ 3,0,3 }, { 3,10,3 });
	emitter->SetRotateMinMax(-DegreesToRadians(Vector3{ 180,180,180 }), DegreesToRadians(Vector3{ 180,180,180 }));

}

void PlayerRangeBombingBullet::InitExpBre(ParticleEmitter* emitter, ParticleManager* particleManager, std::string emitName)
{
	emitter->Initialize(particleManager, emitName, emitName, ParticleData::SpawnType::kPoint);
	emitter->GetFrequency() = 0.00f;
	emitter->SetCount(6);
	emitter->SetLifeTimeMinMax(3.25f, 3.27f);
	//emitter->SetIsAlpha(true);
	emitter->SetAlphaClipping(0.23f);
	emitter->SetIsLifeTimeScale(true);
	emitter->SetLifeTimeScaleTopBottom(ParticleData::TopBottom::kBottom);
	emitter->SetIsGravity(true);
	emitter->SetUsebillboard(false);
	emitter->SetUsebillboardRotZ(true);
	emitter->SetEnableLighting(false);
	emitter->SetColorMinMax({ 1.0f,1.0f,0.0f,1.0f }, { 1.0f,1.0f,0.0f,1.0f });
	emitter->SetIsAcceleration(true);
	emitter->SetIsBounce(true);
	emitter->SetIsRotateVelocity(true);
	emitter->SetRotateVelocityMinMax(-DegreesToRadians(Vector3{ 180,180,180 }), DegreesToRadians(Vector3{ 180,180,180 }));
	emitter->SetRengeMinMax({ -0.25f,-0.25f ,-0.25f }, { 0.25f,0.25f,0.25f });
	emitter->SetSizeMinMax(Vector3{ 0.1f,0.1f,0.1f }, { 0.2f,0.2f,0.2f });
	emitter->SetVelocityMinMax(Vector3{ -40,20,-60 }, { 40,40,40 });
	emitter->SetRotateMinMax(-DegreesToRadians(Vector3{ 180,180,180 }), DegreesToRadians(Vector3{ 180,180,180 }));
}

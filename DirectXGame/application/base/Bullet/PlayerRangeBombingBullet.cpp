#include "PlayerRangeBombingBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Normal/Player.h"

#include "DirectXGame/engine/math/random.h"

void PlayerRangeBombingBullet::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera)
{
	//// オブジェクト設定
	object_ = entity3DManager->CreateObject3D("playerbullet", Object3d::ObjectModelType::kNormal, position, camera);
	object_->SetModel("player_bullet.obj");
	object_->UseTrailEffect("resources/Texture/Image.png", 0.15f, { 1.0f,1.0f,1.0f,1.0f }, { 0,1.5f,0 }, { 0,-1.5f,0 });
	object_->Update();
	object_->InitColliderComponent();
	
	auto sphere = std::make_unique<SphereCollider>();
	sphere->tag = CollisionTag::PlayerAttack;
	sphere->layer = CollisionLayer::PlayerAttack;
	sphere->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));
	sphere->radius = 10.0f; // 半径を適宜設定
	sphere->Disable(); // 初期状態では無効化
	object_->GetColliderComponent()->AddCollider(std::move(sphere));



	// 衝突時のコールバック登録
	object_->GetColliderComponent()->onHitCallback = [this](Collider* self, Collider* other) {
		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent || other->tag != CollisionTag::Enemy) return;
		BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());

		uint32_t otherId = otherComponent->GetUniqueId();




		float nowTime = MyGame::NowTime(); // ← 時間取得関数（例）

		if (object_->GetColliderComponent()->contactRecord_.CheckHistory(otherId)) {
			return; // クールタイム中のため無視
		}

		object_->GetColliderComponent()->contactRecord_.AddHistory(otherId, nowTime);

		enemy->AddDamage(parameter_.damege);
		enemy->SetHit();
		enemy->Emit();
		enemy->hitStop(0.1f);
		};

	
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

	
	// 初期地点記録
	str = object_->worldtransform_.translate_;

	// ランダムな空中位置を設定
	randPosSky = str + Vector3{ -1000,0,0};

	max_y = 600.0f; // 最大Y座標

	randPosSky.y = max_y;


	//posGround = targetRange_.position;

	//// 各演出設定
	ParticleManager* particleManager = entity3DManager->GetEffectManager()->GetParticleManager();

	//// ミサイル移動中の煙パーティクルエミッター
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

	// ミサイル爆発時のリングパーティクルエミッター
	ringEmitter_ = std::make_unique <ParticleEmitter>();
	InitRingEmitter(ringEmitter_.get(), particleManager, "ringEmit");


	Vector3 size = { 2.0f, 2.0f, 2.0f };
	hitEmitter_ = std::make_unique <ParticleEmitter>();
	hitEmitter_->Initialize(particleManager, "missileHit", "missileHit", ParticleData::SpawnType::kPoint);
	hitEmitter_->SetCount(2);
	hitEmitter_->SetLifeTimeMinMax(0.25f, 0.25f);
	hitEmitter_->SetColorMinMax({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	hitEmitter_->SetSizeMinMax(size, size);
	hitEmitter_->SetFrequency(0.00f);
	hitEmitter_->SetParent(object_->worldtransform_);
	hitEmitter_->SetPos({0,0,0});
	hitEmitter_->SetUsebillboardRotZ(true);
	hitEmitter_->SetIsAlpha(true);
	hitEmitter_->SetVelocityMinMax({}, {});
	hitEmitter_->SetRotateMinMax({ 0,0,-3.14f }, { 0,0,3.14f });
	hitEmitter_->SetIsEmit(false);


	ShapeParameter::Cylinder cylinderParam;
	cylinderParam.height = cilnderHeight_;
	cylinderParam.innerRadius = 6.0f;
	cylinderParam.outerRadius = 12.0f;
	cylinderParam.isCover = false;
	cylinderParam.segments = 16;
	
	hitObject_ = entity3DManager->CreatePrimitiveObject3D("cylinder", cylinderParam, "resources/Texture/effect/gradationLine.png", Primitive::ShapeType::Cylinder, camera);
	hitObject_->GetPrimitive()->SetPsoType(Primitive::PsoType::kNoCullRingClamp);
	hitObject_->SetObjectRasterizerType(Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_NONE);
	hitObject_->worldtransform_.translate_.z = 50.0f;
	hitObject_->worldtransform_.rotate_.y = DegreesToRadians(-90);
	hitObject_->SetIsDraw(false);
	
	cylinderParam.height = 10.0f;
	cylinderParam.innerRadius = 12.0f;
	cylinderParam.outerRadius = 12.0f;
	cylinderParam.isCover = true;

	
	hitObject2_ = entity3DManager->CreatePrimitiveObject3D("cylinder", cylinderParam, "resources/Texture/Image.png", Primitive::ShapeType::Cylinder, camera);
	hitObject2_->GetPrimitive()->SetPsoType(Primitive::PsoType::kRingClamp);
	hitObject2_->SetIsDraw(false);
	hitObject2_->GetPrimitive()->GetMaterial()->color = {1.0f,0.0f,0.0f,0.5f};
	
	enemyPos_.x = targetRange_.position.x + Random::RandomFloat(-targetRange_.radius,targetRange_.radius);
	enemyPos_.z = targetRange_.position.z + Random::RandomFloat(-targetRange_.radius,targetRange_.radius);
	enemyPos_.y = 2.0f;
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

					hitObject2_->SetIsDraw(true);
				}

			}
			break;
		case 2:
			flag_ = true;
			
			moveSmokeEmitter_->SetCount(5);
			moveSmokeEmitter2_->SetCount(5);

			// リングエフェクトの位置を設定
			ringEmitter_->SetPos(object_->worldtransform_.worldMat_.GetWorldPosition() + Vector3{ 0,0,0 });
			ringEmitter_->Update();

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
				//hitObject2_->SetIsDraw(false);
				moveExpEmitter_->SetPos(object_->worldtransform_.worldMat_.GetWorldPosition());
				moveExpEmitter_->Update();
				expFireEmitter_->SetPos(object_->worldtransform_.worldMat_.GetWorldPosition() +Vector3{0,5,0});
				expFireEmitter_->Update();
				animatedCubeEmitter_->SetPos(object_->worldtransform_.worldMat_.GetWorldPosition() + Vector3{ 0,0,0 });
				animatedCubeEmitter_->Update();
			}

			break;
		}

		Vector3 velo = velocity_.Normalize();

		moveSmokeEmitter_->SetVelocityMinMax(-velo, -velo);


		// Y軸周り角度(θy)
		object_->worldtransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
		float length = Length(Vector3(velocity_.x, 0, velocity_.z));
		// X軸周り角度(θx)
		object_->worldtransform_.rotate_.x = std::atan2(velocity_.y, -length);


		hitObject2_->worldtransform_.translate_ = posGround + Vector3{ 0,-6.0f,0 };


		//object_->Update();
		 
		hitObject_->worldtransform_.translate_ = object_->worldtransform_.translate_;
		hitObject_->worldtransform_.rotate_.x = DegreesToRadians(-90);

		hitObject2_->worldtransform_.rotate_.x = DegreesToRadians(-90);

		//colliderComponent_->UpdateAll(object_->worldtransform_);
	}


	

	if (isEffectPlay_) {
		time_ += GetTimer();
		object_->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, true);
		object_->SetIsDraw(false);
		hitObject_->SetIsDraw(true);
		hitObject_->worldtransform_.translate_ = object_->worldtransform_.translate_ + Vector3{ 0.0f,5.0f ,0.0f };
		hitObject_->worldtransform_.translate_.y = cilnderHeight_ /3.0f;
		hitObject_->worldtransform_.rotate_.x = DegreesToRadians(-90);
		hitObject_->worldtransform_.scale_ += Vector3(0.15f, 0.15f, 0.00f);
		hitObject_->GetPrimitive()->GetMaterial()->color.a -= 0.05f;


		if (time_ >= 0.5f) {
			object_->IsDelete();
			hitObject_->IsDelete();
			hitObject2_->IsDelete();
			isEffectPlay_ = false;
			//object_->GetColliderComponent()->ClearColliders();

		
		}



	}
	object_->SetIsEmitTrailEffect(true);
	hitEmitter_->Update();

}

void PlayerRangeBombingBullet::Draw()
{

}

void PlayerRangeBombingBullet::DrawP()
{
	object_->DrawTrailEffect();
}

void PlayerRangeBombingBullet::Draw2D()
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
	emitter->SetColorMinMax({ 1.0f,0.5f,0.0f,1.0f }, { 1.0f,1.0f,0.0f,1.0f });
	emitter->SetIsAcceleration(true);
	emitter->SetIsBounce(true);
	emitter->SetIsRotateVelocity(true);
	emitter->SetRotateVelocityMinMax(-DegreesToRadians(Vector3{ 180,180,180 }), DegreesToRadians(Vector3{ 180,180,180 }));
	emitter->SetRengeMinMax({ -0.25f,-0.25f ,-0.25f }, { 0.25f,0.25f,0.25f });
	emitter->SetSizeMinMax(Vector3{ 0.5f,0.5f,0.5f }, { 0.5f,0.5f,0.5f });
	emitter->SetVelocityMinMax(Vector3{ -40,20,-60 }, { 40,40,40 });
	emitter->SetRotateMinMax(-DegreesToRadians(Vector3{ 180,180,180 }), DegreesToRadians(Vector3{ 180,180,180 }));
}

void PlayerRangeBombingBullet::InitRingEmitter(ParticleEmitter* emitter, ParticleManager* particleManager, std::string emitName)
{
	emitter->Initialize(particleManager, emitName, emitName, ParticleData::SpawnType::kAABB);
	emitter->GetFrequency() = 0.00f;
	emitter->SetCount(3);
	emitter->SetLifeTimeMinMax(0.10f, 0.20f);
	emitter->SetIsAlpha(true);
	emitter->SetAlphaClipping(0.23f);
	emitter->SetIsLifeTimeScale(true);
	emitter->SetUsebillboard(false);
	emitter->SetUsebillboardRotZ(false);
	emitter->SetEnableLighting(false);
	emitter->SetRengeMinMax({ 0.0f,-4.0f ,0.0f }, { 0.0f,20.0f,0.0f });
	emitter->SetColorMinMax({ 1.0f,1.0f,0.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	emitter->SetSizeMinMax(Vector3{ 5.8f,5.8f,5.8f }, { 5.8f,5.8f,5.8f });
	emitter->SetVelocityMinMax(-Vector3{ 0,0,0 }, { 0,0,0 });
	emitter->SetRotateMinMax(DegreesToRadians(Vector3{ 90,0,0 }), DegreesToRadians(Vector3{ 90,0,0 }));
}

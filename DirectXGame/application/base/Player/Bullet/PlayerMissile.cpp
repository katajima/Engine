#include "PlayerMissile.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/application/base/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Player/Player.h"

void PlayerMissile::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera)
{
	Collider::Initialize(camera);
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon));
	Collider::SetRadius(3.0f);


	// プレイヤー

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

	str = object_->worldtransform_.translate_;

	phase_ = 0;

	isAlive_ = true;

	randPosSky = str + float(rand() % 11 - 5);
	randPosSky.y = max_y;





	parameter_.damege = 30;





	ｍSmokeEmitter_ = std::make_unique <ParticleEmitter>();
	ｍSmokeEmitter_->Initialize(entity3DManager->GetEffectManager()->GetParticleManager(), "smokePlane01", "smokePlane01", ParticleData::SpawnType::kSegmentLine);
	ｍSmokeEmitter_->GetFrequency() = 0.00f;
	ｍSmokeEmitter_->SetCount(5);
	ｍSmokeEmitter_->SetLifeTimeMinMax(0.7f, 1.0f);
	ｍSmokeEmitter_->SetIsAlpha(true);
	ｍSmokeEmitter_->SetAlphaClipping(0.23f);
	ｍSmokeEmitter_->SetIsLifeTimeScale(true);
	ｍSmokeEmitter_->SetUsebillboard(true);
	ｍSmokeEmitter_->SetUsebillboardRotZ(true);
	ｍSmokeEmitter_->SetEnableLighting(false);

	ｍSmokeEmitter_->SetColorMinMax({ 1,1,1,0.5f }, { 1,1,1,0.5f });
	ｍSmokeEmitter_->SetRengeMinMax({ -0.25f,-0.25f ,-0.25f }, { 0.25f,0.25f,0.25f });
	ｍSmokeEmitter_->SetSizeMinMax(Vector3{ 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f });
	ｍSmokeEmitter_->SetVelocityMinMax(-velocity_, -velocity_);
	ｍSmokeEmitter_->SetRotateMinMax(-DegreesToRadians(Vector3{ 180,180,180 }), DegreesToRadians(Vector3{ 180,180,180 }));

	ｍSmokeEmitter2_ = std::make_unique <ParticleEmitter>();
	ｍSmokeEmitter2_->Initialize(entity3DManager->GetEffectManager()->GetParticleManager(), "smokePlane04", "smokePlane04", ParticleData::SpawnType::kSegmentLine);
	ｍSmokeEmitter2_->GetFrequency() = 0.00f;
	ｍSmokeEmitter2_->SetCount(5);
	ｍSmokeEmitter2_->SetLifeTimeMinMax(0.7f, 1.0f);
	ｍSmokeEmitter2_->SetIsAlpha(true);
	ｍSmokeEmitter2_->SetAlphaClipping(0.23f);
	ｍSmokeEmitter2_->SetIsLifeTimeScale(true);
	ｍSmokeEmitter2_->SetUsebillboard(true);
	ｍSmokeEmitter2_->SetUsebillboardRotZ(true);

	
	ｍSmokeEmitter2_->SetColorMinMax({ 1,0,0,1.5f }, { 1,0,0,1.5f });
	ｍSmokeEmitter2_->SetRengeMinMax({ -0.25f,-0.25f ,-0.25f }, { 0.25f,0.25f,0.25f });
	ｍSmokeEmitter2_->SetSizeMinMax(Vector3{ 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f });
	ｍSmokeEmitter2_->SetVelocityMinMax(-velocity_, -velocity_);
	ｍSmokeEmitter2_->SetRotateMinMax(-DegreesToRadians(Vector3{ 180,180,180 }), DegreesToRadians(Vector3{ 180,180,180 }));
	ｍSmokeEmitter_->SetRengeMinMax(object_->worldtransform_.translate_, object_->worldtransform_.translate_);




	Vector3 size = { 10,10,10 };


	mExplosionSmokeEmitter_ = std::make_unique <ParticleEmitter>();
	mExplosionSmokeEmitter_->Initialize(entity3DManager->GetEffectManager()->GetParticleManager(), "smokePlane01_1", "smokePlane01_1", ParticleData::SpawnType::kAABB);
	mExplosionSmokeEmitter_->SetParent(object_->worldtransform_);
	mExplosionSmokeEmitter_->GetFrequency() = 0.00f;
	mExplosionSmokeEmitter_->SetCount(50);
	mExplosionSmokeEmitter_->SetLifeTimeMinMax(2.0f, 2.5f);
	mExplosionSmokeEmitter_->SetIsAlpha(true);
	mExplosionSmokeEmitter_->SetIsEmit(false);
	mExplosionSmokeEmitter_->SetColorMinMax({ 0.7f,0.7f,0.7f,0.9f }, { 0.7f,0.7f,0.7f,0.9f });
	mExplosionSmokeEmitter_->SetRengeMinMax({ -1.25f,-1.25f ,-1.25f }, { 1.25f,1.25f,1.25f });
	mExplosionSmokeEmitter_->SetSizeMinMax(size, size);
	mExplosionSmokeEmitter_->SetVelocityMinMax(-Vector3{ 3,0.1f,3 }, Vector3{ 3,7,3 });
	mExplosionSmokeEmitter_->SetEnableLighting(false);
	mExplosionSmokeEmitter_->SetAlphaClipping(0.15f);
	mExplosionSmokeEmitter_->SetIsLifeTimeScale(true);
	mExplosionSmokeEmitter_->SetLifeTimeScaleTopBottom(ParticleData::TopBottom::kTop);


	mExplosionSmokeEmitter2_ = std::make_unique <ParticleEmitter>();
	mExplosionSmokeEmitter2_->Initialize(entity3DManager->GetEffectManager()->GetParticleManager(), "smokePlane02_1", "smokePlane02_1", ParticleData::SpawnType::kAABB);
	mExplosionSmokeEmitter2_->SetParent(object_->worldtransform_);
	mExplosionSmokeEmitter2_->GetFrequency() = 0.00f;
	mExplosionSmokeEmitter2_->SetCount(50);
	mExplosionSmokeEmitter2_->SetLifeTimeMinMax(2.0f, 2.0f);
	mExplosionSmokeEmitter2_->SetIsAlpha(true);
	mExplosionSmokeEmitter2_->SetIsEmit(false);
	mExplosionSmokeEmitter2_->SetColorMinMax({ 0.7f,0.7f,0.7f,0.9f }, { 0.7f,0.7f,0.7f,0.9f });
	mExplosionSmokeEmitter2_->SetRengeMinMax({ -1.25f,-1.25f ,-1.25f }, { 1.25f,1.25f,1.25f });
	mExplosionSmokeEmitter2_->SetSizeMinMax(size, size);
	mExplosionSmokeEmitter2_->SetVelocityMinMax(-Vector3{ 1,0.1f,1 }, Vector3{ 1,5,1 });
	mExplosionSmokeEmitter2_->SetEnableLighting(false);
	mExplosionSmokeEmitter2_->SetAlphaClipping(0.15f);
	mExplosionSmokeEmitter2_->SetIsLifeTimeScale(true);
	mExplosionSmokeEmitter2_->SetLifeTimeScaleTopBottom(ParticleData::TopBottom::kTop);


	mExplosionSmokeEmitter3_ = std::make_unique <ParticleEmitter>();
	mExplosionSmokeEmitter3_->Initialize(entity3DManager->GetEffectManager()->GetParticleManager(), "smokePlane03_1", "smokePlane03_1", ParticleData::SpawnType::kAABB);
	mExplosionSmokeEmitter3_->SetParent(object_->worldtransform_);
	mExplosionSmokeEmitter3_->GetFrequency() = 0.00f;
	mExplosionSmokeEmitter3_->SetCount(50);
	mExplosionSmokeEmitter3_->SetLifeTimeMinMax(2.0f, 2.0f);
	mExplosionSmokeEmitter3_->SetIsAlpha(true);
	mExplosionSmokeEmitter3_->SetIsEmit(false);
	mExplosionSmokeEmitter3_->SetColorMinMax({ 0.7f,0.7f,0.7f,0.9f }, { 0.7f,0.7f,0.7f,0.9f });
	mExplosionSmokeEmitter3_->SetRengeMinMax({ -1.25f,-1.25f ,-1.25f }, { 1.25f,1.25f,1.25f });
	mExplosionSmokeEmitter3_->SetSizeMinMax(size, size);
	mExplosionSmokeEmitter3_->SetVelocityMinMax(-Vector3{ 2,0.1f,2 }, Vector3{ 2,7,2 });
	mExplosionSmokeEmitter3_->SetEnableLighting(false);
	mExplosionSmokeEmitter3_->SetAlphaClipping(0.15f);
	mExplosionSmokeEmitter3_->SetIsLifeTimeScale(true);
	mExplosionSmokeEmitter3_->SetLifeTimeScaleTopBottom(ParticleData::TopBottom::kTop);

}

void PlayerMissile::Update()
{
	// 時間経過でデス
	if (Hit) {
		mExplosionSmokeEmitter_->SetIsEmit(true);
		isAlive_ = false;
	}

	Vector3 strSmoke = object_->worldtransform_.worldMat_.GetWorldPosition();
	Vector3 endSmoke = object_->worldtransform_.worldPreMat_.GetWorldPosition();
	ｍSmokeEmitter_->SetRengeMinMax(strSmoke, endSmoke);



	if (count > 0 || phase_ > 0) {
		ｍSmokeEmitter_->Update();
		ｍSmokeEmitter2_->Update();
	}

	if (enemy_->nullChek == Matrix4x4::Identity()) {
		enemyPos_ = enemy_->GetObject3D()->GetWorldPosition();
	}

	if (isAlive_) {
		Vector3 norm;

		switch (phase_)
		{
		case 0:
			count += GetTimer();

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
			count += GetTimer();
			if (count >= max_count)
			{
				Vector3 pos = enemyPos_ - object_->GetWorldPosition();

				Vector3 pos2 = pos;

				velocity_ = pos2;
				velocity_.y = 0;

				object_->worldtransform_.translate_ += velocity_ * GetTimer();

				if (5 >= DistanceXZ(object_->GetWorldPosition(), enemyPos_)) {
					phase_++;
					posGround = enemyPos_;
					tragetPos = posGround - object_->GetWorldPosition();
					count = 0;
				}

			}
			break;
		case 2:
			count += GetTimer();
			if (count >= max_count)
			{
				velocity_ = tragetPos * 3;

				object_->worldtransform_.translate_ += velocity_ * GetTimer();
			}

			if (2.5f >= object_->GetWorldPosition().y) {
				mExplosionSmokeEmitter_->SetIsEmit(true);
				isAlive_ = false;
				count = 0;
				phase_ = 0;
			}

			break;
		}
	}






	Vector3 velo = velocity_.Normalize();

	ｍSmokeEmitter_->SetVelocityMinMax(-velo, -velo);


	// Y軸周り角度(θy)
	object_->worldtransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));

	// X軸周り角度(θx)
	object_->worldtransform_.rotate_.x = std::atan2(velocity_.y, -length);


	mExplosionSmokeEmitter_->Update();
}

void PlayerMissile::Draw()
{
	object_->Draw();
}

void PlayerMissile::DrawP()
{
}

void PlayerMissile::Draw2D()
{
}

void PlayerMissile::EnemyToColl()
{
	player_->AddHit();
	player_->SetHitTime();
}

void PlayerMissile::PlayerToColl()
{
}

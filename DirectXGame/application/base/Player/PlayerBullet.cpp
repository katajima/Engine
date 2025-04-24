#include "PlayerBullet.h"
#include "DirectXGame/application/base/Enemy/Enemy.h"
#include "Player.h"

void PlayerBullet::Initialize(Entity3DManager* entity3DManager,Vector3 position, Camera* camera)
{
	Collider::Initialize(camera);
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon));
	Collider::SetRadius(3.0f);
	//モデル

	// プレイヤー
	object_.Initialize(entity3DManager);
	object_.SetCamera(camera);
	object_.SetModel("player_bullet.obj");
	//object_.SetModel("AnimatedCube.gltf");
	object_.worldtransform_.translate_= position;
	object_.Update();

	
	

	// Y軸周り角度(θy)
	object_.worldtransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));

	// X軸周り角度(θx)
	object_.worldtransform_.rotate_.x = std::atan2(velocity_.y, -length);

	str = object_.worldtransform_.translate_;

	phase_ = 0;

	isAlive_ = true;

	randPosSky = str + float(rand() % 11 - 5);
	randPosSky.y = max_y;

	
	







	
	ｍSmokeEmitter_ = std::make_unique <ParticleEmitter>();
	ｍSmokeEmitter_->Initialize(entity3DManager->GetEffectManager()->GetParticleManager(), "smokePlane01", "smokePlane01", ParticleEmitter::EmitSpawnShapeType::kSegmentLine);
	ｍSmokeEmitter_->GetFrequency() = 0.01f;
	ｍSmokeEmitter_->SetCount(5);
	ｍSmokeEmitter_->SetLifeTimeMinMax(0.7f, 1.0f);
	ｍSmokeEmitter_->SetIsAlpha(true);
	ｍSmokeEmitter_->SetAlphaClipping(0.23f);
	ｍSmokeEmitter_->SetIsLifeTimeScale(true);
	ｍSmokeEmitter_->SetUsebillboard(false);
	ｍSmokeEmitter_->SetEnableLighting(false);

	ｍSmokeEmitter_->SetColorMinMax({1,1,1,0.5f}, { 1,1,1,0.5f });
	ｍSmokeEmitter_->SetRengeMinMax({-0.25f,-0.25f ,-0.25f },{ 0.25f,0.25f,0.25f});
	ｍSmokeEmitter_->SetSizeMinMax(Vector3{ 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f });
	ｍSmokeEmitter_->SetVelocityMinMax(-velocity_, -velocity_);
	ｍSmokeEmitter_->SetRotateMinMax(-DegreesToRadians(Vector3{ 180,180,180 }), DegreesToRadians(Vector3{ 180,180,180 }));

	object_.Update();
	ｍSmokeEmitter_->SetRengeMinMax(object_.worldtransform_.translate_, object_.worldtransform_.translate_);




	Vector3 size = { 10,10,10 };


	mExplosionSmokeEmitter_ = std::make_unique <ParticleEmitter>();
	mExplosionSmokeEmitter_->Initialize(entity3DManager->GetEffectManager()->GetParticleManager(), "smokePlane01_1", "smokePlane01_1", ParticleEmitter::EmitSpawnShapeType::kAABB);
	mExplosionSmokeEmitter_->SetParent(object_.worldtransform_);
	mExplosionSmokeEmitter_->GetFrequency() = 0.00f;
	mExplosionSmokeEmitter_->SetCount(50);
	mExplosionSmokeEmitter_->SetLifeTimeMinMax(2.0f, 2.0f);
	mExplosionSmokeEmitter_->SetIsAlpha(true);
	mExplosionSmokeEmitter_->SetIsEmit(false);
	mExplosionSmokeEmitter_->SetColorMinMax({ 0.7f,0.7f,0.7f,0.9f }, { 0.7f,0.7f,0.7f,0.9f });
	mExplosionSmokeEmitter_->SetRengeMinMax({ -1.25f,-1.25f ,-1.25f }, { 1.25f,1.25f,1.25f });
	mExplosionSmokeEmitter_->SetSizeMinMax(size, size);
	mExplosionSmokeEmitter_->SetVelocityMinMax(-Vector3{5,5,5}, Vector3{ 5,5,5 });
	mExplosionSmokeEmitter_->SetEnableLighting(false);
	mExplosionSmokeEmitter_->SetAlphaClipping(0.15f);
	mExplosionSmokeEmitter_->SetIsLifeTimeScale(true);
	mExplosionSmokeEmitter_->SetLifeTimeScaleTopBottom(ParticleManager::TopBottom::kTop);


	mExplosionSmokeEmitter2_ = std::make_unique <ParticleEmitter>();
	mExplosionSmokeEmitter2_->Initialize(entity3DManager->GetEffectManager()->GetParticleManager(), "smokePlane02_1", "smokePlane02_1", ParticleEmitter::EmitSpawnShapeType::kAABB);
	mExplosionSmokeEmitter2_->SetParent(object_.worldtransform_);
	mExplosionSmokeEmitter2_->GetFrequency() = 0.00f;
	mExplosionSmokeEmitter2_->SetCount(50);
	mExplosionSmokeEmitter2_->SetLifeTimeMinMax(2.0f, 2.0f);
	mExplosionSmokeEmitter2_->SetIsAlpha(true);
	mExplosionSmokeEmitter2_->SetIsEmit(false);
	mExplosionSmokeEmitter2_->SetColorMinMax({ 0.7f,0.7f,0.7f,0.9f }, { 0.7f,0.7f,0.7f,0.9f });
	mExplosionSmokeEmitter2_->SetRengeMinMax({ -1.25f,-1.25f ,-1.25f }, { 1.25f,1.25f,1.25f });
	mExplosionSmokeEmitter2_->SetSizeMinMax(size, size);
	mExplosionSmokeEmitter2_->SetVelocityMinMax(-Vector3{5,5,5}, Vector3{ 5,5,5 });
	mExplosionSmokeEmitter2_->SetEnableLighting(false);
	mExplosionSmokeEmitter2_->SetAlphaClipping(0.15f);
	mExplosionSmokeEmitter2_->SetIsLifeTimeScale(true);
	mExplosionSmokeEmitter2_->SetLifeTimeScaleTopBottom(ParticleManager::TopBottom::kTop);


	mExplosionSmokeEmitter3_ = std::make_unique <ParticleEmitter>();
	mExplosionSmokeEmitter3_->Initialize(entity3DManager->GetEffectManager()->GetParticleManager(), "smokePlane03_1", "smokePlane03_1", ParticleEmitter::EmitSpawnShapeType::kAABB);
	mExplosionSmokeEmitter3_->SetParent(object_.worldtransform_);
	mExplosionSmokeEmitter3_->GetFrequency() = 0.00f;
	mExplosionSmokeEmitter3_->SetCount(50);
	mExplosionSmokeEmitter3_->SetLifeTimeMinMax(2.0f, 2.0f);
	mExplosionSmokeEmitter3_->SetIsAlpha(true);
	mExplosionSmokeEmitter3_->SetIsEmit(false);
	mExplosionSmokeEmitter3_->SetColorMinMax({ 0.7f,0.7f,0.7f,0.9f }, { 0.7f,0.7f,0.7f,0.9f });
	mExplosionSmokeEmitter3_->SetRengeMinMax({ -1.25f,-1.25f ,-1.25f }, { 1.25f,1.25f,1.25f });
	mExplosionSmokeEmitter3_->SetSizeMinMax(size, size);
	mExplosionSmokeEmitter3_->SetVelocityMinMax(-Vector3{5,0,5}, Vector3{ 5,5,5 });
	mExplosionSmokeEmitter3_->SetEnableLighting(false);
	mExplosionSmokeEmitter3_->SetAlphaClipping(0.15f);
	mExplosionSmokeEmitter3_->SetIsLifeTimeScale(true);
	mExplosionSmokeEmitter3_->SetLifeTimeScaleTopBottom(ParticleManager::TopBottom::kTop);

}

void PlayerBullet::Update()
{
	// 時間経過でデス
	if (Hit) {
		mExplosionSmokeEmitter_->SetIsEmit(true);
		isAlive_ = false;
	}

	Vector3 strSmoke = object_.worldtransform_.worldMat_.GetWorldPosition();
	Vector3 endSmoke = object_.worldtransform_.worldPreMat_.GetWorldPosition();
	if (count > 0 || phase_ > 0) {
		ｍSmokeEmitter_->Update();
	}
	
	if (isAlive_) {
		countTrail++;
		Vector3 norm;

		switch (phase_)
		{
		case 0:
			count += MyGame::GameTime();

			t = count;

			object_.worldtransform_.translate_ = Lerp(str, randPosSky, t);

			norm =  randPosSky - str;
			
			velocity_ = norm * MyGame::GameTime();

			if (t >= 1) {
				t = 0;
				count = 0;
				phase_++;
			}

			break;
		case 1:
			count += MyGame::GameTime();
			if (count >= max_count)
			{
				Vector3 pos = enemy_->GetObject3D().GetWorldPosition() - object_.GetWorldPosition();

				Vector3 pos2 = pos;

				velocity_ = pos2;
				velocity_.y = 0;

				object_.worldtransform_.translate_ += velocity_ * MyGame::GameTime();

				if (5 >= DistanceXZ(object_.GetWorldPosition(), enemy_->GetObject3D().GetWorldPosition())) {
					phase_++;
					posGround = enemy_->GetObject3D().GetWorldPosition();
					tragetPos = posGround - object_.GetWorldPosition();
					count = 0;
				}

			}
			break;
		case 2:
			count += MyGame::GameTime();
			if (count >= max_count)
			{
				velocity_ = tragetPos * 3;
				
				object_.worldtransform_.translate_ += velocity_ * MyGame::GameTime();
			}

			if (2.5f >= object_.GetWorldPosition().y) {
				mExplosionSmokeEmitter_->SetIsEmit(true);
				isAlive_ = false;
				count = 0;
				phase_ = 0;
			}

			break;
		}
	}

	
	if (countTrail >= 5) {
		bool flag_ =true ;
	}

	

	Vector3 velo = velocity_.Normalize();

	ｍSmokeEmitter_->SetRengeMinMax(strSmoke, endSmoke);
	ｍSmokeEmitter_->SetVelocityMinMax(-velo, -velo);

	
	// Y軸周り角度(θy)
	object_.worldtransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));

	// X軸周り角度(θx)
	object_.worldtransform_.rotate_.x = std::atan2(velocity_.y, -length);


	mExplosionSmokeEmitter_->Update();
	

	object_.Update();
}

void PlayerBullet::Draw()
{
	object_.Draw();
}

void PlayerBullet::DrawP()
{

}

void PlayerBullet::OnCollision(Collider* other)
{
	// 衝突判定の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	// 衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {
		Enemy* enemy = static_cast<Enemy*>(other);
		uint32_t serialNumber = enemy->GetSerialNumber();



		// 接触履歴があれば何もせず抜ける
		if (contactRecord_.CheckHistory(serialNumber)) {
			return;
		}

		
		contactRecord_.AddHistory(serialNumber);

		if (enemy->GetAlive()) {

			enemy->AddDamege(30);

			enemy->SetHit();

			enemy->Emit();

			player_->AddHit();
			player_->SetHitTime();
		}
	}
	
}

Vector3 PlayerBullet::GetCenterPosition() const
{
	return  object_.GetWorldPosition();
}

void PlayerBullet::SetPlayer(Player* player)
{
	player_ = player;
}

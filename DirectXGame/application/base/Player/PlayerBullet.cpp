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

	
	objectStr_.Initialize(entity3DManager);
	objectStr_.worldtransform_.parent_ = &object_.worldtransform_;
	objectStr_.worldtransform_.translate_.y = object_.GetMesh(0)->GetMin().y * 1;

	objectEnd_.Initialize(entity3DManager);
	objectEnd_.worldtransform_.parent_ = &object_.worldtransform_;
	objectEnd_.worldtransform_.translate_.y = object_.GetMesh(0)->GetMax().y * 1;










	
	ｍSmokeEmitter_ = std::make_unique <ParticleEmitter>();
	ｍSmokeEmitter_->Initialize(entity3DManager->GetEffectManager()->GetParticleManager(),"emitterSmoke", "smoke", ParticleEmitter::EmitSpawnShapeType::kSegmentLine);
	ｍSmokeEmitter_->GetFrequency() = 0.01f;
	ｍSmokeEmitter_->SetCount(5);
	ｍSmokeEmitter_->SetLifeTimeMinMax(0.5f, 1.0f);
	ｍSmokeEmitter_->SetIsAlpha(true);
	ｍSmokeEmitter_->SetColorMinMax({1,1,1,0.5f}, { 1,1,1,0.5f });
	ｍSmokeEmitter_->SetRengeMinMax({-0.25f,-0.25f ,-0.25f },{ 0.25f,0.25f,0.25f});
	ｍSmokeEmitter_->SetSizeMinMax(Vector3{ 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f });
	ｍSmokeEmitter_->SetVelocityMinMax(-velocity_, -velocity_);
	object_.Update();
	ｍSmokeEmitter_->SetRengeMinMax(object_.worldtransform_.translate_, object_.worldtransform_.translate_);







	mExplosionSmokeEmitter_ = std::make_unique <ParticleEmitter>();
	mExplosionSmokeEmitter_->Initialize(entity3DManager->GetEffectManager()->GetParticleManager(),"emitterSmoke", "explosionSmoke", ParticleEmitter::EmitSpawnShapeType::kAABB);
	mExplosionSmokeEmitter_->SetParent(object_.worldtransform_);
	mExplosionSmokeEmitter_->GetFrequency() = 0.00f;
	mExplosionSmokeEmitter_->SetCount(50);
	mExplosionSmokeEmitter_->SetLifeTimeMinMax(2.0f, 2.0f);
	mExplosionSmokeEmitter_->SetIsAlpha(true);
	mExplosionSmokeEmitter_->SetIsEmit(false);
	mExplosionSmokeEmitter_->SetColorMinMax({ 0.7f,0.7f,0.7f,0.9f }, { 0.7f,0.7f,0.7f,0.9f });
	mExplosionSmokeEmitter_->SetRengeMinMax({ -1.25f,-1.25f ,-1.25f }, { 1.25f,1.25f,1.25f });
	mExplosionSmokeEmitter_->SetSizeMinMax(Vector3{ 5.0f,5.0f,5.0f }, { 5.0f,5.0f,5.0f });
	mExplosionSmokeEmitter_->SetVelocityMinMax(-Vector3{5,5,5}, Vector3{ 5,5,5 });


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
		//trailEffect_->Update(flag_, objectStr_, objectEnd_);
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
	
	objectStr_.Update();
	objectEnd_.Update();
	object_.Update();
}

void PlayerBullet::Draw()
{
	object_.Draw();
}

void PlayerBullet::DrawP()
{
	//trailEffect_->Draw();
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

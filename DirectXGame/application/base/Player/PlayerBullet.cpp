#include "PlayerBullet.h"
#include "DirectXGame/application/base/Enemy/Enemy.h"
#include "Player.h"

void PlayerBullet::Initialize(Vector3 position, Camera* camera)
{
	Collider::Initialize(camera);
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon));
	Collider::SetRadius(3.0f);
	//モデル

	// プレイヤー
	object_.Initialize();
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

	
	


	trailEffect_ = std::make_unique<TrailEffect>();
	trailEffect_->Initialize("resources/Texture/Image.png", 0.1f,{1.00f,1.00f,1.00f,0.9f});
	trailEffect_->SetCamera(camera);
	


	objectStr_.Initialize();
	objectStr_.worldtransform_.parent_ = &object_.worldtransform_;
	objectStr_.worldtransform_.translate_.y = object_.GetMesh(0)->GetMin().y * 1;

	objectEnd_.Initialize();
	objectEnd_.worldtransform_.parent_ = &object_.worldtransform_;
	objectEnd_.worldtransform_.translate_.y = object_.GetMesh(0)->GetMax().y * 1;

}

void PlayerBullet::Update()
{
	// 時間経過でデス
	if (Hit) {
		isAlive_ = false;
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

				//pos =  enemy_->GetObject3D().GetWorldPosition() - object_.GetWorldPosition();
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
				isAlive_ = false;
				count = 0;
				phase_ = 0;
			}

			break;
		}
	}

	
	if (countTrail >= 5) {
		bool flag_ =true ;
		trailEffect_->Update(flag_, objectStr_, objectEnd_);
	}


	

	
	// Y軸周り角度(θy)
	object_.worldtransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));

	// X軸周り角度(θx)
	object_.worldtransform_.rotate_.x = std::atan2(velocity_.y, -length);


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
	trailEffect_->Draw();
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

#include "BulletSpecial.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include"DirectXGame/application/base/BaseClass/Bullet/BulletManager.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"


void BulletSpecial::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera)
{
	injectionLeftObj_ = std::make_unique<Object3d>();
	injectionLeftObj_->Initialize(entity3DManager);
	injectionLeftObj_->SetCamera(camera);
	injectionLeftObj_->SetModel("AnimatedCube.gltf");
	injectionLeftObj_->SetIsDraw(false);
	injectionLeftObj_->worldtransform_.translate_ = injectionLeftPos_;
	injectionLeftObj_->worldtransform_.scale_ = { 0.75f,1.25f,1.0f };

	injectionRightObj_ = std::make_unique<Object3d>();
	injectionRightObj_->Initialize(entity3DManager);
	injectionRightObj_->SetCamera(camera);
	injectionRightObj_->SetModel("AnimatedCube.gltf");
	injectionRightObj_->SetIsDraw(false);
	injectionRightObj_->worldtransform_.translate_ = injectionRightPos_;
	injectionRightObj_->worldtransform_.scale_ = { 0.75f,1.25f,1.0f };

	isSpecial_ = false;

	maxGauge_ = 20;
	
	clock_ = 1;
	maxBullet = 10;
	bulletNum = 0;

}

void BulletSpecial::Update()
{
	if (gauge_ >= maxGauge_) {
		isSpecial_ = true;
		gauge_ = maxGauge_;
	}
	else {
		isSpecial_ = false;
	}
}

void BulletSpecial::Draw()
{

}

void BulletSpecial::InAction(FollowCamera* followCamera, BulletManager* bulletManager, std::vector<BaseEnemy*> enemy)
{
	

	int time = 0;
	switch (phese_)
	{
	case 0:
		time_ += MyGame::GameTime();
		// 移動
		
		if (time_ >= 0.5f) {
			if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_RB)) {
				phese_ = 1;
				time_= 0;
			}
		}
		index_b = 0;

		injectionLeftObj_->SetIsDraw(true);
		injectionRightObj_->SetIsDraw(true);
		break;
	case 1:
		// 弾を発射
		time_ += MyGame::GameTime();
		time = int(time_ * 60);
		injectionLeftObj_->worldtransform_.translate_.y = injectionLeftPos_.y;
		injectionRightObj_->worldtransform_.translate_.y = injectionRightPos_.y;


		if (time % 10 == 0) {
			clock_ *= -1;
			while (index_b < enemy.size())
			{
				
				if (clock_ == 1) {
					followCamera->GetUniqueCamera()->SetShake(1.3f, { 0.2f,0.2f,0.2f });
					bulletManager->GenerateBullet(BulletManager::BulletType::kPlayerMissile, injectionLeftObj_->GetWorldPosition(), enemy[index_b]);
					injectionLeftObj_->worldtransform_.translate_.y -= 0.5f;

				}
				else {
					followCamera->GetUniqueCamera()->SetShake(1.3f, { 0.2f,0.2f,0.2f });
					bulletManager->GenerateBullet(BulletManager::BulletType::kPlayerMissile, injectionRightObj_->GetWorldPosition(), enemy[index_b]);
					injectionRightObj_->worldtransform_.translate_.y -= 0.5f;
				}


				bulletNum++;
				index_b++;
				if (index_b >= enemy.size()) {
					index_b = 0;
				}
				break;
			}
			
			
		}





		if (maxBullet <= bulletNum)
		{
			clock_ = 1;
			bulletNum = 0;
			phese_ = 2;
			injectionLeftObj_->SetIsDraw(false);
			injectionRightObj_->SetIsDraw(false);
		}

		break;
	case 2:
		
		break;
	}
}

void BulletSpecial::SetParent(WorldTransform* parent)
{
	injectionLeftObj_->worldtransform_.parent_ = parent;
	injectionRightObj_->worldtransform_.parent_ = parent;
}

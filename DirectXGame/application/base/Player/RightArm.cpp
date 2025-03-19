#include "RightArm.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

void RightArm::Initialize(Camera* camera)
{

	input_ = Input::GetInstance();
	camera_ = camera;



	// プレイヤー
	objectBase_ = std::make_unique<Object3d>();
	objectBase_->Initialize();
	objectBase_->SetCamera(camera);
	objectBase_->SetModel("AnimatedCube.gltf");

	objectBase_->worldtransform_.translate_ = 0;
	objectBase_->Update();
}

void RightArm::Update()
{
	ChargeAttack();


	Levelup();

	objectBase_->Update();
}

void RightArm::Draw()
{
	objectBase_->Draw();
}

void RightArm::LevelParameters()
{

	switch (level_)
	{
	case 1:
		maxVeloTime_ = 0.5f;



		break;
	case 2:
		maxVeloTime_ = 0.75f;

		break;
	case 3:
		maxVeloTime_ = 1.0f;


		break;
	default:
		break;
	}




}

void RightArm::Levelup()
{

}

void RightArm::ChargeAttack()
{
	if (input_->IsGamePadPressed(GamePadButton::GAMEPAD_RB)) {
		MyGame::kTimeSpeed_ = 0.25f;
		levelUpTime_ += MyGame::kDeltaTime_ * 1.0f;
		objectBase_->worldtransform_.translate_.z = -static_cast<float>(level_ * 2);
		velo_ = camera_->GetForward(); //方向飛ばす

		if (levelUpTime_ >= maxLevelUpTime_) {
			levelUpTime_ = 0;
			level_++;

			if (level_ >= 3) {
				level_ = 3;
			}
		}
		maxVelo_ = velo_ * static_cast<float>(level_) * 100;
		velo_ = maxVelo_ ;

		LevelParameters();
		

		isTake_ = true;
	}
	if (input_->IsGamePadReleased(GamePadButton::GAMEPAD_RB)) {
		objectBase_->worldtransform_.translate_.z = 0;
		isTake_ = false;
		isShot_ = true;
		veloTime_ = maxVeloTime_;
		MyGame::kTimeSpeed_ = 1.0f;
		level_ = 1;
	}

	if (isShot_) {
		veloTime_ -= MyGame::kDeltaTime_ * 1.0f;

		float t = GetVeloctiyTime_t();

		velo_ = Lerp(maxVelo_, Vector3{ 0,0,0 },t);

		if (veloTime_ <= 0.0f) {
		//	isShot_ = false;
		}

		if (GetVeloctiyTime_t() >= 1.0f) {
			veloTime_ = 0.0f;
			isShot_ = false;
		}
	}


}

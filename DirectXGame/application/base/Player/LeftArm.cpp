#include "LeftArm.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "Player.h"

void LeftArm::Initialize(Camera* camera)
{
	camera_ = camera;

	input_ = Input::GetInstance();

	// プレイヤー
	objectBase_ = std::make_unique<Object3d>();
	objectBase_->Initialize();
	objectBase_->SetCamera(camera);
	objectBase_->SetModel("leftArm.obj");

	objectBase_->worldtransform_.translate_ = 0;
	objectBase_->Update();

	objectThread_ = std::make_unique<Object3d>();
	objectThread_->Initialize();
	objectThread_->SetCamera(camera);
	//objectThread_->SetModel("leftNeedle.obj");
	objectThread_->SetModel("AnimatedCube.gltf");
	objectThread_->worldtransform_.scale_ = { 0.5f,0.5f,0.5f };
	//objectThread_ = 
	//objectThread_->worldtransform_.parent_ = &objectBase_->worldtransform_;

}

void LeftArm::Update()
{
	// 撃つ
	Shot();

	//

	//objectBase_->worldtransform_.rotate_ = camera_->GetForward();

	Vector3 worldposStr = objectBase_->worldtransform_.worldMat_.GetWorldPosition();
	Vector3 worldposEnd = objectThread_->worldtransform_.worldMat_.GetWorldPosition();

	
	

	
	// ライン
	LineCommon::GetInstance()->AddLine(worldposStr, worldposEnd,{1,1,0,1});

	objectBase_->Update();
	objectThread_->Update();
}

void LeftArm::Draw()
{
	objectBase_->Draw();
	objectThread_->Draw();
}

void LeftArm::Shot()
{
	// Y軸周り角度(θy)
	objectBase_->worldtransform_.rotate_.y = std::atan2(-velo_.x, velo_.z);
	float length = Length(Vector3(velo_.x, 0, velo_.z));

	// X軸周り角度(θx)
	objectBase_->worldtransform_.rotate_.x = std::atan2(velo_.y, -length);
	//objectThread_->worldtransform_.rotate_ = objectBase_->worldtransform_.rotate_;
	length_ = Length(objectBase_->worldtransform_.worldMat_.GetWorldPosition() - objectThread_->worldtransform_.worldMat_.GetWorldPosition());
	lengthTime_ = length_ / maxlength_;

	// 飛ばす
	if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_LB) && !isShot_ && !isWallFloor_) {
		objectThread_->worldtransform_.translate_ = objectBase_->GetWorldPosition(); // 撃つ位置(開始点)

		//objectThread_->worldtransform_.parent_ = nullptr;

		velo_ = camera_->GetForward(); //方向飛ばす
		isShot_ = true; // 発射開始
		shotTime_ = maxShotTime_;     // 撃つ時間
		attracTime_ = maxAttracTime_; // 引き寄せる時間
		removeTime_ = maxRemoveTime_; // 戻す時間 
		isInTheMiddle_ = false; // 途中引き戻しfalse
	}
	// 引き寄せる
	else if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_LB) && isWallFloor_&& !isAttract_) {
		attracPos_ = player_->GetObject3D().GetWorldPosition(); // プレイヤー位置
		maxAttracTime_ = lengthTime_;
		attracTime_ = maxAttracTime_;
		isAttract_ = true; // 引き寄せ開始
	}
	// 途中で糸を戻す
	else if(input_->IsGamePadTriggered(GamePadButton::GAMEPAD_LB) && isAttract_){
		isShot_ = false;
		removePos_ = objectThread_->worldtransform_.worldMat_.GetWorldPosition(); // 戻る位置(始点)
		maxRemoveTime_ = lengthTime_;
		removeTime_ = maxRemoveTime_; // 戻す時間 
		isRemove_ = true; // 戻す開始
		isAttract_ = false; // 引き寄せ終わり
		isInTheMiddle_ = true; // 途中引き戻し
		isWallFloor_ = false;// 一旦刺さり取れ
		shotTime_ = 0.0f;
		attracTime_ = 0.0f;
		
	}
	// 戻す
	else if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_LB) && !isRemove_ && !isInTheMiddle_) {
		removePos_ = objectThread_->worldtransform_.worldMat_.GetWorldPosition(); // 戻る位置(始点)
		
		maxRemoveTime_ = lengthTime_;
		removeTime_ = maxRemoveTime_; // 戻す時間 
		isRemove_ = true; // 戻す開始
		isInTheMiddle_ = false;
		
	}

	// 発射しているか
	if (isShot_ && !isRemove_) {
		shotTime_ -= MyGame::kDeltaTime_ * 1.0f;

		if (shotTime_ <= 0.0f) {
			isShot_ = false; 
			isWallFloor_ = true; // 刺さっていることにする
		}

		
		veloAll_ = velo_ * shotSpeed;
		objectThread_->worldtransform_.translate_ += veloAll_;
	

		if (objectThread_->worldtransform_.translate_.y < 2.0f) {
			objectThread_->worldtransform_.translate_.y = 2.0f;
			isShot_ = false;
			isWallFloor_ = true; // 刺さっていることにする
		}

	}

	// 引き延ばし中に戻す
	if (isRemove_) {
		shotTime_ = 0;
		attracTime_ = 0;
		removeTime_ -= MyGame::kDeltaTime_ * 1.0f;

		float remove_t = (maxRemoveTime_ - removeTime_) / maxRemoveTime_;


		objectThread_->worldtransform_.translate_ = Lerp(removePos_, objectBase_->worldtransform_.worldMat_.GetWorldPosition(), remove_t);
	
		if (removeTime_ <= 0.0f) {
			shotTime_ = 0.0f;
			attracTime_ = 0.0f;
			removeTime_ = 0.0f;
			isRemove_ = false;
			isShot_ = false;
			isWallFloor_ = false;
			//objectThread_->worldtransform_.translate_ = { 0,0,0 };
			//objectThread_->worldtransform_.parent_ = &objectBase_->worldtransform_;
		}
	}

	playerPos_ = player_->GetObject3D().GetWorldPosition();

	playerOldPos_ = player_->GetObject3D().GetPreWorldPosition();

	veloTotal_ = Vector3{ playerPos_.x ,playerPos_.y ,playerPos_.z} - Vector3{ playerOldPos_.x,playerOldPos_.y,playerOldPos_.z };
	//veloTotal_.y = 0;


	// 刺さっているか
	if (isAttract_ && !isRemove_) {
		attracTime_ -= MyGame::GameTime();

		// 時間が0.0になったら
		if (attracTime_ <= 0.0f) {
			attracTime_ = 0.0f;
			isAttract_ = false; // 引き寄せ終わり
			isWallFloor_ = false;// 一旦刺さり取れ
			//objectThread_->worldtransform_.translate_ = { 0,0,0 };
			//objectThread_->worldtransform_.parent_ = &objectBase_->worldtransform_;
			
		}

	}
	

	// 何もしていないなら
	if (!isShot_ && !isAttract_ && !isRemove_ && !isWallFloor_) {
		objectThread_->worldtransform_.translate_ = objectBase_->worldtransform_.worldMat_.GetWorldPosition();
		objectBase_->worldtransform_.rotate_ = 0;
		objectThread_->worldtransform_.rotate_ = 0;
	}

	if (player_->GetObject3D().worldtransform_.translate_.y < 2.0f) {
		velo_ = { 0,0,0 };
		isInTheMiddle_ = false;
	}

}

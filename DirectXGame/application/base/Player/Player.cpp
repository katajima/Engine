#include "Player.h"
#include"DirectXGame/application/base/Enemy/Enemy.h"
#include "DirectXGame/application/base/FollowCamera/FollowCamera.h"

#include "assert.h"

void Player::Initialize(Vector3 position, Camera* camera)
{
	Collider::Initialize(camera);
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayer));

	camera_ = camera;



	// プレイヤー
	objectBase_.Initialize();
	objectBase_.SetCamera(camera_);
	objectBase_.worldtransform_.translate_ = position;
	objectBase_.Update();

	// レティクル
	objectReticle_.Initialize();
	objectReticle_.SetCamera(camera_);
	objectReticle_.SetModel("enemy.obj");
	objectReticle_.worldtransform_.parent_ = &objectBase_.worldtransform_;
	objectReticle_.worldtransform_.translate_ = { 0,0,30 };

	//assert(objectBody_ != nullptr);
	//assert(objectBase_ != nullptr);

	// 体
	objectBody_.Initialize();
	objectBody_.SetCamera(camera_);
	objectBody_.SetModel("AnimatedCube.gltf");
	objectBody_.worldtransform_.parent_ = &objectBase_.worldtransform_;
	objectBody_.model->modelData.material[0]->shininess_ = 10000;



	// 左ミサイル発射口
	injectionLeftObj_.Initialize();
	injectionLeftObj_.SetCamera(camera_);
	injectionLeftObj_.SetModel("AnimatedCube.gltf");
	injectionLeftObj_.worldtransform_.parent_ = &objectBase_.worldtransform_;
	injectionLeftObj_.worldtransform_.translate_ = injectionLeftPos_;

	injectionLeftObj_.worldtransform_.scale_= { 0.75f,1.25f,1.0f };

	// 右ミサイル発射口
	injectionRightObj_.Initialize();
	injectionRightObj_.SetCamera(camera_);
	injectionRightObj_.SetModel("AnimatedCube.gltf");
	injectionRightObj_.worldtransform_.parent_ = &objectBase_.worldtransform_;
	injectionRightObj_.worldtransform_.translate_ = injectionRightPos_;

	injectionRightObj_.worldtransform_.scale_ = { 0.75f,1.25f,1.0f };




	// 影
	objectSha_.Initialize();
	objectSha_.SetCamera(camera_);
	objectSha_.SetModel("plane.obj");
	objectSha_.model->modelData.material[0]->tex_.diffuseFilePath = "resources/Texture/aa.png";
	objectSha_.model->modelData.material[0]->color = { 0.9f,0.0f,0.0f,1 };
	objectSha_.worldtransform_.translate_ = position;
	objectSha_.worldtransform_.scale_ = { 4,4,4 };
	objectSha_.worldtransform_.rotate_.x = DegreesToRadians(-90);


	
	

	

	//particleManager_ = ParticleManager::GetInstance();
	ParticleManager::GetInstance()->CreateParticleGroup("dust", "resources/Texture/uvChecker.png", ModelManager::GetInstance()->FindModel("plane.obj"));
	
	
	

	



	HpBer_ = std::make_unique<Sprite>();
	HpBer_->Initialize("resources/Texture/Image.png");
	HpBer_->SetSize({ 50,-float(hp) });
	HpBer_->SetColor({ 0,1,0,1 });
	HpBer_->SetPosition({ 100,650 });






	
	flag33 = false;



	dashEmitter_ = std::make_unique <ParticleEmitter>();
	dashEmitter_->Initialize("dash", "dashEmit", ParticleEmitter::EmitSpawnShapeType::kCornerLine);
	//dashEmitter_->SetParent(weapon_->GetObject3D().worldtransform_);
	dashEmitter_->GetFrequency() = 0.05f;
	dashEmitter_->SetCount(5);
	dashEmitter_->SetLifeTimeMinMax(0.1f, 0.1f);
	dashEmitter_->SetIsAlpha(true);
	dashEmitter_->SetIsEmit(false);
	dashEmitter_->SetColorMinMax({ 0.7f,0.7f,0.7f,0.9f }, { 0.7f,0.7f,0.7f,0.9f });
	dashEmitter_->SetRengeMinMax({ -1.25f,-1.25f ,-1.25f }, { 1.25f,1.25f,1.25f });
	dashEmitter_->SetSizeMinMax(Vector3{ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f });
	dashEmitter_->SetVelocityMinMax({},{});
	dashEmitter_->SetPos({0,7,0});
	dashEmitter_->SetCorner(16,0.5f);
	dashEmitter_->transform_.rotate_.x = DegreesToRadians(90);

}

void Player::Update()
{
	dashEmitter_->transform_.rotate_.y = objectBase_.worldtransform_.rotate_.y;






#ifdef _DEBUG
	if (isInvincible) {
		objectBody_.model->modelData.material[0]->color = { 0,0,1,1 };
	}
	else {
		objectBody_.model->modelData.material[0]->color = { 1,1,1,1 };
	}

#endif // _DEBUG


	if (objectBase_.worldtransform_.translate_.x > moveLimit) {
		objectBase_.worldtransform_.translate_.x = moveLimit;
	}
	if (objectBase_.worldtransform_.translate_.x < -(moveLimit + 100)) {
		objectBase_.worldtransform_.translate_.x = -(moveLimit + 100);
	}
	if (objectBase_.worldtransform_.translate_.z > (moveLimit + 100)) {
		objectBase_.worldtransform_.translate_.z = (moveLimit + 100);
	}
	if (objectBase_.worldtransform_.translate_.z < -(moveLimit + 100)) {
		objectBase_.worldtransform_.translate_.z = -(moveLimit + 100);
	}

	


	Gravity();
	
	dashEmitter_->Update();

	objectBase_.Update();
	objectBody_.Update();
	
	injectionLeftObj_.Update();
	injectionRightObj_.Update();

	
	objectReticle_.Update();
	objectSha_.Update();

	
}

void Player::Draw()
{

	objectBody_.Draw();
}

void Player::DrawP()
{

	
}

void Player::Draw2D()
{

	

}

void Player::OnCollision(Collider* other)
{
	// 衝突判定の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {
	}
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemyWeapon)) {
	}
}

Vector3 Player::GetCenterPosition() const
{
	return objectBase_.GetWorldPosition();
}


void Player::Move()
{
	speed = 20.0f;
	velocity_ = { 0,0,0 };
	isMove = false;



	if (Input::GetInstance()->IsControllerConnected()) {


		velocity_.x = Input::GetInstance()->GetGamePadLeftStick().x;
		velocity_.z = Input::GetInstance()->GetGamePadLeftStick().y;


		if (velocity_.x != 0.0f || velocity_.z != 0.0f) {
			isMove = true;
			// 入力方向を正規化
			velocity_ = Normalize(velocity_);
			velocity_ = Multiply(velocity_, speed);


			// カメラのビュー行列の逆行列（カメラのワールド変換行列）を取得
			Matrix4x4 cameraWorldMatrix = Inverse(camera_->viewMatrix_);

			// カメラの向きに基づいて移動方向をワールド座標系に変換
			Vector3 worldDirection = {
				velocity_.x * cameraWorldMatrix.m[0][0] + velocity_.z * cameraWorldMatrix.m[2][0],
				0.0f,
				velocity_.x * cameraWorldMatrix.m[0][2] + velocity_.z * cameraWorldMatrix.m[2][2]
			};

			velocity_ = Multiply(Normalize(worldDirection), speed);

			//// 移動ベクトルをカメラの角度だけ回転する
			//Matrix4x4 rotateMatrixY = MakeRotateYMatrix(camera_->transform_.rotate.y);
			//velocity_ = TransformNormal(velocity_, rotateMatrixY);
			//
			if (velocity_.Length() != 0) {
				objectBase_.worldtransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
			}


		}
	}
	else {


		if (Input::GetInstance()->IsPushKey(DIK_W)) {
			velocity_.z += 0.3f;
		}
		if (Input::GetInstance()->IsPushKey(DIK_S)) {
			velocity_.z -= 0.3f;

		}
		if (Input::GetInstance()->IsPushKey(DIK_A)) {
			velocity_.x -= 0.3f;

		}
		if (Input::GetInstance()->IsPushKey(DIK_D)) {
			velocity_.x += 0.3f;
		}


		if (velocity_.x != 0.0f || velocity_.z != 0.0f) {
			// 入力方向を正規化
			velocity_ = Normalize(velocity_);
			isMove = true;

			// カメラのビュー行列の逆行列（カメラのワールド変換行列）を取得
			Matrix4x4 cameraWorldMatrix = Inverse(camera_->viewMatrix_);

			// カメラの向きに基づいて移動方向をワールド座標系に変換
			Vector3 worldDirection = {
				velocity_.x * cameraWorldMatrix.m[0][0] + velocity_.z * cameraWorldMatrix.m[2][0],
				0.0f,
				velocity_.x * cameraWorldMatrix.m[0][2] + velocity_.z * cameraWorldMatrix.m[2][2]
			};

			velocity_ = Multiply(Normalize(worldDirection), speed);


		}
	}

	
}

void Player::Gravity() {
	// 移動
	//velocity_.y = graVelo;

	// 重力加速度
	const float kGravityAcceleration = 4.4f;

	// 加速度ベクトル
	float accelerationVector = -kGravityAcceleration; // 毎フレームのデルタ時間で重力を適用

	// 加速する
	velocity_.y += accelerationVector;

	AddMove();
	// 着地
	if (objectBase_.worldtransform_.translate_.y <= groundY) {
		objectBase_.worldtransform_.translate_.y = groundY;
		graVelo = 0;
		isJamp = false;
	}
}

void Player::AddMove()
{
	if(isAlive)
	objectBase_.worldtransform_.translate_ += velocity_ * MyGame::GameTime();
}









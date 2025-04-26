#include "NormalEnemy.h"

#include "DirectXGame/application/base/Player/Player.h"

void NormalEnemy::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera)
{
	Collider::Initialize(camera);
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));


	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;

	transBase_.Initialize();
	transBase_.translate_ = position;

	object_->Initialize(entity3DManager_);
	object_->SetModel("enemy2.obj");
	object_->SetCamera(camera);
	object_->worldtransform_.parent_ = &transBase_;
	object_->worldtransform_.scale_ = { 1.7f,1.7f,1.7f };


	isAlive_ = true;

	parameter_.HP = 100;

	parameter_.moveSpeed = 3.0f;

	parameter_.moveSpeed = 3.0f;

}

void NormalEnemy::Update()
{
	HitStpoTime();
	if (parameter_.HP <= 0) {
		if (isAlive_ == true) {
			/*ductEmit_->Update();
			tireEmit_->Update();
			plankEmit_->Update();
			gearEmit_->Update();
			fenceEmit_->Update();*/
		}
		isAlive_ = false;
	}

	if (isAlive_) {
		if (!hit) {
			count = 0.0f;
			Move();
			oldPos_ = { 0,0,0 };
		}
		else {
			oldPos_ = { 0,0,0 };
			HitMotion();
		}
		// 影
		//objectSha_.worldtransform_.translate_ = transBase_.translate_;
		//objectSha_.worldtransform_.translate_.y = 0.1f;

		//Vector3 scale{};
		//scale = 7;

		//objectSha_.worldtransform_.scale_ = scale;

		// 煙
		//dustEmit_->Update();
		//dustEmit2_->Update();
		//dustEmit3_->Update();
	}

	object_->Update();
	transBase_.Update();
	//objectSha_.Update();
}

void NormalEnemy::Draw()
{
	// 生きていたら
	if (isAlive_) {
		object_->Draw();
		//objectSha_.Draw();
	}
}

void NormalEnemy::DrawP()
{
}

void NormalEnemy::Draw2D()
{
}

void NormalEnemy::SetPlayer(Player* player)
{
	player_ = player; 
}

void NormalEnemy::Move()
{
	// 回転と移動量の設定
	const float kMoveSpeed = parameter_.moveSpeed; // 移動速度
	// worldTransformBase_.rotation_.y += 0.00f; // 一定量のY軸回転

	// 向いている方向への移動ベクトルの計算
	Vector3 moveDirection = { 0.0f, 0.0f, kMoveSpeed };
	Matrix4x4 rotationMatrix = MakeRotateYMatrix(transBase_.rotate_.y);
	moveDirection = TransformNormal(moveDirection, rotationMatrix);

	// ロックオン座標
	Vector3 lockOnPosition = player_->GetObject3D().GetWorldPosition();

	// 追跡対象からロックオン対象へのベクトル
	Vector3 sub = Subtract(lockOnPosition, transBase_.translate_);

	// Y軸周り角度
	transBase_.rotate_.y = std::atan2(sub.x, sub.z);

	if (Distance(player_->GetCenterPosition(), object_->GetWorldPosition()) >= 5) {

		// 移動
		transBase_.translate_ = Add(transBase_.translate_, moveDirection * Timer());
	}
	/*groundRightEmit_->SetVelocityMinMax(-moveDirection * 2, -moveDirection * 2);
	groundRightEmit_->Update();
	groundLeftEmit_->SetVelocityMinMax(-moveDirection * 2, -moveDirection * 2);
	groundLeftEmit_->Update();*/



}

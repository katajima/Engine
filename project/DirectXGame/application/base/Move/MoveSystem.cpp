#include "MoveSystem.h"


void MoveSystem::Initialize() {
}

void MoveSystem::Update(float dt, WorldTransform& world, Input* input)
{

	Vector3 velo = GetVelocity();

	// スティックを動かした方向
	velo.x = input->GetGamePadLeftStick().x;
	velo.z = input->GetGamePadLeftStick().y;
	// 入力方向を正規化
	velo = Normalize(velo);

	// スティック入力を保存
	stickInput_ = input->GetGamePadLeftStick();

	// 移動方向を保存
	DirectionProcess(velo);

	// 攻撃中の回転処理
	AttackProcess(world);

	if (isAttack_ && !isAttackCanMove_) return;// 攻撃中は移動処理しない

	// ダッシュ時の回転処理
	DashProcess(world);
	
	if (isDash_) return; // ダッシュ中は移動処理しない

	// 回転処理
	RotateProcess(dt, world, direction_);


	// 移動処理
	MoveProcess(dt, world, velo);
}

void MoveSystem::Update(float dt, WorldTransform& world)
{
	if (isDash_) return; // ダッシュ中は移動処理しない

	Vector3 velo = GetVelocity();
	velo = Normalize(velo);

	// 移動処理
	MoveProcess(dt, world, velo);
}

void MoveSystem::UpdateEnemy(float dt, WorldTransform& world)
{
	if (isDash_) return; // ダッシュ中は移動処理しない

	Vector3 velo = GetVelocity();
	velo = Normalize(velo);

	// 移動処理
	MoveProcess(dt, world, velo,false);
}

void MoveSystem::UpdateAttack(float dt, WorldTransform& world)
{
	MoveProcess(dt, world,keepDirection_);
}


#pragma region Process

void MoveSystem::SpeedProcess(float dt)
{

	if (data_.moveType == MoveType::LINEAR) { // 一定
		// スピードをだんだん上げる
		speed_ = data_.maxSpeed;
	}
	else if (data_.moveType == MoveType::ACCELERATE) { // 加速
		// 目標は maxSpeed
		float targetSpeed = data_.maxSpeed;

		// dt を使って徐々に近づける
		// speedAcceleration は「1秒あたりどれくらい寄せるか」
		float t = Math::Clamp(data_.speedAcceleration, 0.0f, 1.0f);

		// 現在の speed から targetSpeed へ補間していく
		speed_ = Lerp(speed_, targetSpeed, t);

		// 最低/最高速度の範囲に収める
		speed_ = Math::Clamp(speed_, data_.minSpeed, data_.maxSpeed);
	}

	// スティックの倒し方に応じてスピードを変化させる
	if (data_.isStickToSpeed) {
		float stickLen = Math::Clamp(std::abs(stickInput_.Length()), 0.0f, 1.0f);
		speed_ *= stickLen;
	}

	// 空中での速度制限
	if (data_.isLimitAirSpeed && isAir_) {
		speed_ *= data_.airSpeedRate;	// 空中速度倍率をかける(0.0f以上~1.0f以下での使用をおすすめ)
	}

}

void MoveSystem::RotateProcess(float dt, WorldTransform& world, const Vector3& velo)
{
	// 移動ベクトルがゼロなら回転処理しない
	if (velo.Length() == 0.0f) return;

	// 目標方向（X=Right, Y=Up, Z=Forward）
	float targetYaw = std::atan2(velo.x, velo.z);

	float& currentYaw = world.rotate_.y;

	// ----------------------------
	// 角度差を [-π, π] に正規化
	// ----------------------------
	float delta = targetYaw - currentYaw;
	while (delta > DirectX::XM_PI)  delta -= DirectX::XM_2PI;
	while (delta < -DirectX::XM_PI)  delta += DirectX::XM_2PI;

	// ----------------------------
	// 補間（rotationSpeed は 0〜1 の割合）
	// rotationSpeed = 0.1f なら 10% だけ近づく
	// ----------------------------
	currentYaw += delta * data_.rotationSpeed;
}

void MoveSystem::MoveProcess(float dt, WorldTransform& world, Vector3& velo,bool isSpeed)
{
	// 動いているなら
	if (velo.x != 0.0f || velo.z != 0.0f) {
		// 入力方向を正規化
		velo = Normalize(velo);

		// スピード処理
		if (isSpeed) {
			SpeedProcess(dt);
		}
		// カメラのビュー行列の逆行列（カメラのワールド変換行列）を取得
		if (camera_) {
			CameraDirectionToMoveDirection(velo);
		}
		else {
			// 移動方向にスピードを掛ける
			if (isSpeed) {
				velo = Multiply(velo, speed_);
			}
			else {
				velo = Multiply(velo, data_.maxSpeed);
			}
		}

		inputMove_ = true;
	}
	else {
		inputMove_ = false;
	}

	// 移動速度をセット
	velocity_.x = velo.x;
	velocity_.z = velo.z;

	// 方向保存
	keepDirection_ = direction_;

	// 状態処理
	StateProcess();

	// アニメーション速度処理
	AnimationSpeedProcess();

	// 移動可能かどうか
	if (data_.canMove) {
		world.translate_ += Multiply(velocity_, dt);
	}
}

void MoveSystem::StateProcess()
{
	float len = GetVelocity().Length();

	if (len == 0.0f) {
		state_ = State::kIdle;
	}
	else {
		if (len < data_.moveStateThreshold) {
			state_ = State::kWalk;
		}
		else {
			state_ = State::kRun;
		}
	}
}

void MoveSystem::AnimationSpeedProcess()
{
	// 速度を正規化して 0〜1 に収める
	float t = Math::NormalizeClamp(speed_, data_.minSpeed, data_.maxSpeed);
	if (data_.isSameAnimation) {
		// アニメーション速度を
		animationSpeed_ = Lerp(data_.animetionSpeedMinWalk, data_.animetionSpeedMaxWalk, t);
	}
	else {
		if (state_ == State::kWalk) {	// 走り
			animationSpeed_ = Lerp(data_.animetionSpeedMinWalk, data_.animetionSpeedMaxWalk, t);
		}
		else if (state_ == State::kRun) { // 走り
			animationSpeed_ = Lerp(data_.animetionSpeedMinRun, data_.animetionSpeedMaxRun, t);
		}
	}
}

void MoveSystem::CameraDirectionToMoveDirection(Vector3& velo) {
	Matrix4x4 cameraWorldMatrix = Inverse(camera_->GetViewMatrix());

	// カメラの向きに基づいて移動方向をワールド座標系に変換
	Vector3 worldDirection = {
		velo.x * cameraWorldMatrix.m[0][0] + velo.z * cameraWorldMatrix.m[2][0],
		0.0f,
		velo.x * cameraWorldMatrix.m[0][2] + velo.z * cameraWorldMatrix.m[2][2]
	};

	velo = Multiply(Normalize(worldDirection), speed_);
}

void MoveSystem::DirectionProcess(const Vector3& velo)
{
	if (velo.Length() == 0.0f) return;
	Matrix4x4 cameraWorldMatrix = Inverse(camera_->GetViewMatrix());

	// カメラの向きに基づいて移動方向をワールド座標系に変換
	Vector3 worldDirection = {
		velo.x * cameraWorldMatrix.m[0][0] + velo.z * cameraWorldMatrix.m[2][0],
		0.0f,
		velo.x * cameraWorldMatrix.m[0][2] + velo.z * cameraWorldMatrix.m[2][2]
	};

	direction_ = Multiply(Normalize(worldDirection), 1.0f);
}

void MoveSystem::DashProcess(WorldTransform& world)
{
	// ダッシュ中は移動処理しない
	if (!isDash_) return;

	// 移動ベクトルがゼロなら回転処理しない
	if (keepDirection_.Length() == 0.0f) return;

	// 目標方向（X=Right, Y=Up, Z=Forward）
	float targetYaw = std::atan2(keepDirection_.x, keepDirection_.z);

	float& currentYaw = world.rotate_.y;

	currentYaw = targetYaw;
}

void MoveSystem::AttackProcess(WorldTransform& world)
{
	// ダッシュ中は移動処理しない
	if (!isAttack_) return;

	// 移動ベクトルがゼロなら回転処理しない
	if (keepDirection_.Length() == 0.0f) return;

	// 目標方向（X=Right, Y=Up, Z=Forward）
	float targetYaw = std::atan2(keepDirection_.x, keepDirection_.z);

	float& currentYaw = world.rotate_.y;

	currentYaw = targetYaw;
}


#pragma endregion // 処理



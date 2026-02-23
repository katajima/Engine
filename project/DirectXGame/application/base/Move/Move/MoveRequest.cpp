#include "MoveRequest.h"


void MoveRequest::Initialize() {
}

void MoveRequest::Update(float dt, Engine::WorldTransform& world, InputSystem* input)
{
	// 攻撃中は通常の移動処理しない
	if (isAttack_) return;

	Vector3 velo = GetVelocity();

	// スティックを動かした方向
	velo.x = input->GetData().moveShick.x;
	velo.z = input->GetData().moveShick.y;
	// 入力方向を正規化
	velo = Normalize(velo);

	// スティック入力を保存
	stickInput_ = input->GetData().moveShick;

	// 移動方向を保存
	DirectionProcess(velo);

	// 回転処理
	RotateProcess(dt, world, direction_);

	// 移動処理
	MoveProcess(dt, world, velo);
}

void MoveRequest::UpdateEnemy(float dt){}

#pragma region Process

void MoveRequest::SpeedProcess(float dt)
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

void MoveRequest::RotateProcess(float dt, Engine::WorldTransform& world, const Vector3& velo)
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

void MoveRequest::MoveProcess(float dt, Engine::WorldTransform& world, Vector3& velo,bool isSpeed)
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
		if (camera) {
			CameraDirectionToMoveDirection(velo);
			velo = Multiply(velo, speed_);
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
	if (canMove_) {
		world.translate_ += Multiply(velocity_, dt);
	}
}

void MoveRequest::StateProcess()
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

void MoveRequest::AnimationSpeedProcess()
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

void MoveRequest::CameraDirectionToMoveDirection(Vector3& velo) {
	Matrix4x4 cameraWorldMatrix = Inverse(camera->GetViewMatrix());

	// カメラの向きに基づいて移動方向をワールド座標系に変換
	Vector3 worldDirection = {
		velo.x * cameraWorldMatrix.m[0][0] + velo.z * cameraWorldMatrix.m[2][0],
		0.0f,
		velo.x * cameraWorldMatrix.m[0][2] + velo.z * cameraWorldMatrix.m[2][2]
	};

	velo = Normalize(worldDirection);
}

void MoveRequest::DirectionProcess(const Vector3& velo)
{
	if (velo.Length() == 0.0f) return;
	Matrix4x4 cameraWorldMatrix = Inverse(camera->GetViewMatrix());

	// カメラの向きに基づいて移動方向をワールド座標系に変換
	Vector3 worldDirection = {
		velo.x * cameraWorldMatrix.m[0][0] + velo.z * cameraWorldMatrix.m[2][0],
		0.0f,
		velo.x * cameraWorldMatrix.m[0][2] + velo.z * cameraWorldMatrix.m[2][2]
	};

	direction_ = Multiply(Normalize(worldDirection), 1.0f);
}

void MoveRequest::AttackProcess(Engine::WorldTransform& world, const Vector3& direction)
{
	// ダッシュ中は移動処理しない
	if (!isAttack_) return;

	// 移動ベクトルがゼロなら回転処理しない
	if (direction.Length() == 0.0f) return;

	// 目標方向（X=Right, Y=Up, Z=Forward）
	float targetYaw = std::atan2(direction.x, direction.z);

	float& currentYaw = world.rotate_.y;

	currentYaw = targetYaw;
}


#pragma endregion // 処理



#include "MoveSystem.h"


void MoveSystem::Initialize() {}

void MoveSystem::Update(const LocomotionContext& ctx, LocomotionCoordinator& coordinator){
	// 攻撃中は通常の移動処理しない
	if (ctx.isAttacking) return;

	MoveRequest request{};

	// 移動処理
	Vector2 dire = ctx.input.GetPlayerInputData().moveShick;
	Vector3 moveVelo{};

	if (dire.Length() == 0.0f) return;
	Matrix4x4 cameraWorldMatrix = Inverse(ctx.camera->GetViewMatrix());

	// カメラの向きに基づいて移動方向をワールド座標系に変換
	Vector3 worldDirection = {
		dire.x * cameraWorldMatrix.m[0][0] + dire.y * cameraWorldMatrix.m[2][0],
		0.0f,
		dire.x * cameraWorldMatrix.m[0][2] + dire.y * cameraWorldMatrix.m[2][2]
	};

	// 動いているなら
	if (dire.Length() != 0.0f) {
		// スピード処理
		SpeedProcess(ctx);
		// カメラのビュー行列の逆行列（カメラのワールド変換行列）を取得
		moveVelo = Multiply(worldDirection, speed_);
	}

	// 移動速度をセット
	velocity_.x = moveVelo.x;
	velocity_.z = moveVelo.z;

	// 状態処理
	StateProcess();

	// アニメーション速度処理
	AnimationSpeedProcess();

	

	



	// 移動可能かどうか
	if (canMove_) {
		request.velocity = velocity_ * ctx.dt;
		request.direction = Normalize(worldDirection);
		request.priority = 1;

		coordinator.Request(request);
	}
	else {
		coordinator.Request(request);
	}
}

void MoveSystem::UpdateEnemy(float dt) {}

#pragma region Process

void MoveSystem::SpeedProcess(const LocomotionContext& ctx)
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
		float stickLen = Math::Clamp(std::abs(ctx.input.GetPlayerInputData().moveShick.Length()), 0.0f, 1.0f);
		speed_ *= stickLen;
	}

	// 空中での速度制限
	if (data_.isLimitAirSpeed && isAir_) {
		speed_ *= data_.airSpeedRate;	// 空中速度倍率をかける(0.0f以上~1.0f以下での使用をおすすめ)
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

#pragma endregion // 処理

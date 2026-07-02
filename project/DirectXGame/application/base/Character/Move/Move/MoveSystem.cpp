#include "MoveSystem.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"

void MoveSystem::Initialize() {}

void MoveSystem::Update(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator) {
	// 攻撃中は通常の移動処理しない
	if (ctx.isAttacking) return;

	MoveRequest request{};

	// 移動処理
	Vector2 dire = ctx.inputData.moveShick;
	Vector3 moveVelo{};

	if (dire.Length() == 0.0f) return;

	Vector3 worldDirection = {
		ctx.worldStickDirection.x,
		0.0f,
		ctx.worldStickDirection.y
	};

	// 動いているなら
	if (dire.Length() != 0.0f) {
		// スピード処理
		SpeedProcess(ctx);
		// カメラのビュー行列の逆行列（カメラのワールド変換行列）を取得
		float fainalSpeed = speed_;
		if (ctx.isDashing) {
			fainalSpeed *= data_.dashSpeedRate;
		}

		moveVelo = Multiply(worldDirection, fainalSpeed);
	}

	// 移動速度をセット
	velocity_.x = moveVelo.x;
	velocity_.z = moveVelo.z;

	// 状態処理
	StateProcess();

	// アニメーション速度処理
	AnimationSpeedProcess();

	if (ctx.isCanMove) {
		// 移動可能かどうか
		request.velocity = velocity_ * ctx.dt;
		request.direction = Normalize(worldDirection);
		request.priority = 1;
		coordinator.Request(request);
	}
}

void MoveSystem::UpdateEnemy(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator) {
	// 攻撃中は通常移動しない
	if (ctx.isAttacking) return;
	MoveRequest request{};

	// =========================
	// 水平移動ベクトルを作る
	// =========================
	if (!ctx.target) {
		return;
	}

	Vector3 moveTarget = ctx.target->GetWorldPosition();
	if (ctx.hasMoveTarget) {
		// 群衆隊形、攻撃スロット、フロッキングの移動先を優先する
		moveTarget = ctx.moveTarget;
	}

	Vector3 toTarget = Subtract(moveTarget, ctx.position);
	toTarget.y = 0.0f;

	Vector3 horizontalDire{};
	float horizontalLength = toTarget.Length();
	if (horizontalLength > 0.001f) {
		horizontalDire = toTarget / horizontalLength;
	}

	// =========================
	// 垂直移動量を作る
	// =========================
	float verticalSpeed = 0.0f;

	// 重力を使わない空中敵だけ高度維持
	if (!data_.useGravity) {
		float heightError = ctx.skyHeight - ctx.position.y;
		const float hoverDeadZone = 0.05f;
		const float hoverGain = 3.0f;
		const float maxHoverSpeed = 1.2f;

		if (std::abs(heightError) > hoverDeadZone) {
			verticalSpeed = std::clamp(heightError * hoverGain, -maxHoverSpeed, maxHoverSpeed);
		}
		else {
			verticalSpeed = 0.0f;
		}
	}

	// =========================
	// 合成
	// =========================
	Vector3 velocity{};

	// 水平移動は moveSpeed を使う
	velocity.x = horizontalDire.x * ctx.moveSpeed * ctx.dt;
	velocity.z = horizontalDire.z * ctx.moveSpeed * ctx.dt;

	// 垂直移動は専用速度を使う
	velocity.y = verticalSpeed * ctx.dt;

	// 実際に水平移動する時だけ、移動システムが向きも制御する。
	// 停止中の敵は攻撃システムがプレイヤーを向くため、回転要求を重ねない。
	Vector3 moveDirection{};
	if (std::abs(ctx.moveSpeed) > 0.001f) {
		moveDirection = horizontalDire;
	}

	// 水平移動がない場合でも、浮遊だけしているなら上方向を向きに入れてもよい
	// ただし見た目の向きは水平だけで十分なら horizontalDire のままでOK
	if (horizontalLength <= 0.001f && std::abs(verticalSpeed) > 0.0f) {
		moveDirection = Vector3{ 0.0f, (verticalSpeed > 0.0f ? 1.0f : -1.0f), 0.0f };
	}

	if (ctx.isCanMove) {
		request.velocity = velocity;
		request.direction = moveDirection;
		request.priority = 1;
		coordinator.Request(request);
	}
}

#pragma region Process

void MoveSystem::SpeedProcess(const Character::CharacterContext& ctx)
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
		float stickLen = Math::Clamp(std::abs(ctx.inputData.moveShick.Length()), 0.0f, 1.0f);
		speed_ *= stickLen;
	}

	// 空中での速度制限
	if (data_.isLimitAirSpeed && !ctx.onGround) {
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
		if (state_ == State::kWalk) {	// 歩き
			animationSpeed_ = Lerp(data_.animetionSpeedMinWalk, data_.animetionSpeedMaxWalk, t);
		}
		else if (state_ == State::kRun) { // 走り
			animationSpeed_ = Lerp(data_.animetionSpeedMinRun, data_.animetionSpeedMaxRun, t);
		}
	}
}

#pragma endregion // 処理

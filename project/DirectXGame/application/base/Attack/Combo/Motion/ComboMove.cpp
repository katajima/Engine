#include "ComboMove.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"
#include "DirectXGame/application/base/Camera/Base/CameraManeger.h"

#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include <DirectXGame/application/base/Attack/AttackController.h>
#include "DirectXGame/engine/Math/MathFunctions.h"
#include <algorithm>
#include <cmath>

namespace Combo {

	// 開始
	void ComboMove::Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
		moveComponent = owner->GetMoveComponent();
		worldTransform = &owner->GetObjectComponent()->GetWorldTransform();
		lockOnSystem = owner->GetAttackController()->GeyLockOnSysutem();
		moveRequestSystem = owner->GetMoveComponent()->GetMoveRequestSystem();
		
		// カメラ取得
		if(owner->GetCameraManager())
		camera = owner->GetCameraManager()->GetCamera();

		// 設定が有効なコンボだけ、攻撃へ入る直前の通常移動速度を水平慣性として保持する
		moveInertiaVelocity_ = data_.inheritMoveInertia ? moveComponent->GetVelocity() : Vector3{};
		moveInertiaVelocity_.y = 0.0f;
		// ターゲット指定
		lockOnSystem->GetData() = data_.lockOnData;
		traget = lockOnSystem->SoftLockOn();
		targetWorldTransform = nullptr;
		if (traget) {
			targetWorldTransform = traget->GetConstWorldTransform();
			targetPos_ = traget->GetWorldPosition();
		}

		stickDirection_ = ctx.worldStickDirection;
		// 基準方向指定
		MoveTypeDirectionProcess();
		// 最終移動方向作成
		moveDirection_ = BuildMoveDirection();
		isMove_ = false;
		isTargetTeleported_ = false;
		stickDirection_ = {};
		// 座標更新
		owner->GetWorldTransform().Update();
	}

	// 更新
	void ComboMove::Update(const Character::CharacterContext& ctx, float timer) {
		// ゲームパッドの左スティックを動かしているか
		const bool isMoveStick = ctx.worldStickDirection.Length() != 0.0f;
		if (data_.isUpdateTargetPositionEachFrame && targetWorldTransform) {
			// 動くターゲットを追い続けられるように、現在のターゲット座標を更新する
			targetPos_ = targetWorldTransform->GetWorldPosition();
		}

		// 強制的に移動
		if (data_.isCompulsionMove) {
			isMove_ = true;
		}
		else {
			// 動かしていたら開始
			if (!isMove_ && isMoveStick) {
				isMove_ = true;
			}

			// 入力型は毎フレーム更新した方が自然
			if (isMove_) {
				stickDirection_ = ctx.worldStickDirection;
			}
		}

		// 方向を毎フレーム更新したい場合
		if (data_.isUpdateDirectionEachFrame) {
			MoveTypeDirectionProcess();
			moveDirection_ = BuildMoveDirection();
		}

		// 移動処理
		MoveTypeProcess(timer, ctx.dt);
		// コンボ固有の踏み込みとは別に、開始前の移動慣性を加算する
		RequestMoveInertia(ctx.dt);
	}

	// 終了
	void ComboMove::Exit(Character::BaseCharacter* owner) {
		isMove_ = false;
		isTargetTeleported_ = false;
		lockOnSystem->ClearTag();
		stickDirection_ = {};
		moveDirection_ = {};
		moveInertiaVelocity_ = {};
	}

	void ComboMove::RequestMoveInertia(float dt) {
		// 慣性を使用しないコンボ、または停止状態から始めたコンボでは要求を追加しない
		if (!data_.inheritMoveInertia || moveInertiaVelocity_.LengthSq() <= 0.000001f) {
			return;
		}

		MoveRequest request{};
		request.velocity = moveInertiaVelocity_ * dt;
		request.priority = 1;
		moveRequestSystem->SetRequest(request);
	}

	// 移動処理
	void ComboMove::MoveTypeProcess(float timer, float dt) {
		bool isStart = data_.moveWindow.startTime <= timer;		// 受付開始時間を過ぎたら
		bool isEnd = data_.moveWindow.endTime >= timer;			// 受付終了時間より前なら

		if (isMove_ && isStart && isEnd) {
			MoveRequest request{};
			bool canMove = true;

			switch (data_.moveType)
			{
			case MoveType::kInput:
				// 入力ベース。入力がない場合は動かない
				if (moveDirection_.Length() <= 0.0001f) {
					canMove = false;
				}
				break;

			case MoveType::kForward:
				break;

			case MoveType::kTraget:
				if (!traget) {
					// ターゲットなし上書きがない場合は、従来通りターゲット移動を発生させない
					if (!data_.noTargetMove.enabled) {
						canMove = false;
					}
					break;
				}
				canMove = ApplyTargetMove(request, dt);
				break;

			case MoveType::kLockAt:
				break;

			default:
				canMove = false;
				break;
			}

			if (canMove) {
				if (data_.moveType != MoveType::kTraget || !traget) {
					request.velocity = Multiply(moveDirection_, dt);
				}
				// 攻撃ごとの踏み込み感を作るため、時間進行に応じた速度倍率を掛ける
				request.velocity = request.velocity * CalculateMoveCurveScale(timer);
				if (!data_.isVerticalMove) {
					request.velocity.y = 0.0f;
				}
				request.priority = 0;
				if (data_.alignCharacterToMovement) {
					request.direction = moveDirection_.Normalize();
				}
				moveRequestSystem->SetRequest(request);
			}
		}
	}

	void ComboMove::MoveTypeDirectionProcess() {
		switch (data_.moveType)
		{
		case MoveType::kInput: // 入力方向
		{
			if (stickDirection_.Length() != 0.0f) {
				direction_ = Math::NormalizeSafe(Vector3{ stickDirection_.x, 0.0f, stickDirection_.y });
			}
			else {
				direction_ = {};
			}
			break;
		}

		case MoveType::kForward: // 所有者の向いている方向
		{
			direction_ = Math::NormalizeSafe(moveComponent->GetDirection());
			break;
		}

		case MoveType::kTraget: // ターゲット方向
		{
			if (traget) {
				direction_ = Subtract(targetPos_, worldTransform->translate_);

				if (data_.isFlattenTargetDirection) {
					direction_.y = 0.0f;
				}

				direction_ = Math::NormalizeSafe(direction_, Math::NormalizeSafe(moveComponent->GetDirection()));
			}
			else {
				if (stickDirection_.Length() != 0.0f) {
					direction_ = Math::NormalizeSafe(Vector3{ stickDirection_.x, 0.0f, stickDirection_.y });
				}
				else {
					direction_ = Math::NormalizeSafe(moveComponent->GetDirection());
				}
			}
			break;
		}

		case MoveType::kLockAt: // カメラ方向ベース
		{
			// 今のコードベースだと camera の forward 取得関数が見えないので
			// 既存挙動を維持して所有者前方を使う
			direction_ = Math::NormalizeSafe(moveComponent->GetDirection());
			break;
		}

		default:
		{
			direction_ = Math::NormalizeSafe(moveComponent->GetDirection());
			break;
		}
		}
	}

	Vector3 ComboMove::BuildMoveDirection() const {
		// 基準前方
		Vector3 baseForward = direction_;

		if (data_.moveType == MoveType::kInput && baseForward.Length() <= 0.0001f) {
			return {};
		}

		baseForward = Math::NormalizeSafe(baseForward, Vector3{ 0.0f,0.0f,1.0f });

		// ローカル入力
		Vector3 local = GetActiveLocalMoveVector();
		if (GetActiveNormalizeLocalMove() && local.Length() > 0.0001f) {
			local = local.Normalize();
		}

		// 基底
		const Vector3 up = Vector3{ 0.0f,1.0f,0.0f };
		const Vector3 right = Math::MakeRightFromForwardXZ(baseForward);

		// ★ここが重要（速度を掛ける）
		const Vector3 moveSpeed = GetActiveMoveSpeed();
		Vector3 result =
			(right * (local.x * moveSpeed.x)) +
			(up * (local.y * moveSpeed.y)) +
			(baseForward * (local.z * moveSpeed.z));

		return result;
	}

	Vector3 ComboMove::BuildTargetMoveGoal() const {
		Vector3 currentPos = worldTransform->translate_;
		Vector3 targetPos = targetPos_;

		Vector3 toTarget = targetPos - currentPos;
		if (data_.isFlattenTargetDirection) {
			toTarget.y = 0.0f;
		}

		Vector3 toTargetDir = Math::NormalizeSafe(toTarget, Math::NormalizeSafe(moveComponent->GetDirection()));
		return targetPos - toTargetDir * GetActiveMoveTargetRadius();
	}

	bool ComboMove::ApplyTargetMove(MoveRequest& request, float dt) {
		if (!traget) {
			return false;
		}


		const Vector3 currentPos = worldTransform->translate_;
		const Vector3 goalPos = BuildTargetMoveGoal();
		Vector3 toGoal = goalPos - currentPos;

		if (data_.isFlattenTargetDirection) {
			toGoal.y = 0.0f;
		}

		if (toGoal.Length() <= 0.0001f) {
			return false;
		}

		const TargetMoveType targetMoveType = GetActiveTargetMoveType();
		switch (targetMoveType)
		{
		case TargetMoveType::kNone:
			return false;

		case TargetMoveType::kMove:
		{
			Vector3 velocity = Multiply(moveDirection_, dt);
			if (velocity.LengthSq() >= toGoal.LengthSq()) {
				velocity = toGoal;
			}
			request.velocity = velocity;
			break;
		}

		case TargetMoveType::kTeleport:
			if (isTargetTeleported_) {
				return false;
			}
			worldTransform->translate_ = goalPos;
			worldTransform->Update();
			isTargetTeleported_ = true;
			request.velocity = {};
			break;

		case TargetMoveType::kInterpolation:
		{
			const Vector3 moveSpeed = GetActiveMoveSpeed();
			const float speed = (std::max)((std::max)(std::abs(moveSpeed.x), std::abs(moveSpeed.y)), std::abs(moveSpeed.z));
			const float t = std::clamp(speed * dt, 0.0f, 1.0f);
			request.velocity = Lerp(currentPos, goalPos, t) - currentPos;
			break;
		}

		default:
			return false;
		}

		if (!data_.isVerticalMove) {
			request.velocity.y = 0.0f;
		}
		return targetMoveType == TargetMoveType::kTeleport || request.velocity.Length() > 0.0001f;
	}

	float ComboMove::CalculateMoveCurveScale(float timer) const {
		// 移動時間が無い場合は従来通り等速で扱う
		const float duration = data_.moveWindow.endTime - data_.moveWindow.startTime;
		if (duration <= 0.0001f) {
			return 1.0f;
		}

		const float t = std::clamp((timer - data_.moveWindow.startTime) / duration, 0.0f, 1.0f);
		const float power = (std::max)(data_.speedCurvePower, 0.001f);
		switch (data_.speedCurveType)
		{
		case MoveSpeedCurveType::kEaseIn:
			// 出始めを抑えて、後半で伸びる踏み込みにする
			return std::pow(t, power);
		case MoveSpeedCurveType::kEaseOut:
			// 出始めを速くして、後半で止まりやすい踏み込みにする
			return 1.0f - std::pow(1.0f - t, power);
		case MoveSpeedCurveType::kEaseInOut:
		{
			// 前半は加速、後半は減速する山なりの速度にする
			const float phase = t < 0.5f ? t * 2.0f : (1.0f - t) * 2.0f;
			return std::pow((std::max)(phase, 0.0f), power);
		}
		case MoveSpeedCurveType::kConstant:
		default:
			return 1.0f;
		}
	}

	Vector3 ComboMove::GetActiveMoveSpeed() const {
		if (traget && data_.targetMove.enabled) {
			return data_.targetMove.moveSpeed;
		}
		if (!traget && data_.noTargetMove.enabled) {
			return data_.noTargetMove.moveSpeed;
		}
		return data_.moveSpeed;
	}

	Vector3 ComboMove::GetActiveLocalMoveVector() const {
		if (traget && data_.targetMove.enabled) {
			return data_.targetMove.localMoveVector;
		}
		if (!traget && data_.noTargetMove.enabled) {
			return data_.noTargetMove.localMoveVector;
		}
		return data_.localMoveVector;
	}

	bool ComboMove::GetActiveNormalizeLocalMove() const {
		if (traget && data_.targetMove.enabled) {
			return data_.targetMove.isNormalizeLocalMove;
		}
		if (!traget && data_.noTargetMove.enabled) {
			return data_.noTargetMove.isNormalizeLocalMove;
		}
		return data_.isNormalizeLocalMove;
	}

	TargetMoveType ComboMove::GetActiveTargetMoveType() const {
		if (traget && data_.targetMove.enabled) {
			return data_.targetMove.targetMoveType;
		}
		return data_.lockOnData.targetMoveType;
	}

	float ComboMove::GetActiveMoveTargetRadius() const {
		if (traget && data_.targetMove.enabled) {
			return data_.targetMove.moveTargetRadius;
		}
		return data_.lockOnData.moveTargetRadius;
	}

	const Engine::WorldTransform* ComboMove::GetTarget() {
		return targetWorldTransform;
	};

};

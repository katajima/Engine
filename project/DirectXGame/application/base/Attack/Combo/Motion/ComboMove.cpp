#include "ComboMove.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"
#include "DirectXGame/application/base/Camera/Base/CameraManeger.h"

namespace {

	Vector3 NormalizeSafe(const Vector3& v, const Vector3& fallback = Vector3{ 0.0f,0.0f,1.0f }) {
		if (v.Length() <= 0.0001f) {
			return fallback;
		}
		return v.Normalize();
	}

	// X=Right Y=Up Z=Forward
	// Up(0,1,0) と Forward から Right を作る
	Vector3 MakeRightFromForward(const Vector3& forward) {
		Vector3 f = forward;
		f.y = 0.0f;
		f = NormalizeSafe(f, Vector3{ 0.0f,0.0f,1.0f });

		// right = cross(up, forward)
		// up=(0,1,0), forward=(fx,fy,fz) -> (fz,0,-fx)
		return NormalizeSafe(Vector3{ f.z, 0.0f, -f.x }, Vector3{ 1.0f,0.0f,0.0f });
	}
}

namespace Combo {

	// 開始
	void ComboMove::Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
		isMove_ = false;
		stickDirection_ = {};
		moveComponent = owner->GetMoveComponent();
		worldTransform = &owner->GetObjectComponent()->GetWorldTransform();
		rigidBodyComponent = owner->GetObjectComponent()->GetRigidBodyComponent();
		lockOnSystem = owner->GetAttackController()->GeyLockOnSysutem();
		attackMoveSystem = owner->GetMoveComponent()->GetAttackMoveSystem();
		camera = owner->GetCameraManager()->GetCamera();
		// ターゲット指定
		lockOnSystem->GetData() = data_.lockOnData;
		traget = lockOnSystem->SoftLockOn();
		stickDirection_ = ctx.worldStickDirection;
		// 基準方向指定
		MoveTypeDirectionProcess();
		// 最終移動方向作成
		moveDirection_ = BuildMoveDirection();

		// 重力速度リセット
		if (data_.isResetGravity) {
			owner->GetObjectComponent()->GetRigidBodyComponent()->ResetAcceleration();
			owner->GetObjectComponent()->GetRigidBodyComponent()->ResetVelocity();
		}

		// 座標更新
		owner->GetWorldTransform().Update();
	}

	// 更新
	void ComboMove::Update(const Character::CharacterContext& ctx, float timer) {
		// ゲームパッドの左スティックを動かしているか
		const bool isMoveStick = ctx.worldStickDirection.Length() != 0.0f;

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

		// 重力処理
		GravityProcess();
	}

	// 終了
	void ComboMove::Exit(Character::BaseCharacter* owner) {
		isMove_ = false;
		lockOnSystem->ClearTag();
		stickDirection_ = {};
		moveDirection_ = {};
	}

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
				if (traget) {
					// 半径以内なら近づき停止
					if (data_.lockOnData.moveTargetRadius > targetPos_.Distance(worldTransform->translate_)) {
						canMove = false;
					}
				}
				break;

			case MoveType::kLockAt:
				break;

			default:
				canMove = false;
				break;
			}

			if (canMove) {
				request.velocity = Multiply(moveDirection_, dt);
				request.priority = 0;
				if (data_.alignCharacterToMovement) {
					request.direction = moveDirection_.Normalize();
				}
				attackMoveSystem->SetRequest(request);
			}
		}
	}

	void ComboMove::GravityProcess() {
		// 重力の設定
		if (!data_.isGravity) {
			rigidBodyComponent->Velocity().y = 0;
		}
		rigidBodyComponent->SetIsGravity(data_.isGravity);
	}

	void ComboMove::MoveTypeDirectionProcess() {
		switch (data_.moveType)
		{
		case MoveType::kInput: // 入力方向
		{
			if (stickDirection_.Length() != 0.0f) {
				direction_ = NormalizeSafe(Vector3{ stickDirection_.x, 0.0f, stickDirection_.y });
			}
			else {
				direction_ = {};
			}
			break;
		}

		case MoveType::kForward: // 所有者の向いている方向
		{
			direction_ = NormalizeSafe(moveComponent->GetDirection());
			break;
		}

		case MoveType::kTraget: // ターゲット方向
		{
			if (traget) {
				targetPos_ = traget->GetWorldPosition();
				direction_ = Subtract(targetPos_, worldTransform->translate_);

				if (data_.isFlattenTargetDirection) {
					direction_.y = 0.0f;
				}

				direction_ = NormalizeSafe(direction_, NormalizeSafe(moveComponent->GetDirection()));
			}
			else {
				if (stickDirection_.Length() != 0.0f) {
					direction_ = NormalizeSafe(Vector3{ stickDirection_.x, 0.0f, stickDirection_.y });
				}
				else {
					direction_ = NormalizeSafe(moveComponent->GetDirection());
				}
			}
			break;
		}

		case MoveType::kLockAt: // カメラ方向ベース
		{
			// 今のコードベースだと camera の forward 取得関数が見えないので
			// 既存挙動を維持して所有者前方を使う
			direction_ = NormalizeSafe(moveComponent->GetDirection());
			break;
		}

		default:
		{
			direction_ = NormalizeSafe(moveComponent->GetDirection());
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

		baseForward = NormalizeSafe(baseForward, Vector3{ 0.0f,0.0f,1.0f });

		// ローカル入力
		Vector3 local = data_.localMoveVector;
		if (data_.isNormalizeLocalMove && local.Length() > 0.0001f) {
			local = local.Normalize();
		}

		// 基底
		const Vector3 up = Vector3{ 0.0f,1.0f,0.0f };
		const Vector3 right = MakeRightFromForward(baseForward);

		// ★ここが重要（速度を掛ける）
		Vector3 result =
			(right * (local.x * data_.moveSpeed.x)) +
			(up * (local.y * data_.moveSpeed.y)) +
			(baseForward * (local.z * data_.moveSpeed.z));

		return result;
	}
};
#include "ComboMove.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"
#include "DirectXGame/application/base/Camera/Base/CameraManeger.h"

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
		// 方向指定
		MoveTypeDirectionProcess();
		// 座標更新
		owner->GetWorldTransform().Update();
	}

	// 更新
	void ComboMove::Update(const Character::CharacterContext& ctx, float timer) {
		// ゲームパッドの左スティックを動かしているか
		bool isMoveStick = ctx.worldStickDirection.Length() != 0;
		// 強制的に移動
		if (data_.isCompulsionMove) {
			isMove_ = true;
		}
		else {
			// 動かしていたら
			if (!isMove_ && isMoveStick) {
				isMove_ = true;
				stickDirection_ = ctx.worldStickDirection;
			}
		}
		// 移動処理
		MoveTypeProcess(timer, ctx.dt);
		// 重力処理
		GravityProcess();
	}

	// 終了
	void ComboMove::Exit(Character::BaseCharacter* owner) {
		isMove_ = false;
		// 
		lockOnSystem->ClearTag();
		//
		stickDirection_ = {};
	}

	void ComboMove::MoveTypeProcess(float timer, float dt) {
		bool isStart = data_.moveWindowStart <= timer;		// 受付開始時間を過ぎたら
		bool isEnd = data_.moveWindowEnd >= timer;			// 受付終了時間より前なら

		float t = timer / data_.moveWindowEnd - data_.moveWindowStart;

		if (isMove_ && isStart && isEnd) {
			MoveRequest request;
			switch (data_.moveType)
			{
			case MoveType::kNone:
				request.velocity = Multiply(Vector3{ stickDirection_.x,0,stickDirection_.y }, dt) * data_.speed;
				break;
			case MoveType::kForward:
				request.velocity = Multiply(direction_, dt) * data_.speed;
				break;
			case MoveType::kTraget:
				if (traget) {
					if (data_.moveTargetRadius <= targetPos_.Distance(worldTransform->translate_)) {
						request.velocity = Multiply(direction_, dt) * data_.speed;
					}
				}
				else {
					request.velocity = Multiply(direction_, dt) * data_.speed;
				}
				break;
			case MoveType::kLockAt: // カメラ方向
				request.velocity = Multiply(direction_, dt) * data_.speed;

				break;
			default:
				break;
			}

			request.priority = 0;
			request.direction = direction_;
			attackMoveSystem->SetRequest(request);
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
		case MoveType::kNone: // 特に無し
			direction_;
			break;
		case MoveType::kForward: // 所有者の向いている方向
			// 方向指定
			direction_ = moveComponent->GetDirection();
			break;
		case MoveType::kTraget: // ターゲット方向
			if (traget) {
				direction_ = Subtract(traget->GetWorldPosition(), worldTransform->translate_).Normalize();
				direction_.y = 0.0f;

				targetPos_ = traget->GetWorldPosition();
			}
			else {
				if (stickDirection_.Length() != 0.0f) {
					direction_ = Vector3{ stickDirection_.x,0,stickDirection_.y };
				}
				else {
					direction_ = moveComponent->GetDirection();
				}
			}
			break;
		case MoveType::kLockAt: // カメラ方向
			// 方向指定
			direction_ = moveComponent->GetDirection();
			break;
		default:
			break;
		}
	}
};
#include "ComboMove.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"
#include "DirectXGame/application/base/Camera/Base/CameraManeger.h"

namespace Combo {

	// 開始
	void ComboMove::Enter(Character::BaseCharacter* owner) {
		isMove_ = false;
		stickDirection_ = {};
		moveComponent = owner->GetMoveComponent();
		worldTransform = &owner->GetObjectComponent()->GetWorldTransform();
		rigidBodyComponent = owner->GetObjectComponent()->GetRigidBodyComponent();
		lockOnSystem = owner->GetAttackController()->GeyLockOnSysutem();
		attackMoveSystem = owner->GetMoveComponent()->GetAttackMoveSystem();
		camera = owner->GetCameraManager()->GetCamera();
		// ターゲット指定
		lockOnSystem->GetData() = data_.lockOnData_;
		traget = lockOnSystem->SoftLockOn();
		// 方向指定
		MoveTypeDirectionProcess();
		// 座標更新
		owner->GetWorldTransform().Update();
	}

	// 更新
	void ComboMove::Update(const InputSystem& inputSystem, float timer, float dt) {
		// スティック方向取得
		Vector2 stick = inputSystem.GetPlayerInputData().moveShick;
		// ゲームパッドの左スティックを動かしているか
		bool isMoveStick = stick.Length() != 0;

		Matrix4x4 cameraWorldMatrix = Inverse(camera->GetViewMatrix());

		// カメラの向きに基づいて移動方向をワールド座標系に変換
		Vector2 worldDirection = {
			stick.x * cameraWorldMatrix.m[0][0] + stick.y * cameraWorldMatrix.m[2][0],
			stick.x * cameraWorldMatrix.m[0][2] + stick.y * cameraWorldMatrix.m[2][2]
		};


		// 強制的に移動
		if (data_.isCompulsionMove_) {
			isMove_ = true;
		}
		else {
			// 動かしていたら
			if (!isMove_ && isMoveStick) {
				isMove_ = true;
				stickDirection_ = worldDirection;
			}
		}

		// 移動処理
		MoveTypeProcess(inputSystem, timer, dt);
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

	void ComboMove::MoveTypeProcess(const InputSystem& inputSystem, float timer, float dt) {
		bool isStart = data_.moveWindowStart_ <= timer;		// 受付開始時間を過ぎたら
		bool isEnd = data_.moveWindowEnd_ >= timer;			// 受付終了時間より前なら

		float t = timer / data_.moveWindowEnd_ - data_.moveWindowStart_;

		if (isMove_ && isStart && isEnd) {
			MoveRequest request;
			switch (data_.moveType)
			{
			case MoveType::kNone:
				request.velocity = Multiply(Vector3{ stickDirection_.x,0,stickDirection_.y }, dt) * data_.speed_;
				break;
			case MoveType::kForward:
				request.velocity = Multiply(direction_, dt) * data_.speed_;
				break;
			case MoveType::kTraget:

				if (traget) {
					if (data_.moveTargetRadius_ <= targetPos_.Distance(worldTransform->translate_)) {
						request.velocity = Multiply(direction_, dt) * data_.speed_;
					}
				}
				else {
					request.velocity = Multiply(direction_, dt) * data_.speed_;
				}
				break;
			case MoveType::kLockAt: // カメラ方向
				request.velocity = Multiply(direction_, dt) * data_.speed_;
				
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
		if (!data_.isGravity_) {
			rigidBodyComponent->Velocity().y = 0;
		}
		rigidBodyComponent->SetIsGravity(data_.isGravity_);
	}

	void ComboMove::MoveTypeDirectionProcess() {
		switch (data_.moveType)
		{
		case MoveType::kNone: // 特に無し
			//direction_
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
				direction_ = moveComponent->GetDirection();
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
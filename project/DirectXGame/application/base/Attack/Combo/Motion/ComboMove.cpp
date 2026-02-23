#include "ComboMove.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"

namespace Combo {

	// 開始
	void ComboMove::Enter(Character::BaseCharacter* owner) {
		isMove_ = false;
		stickDirection_ = {};
		moveComponent = owner->GetMoveComponent();
		worldTransform = &owner->GetObjectComponent()->GetWorldTransform();
		rigidBodyComponent = owner->GetObjectComponent()->GetRigidBodyComponent();
		lockOnSystem = owner->GetAttackController()->GeyLockOnSysutem();

		// ターゲット指定
		lockOnSystem->GetData() = data_.lockOnData_;
		traget = lockOnSystem->SoftLockOn();
		// 方向指定
		MoveTypeDirectionProcess();

		// 回転
		owner->GetMoveComponent()->GetMoveSystem()->AttackProcess(owner->GetWorldTransform(), direction_);
		// 座標更新
		owner->GetWorldTransform().Update();
	}

	// 更新
	void ComboMove::Update(const InputSystem& inputSystem, float timer, float dt) {
		// ゲームパッドの左スティックを動かしているか
		bool isMoveStick = inputSystem.GetData().moveShick.Length() != 0;

		// 強制的に移動
		if (data_.isCompulsionMove_) {
			isMove_ = true;
		}
		else {
			// 動かしていたら
			if (!isMove_ && isMoveStick) {
				isMove_ = true;
				stickDirection_ = inputSystem.GetData().moveShick;
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
			switch (data_.moveType)
			{
			case MoveType::kNone:
				worldTransform->translate_ += Multiply(Vector3{ stickDirection_.x,0,stickDirection_.y }, dt) * data_.speed_;
				break;
			case MoveType::kForward:
				worldTransform->translate_ += Multiply(direction_, dt) * data_.speed_;
				break;
			case MoveType::kTraget:

				if (traget) {
					if (data_.moveTargetRadius_ <= targetPos_.Distance(worldTransform->translate_)) {
						worldTransform->translate_ += Multiply(direction_, dt) * data_.speed_;
					}
				}
				else {
					worldTransform->translate_ += Multiply(direction_, dt) * data_.speed_;
				}
				break;
			case MoveType::kLockAt: // カメラ方向
				worldTransform->translate_ += Multiply(direction_, dt) * data_.speed_;
				break;
			default:
				break;
			}
		}
	}

	void ComboMove::GravityProcess() {
		// 重力の設定
		if (!data_.isGravity_) {
			rigidBodyComponent->Velocity().y = 0;
		}
		moveComponent->SetAttackingGravity(data_.gravityScale_);
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
				direction_ = Subtract(traget->GetWorldTransform().GetWorldPosition(), worldTransform->translate_).Normalize();
				direction_.y = 0.0f;

				targetPos_ = traget->GetWorldTransform().GetWorldPosition();
			}
			else {
				direction_ = moveComponent->GetDirection();
			}
			break;
		case MoveType::kLockAt: // カメラ方向
			// 方向指定
			moveComponent->GetMoveSystem()->CameraDirectionToMoveDirection(direction_);
			break;
		default:
			break;
		}
	}
};
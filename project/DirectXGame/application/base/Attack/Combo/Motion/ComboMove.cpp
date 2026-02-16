#include "ComboMove.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"

namespace Combo {

	// 開始
	void ComboMove::Enter(Character::BaseCharacter* owner) {
		isMove_ = false;
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
	void ComboMove::Update(const Engine::Input& input, float timer, float dt) {
		// ゲームパッドの左スティックを動かしているか
		bool isMoveStick = input.GetGamePadLeftStick().Length() != 0;

		// 移動可能か
		moveComponent->GetMoveSystem()->SetIsAttackCanMove(IsMove());

		// 強制的に移動
		if (data_.isCompulsionMove_) {
			isMove_ = true;
		}
		else {
			// 動かしていたら
			if (isMoveStick) {
				isMove_ = true;
			}
		}

		// 移動処理
		MoveTypeProcess(input, timer, dt);
		// 重力処理
		GravityProcess();

		// 移動可能なら
		if (isMove_) {
			// 前進する
			moveComponent->SetCanMove(true);				
		}
	}

	// 終了
	void ComboMove::Exit(Character::BaseCharacter* owner) {
		isMove_ = false;
	}

	void ComboMove::MoveTypeProcess(const Engine::Input& input, float timer, float dt) {
		bool isStart = data_.moveWindowStart_ <= timer;		// 受付開始時間を過ぎたら
		bool isEnd = data_.moveWindowEnd_ >= timer;			// 受付終了時間より前なら

		float t = timer / data_.moveWindowEnd_ - data_.moveWindowStart_;

		if (isMove_ && isStart && isEnd) {
			switch (data_.moveType)
			{
			case MoveType::kNone:
				break;
			case MoveType::kForward:
				worldTransform->translate_ += Multiply(direction_, dt) * data_.speed_;
				break;
			case MoveType::kTraget:
				if (traget) {
					if (data_.moveRadius_ <= targetPos_.Distance(worldTransform->translate_)) {
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
		rigidBodyComponent->SetGravityScale(data_.gravityScale_);
		rigidBodyComponent->SetIsGravity(data_.isGravity_);
	}
	void ComboMove::MoveTypeDirectionProcess() {
		switch (data_.moveType)
		{
		case MoveType::kNone: // 特に無し
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
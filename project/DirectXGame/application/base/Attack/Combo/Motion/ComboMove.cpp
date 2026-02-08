#include "ComboMove.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"

namespace Combo {

	// 開始
	void ComboMove::Enter(BaseCharacter* owner) {
		isMove_ = false;
		moveComponent = owner->GetMoveComponent();
		worldTransform = &owner->GetObjectComponent()->GetWorldTransform();
		rigidBodyComponent = owner->GetObjectComponent()->GetRigidBodyComponent();
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

		// 受付時間内なら
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




		


		//
		MoveTypeProcess(input, timer, dt);


		// 重力処理
		GravityProcess();

		// 移動可能なら
		if (isMove_) {
			moveComponent->SetCanMove(true);				// 前進する
		}
	}

	// 終了
	void ComboMove::Exit(BaseCharacter* owner) {
		isMove_ = false;
	}

	void ComboMove::MoveTypeProcess(const Engine::Input& input, float timer, float dt) {
		bool isStart = data_.moveWindowStart_ <= timer;		// 受付開始時間を過ぎたら
		bool isEnd = data_.moveWindowEnd_ >= timer;			// 受付終了時間より前なら

		switch (data_.moveType)
		{
		case MoveType::kNone:
			break;
		case MoveType::kForward:
			// 移動
			if (isMove_ && isStart && isEnd) {
				worldTransform->translate_ += Multiply(direction_, dt) * data_.speed_;
			}
			break;
		case MoveType::kTraget:
			break;
		default:
			break;
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
};
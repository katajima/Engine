#include "EnemyAttackSystem.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/engine/Math/MathFunctions.h>
#include <algorithm>

namespace Character {
	namespace {
		struct RingRange {
			float min;
			float max;
		};

		RingRange GetRingRange(EnemyAttackRing ring) {
			switch (ring) {
			case EnemyAttackRing::Attack:
				return { 7.0f, 10.0f };
			case EnemyAttackRing::Approach:
				return { 10.0f, 15.0f };
			case EnemyAttackRing::Standby:
				return { 15.0f, 20.0f };
			default:
				return { 7.0f, 10.0f };
			}
		}

		// 停止している敵だけが利用する、プレイヤー方向への滑らかな旋回処理
		void RotateTowardTarget(
			const Vector3& ownerPos,
			const Vector3& targetPos,
			float dt,
			float turnSpeed,
			float& rotateY
		) {
			Vector3 toTarget = Subtract(targetPos, ownerPos);
			toTarget.y = 0.0f;
			if (toTarget.Length() <= 0.001f) {
				return;
			}

			Vector3 rotate = Math::DirectionToRotate(toTarget.Normalize(), Dire::Z);
			float diff = rotate.y - rotateY;

			while (diff > std::numbers::pi_v<float>) {
				diff -= std::numbers::pi_v<float> * 2.0f;
			}
			while (diff < -std::numbers::pi_v<float>) {
				diff += std::numbers::pi_v<float> * 2.0f;
			}

			float maxTurn = turnSpeed * dt;
			rotateY += std::clamp(diff, -maxTurn, maxTurn);
		}

	}

	void EnemyAttackSystem::Initialize(CharacterStateMachine* characterStateMachine) {
		this->characterStateMachine = characterStateMachine;

		// 攻撃要求の初期化
		attackRequest_ = std::make_unique<EnemyAttackRequest>();	
	}

	void EnemyAttackSystem::Update(
		float dt,
		const Vector3& ownerPos,
		const Vector3& targetPos,
		const Vector3& slotPos,
		float& speed,
		float& rotateY,
		const EnemyGlobalData& data,
		float moveSpeed
	) {
		Vector3 toMoveSlot = Subtract(slotPos, ownerPos);
		toMoveSlot.y = 0.0f;
		float slotDistance = toMoveSlot.Length();

		Vector3 toTarget = Subtract(targetPos, ownerPos);
		toTarget.y = 0.0f;
		float targetDistance = toTarget.Length();

		attackRequest_->Activate();

		EnemyAttackRing currentRing = attackRequest_->GetRing();
		if (currentRing == EnemyAttackRing::None || currentRing == EnemyAttackRing::TooClose) {
			currentRing = EnemyAttackRing::Approach;
			attackRequest_->SetRing(currentRing);
		}

		RingRange ringRange = GetRingRange(currentRing);
		const float ringMargin = 0.25f;

		if (targetDistance < ringRange.min - ringMargin) {
			timer_ = 0.0f;
			attackRequest_->SetRing(EnemyAttackRing::TooClose);
			// 群衆AIが後退側の移動目標を作るため、速度は正方向で進ませる
			// 負の速度にすると後退目標と二重反転し、プレイヤー付近で振動する
			speed = std::max(data.retreatSpeed, moveSpeed * 0.5f);
			return;
		}

		if (targetDistance > ringRange.max + ringMargin) {
			timer_ = 0.0f;
			speed = moveSpeed;
			return;
		}

		const float slotArrivalRange = 3.0f;
		if (slotDistance > slotArrivalRange) {
			timer_ = 0.0f;
			speed = moveSpeed;
			return;
		}

		// スロットへ到着して停止した後だけ、攻撃対象へ向きを合わせる。
		// 移動中までここで回すと、移動方向を向く処理と競合して回転が震える。
		RotateTowardTarget(ownerPos, targetPos, dt, data.turnSpeed, rotateY);

		if (currentRing != EnemyAttackRing::Attack) {
			timer_ = 0.0f;
			speed = 0.0f;
			return;
		}

		speed = 0.0f;

		if (attackRequest_->IsCooldown()) {
			timer_ = 0.0f;
			return;
		}

		timer_ += dt;

		//if (timer_ >= data.attackTimer) {
			attackRequest_->Activate();

			if (attackRequest_->GetIsGranted()) {
				characterStateMachine->ChangeState(CharacterMainState::Attack);

				attackRequest_->StartCooldown(3.0f);

				attackRequest_->Deactivate();
				timer_ = 0.0f;
				return;
			}
		//}
	}

	EnemyAttackRequest* EnemyAttackSystem::GetAttackRequest() const {
		return attackRequest_.get();
	}


}

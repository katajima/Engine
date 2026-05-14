#include "EnemyAttackSystem.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/engine/Math/MathFunctions.h>

namespace Character {

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
		Vector3 toTarget = Subtract(targetPos, ownerPos);
		toTarget.y = 0.0f;

		if (toTarget.Length() > 0.001f) {
			Vector3 lookDir = toTarget.Normalize();
			Vector3 rotate = Math::DirectionToRotate(lookDir, Dire::Z);

			float currentY = rotateY;
			float targetY = rotate.y;
			float diff = targetY - currentY;

			while (diff > std::numbers::pi_v<float>) {
				diff -= std::numbers::pi_v<float> *2.0f;
			}
			while (diff < -std::numbers::pi_v<float>) {
				diff += std::numbers::pi_v<float> *2.0f;
			}

			float turnSpeed = data.turnSpeed * dt;
			diff = std::clamp(diff, -turnSpeed, turnSpeed);

			rotateY = currentY + diff;
		}


		Vector3 toSlot = Subtract(slotPos, ownerPos);
		toSlot.y = 0.0f;

		float slotDistance = toSlot.Length();

		const float slotArrivalRange = 10.0f;

		if (slotDistance > slotArrivalRange) {
			if (slotDistance > 0.001f) {
				Vector3 moveDir = toSlot.Normalize();
				Vector3 rotate = Math::DirectionToRotate(moveDir, Dire::Z);

				rotateY = rotate.y;
			}

			timer_ = 0.0f;
			attackRequest_->Deactivate();
			attackRequest_->SetRing(EnemyAttackRing::Approach);

			speed = moveSpeed;
			return;
		}

		toTarget = Subtract(targetPos, ownerPos);
		toTarget.y = 0.0f;

		if (toTarget.Length() > 0.001f) {
			Vector3 lookDir = toTarget.Normalize();
			Vector3 rotate = Math::DirectionToRotate(lookDir, Dire::Z);
			rotateY = rotate.y;
		}

		// スロット到着後、攻撃リング内として扱う
		speed = 0.0f;
		attackRequest_->SetRing(EnemyAttackRing::Attack);

		if (attackRequest_->IsCooldown()) {
			timer_ = 0.0f;
			return;
		}

		timer_ += dt;

		if (timer_ >= data.attackTimer) {
			attackRequest_->Activate();

			if (attackRequest_->GetIsGranted()) {
				characterStateMachine->ChangeState(CharacterMainState::Attack);

				attackRequest_->StartCooldown(3.0f);

				attackRequest_->Deactivate();
				timer_ = 0.0f;
				return;
			}
		}
	}

	EnemyAttackRequest* EnemyAttackSystem::GetAttackRequest() const {
		return attackRequest_.get();
	}


}

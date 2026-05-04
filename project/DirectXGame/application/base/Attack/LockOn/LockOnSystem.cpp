#include "LockOnSystem.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Camera/Base/CameraManeger.h"

void LockOnSystem::Initialize(Character::BaseCharacter* owner) {
	this->owner = owner;
	cameraManager = owner->GetCameraManager();
}

const Character::BaseCharacter* LockOnSystem::SoftLockOn() const {
	if (targetCharacters.empty()) {
		return nullptr;
	}

	switch (data_.type) {
	case LockOnType::kHit: // 以前ヒットした相手にロックオン       
		return GetHitLockOn();
		break;
	case LockOnType::kNear: // 近くの敵
		return GetNearLockOn();
		break;
	default:
		break;
	}
	return nullptr;
}

const Character::BaseCharacter* LockOnSystem::GetTarget() const {
	if (!targetCharacters.empty()) {
		return targetCharacters.front();
	}
	else {
		return nullptr;
	}
};


Vector3 LockOnSystem::GetOwnerPos() const {
	return owner->GetWorldPosition();
}


const Character::BaseCharacter* LockOnSystem::GetNearLockOn() const {
	if (targetCharacters.empty())
	{
		return nullptr;
	}

	Vector3 ownerPos = GetOwnerPos();

	// カメラ前 or 入力方向
	Vector3 aimDir = cameraManager->GetCamera()->GetForward();
	aimDir.y = 0.0f;

	if (aimDir.LengthSq() < 1e-6f)
	{
		return nullptr;
	}
	aimDir = aimDir.Normalize();

	auto it = std::min_element(
		targetCharacters.begin(),
		targetCharacters.end(),
		[&](const Character::BaseCharacter* a, const Character::BaseCharacter* b)
		{
			if (!a || !b) return false;

			bool is = a->GetAlive() && a->GetCurrentMainState() != Character::CharacterMainState::Die;
			bool is2 = b->GetAlive() && b->GetCurrentMainState() != Character::CharacterMainState::Die;
			if (is && is2) {
				float scoreA = CalcSoftLockScore(ownerPos, aimDir, a);
				float scoreB = CalcSoftLockScore(ownerPos, aimDir, b);

				return scoreA < scoreB;
			}
			else {
				return false;
			}
		}
	);

	if (it == targetCharacters.end())
	{
		return nullptr;
	}

	const Character::BaseCharacter* bestTarget = *it;
	if (bestTarget && data_.radius >= bestTarget->GetWorldPosition().Distance(ownerPos)) {
		return bestTarget;
	}

	return nullptr;
}

const Character::BaseCharacter* LockOnSystem::GetHitLockOn()const {
	if (targetCharacters.empty())
	{
		return nullptr;
	}
	Vector3 ownerPos = GetOwnerPos();
	for (auto& target : targetCharacters) {
		if (target->GetTagNumber() == hitTag && target->GetAlive() &&
			target->GetCurrentMainState() != Character::CharacterMainState::Die) {
			if (data_.radius >= target->GetWorldPosition().Distance(ownerPos)) {
				return target;
			}
		}
	}

	// もし最後に当てた相手がいない場合は近くの敵にロックオンする
	return GetNearLockOn();
}

float LockOnSystem::CalcSoftLockScore(
	const Vector3& playerPos,
	const Vector3& aimDir,
	const Character::BaseCharacter* enemy)const
{
	Vector3 toEnemy = enemy->GetWorldPosition() - playerPos;
	toEnemy.y = 0.0f;

	float distSq = toEnemy.LengthSq();
	if (distSq < 1e-6f) return FLT_MAX;

	Vector3 dir = toEnemy / std::sqrt(distSq);

	// 角度（dotが1に近いほど良い）
	float dot = std::clamp(aimDir.Dot(dir), -1.0f, 1.0f);
	float angle = std::acos(dot); // rad

	// スコア（小さいほど良い）
	constexpr float kAngleWeight = 2.0f;
	constexpr float kDistWeight = 0.15f;

	return angle * kAngleWeight + std::sqrt(distSq) * kDistWeight;
}
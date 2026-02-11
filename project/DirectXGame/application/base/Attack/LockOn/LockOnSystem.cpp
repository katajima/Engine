#include "LockOnSystem.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Camera/Base/CameraManeger.h"

void LockOnSystem::Initialize(Character::BaseCharacter* owner) {
	this->owner = owner;
    cameraManager = owner->GetCameraManager();
}

Character::BaseCharacter* LockOnSystem::SoftLockOn(){
	if (targetCharacters.empty())
	{
		return nullptr;
	}
	
    switch (data_.type)
    {
    case LockOnType::kHit:

        break;
    case LockOnType::kNear:
        return GetNearLockOn();
        break;
    default:
        break;
    }
    return nullptr;
}

Vector3 LockOnSystem::GetOwnerPos() {
	return owner->GetWorldTransform().GetWorldPosition();
}


Character::BaseCharacter* LockOnSystem::GetNearLockOn()
{
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
    aimDir.Normalize();

    std::sort(
        targetCharacters.begin(),
        targetCharacters.end(),
        [&](Character::BaseCharacter* a, Character::BaseCharacter* b)
        {
            float scoreA = CalcSoftLockScore(ownerPos, aimDir, a);
            float scoreB = CalcSoftLockScore(ownerPos, aimDir, b);
            return scoreA < scoreB;
        }
    );

    if (data_.radius >= targetCharacters.front()->GetWorldTransform().GetWorldPosition().Distance(ownerPos)) {
        //// 先頭が最適なロックオン相手
        return targetCharacters.front();
    }
    else {
        return nullptr;
    }
}

float LockOnSystem::CalcSoftLockScore(
    const Vector3& playerPos,
    const Vector3& aimDir,
    Character::BaseCharacter* enemy)
{
    Vector3 toEnemy = enemy->GetWorldTransform().GetWorldPosition() - playerPos;
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
#include "LockOnSystem.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Camera/Base/CameraManeger.h"

void LockOnSystem::Initialize(BaseCharacter* owner) {
	this->owner = owner;
    cameraManager = owner->GetCameraManager();
}

void LockOnSystem::Update(float timer, float dt) {


}

BaseCharacter* LockOnSystem::SoftLockOn(){
	if (targetCharacters.empty())
	{
		return nullptr;
	}
	Vector3 ownerPos = GetOwnerPos();


   
    //// カメラ前 or 入力方向
    //Vector3 aimDir =    camera_->GetForward();
    //aimDir.y = 0.0f;

    //if (aimDir.LengthSq() < 1e-6f)
    //{
    //    return nullptr;
    //}
    //aimDir.Normalize();

    //std::sort(
    //    targetCharacters.begin(),
    //    targetCharacters.end(),
    //    [&](BaseCharacter* a, BaseCharacter* b)
    //    {
    //        float scoreA = CalcSoftLockScore(playerPos, aimDir, a);
    //        float scoreB = CalcSoftLockScore(playerPos, aimDir, b);
    //        return scoreA < scoreB;
    //    }
    //);

    //// 先頭が最適なロックオン相手
    return targetCharacters.front();
}

Vector3 LockOnSystem::GetOwnerPos() {
	return owner->GetWorldTransform().GetWorldPosition();
}


float LockOnSystem::CalcSoftLockScore(
    const Vector3& playerPos,
    const Vector3& aimDir,
    BaseCharacter* enemy)
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
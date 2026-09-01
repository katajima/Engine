#include "LockOnSystem.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/engine/Camera/CameraManager.h"
#include <algorithm>
#include <limits>

void LockOnSystem::Initialize(Character::BaseCharacter* owner) {
	// ロックオンを行う攻撃者と、その視線基準になるカメラ管理を保持する
	this->owner = owner;
	cameraManager = owner->GetCameraManager();
}

const Character::BaseCharacter* LockOnSystem::UpdateLockOn(bool isLockOnRequest) {
	if (!isLockOnRequest) {
		// 入力が無い時は保持中のターゲットを解除する
		isLockOn_ = false;
		currentTarget_ = nullptr;
		return nullptr;
	}

	if (!IsValidTarget(currentTarget_, data_.lockOnRadius)) {
		// 現在の対象が無効なら、カメラ前方に近い相手を探し直す
		currentTarget_ = GetNearLockOn(data_.lockOnRadius);
	}

	isLockOn_ = currentTarget_ != nullptr;
	return currentTarget_;
}

const Character::BaseCharacter* LockOnSystem::SoftLockOn() const {
	if (targetCharacters.empty()) {
		// 候補が無い場合はロックオンしない
		return nullptr;
	}

	// 設定されたロックオン方式に応じてターゲットを選ぶ
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
		// 明示ターゲットがある場合は先頭を現在ターゲットとして扱う
		return targetCharacters.front();
	}
	else {
		return nullptr;
	}
};


Vector3 LockOnSystem::GetOwnerPos() const {
	// スコア計算で何度も使う所有者座標を取得する
	return owner->GetWorldPosition();
}

bool LockOnSystem::IsValidTarget(const Character::BaseCharacter* target, float radius) const {
	if (!owner || !target) {
		// 所有者か対象が無ければロックオンを維持できない
		return false;
	}

	if (!target->GetAlive() || target->GetDelete() ||
		target->GetCurrentMainState() == Character::CharacterMainState::Die) {
		// 死亡・削除済みの相手はロックオン対象から外す
		return false;
	}

	if (radius < target->GetWorldPosition().Distance(GetOwnerPos())) {
		// 設定半径の外へ出た相手は解除する
		return false;
	}

	// 現在の候補リストに残っている相手だけを有効にする
	return std::find(targetCharacters.begin(), targetCharacters.end(), target) != targetCharacters.end();
}

const Character::BaseCharacter* LockOnSystem::GetNearLockOn() const {
	return GetNearLockOn(data_.softLockRadius);
}

const Character::BaseCharacter* LockOnSystem::GetNearLockOn(float radius) const {
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
		// カメラ方向が無効な時は安全にロックオンしない
		return nullptr;
	}
	aimDir = aimDir.Normalize();

	const Character::BaseCharacter* bestTarget = nullptr;
	float bestScore = (std::numeric_limits<float>::max)();

	for (const Character::BaseCharacter* target : targetCharacters) {
		if (!IsValidTarget(target, radius)) {
			// 無効な候補はスコア計算に含めない
			continue;
		}

		const float score = CalcSoftLockScore(ownerPos, aimDir, target);
		if (score < bestScore) {
			// 角度と距離のスコアが一番小さい相手を採用する
			bestScore = score;
			bestTarget = target;
		}
	}

	return bestTarget;
}

const Character::BaseCharacter* LockOnSystem::GetHitLockOn()const {
	if (targetCharacters.empty())
	{
		// 候補が無い場合はヒット履歴ロックオンも成立しない
		return nullptr;
	}
	Vector3 ownerPos = GetOwnerPos();
	for (auto& target : targetCharacters) {
		// 最後にヒットしたタグ番号と一致し、現在も有効な敵を優先する
		if (target->GetTagNumber() == hitTag && target->GetAlive() && !target->GetDelete() &&
			target->GetCurrentMainState() != Character::CharacterMainState::Die) {
			if (data_.softLockRadius >= target->GetWorldPosition().Distance(ownerPos)) {
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
	// プレイヤーから敵への水平ベクトルを作る
	Vector3 toEnemy = enemy->GetWorldPosition() - playerPos;
	toEnemy.y = 0.0f;

	float distSq = toEnemy.LengthSq();
	if (distSq < 1e-6f) return (std::numeric_limits<float>::max)();

	// 方向ベクトルに正規化して、カメラ前方との角度を計算する
	Vector3 dir = toEnemy / std::sqrt(distSq);

	// 角度（dotが1に近いほど良い）
	float dot = std::clamp(aimDir.Dot(dir), -1.0f, 1.0f);
	float angle = std::acos(dot); // rad

	// スコア（小さいほど良い）
	constexpr float kAngleWeight = 2.0f;
	constexpr float kDistWeight = 0.15f;

	// 角度を強め、距離を弱めに足して「向いている近い敵」を優先する
	return angle * kAngleWeight + std::sqrt(distSq) * kDistWeight;
}

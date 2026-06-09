#include "LockOnSystem.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Camera/Base/CameraManeger.h"

void LockOnSystem::Initialize(Character::BaseCharacter* owner) {
	// ロックオンを行う攻撃者と、その視線基準になるカメラ管理を保持する
	this->owner = owner;
	cameraManager = owner->GetCameraManager();
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
		// カメラ方向が無効な時は安全にロックオンしない
		return nullptr;
	}
	aimDir = aimDir.Normalize();

	// 角度と距離のスコアが最も小さい有効な敵を探す
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
		// 半径内かつ生存中の敵だけをロックオン対象として返す
		if (bestTarget->GetAlive() && !bestTarget->GetDelete())
			return bestTarget;
	}
	return nullptr;
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
	// プレイヤーから敵への水平ベクトルを作る
	Vector3 toEnemy = enemy->GetWorldPosition() - playerPos;
	toEnemy.y = 0.0f;

	float distSq = toEnemy.LengthSq();
	if (distSq < 1e-6f) return FLT_MAX;

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

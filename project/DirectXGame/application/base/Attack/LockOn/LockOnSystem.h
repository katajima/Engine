#pragma once
#include "vector"
#include "LockOnData.h"
#include <DirectXGame/engine/struct/Vector3.h>


// 前方宣言
namespace Character {
	class BaseCharacter;
	class BaseEnemy;
}
class CameraManager;

/// <summary>
/// ロックオンシステム
/// </summary>
class LockOnSystem {
public:
	// 初期化
	void Initialize(Character::BaseCharacter* owner);

	// 入力状態に合わせてロックオン対象を更新
	const Character::BaseCharacter* UpdateLockOn(bool isLockOnRequest);

	// ロックオン相手を選別
	const Character::BaseCharacter* SoftLockOn() const;
	// ターゲット取得
	const Character::BaseCharacter* GetTarget() const;
	// 現在ロックオン中のターゲット取得
	const Character::BaseCharacter* GetLockOnTarget() const { return currentTarget_; }
	// ロックオン中か取得
	bool IsLockOn() const { return isLockOn_; }
public:

	// ターゲット指定
	void SetTargets(const std::vector<const Character::BaseCharacter*>& targetCharacters) { this->targetCharacters = targetCharacters; };

	// ロックオンデータ取得
	LockOnData& GetData() { return data_; }

	// 所有者の位置取得
	Vector3 GetOwnerPos() const;

	// 当てた相手のタグを設定
	void SetHitTag(uint32_t tag) { hitTag = tag; }

	// ロックオンタグクリア
	void ClearTag() { hitTag = -1; }

private:
	// ロックオン対象として有効か確認
	bool IsValidTarget(const Character::BaseCharacter* target) const;
	// 範囲内の近くの相手を取得
	const Character::BaseCharacter* GetNearLockOn() const;
	// 最後に当てた相手を取得
	const Character::BaseCharacter* GetHitLockOn() const;

	float CalcSoftLockScore(
		const Vector3& playerPos,
		const Vector3& aimDir,
		const Character::BaseCharacter* enemy) const;
private:
	// 所有者
	Character::BaseCharacter* owner = nullptr;
	// カメラマネージャー
	CameraManager* cameraManager = nullptr;
	// 攻撃対象キャラクターリスト
	std::vector<const Character::BaseCharacter*> targetCharacters;
private:
	// ロックオンデータ
	LockOnData data_;
	// 現在ロックオン中か
	bool isLockOn_ = false;
	// 現在ロックオンしている相手
	const Character::BaseCharacter* currentTarget_ = nullptr;
	// 当てた相手
	uint32_t hitTag = -1;
};

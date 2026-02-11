#pragma once
#include "vector"
#include "LockOnData.h"


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

	// ロックオン相手を選別
	Character::BaseCharacter* SoftLockOn();
public:

	// ターゲット指定
	void SetTargets(const std::vector<Character::BaseCharacter*>& targetCharacters) { this->targetCharacters = targetCharacters; };

	// ロックオンデータ取得
	LockOnData& GetData() { return data_; }

	// 所有者の位置取得
	Vector3 GetOwnerPos();
private:

	Character::BaseCharacter* GetNearLockOn();

	float CalcSoftLockScore(
		const Vector3& playerPos,
		const Vector3& aimDir,
		Character::BaseCharacter* enemy);
private:
	// 所有者
	Character::BaseCharacter* owner = nullptr;
	// カメラマネージャー
	CameraManager* cameraManager = nullptr;
	// 攻撃対象キャラクターリスト
	std::vector<Character::BaseCharacter*> targetCharacters;
private:
	// ロックオンデータ
	LockOnData data_;
	//
	bool isLockOn_ = false;
};

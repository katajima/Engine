#pragma once
#include "vector"
#include "LockOnData.h"


// 前方宣言
class BaseCharacter;
class BaseEnemy;
class CameraManager;

/// <summary>
/// ロックオンシステム
/// </summary>
class LockOnSystem {
public:



	// 初期化
	void Initialize(BaseCharacter* owner);

	// 更新
	void Update(float timer, float dt);

	// ロックオン相手を選別
	BaseCharacter* SoftLockOn();
public:

	// ターゲット指定
	void SetTargets(const std::vector<BaseCharacter*>& targetCharacters) { this->targetCharacters = targetCharacters; };

	// ロックオンデータ取得
	LockOnData& GetData() { return data_; }

	// 所有者の位置取得
	Vector3 GetOwnerPos();


	float CalcSoftLockScore(
		const Vector3& playerPos,
		const Vector3& aimDir,
		BaseCharacter* enemy);
private:
	// 所有者
	BaseCharacter* owner = nullptr;
	// カメラマネージャー
	CameraManager* cameraManager = nullptr;
	// 攻撃対象キャラクターリスト
	std::vector<BaseCharacter*> targetCharacters;
private:
	// ロックオンデータ
	LockOnData data_;
	//
	bool isLockOn_ = false;
};

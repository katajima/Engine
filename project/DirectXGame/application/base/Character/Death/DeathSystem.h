#pragma once
#include "DeathData.h"
// 前方宣言

namespace Character {
	class BaseCharacter;
}
class EffectSystem;
class ObjectComponent;

// 死亡システム
/// <summary>
/// DeathSystemを管理・実装するクラス。
/// </summary>
class DeathSystem {
public:

	// 所有者設定
	void SetOwner(Character::BaseCharacter* owner) { this->owner = owner; }

	// 更新
	void Update(float dt);

	// 死亡開始
	void StartDeath(DeathType type,const DeathData& deathData);

	// 死亡タイプ処理
	void TypeProcess();

	// 死亡中か
	bool IsActive() const { return isActive; }

	// 死亡データ取得
	DeathData GetData() const { return data; }
private:
	// 死亡タイプ
	DeathType type = DeathType::Normal;
	// 死亡データ
	DeathData data;
	// 開始フラグ
	bool isActive = false;
	// 死亡タイマー
	float dieTimer = 0.0f;
private:
	Character::BaseCharacter* owner = nullptr;
};

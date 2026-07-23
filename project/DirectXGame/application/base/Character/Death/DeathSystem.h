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

	/// <summary>
	/// 所有者設定
	/// </summary>
	void SetOwner(Character::BaseCharacter* owner) { this->owner = owner; }

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt);

	/// <summary>
	/// 死亡開始
	/// </summary>
	void StartDeath(DeathType type,const DeathData& deathData);

	/// <summary>
	/// 死亡タイプ処理
	/// </summary>
	void TypeProcess();

	/// <summary>
	/// 死亡中か
	/// </summary>
	bool IsActive() const { return isActive; }

	/// <summary>
	/// 死亡データ取得
	/// </summary>
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

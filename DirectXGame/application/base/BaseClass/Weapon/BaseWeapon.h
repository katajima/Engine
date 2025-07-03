#pragma once
#include "WeaponData.h"

// 武器のベースクラス
class BaseWeapon : public BaseObject
{
public:
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;

	virtual void Update() = 0;

	virtual void DrawEffect() = 0;

	virtual void Draw2D() = 0;

public:
	virtual void SetCharacter(BaseCharacter* character) = 0;

	// タグによるコライダーの有効・無効を設定
	void SetIsCollider(CollisionTag tag ,bool is) { objectBase_->GetColliderComponent()->SetEnableByTag(tag, is); };

	// ダメージ取得
	float GetDamage() const { return data_.damage; };
	// 攻撃中かどうかのフラグを取得
	bool IsActive() const { return data_.isActive; };
	// オートマチックかどうかのフラグを取得
	bool IsAutomatic() const { return data_.isAutomatic; };
	// 攻撃中かどうかのフラグを設定
	void SetIsActive(bool isActive) { data_.isActive = isActive; };
	// 現在時間を加算
	void AddCurrentTime(float deltaTime) { data_.currentTime += deltaTime; };
	// 現在経過時間をリセット
	void ResetCurrentTime() { data_.currentTime = 0.0f; };
	// 現在経過時間を取得
	float GetCurrentTimer() const { return data_.currentTime; };
	// 全体時間を取得
	float GetAllTime() const { return data_.startupTime + data_.attackAnimationTime + data_.recoveryTime; };
	// 攻撃のスタートアップ時間を取得
	float GetStartupTime() const { return data_.startupTime; };
	// 攻撃のアニメーション時間を取得
	float GetAttackAnimationTime() const { return data_.attackAnimationTime; };
	// 攻撃のリカバリー時間を取得
	float GetRecoveryTime() const { return data_.recoveryTime; };
	// コンボ武器かどうかのフラグを取得
	bool IsCommonWeapon() const { return data_.isComboWeapon; }
	// コンボ武器かどうかのフラグを設定
	void SetIsComboWeapon(bool isComboWeapon) { data_.isComboWeapon = isComboWeapon; }
	// コンボの最大回数を取得
	int GetComboMaxCount() const { return data_.comboMaxCount; }
	// コンボの最大回数を設定
	void SetComboMaxCount(int comboMaxCount) { data_.comboMaxCount = comboMaxCount; }
	// コンボの現在の回数を取得
	int GetCurrentComboCount() const { return data_.currentComboCount; }
	// コンボの現在の回数を設定
	void SetCurrentComboCount(int currentComboCount) { data_.currentComboCount = currentComboCount; }
	// コンボの現在の回数をリセット
	void ResetCurrentComboCount() { data_.currentComboCount = 0; }
	// コンボの現在の回数をインクリメント
	void IncrementCurrentComboCount() { data_.currentComboCount++; }
	// コンボを次にするかどうかのフラグを取得
	bool IsComboNext() const { return data_.isComboNext; }
	// コンボを次にするかどうかのフラグを設定
	void SetIsComboNext(bool isComboNext) { data_.isComboNext = isComboNext; }
	//
	Timer& GetTimer() { return data_.animetionTimer; }
	// 移動速度倍率を取得 
	float GetMovementSpeedMultiplier() const { return data_.movementSpeedMultiplier; }
	// 移動速度倍率を設定
	void SetMovementSpeedMultiplier(float multiplier) { data_.movementSpeedMultiplier = multiplier; }
	
	// ヒットデータを取得
	AttackHitData& GetHitData() { return hitData_; }
public:

	// 時間を設定
	void SetTime(float startupTime, float attackAnimationTime, float recoveryTime)
	{
		data_.startupTime = startupTime;
		data_.attackAnimationTime = attackAnimationTime;
		data_.recoveryTime = recoveryTime;
	}


protected:
	WeaponData data_; // 武器データ
	AttackHitData hitData_; // 攻撃ヒットデータ
};

// 近距離の武器クラス
class MellWeapon : public BaseWeapon
{
public:
	/// 初期化
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;
	/// 更新
	virtual void Update() = 0;
	/// エフェクトの描画
	virtual void DrawEffect() = 0;
	/// 2d描画
	virtual void Draw2D() = 0;
	// 使っているキャラクター設定
	virtual void SetCharacter(BaseCharacter* character) = 0;

public:
	// ヒットストップ時間を取得
	float GetHitStopTime() const { return mellData_.hitStopTime; } 
protected:
	MellWeaponData mellData_; // 近距離武器データ

};

// 遠距離の武器クラス
class RangedWeapon : public BaseWeapon
{
public:
	/// 初期化
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;
	/// 更新
	virtual void Update() = 0;
	/// エフェクトの描画
	virtual void DrawEffect() = 0;
	/// 2d描画
	virtual void Draw2D() = 0;
	// 使っているキャラクター設定
	virtual void SetCharacter(BaseCharacter* character) = 0;
protected:
	RangedWeaponData rengedData_; // 遠距離武器データ

};
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

	// 全体時間を取得
	float GetAllTime() const { return data_.startupTime + data_.attackAnimationTime + data_.recoveryTime; };

	// 攻撃のスタートアップ時間を取得
	float GetStartupTime() const { return data_.startupTime; };

	// 攻撃のアニメーション時間を取得
	float GetAttackAnimationTime() const { return data_.attackAnimationTime; };

	// 攻撃のリカバリー時間を取得
	float GetRecoveryTime() const { return data_.recoveryTime; };
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
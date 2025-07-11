#pragma once
#include "WeaponData.h"
//#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayer.h"
//#include "DirectXGame/application/base/Character/Player/BasePlayerState.h"

// 武器のベースクラス
class BaseWeapon : public BaseObject
{
public:
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;

	virtual void Update() = 0;

	virtual void DrawEffect() = 0;

	virtual void Draw2D() = 0;

public:
	// 使っているキャラクター設定
	void SetCharacter(BaseCharacter* character) { this->character = character; };
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
	// 移動速度倍率を取得 
	float GetMovementSpeedMultiplier() const { return data_.movementSpeedMultiplier; }
	// 移動速度倍率を設定
	void SetMovementSpeedMultiplier(float multiplier) { data_.movementSpeedMultiplier = multiplier; }
	// コンボモーションデータ
	ComboMotionData GetComboMotionData() const { return data_.motionData; }
	// コンボデータ
	ComboData GetComboData() const { return data_.comboData; }
	// アニメーション中の時間取得
	Timer& GetTimer() { return data_.animetionTimer; }
	// ヒットデータを取得
	AttackHitData& GetHitData() { return hitData_; }
	// 攻撃入力系クラス取得
	AttackInput& GetAttackInput() { return attackInput_; }

	



	bool GetIsRecastTimeOver() const { return data_.MaxRecastTime <= data_.recastTime; }

	void RecastTime(float timer) { data_.recastTime += timer; }

public:

	// 時間を設定
	void SetTime(float startupTime, float attackAnimationTime, float recoveryTime)
	{
		data_.motionData.SetTime(startupTime, attackAnimationTime, recoveryTime);
	}

	void CreateData(ComboMotionData attackM, KnockbackData knock,Vector3 rotateIint, Vector3 rotateSpeed, float damage, float staminaCost, float movementSpeedMultiplier) {
		AttackMotions attack;

		attack.motionData = attackM;
		attack.knockbackData = knock;
		attack.initRotate = rotateIint;
		attack.rotateSpeed = rotateSpeed;
		attack.damage = damage;
		attack.staminaCost = staminaCost;
		attack.movementSpeedMultiplier = movementSpeedMultiplier;
		attack_.push_back(attack);
	}

	void SetAttackCombo(float deltaTime);

	// 攻撃各コンボによる初期化
	virtual void AttackTypeInit(int comboIndex) = 0;

	// 攻撃更新
	virtual void AttackUpdate(float deltaTime, WorldTransform& worldTransform) = 0;

	// 攻撃方法設定
	void KeyAttackTypes(bool is) {
		if (attackInput_.GetIsAttack()) {
			if (attackInput_.GetAttackKeyFlag().IsNormalAttack) {
				if (is && GetComboData().currentComboCount == 0) {
					attackInput_.SetRequest(AttackTypePlay::kJump);
				}
				else {
					attackInput_.SetRequest(AttackTypePlay::kNormal);
				}
			}
		}
	}

	void AttackUpdate();

protected:
	WeaponData data_;		// 武器データ
	AttackHitData hitData_; // 攻撃ヒットデータ
	std::vector<AttackMotions> attack_;//

	// 攻撃入力系クラス
	AttackInput attackInput_;

	

protected:
	BaseCharacter* character;	// 使っているキャラクター
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
	
	// 攻撃各コンボによる初期化
	virtual void AttackTypeInit(int comboIndex) = 0;

	// 攻撃更新
	virtual void AttackUpdate(float deltaTime, WorldTransform& worldTransform) = 0;

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
	
	// 攻撃各コンボによる初期化
	virtual void AttackTypeInit(int comboIndex) = 0;

	// 攻撃更新
	virtual void AttackUpdate(float deltaTime, WorldTransform& worldTransform) = 0;

protected:
	RangedWeaponData rengedData_; // 遠距離武器データ

};
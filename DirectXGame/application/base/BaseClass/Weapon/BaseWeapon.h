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
	void AddCurrentTime(float deltaTime) { data_.motionData.AddCurrentTime(deltaTime); };
	// 現在経過時間をリセット
	void ResetCurrentTime() { data_.motionData.ResetTime(); };
	// 現在経過時間を取得
	float GetCurrentTimer() const { return  data_.motionData.currentTime; };
	// 全体時間を取得
	float GetAllTime() const { return data_.motionData.AllTime(); };
	// 攻撃のスタートアップ時間を取得
	float GetStartupTime() const { return data_.motionData.startupTime; };
	// 攻撃のアニメーション時間を取得
	float GetAttackAnimationTime() const { return data_.motionData.attackAnimationTime; };
	// 攻撃のリカバリー時間を取得
	float GetRecoveryTime() const { return data_.motionData.recoveryTime; };
	// コンボ武器かどうかのフラグを取得
	bool IsCommonWeapon() const { return data_.comboData.isComboWeapon; }
	// コンボ武器かどうかのフラグを設定
	void SetIsComboWeapon(bool isComboWeapon) { data_.comboData.isComboWeapon = isComboWeapon; }
	// コンボの最大回数を取得
	int GetComboMaxCount() const { return data_.comboData.comboMaxCount; }
	// コンボの最大回数を設定
	void SetComboMaxCount(int comboMaxCount) { data_.comboData.comboMaxCount = comboMaxCount; }
	// コンボの現在の回数を取得
	int GetCurrentComboCount() const { return data_.comboData.currentComboCount; }
	// コンボの現在の回数を設定
	void SetCurrentComboCount(int currentComboCount) { data_.comboData.currentComboCount = currentComboCount; }
	// コンボの現在の回数をリセット
	void ResetCurrentComboCount() { data_.comboData.ResetCurrentComboCount(); }
	// コンボの現在の回数をインクリメント
	void IncrementCurrentComboCount() { data_.comboData.IncrementCurrentComboCount(); }
	// コンボを次にするかどうかのフラグを取得
	bool IsComboNext() const { return data_.comboData.isComboNext; }
	// コンボを次にするかどうかのフラグを設定
	void SetIsComboNext(bool isComboNext) { data_.comboData.isComboNext = isComboNext; }
	//
	Timer& GetTimer() { return data_.animetionTimer; }
	// 移動速度倍率を取得 
	float GetMovementSpeedMultiplier() const { return data_.movementSpeedMultiplier; }
	// 移動速度倍率を設定
	void SetMovementSpeedMultiplier(float multiplier) { data_.movementSpeedMultiplier = multiplier; }
	
	// ヒットデータを取得
	AttackHitData& GetHitData() { return hitData_; }

	bool IsAttack() const { return isAttack; };

	void SetIsAttack(bool is) { isAttack = is; }


	// 攻撃方法取得
	AttackTypePlay GetAttackTypePlay() const { return type; }

	// リクエスト取得
	std::optional<AttackTypePlay> GetTypeRequest() const { return typeRequest_; }

	// ふるまい変更
	void ChangeRequest(){ type = typeRequest_.value(); }

	// ふるまいリクエストリセット
	void ResetRequest(){ typeRequest_ = std::nullopt;}
	// ふるまいリクエストの設定
	void SetRequest(AttackTypePlay type) { typeRequest_ = type; }

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

	void InitData(int index) {

	}


	void SetAttackCombo(float deltaTime) {
		//  既定の時間経過で通常行動に戻る
		AddCurrentTime(deltaTime);
		if (GetCurrentTimer() >= GetAllTime()) {
			// コンボ継続なら次のコンボに進む
			if (IsComboNext()) {

				ResetCurrentTime();

				GetTimer().t = 0;
				GetTimer().maxT = GetAllTime();

				// 各パーツの角度などを次のコンボ用に初期化

				IncrementCurrentComboCount();

				// 攻撃タイプによって初期化
				AttackTypeInit(GetCurrentComboCount());


				// コンボフラグをリセット
				SetIsComboNext(false);
				SetIsAttack(false);
				ColliderHistoryClear();
			}
			else {
				ResetCurrentComboCount();
			}
		}
		else {
			// コンボ上限に達していない
			if (GetCurrentComboCount() < GetComboMaxCount() - 1) {
				if (IsAttack()) {
					// コンボ有効
					SetIsComboNext(true);
				}
			}
		}
	}

	// 
	void AttackTypeInit(int comboIndex) {
		ColliderHistoryClear();
		if (GetTypeRequest()) {
			// ふるまいを変更する
			ChangeRequest();
			// 各ふるまいごとの初期化を実行
			switch (GetAttackTypePlay())
			{
			case AttackTypePlay::kNormal:


				if (comboIndex == 0) {
					SetTime(0.0f, 0.2f, 0.1f);
					GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,0 });
				}
				if (comboIndex == 1) {
					SetTime(0.0f, 0.2f, 0.1f);
					GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,90 });
				}
				if (comboIndex == 2) {
					SetTime(0.0f, 0.2f, 0.1f);
					GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,-90 });
				}
				if (comboIndex == 3) {
					SetTime(0.0f, 0.2f, 0.1f);
					GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,90 });
				}
				break;
			case AttackTypePlay::kJump:
				if (comboIndex == 0) {
					SetTime(0.0f, 0.4f, 0.1f);
					GetWorldTransform().rotate_ = DegreesToRadians({ 0,0,0 });
				}
				break;
			}
			// ふるまいリクエストリセット
			ResetRequest();
		}
	}


protected:
	WeaponData data_;		// 武器データ
	AttackHitData hitData_; // 攻撃ヒットデータ
	bool isAttack = false;	// 攻撃するか
	std::vector<AttackMotions> attack_;//
	//振るまい
	AttackTypePlay type = AttackTypePlay::kNone;
	// 次の振るまいリクエスト
	std::optional<AttackTypePlay> typeRequest_ = std::nullopt;
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
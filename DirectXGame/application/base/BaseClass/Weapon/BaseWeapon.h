#pragma once
#include "WeaponData.h"
#include "DirectXGame/application/base/BaseClass/Attack/AttackData.h"
#include "DirectXGame/application/base/Attack/Combo/ComboState.h"

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
	void SetCharacter(BaseCharacter* character) {
		this->character = character;
		comboStateMachine_ = std::make_unique<ComboStateMachine>(this->character);
	};
	// タグによるコライダーの有効・無効を設定
	void SetIsCollider(CollisionTag tag ,bool is) { objectBase_->GetColliderComponent()->SetEnableByTag(tag, is); };
	// ダメージ取得
	//float GetDamage() const { return data_.damage; };
	// 攻撃中かどうかのフラグを取得
	bool IsActive() const { return data_.isActive; };
	// オートマチックかどうかのフラグを取得
	bool IsAutomatic() const { return data_.isAutomatic; };
	// 攻撃中かどうかのフラグを設定
	void SetIsActive(bool isActive) { data_.isActive = isActive; };
	// ヒットデータを取得
	AttackHitData& GetHitData() { return hitData_; }
	// データ
	ComboData GetComboData() const { return comboData_; }
	//
	void SetComboData(ComboData data) { comboData_ = data; }
		//
	ComboStateMachine* GetComboStateMachine() { return comboStateMachine_.get(); }



	bool GetIsRecastTimeOver() const { return data_.MaxRecastTime <= data_.recastTime; }

	void RecastTime(float timer) { data_.recastTime += timer; }

public:
	void AddComboNode(const std::string& name, std::shared_ptr<ComboNodeState> node) {
		comboNodes_[name] = node;
	}
	void ConnectCombo(const std::string& from, AttackInput input, const std::string& to) {
		auto itFrom = comboNodes_.find(from);
		auto itTo = comboNodes_.find(to);
		if (itFrom != comboNodes_.end() && itTo != comboNodes_.end()) {
			itFrom->second->SetNextState(input, itTo->second);
		}
	}
	void StartCombo(const std::string& name) {
		auto it = comboNodes_.find(name);
		if (it != comboNodes_.end()) {
			comboStateMachine_->SetRoot(it->second);
		}
	}
	void UpdateCombo(float dt) {
		comboStateMachine_->Update(dt);
	}
	void InputCombo(AttackInput input) {
		comboStateMachine_->HandleInput(input);
	}
	bool IsComboFinished() const {
		return comboStateMachine_->IsComboFinished();
	}
protected:
	WeaponData data_;						// 武器データ
	AttackHitData hitData_;					// 攻撃ヒットデータ
	ComboData comboData_;					// コンボデータ
	std::unique_ptr<ComboStateMachine> comboStateMachine_;	// コンボステートマシーン
	std::map<std::string, std::shared_ptr<ComboNodeState>> comboNodes_;
	//std::shared_ptr<ComboNodeState>

	//std::vector<AttackMotions> attack_;//
	
	// 攻撃入力系クラス
	//AttackInput attackInput_;

	

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
	//virtual void AttackTypeInit(int comboIndex) = 0;

	// 攻撃更新
	//virtual void AttackUpdate(float deltaTime, WorldTransform& worldTransform) = 0;

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
	//virtual void AttackTypeInit(int comboIndex) = 0;

	// 攻撃更新
	//virtual void AttackUpdate(float deltaTime, WorldTransform& worldTransform) = 0;

protected:
	RangedWeaponData rengedData_; // 遠距離武器データ

};
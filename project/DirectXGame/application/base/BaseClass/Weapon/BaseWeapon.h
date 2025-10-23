#pragma once
#include "WeaponData.h"
#include "DirectXGame/application/base/BaseClass/Attack/AttackData.h"
#include "DirectXGame/application/base/Attack/Combo/ComboState.h"
#include "DirectXGame/application/base/BaseClass/Object/ObjectComponent.h"

// 武器のベースクラス
class BaseWeapon : public IHitReceiver
{
public:
	// 初期化
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;
	// 更新
	virtual void Update() = 0;
	// 描画エフェクト
	virtual void DrawEffect() = 0;
	// 描画2d
	virtual void Draw2D() = 0;



public:
	// 使っているキャラクター設定
	void SetCharacter(BaseCharacter* character) {
		this->character = character;
		comboStateMachine_ = std::make_unique<ComboStateMachine>(this->character);
	};
	// タグによるコライダーの有効・無効を設定
	void SetIsCollider(CollisionTag tag ,bool is) { GetObject3D()->GetColliderComponent()->SetEnableByTag(tag, is); };
	// 攻撃中かどうかのフラグを取得
	bool IsActive() const { return data_.isActive; };
	// オートマチックかどうかのフラグを取得
	bool IsAutomatic() const { return data_.isAutomatic; };
	// 攻撃中かどうかのフラグを設定
	void SetIsActive(bool isActive) { data_.isActive = isActive; };
	// ヒットデータを取得
	AttackHitData& GetHitData() { return hitData_; }
	
	
	// コンボデータ取得
	ComboData GetComboData() const { return comboData_; }
	// コンボデータ設定
	void SetComboData(ComboData data) { comboData_ = data; }
	// コンボステートマシーン取得
	ComboStateMachine* GetComboStateMachine() { return comboStateMachine_.get(); }
	
	
	// コライダーコンポーネント
	ColliderComponent* GetColliderComponent() { return objectComponent_->GetColliderComponent(); };
	// オブジェクト3d取得
	Object3d* GetObject3D() { return objectComponent_->GetObject3D(); }
	// ワールド変換取得
	WorldTransform& GetWorldTransform() { return objectComponent_->GetObject3D()->GetWorldTransform(); }


	// リキャストタイム取得
	bool GetIsRecastTimeOver() const { return data_.MaxRecastTime <= data_.recastTime; }
	// リキャストタイム設定
	void RecastTime(float timer) { data_.recastTime += timer; }

public:
	// ノード追加
	void AddComboNode(const std::string& name, std::shared_ptr<ComboNodeState> node) {
		comboNodes_[name] = node;
	}
	// コンボ接続
	void ConnectCombo(const std::string& from, AttackInput input, const std::string& to) {
		auto itFrom = comboNodes_.find(from);
		auto itTo = comboNodes_.find(to);
		if (itFrom != comboNodes_.end() && itTo != comboNodes_.end()) {
			itFrom->second->SetNextState(input, itTo->second);
		}
	}
	// 最初のコンボ
	void StartCombo(const std::string& name) {
		auto it = comboNodes_.find(name);
		if (it != comboNodes_.end()) {
			comboStateMachine_->SetRoot(it->second);
		}
	}
	// コンボ更新
	void UpdateCombo(float dt) {
		comboStateMachine_->Update(dt);
	}
	// インプット
	void InputCombo(AttackInput input) {
		comboStateMachine_->HandleInput(input);
	}
	// コンボが終了したか
	bool IsComboFinished() const {
		return comboStateMachine_->IsComboFinished();
	}
protected:
	WeaponData data_;						// 武器データ
	AttackHitData hitData_;					// 攻撃ヒットデータ
	ComboData comboData_;					// コンボデータ
	std::unique_ptr<ComboStateMachine> comboStateMachine_;	// コンボステートマシーン
	std::map<std::string, std::shared_ptr<ComboNodeState>> comboNodes_;
	
protected:
	BaseCharacter* character;	// 使っているキャラクター
protected:
	std::unique_ptr<ObjectComponent> objectComponent_;	// オブジェクトコンポーネント
protected:
	Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
	Entity2DManager* entity2DManager_ = nullptr;	// 2Dエンティティマネージャー
	GlobalVariables* globalVariables_ = nullptr;	// グローバル変数
	Camera* camera_ = nullptr;						// カメラ
	Input* input_ = nullptr;						// 入力(使わないならnullptr)
	Audio* audio_ = nullptr;
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
	
protected:
	RangedWeaponData rengedData_; // 遠距離武器データ

};
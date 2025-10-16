#pragma once
#include"CharacterData.h"
#include"DirectXGame/application/base/Component/MoveComponent.h"
#include "DirectXGame/application/base/BaseClass/Object/ObjectComponent.h"
#include "DirectXGame/application/base/BaseClass/State/CharacterStateMachine.h"


class Effect;
class BaseSpecial;
class BaseWeapon;
class AttackInputHander;
class Entity3DManager;
class Entity2DManager;
class BulletManager;
class BaseCharacter : public IHitReceiver
{
public:
	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;

	/// <summary>
	/// エフェクトの描画
	/// </summary>
	virtual void DrawEffect() = 0;

	/// <summary>
	/// 2d描画
	/// </summary>
	virtual void Draw2D() = 0;

	/// <summary>
	/// 移動
	/// </summary>
	virtual void Move() = 0;

	/// <summary>
	/// ジャンプ
	/// </summary>
	virtual void Jump() = 0;

	/// <summary>
	/// 攻撃
	/// </summary>
	virtual void Attack() = 0;

	
	virtual void InitStateMachine() {};

public:
	// キャラクタータイプ設定
	void SetCharacterType(CharacterType type) { characterParameterComponent_.characterType_ = type; }

	// ダメージ
	void AddDamage(float damage) {
		HP() -= damage;
		if (GetHP() <= 0) {
			HP() = 0;
			objectComponent_->GetObjectStateFlags().isAlive = false; // 敵が死亡
		}
	}

	// 移動制限
	void LimitMove(Vector3 min, Vector3 max) {
		Object3d* object = objectComponent_->GetObject3D();

		if (object->GetWorldTransform().translate_.x > max.x) {
			object->GetWorldTransform().translate_.x = max.x;
		}
		if (object->GetWorldTransform().translate_.x < min.x) {
			object->GetWorldTransform().translate_.x = min.x;
		}
		if (object->GetWorldTransform().translate_.z > max.z) {
			object->GetWorldTransform().translate_.z = max.z;
		}
		if (object->GetWorldTransform().translate_.z < min.z) {
			object->GetWorldTransform().translate_.z = min.z;
		}
	};

public: // 取得系関数

	//
	CharacterStateMachine* GetCharacterStateMachine() { return stateMachine_.get(); }
	// 必殺技
	BaseSpecial* GetSpecial() { return special_.get(); }
	// 武器
	BaseWeapon* GetWeapon() { return weapon_.get(); }
	
	BulletManager* GetBulletManager() { return bulletManager_; }
	// 弾マネージャーの設定
	void SetBulletManager(BulletManager* bulletManager) { bulletManager_ = bulletManager; };
	// キャラクターの生存状態を取得
	bool GetAlive() const { return objectComponent_->GetObjectStateFlags().isAlive; };
	// キャラクターの生存状態を取得
	void SetAlive(bool is) { objectComponent_->GetObjectStateFlags().isAlive = is; };

	// HP取得
	float GetHP() const { return characterParameterComponent_.parameters_.HP.value; }
	// キャラクター取得
	CharacterType GetCharacterType() const { return characterParameterComponent_.characterType_; }
	// コライダーコンポーネント
	ColliderComponent* GetColliderComponent() { return objectComponent_->GetColliderComponent(); };
	// オブジェクト3d取得
	Object3d* GetObject3D() { return objectComponent_->GetObject3D(); }
	// ワールド変換取得
	WorldTransform& GetWorldTransform() { return objectComponent_->GetObject3D()->GetWorldTransform(); }
	// 削除フラグ
	bool  GetDelete() const { return objectComponent_->GetObjectStateFlags().isDeleted; };
	// 削除する
	void Delete() { objectComponent_->GetObjectStateFlags().isDeleted = true; };
	// 時間
	float GetTime() { return objectComponent_->GetTime(); }
	// インプット取得
	Input* GetInput() { return input_; };
	//
	void SetEffect(Effect* effect) { effect_ = effect; }

protected: // 取得系関数(変更可能)

	// 基本パラメータ
	BasicParameters& Parameters() { return characterParameterComponent_.parameters_; }
	// HP
	float& HP() { return characterParameterComponent_.parameters_.HP.value; }

protected: // 保存機能

	// 保存生成
	void CreateGroup(const std::string name) {
		objectComponent_->SetName(name);
		globalVariables_->CreateGroup(name);
	}

	// 保存するもの追加
	template<typename T>
	void AddItem(const std::string itemName, T& item) {
		globalVariables_->AddItem(objectComponent_->GetName(), itemName, item);
	}

	template<typename T>
	T GetValue(const std::string itemName) {
		return globalVariables_->GetValue<T>(objectComponent_->GetName(), itemName);
	}

	// ベースの保存項目を追加
	void InitializeBaseAddItem() {
		AddItem("speed", characterParameterComponent_.parameters_.speed);
		AddItem("HP", characterParameterComponent_.parameters_.HP.value);
		AddItem("MaxHP", characterParameterComponent_.parameters_.HP.maxValue);
		AddItem("MP", characterParameterComponent_.parameters_.MP.value);
		AddItem("MaxMP", characterParameterComponent_.parameters_.MP.maxValue);
		AddItem("stamina", characterParameterComponent_.parameters_.stamina.value);
		AddItem("MaxStamina", characterParameterComponent_.parameters_.stamina.maxValue);
		AddItem("jampPower", characterParameterComponent_.parameters_.jampPower);



		characterParameterComponent_.parameters_.speed = GetValue<float>("speed");
		characterParameterComponent_.parameters_.HP.value = GetValue<float>("HP");
		characterParameterComponent_.parameters_.HP.maxValue = GetValue<float>("MaxHP");
		characterParameterComponent_.parameters_.MP.value = GetValue<float>("MP");
		characterParameterComponent_.parameters_.MP.maxValue = GetValue<float>("MaxMP");
		characterParameterComponent_.parameters_.stamina.value = GetValue<float>("stamina");
		characterParameterComponent_.parameters_.stamina.maxValue = GetValue<float>("MaxStamina");
		characterParameterComponent_.parameters_.jampPower = GetValue<float>("jampPower");
	}

	void UpdateBaseGetValue() {
		characterParameterComponent_.parameters_.speed = GetValue<float>("speed");
		characterParameterComponent_.parameters_.jampPower = GetValue<float>("jampPower");
	}

protected:
	void InitMoveComponent() { moveComponent_ = std::make_unique<MoveComponent>(); }
	
public:
	// 速度
	Vector3& Velocity() { return moveComponent_->Velocity(); }
	// 速度取得
	Vector3 GetVelocity() const { return moveComponent_->GetVelocity(); }

	MoveComponent* GetMoveComponent() { return moveComponent_.get(); }

public:
	//CharacterStateComponent& GetCharacterStateComponent() { return characterStateComponent_; }

	CombatStatComponent* GetCombatStatComponent() { return combatStatComponent_.get(); }

	CharacterParameterComponent& GetCharacterParameterComponent() { return characterParameterComponent_; }
protected:
	std::unique_ptr<ObjectComponent> objectComponent_;		// オブジェクトコンポーネント
	std::unique_ptr<BaseSpecial> special_;					// スペシャル攻撃
	std::unique_ptr<BaseWeapon> weapon_;					// 武器
	std::unique_ptr<AttackInputHander> attackInputHander_;	// 攻撃入力系クラス
	std::unique_ptr<CharacterStateMachine> stateMachine_;	// キャラクターの状態管理
	std::unique_ptr<MoveComponent> moveComponent_;			// 移動コンポーネント
	std::unique_ptr<CombatStatComponent> combatStatComponent_;	// 攻撃パラメーター補正




protected:
	// キャラクターパラメータコンポーネント
	CharacterParameterComponent characterParameterComponent_;
	// キャラクターの状態コンポーネント
	//CharacterStateComponent characterStateComponent_;
protected:
	Effect* effect_;						// エフェクト
	BulletManager* bulletManager_;			// 弾管理
protected: // 貰ってくるもの
	Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
	Entity2DManager* entity2DManager_ = nullptr;	// 2Dエンティティマネージャー
	GlobalVariables* globalVariables_ = nullptr;	// グローバル変数
	Camera* camera_ = nullptr;						// カメラ
	Input* input_ = nullptr;						// 入力(使わないならnullptr)
	Audio* audio_ = nullptr;
};


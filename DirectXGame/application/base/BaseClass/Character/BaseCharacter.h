#pragma once
#include"CharacterData.h"
#include"DirectXGame/application/base/BaseClass/Object/BaseObject.h"

class Entity3DManager;
class Entity2DManager;
class BaseCharacter : public BaseObject
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

public:
	// キャラクタータイプ設定
	void SetCharacterType(CharacterType type) { characterData_.characterType_ = type; }

	// ダメージ
	void AddDamage(float damage) {
		HP() -= damage;
		if (GetHP() <= 0) {
			HP() = 0;
			flags_.isAlive = false; // 敵が死亡
		}
	}

	// 移動制限
	void LimitMove(Vector3 min, Vector3 max) {
		if (objectBase_->worldtransform_.translate_.x > max.x) {
			objectBase_->worldtransform_.translate_.x = max.x;
		}
		if (objectBase_->worldtransform_.translate_.x < min.x) {
			objectBase_->worldtransform_.translate_.x = min.x;
		}
		if (objectBase_->worldtransform_.translate_.z > max.z) {
			objectBase_->worldtransform_.translate_.z = max.z;
		}
		if (objectBase_->worldtransform_.translate_.z < min.z) {
			objectBase_->worldtransform_.translate_.z = min.z;
		}
	};

public: // 取得系関数

	// キャラクターの生存状態を取得
	bool GetAlive() const { return flags_.isAlive; };
	
	// キャラクターが無敵状態かどうかを取得
	bool GetInvincible() const { return GetSituation().isInvincible; }
	
	// キャラクター状態
	Situation GetSituation() const { return characterData_.situation_; }

	// HP取得
	float GetHP() const { return characterData_.parameters_.HP.value; }

	BasicBehavior GetBasicBehavior() const { return basicbehavior_; }

	// キャラクター取得
	CharacterType GetCharacterType() const { return characterData_.characterType_; }

protected: // 取得系関数(変更可能)
	// 基本パラメータ
	BasicParameters& Parameters() { return characterData_.parameters_; } 

	// キャラクター状態
	Situation& Situations() { return characterData_.situation_; }

	// HP
	float& HP() { return characterData_.parameters_.HP.value; } 

protected: // 保存機能

	// 保存生成
	void CreateGroup(const std::string name) {
		name_ = name;
		globalVariables_->CreateGroup(name_);
	}

	// 保存するもの追加
	template<typename T>
	void AddItem(const std::string itemName, T& item) {
		globalVariables_->AddItem(name_, itemName, item);
	}

	template<typename T>
	T GetValue(const std::string itemName) {
		return globalVariables_->GetValue<T>(name_, itemName);
	}

	// ベースの保存項目を追加
	void InitializeBaseAddItem() {
		//AddItem("Position", objectBase_->worldtransform_.translate_);
		AddItem("speed", characterData_.parameters_.speed);
		AddItem("HP", characterData_.parameters_.HP.value);
		AddItem("MaxHP", characterData_.parameters_.HP.maxValue);
		AddItem("MP", characterData_.parameters_.MP.value);
		AddItem("MaxMP", characterData_.parameters_.MP.maxValue);
		AddItem("stamina", characterData_.parameters_.stamina.value);
		AddItem("MaxStamina", characterData_.parameters_.stamina.maxValue);



		//objectBase_->worldtransform_.translate_ = GetValue<Vector3>("Position");
		characterData_.parameters_.speed = GetValue<float>("speed");
		characterData_.parameters_.HP.value = GetValue<float>("HP");
		characterData_.parameters_.HP.maxValue = GetValue<float>("MaxHP");
		characterData_.parameters_.MP.value = GetValue<float>("MP");
		characterData_.parameters_.MP.maxValue = GetValue<float>("MaxMP");
		characterData_.parameters_.stamina.value = GetValue<float>("stamina");
		characterData_.parameters_.stamina.maxValue = GetValue<float>("MaxStamina");
	}

	void UpdateBaseGetValue() {
		characterData_.parameters_.speed = GetValue<float>("speed");
	}

public:


protected:
	CharacterData characterData_;	// キャラクターデータ


	// 振るまい
	BasicBehavior basicbehavior_ = BasicBehavior::kRoot; 
	// 次の振るまいリクエスト
	std::optional<BasicBehavior> basicbehaviorRequest_ = std::nullopt;
};


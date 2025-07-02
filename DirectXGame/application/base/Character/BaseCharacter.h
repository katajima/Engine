#pragma once
#include"CharacterData.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/input/Input.h"


class Entity3DManager;
class Entity2DManager;
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

public:

	// キャラクタータイプ設定
	void SetCharacterType(CharacterType type) { characterData_.characterType_ = type; }

public: // 取得系関数

	// オブジェクト3d取得
	Object3d* GetObject3D() { return objectBase_; }

	// コライダーコンポーネント取得
	ColliderComponent* GetColliderComponent() { return objectBase_->GetColliderComponent(); }

	// キャラクターの生存状態を取得
	bool GetAlive() const { return characterData_.situation_.isAlive; };

	// 速度取得
	Vector3 GetVelocity() const { return characterData_.velocity_; }

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

	// 速度
	Vector3& Velocity() { return characterData_.velocity_; }
	// 加速度
	Vector3& Acceleration() { return characterData_.acceleration_; }
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
	Object3d* objectBase_ = nullptr;// オブジェクト3d
	CharacterData characterData_;	// キャラクターデータ


	// 振るまい
	BasicBehavior basicbehavior_ = BasicBehavior::kRoot; 
	// 次の振るまいリクエスト
	std::optional<BasicBehavior> basicbehaviorRequest_ = std::nullopt;
private:
	std::string name_ = ""; // キャラクター名

protected: // 貰ってくるもの
	Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
	Entity2DManager* entity2DManager_ = nullptr;	// 2Dエンティティマネージャー
	GlobalVariables* globalVariables_ = nullptr;	// グローバル変数
	Camera* camera_ = nullptr;						// カメラ
	Input* input_ = nullptr;						// 入力

};


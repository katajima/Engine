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
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera) = 0;

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

	// オブジェクト3d取得
	Object3d* GetObject3D() { return objectBase_; }

	// キャラクターの生存状態を取得
	bool GetAlive() const { return characterData_.situation_.isAlive; };

	// 速度取得
	Vector3 GetVelocity() const { return characterData_.velocity_; }

	// キャラクター状態
	Situation GetSituation() const { return characterData_.situation_; }

	// HP取得
	float GetHP() const { return characterData_.parameters_.HP.value; }



protected:
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

public:


protected:
	Object3d* objectBase_ = nullptr;// オブジェクト3d
	CharacterData characterData_;	// キャラクターデータ

protected: // 貰ってくるもの
	Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
	Entity2DManager* Entity2DManager_ = nullptr;	// 2Dエンティティマネージャー
	Camera* camera_ = nullptr;						// カメラ
	Input* input_ = nullptr;						// 入力

};


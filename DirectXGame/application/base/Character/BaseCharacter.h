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
class BaseCharacter
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



public:

	// オブジェクト3d取得
	Object3d* GetObject3D() { return objectBase_; }

	// キャラクターの生存状態を取得
	bool GetAlive() const { return characterData_.situation_.isAlive; };

private:

public:


protected:
	Object3d* objectBase_;			// オブジェクト3d
	CharacterData characterData_;	// キャラクターデータ

protected: // 貰ってくるもの
	Entity3DManager* entity3DManager_;	// 3Dエンティティマネージャー
	Camera* camera_ = nullptr;			// カメラ
	Input* input_;						// 入力

};


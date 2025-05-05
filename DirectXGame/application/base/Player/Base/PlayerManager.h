#pragma once
// engine
#include"DirectXGame/engine/Camera/Camera.h"

// 
#include "BasePlayer.h"
#include "DirectXGame/application/base/Player/Normal/NormalPlayer.h"

class Input;
class DirectXCommon;
class Entity3DManager;
class Entity2DManager;
class PlayerManager
{
public:
	enum class Type
	{
		kNormal,	// 通常
		kBullet,	// 弾撃ち
		kAttacker,	// アタッカー 
	};

	// 初期化
	void Initialize(Input* input,DirectXCommon* dxCommon,Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera);
	// 更新
	void Update();

	// 描画3D
	void Draw();

	// 描画Effect
	void DrawEffect();

	// 描画2D
	void Draw2D();

	// 生成
	void SelectPlayer(Type type, Vector3 position);
public:
	Object3d* GetObject3D() { return player_->GetObject3D(); }

	BasePlayer* GetPlayer() { return player_.get(); }

private:
	std::unique_ptr<BasePlayer> player_;


private:
	Camera* camera_;						// カメラ
	Entity3DManager* entity3DManager_;	// 3dオブジェクト管理
	Entity2DManager* entity2DManager_;  // 2Dオブジェクト管理
	DirectXCommon* dxCommon_;
	Input* input_;
};


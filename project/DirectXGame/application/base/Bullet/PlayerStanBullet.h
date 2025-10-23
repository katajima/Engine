#pragma once

#include "DirectXGame/application/base/BaseClass/Bullet/BaseBullet.h"
#include "DirectXGame/engine/effect/primitive/primitive.h"

// 前方宣言
class Primitive;

/// <summary>
/// プレイヤースタン弾
/// </summary>
class PlayerStanBullet : public BaseBullet {
public:

	// 初期化
	void Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;
	//描画エフェクト
	void DrawP() override;
	//描画2d
	void Draw2D() override;

private:
	float timer_ = 0.0f;
	float deleTimer_ = 0.0f;
	float explodingtimer_ = 3.0f;

	Vector3 velocity_ = {};
};

#pragma once
#include "DirectXGame/application/base/Enemy/Base/BaseEnemy.h"

class NormalEnemy : public BaseEnemy {
public:
	// 初期化
	void Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position,Camera* camera) override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;
	//
	void DrawP() override;
	//
	void Draw2D() override;

	void SetPlayer(Player* player) override;
private:
	// 移動
	void Move();

private:
};

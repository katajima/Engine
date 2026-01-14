#pragma once
#include "DirectXGame/application/base/Bullet/Base/BaseBullet.h"

class Effect;

class EnemyBullet : public BaseBullet {
public:
	// 初期化
	void Initialize(Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables,
		Vector3 position, Engine::Camera* camera) override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;
	//　描画エフェクト
	void DrawP() override;
	// 描画2D
	void Draw2D() override;

private:
	void Final();
	float timer_ = 0.0f;
	float deleTimer_ = 5.0f;

	bool hitDelete_ = false;



private: // 一旦

	struct ProvisionalData {
		const Vector3 objectSize = Vector3::Set(0.3f); // オブジェクトの大きさ
		const float trailLifeTime = 0.15f; // トレイルの寿命
		const float trailWidth = 0.2f; // トレイルの幅
		const float collRadius = 0.2f; // 当たり判定の半径
	};
	ProvisionalData provisionalData_;


};
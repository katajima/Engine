#pragma once

#include "DirectXGame/application/base/Bullet/BaseBullet.h"

class PlayerMissile :public BaseBullet {
public:
	// 初期化
	void Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera) override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;
	//
	void DrawP() override;
	//
	void Draw2D() override;

private:
	//
	void EnemyToColl() override;
	//
	void PlayerToColl() override;
private:
	Vector3 terget;

	
	int phase_ = 0;

	// 上に飛ぶ高さ
	float max_y = 20.0f;
	// 
	Vector3 randPosSky;
	Vector3 posGround;
	Vector3 str;
	float count = 0;
	float max_count = 0.5f;
	float t;

	Vector3 tragetPos;

	Vector3 enemyPos_;


private: //パーティクルエミッター
	// ミサイル煙エミッター
	std::unique_ptr<ParticleEmitter> ｍSmokeEmitter_ = nullptr;
	std::unique_ptr<ParticleEmitter> ｍSmokeEmitter2_ = nullptr;


	std::unique_ptr<ParticleEmitter> mExplosionSmokeEmitter_ = nullptr;
	std::unique_ptr<ParticleEmitter> mExplosionSmokeEmitter2_ = nullptr;
	std::unique_ptr<ParticleEmitter> mExplosionSmokeEmitter3_ = nullptr;
};


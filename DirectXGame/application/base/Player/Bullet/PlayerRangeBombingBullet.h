#pragma once

#include "DirectXGame/application/base/Bullet/BaseBullet.h"
#include "DirectXGame/engine/effect/primitive/primitive.h"

class Primitive;
class PlayerRangeBombingBullet :public BaseBullet {
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


private: // 演出関係

	void InitStartSmoke(ParticleEmitter* emitter, ParticleManager* particleManager, std::string emitName);

	void InitMoveSmoke(ParticleEmitter* emitter, ParticleManager* particleManager, std::string emitName);
	
	void InitExpSmoke(ParticleEmitter* emitter, ParticleManager* particleManager, std::string emitName);
	
	void InitExpFire(ParticleEmitter* emitter, ParticleManager* particleManager, std::string emitName);
	
	void InitExpBre(ParticleEmitter* emitter, ParticleManager* particleManager, std::string emitName);

private:
	WorldTransform worldTransformUp_;
	WorldTransform worldTransformBottom_;


	Vector3 target;


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

	Vector3 targetPos;

	Vector3 enemyPos_;

	float cilnderHeight_ = 30.0f; // 円柱の高さ

	float radius_ = 0.5f; // 半径

	float time_ = 0.0f; // 時間

private: //パーティクルエミッター
	// ミサイルi移動中煙エミッター
	std::unique_ptr<ParticleEmitter> moveSmokeEmitter_ = nullptr;
	std::unique_ptr<ParticleEmitter> moveSmokeEmitter2_ = nullptr;
	
	std::unique_ptr<ParticleEmitter> startSmokeEmitter_ = nullptr;

	// 爆発煙エミッター
	std::unique_ptr<ParticleEmitter> moveExpEmitter_ = nullptr;
	// 爆発火エミッター
	std::unique_ptr<ParticleEmitter> expFireEmitter_ = nullptr;

	// 爆発火エミッター
	std::unique_ptr<ParticleEmitter> animatedCubeEmitter_ = nullptr;

	// ヒット
	std::unique_ptr<ParticleEmitter> hitEmitter_ = nullptr;

	std::unique_ptr<Object3d> hitObject_ = nullptr;

	std::unique_ptr<Primitive> primitiveCylinder_ = nullptr;

	std::unique_ptr<TrailEffect> trailEffect_ = nullptr;

	bool flag_ = false; 
};


#pragma once

#include "DirectXGame/application/base/BaseClass/Bullet/BaseBullet.h"
#include "DirectXGame/engine/effect/primitive/primitive.h"

class Primitive;
class PlayerRangeBombingBullet :public BaseBullet {
public:
	~PlayerRangeBombingBullet();

	// 初期化
	void Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;
	//
	void DrawP() override;
	//
	void Draw2D() override;

private: // 演出関係

	//// 初期煙初期化
	void InitStartSmoke();
	//// ミサイル移動中煙初期化
	void InitMoveSmoke();
	//// 爆発煙初期化
	void InitExpSmoke();
	//// 爆発火初期化
	void InitExpFire();
	//// アニメーションキューブ初期化
	void InitExpBre();
	//// リングエミッター初期化
	void InitRingEmitter();
	//// ヒットエミッター初期化
	void InitHitEmitter();

private:


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

	uint32_t countIndex_ = 0;

private: //パーティクルエミッター
	std::unique_ptr<EffectComponent> effectComponent_ = nullptr;


	//// ミサイルi移動中煙エミッター
	//std::unique_ptr<ParticleEmitter> moveSmokeEmitter_;
	//std::unique_ptr<ParticleEmitter> moveSmokeEmitter2_ = nullptr;
	//
	//std::unique_ptr<ParticleEmitter> startSmokeEmitter_ = nullptr;

	//// 爆発煙エミッター
	//std::unique_ptr<ParticleEmitter> moveExpEmitter_ = nullptr;
	//// 爆発火エミッター
	//std::unique_ptr<ParticleEmitter> expFireEmitter_ = nullptr;

	//// 爆発火エミッター
	//std::unique_ptr<ParticleEmitter> animatedCubeEmitter_ = nullptr;

	//// リングエミッター
	//std::unique_ptr<ParticleEmitter> ringEmitter_ = nullptr;

	//// ヒット
	//std::unique_ptr<ParticleEmitter> hitEmitter_ = nullptr;

	PointParticleEmitter* hitEmitter_ = nullptr;
	PointParticleEmitter* stratSmoke01_ = nullptr;
	PointParticleEmitter* stratSmoke02_ = nullptr;
	PointParticleEmitter* smokePlaneExpSmoke = nullptr;
	//LineParticleEmitter* moveSmoke = nullptr;
	PointParticleEmitter* animatedCube = nullptr;
	AABBParticleEmitter* ringEmit = nullptr;
	PointParticleEmitter* expPlane = nullptr;


	GpuParticleEmitterPoint* emitterPoint = nullptr;

	Object3d* hitObject_ = nullptr;
	Object3d* hitObject2_ = nullptr;

	bool flag_ = false;
};


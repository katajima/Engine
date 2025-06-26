#pragma once

#include "DirectXGame/application/base/Bullet/BaseBullet.h"
#include "DirectXGame/engine/effect/primitive/primitive.h"

class Primitive;
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

private: // 演出関係

	void InitMoveSmoke(ParticleEmitter* emitter,ParticleManager* particleManager, std::string emitName);

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

	
	float radius_ = 0.5f; // 半径

	float time_ = 0.0f; // 時間

private: //パーティクルエミッター
	// ミサイルi移動中煙エミッター
	std::unique_ptr<ParticleEmitter> moveSmokeEmitter_ = nullptr;
	std::unique_ptr<ParticleEmitter> moveSmokeEmitter2_ = nullptr;

	// ヒット
	std::unique_ptr<ParticleEmitter> hitEmitter_ = nullptr;

	std::unique_ptr<Object3d> hitObject_ = nullptr;

	std::unique_ptr<Primitive> primitiveCylinder_ = nullptr;
};


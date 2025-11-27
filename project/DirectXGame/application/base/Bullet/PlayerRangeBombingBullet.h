#pragma once
#include "DirectXGame/application/base/Bullet/Base/BaseBullet.h"

/// <summary>
/// 範囲攻撃用弾クラス
/// </summary>
class PlayerRangeBombingBullet :public BaseBullet {
public:
	~PlayerRangeBombingBullet();

	// 初期化
	void Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;
	//　描画エフェクト
	void DrawP() override;
	// 描画2D
	void Draw2D() override;

private: // 演出関係

	//// ミサイル移動中煙初期化
	void InitMoveSmoke();
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


	float trailTime_ = 0.0f;

private: //パーティクルエミッター
	std::unique_ptr<EffectComponent> effectComponent_ = nullptr;

	GpuParticleEmitterPoint* emitterPoint = nullptr;

	Object3d* hitObject2_ = nullptr;



private: // 一旦

	struct ProvisionalData {
		const Vector3 objectSize = Vector3::Set(0.2f); // オブジェクトの大きさ
		const float trailLifeTime = 0.15f; // トレイルの寿命
		const float trailWidth = 0.2f; // トレイルの幅
		const float collRadius = 10.0f; // 当たり判定の半径
		const float damage = 50.0f; // ダメージ量
		const float skyX = 1000.0f;
		const float skyY = 600.0f;


		const float height = 10.0f;
		const float innerRadius = 6.0f;
		const float innerRadiusHit2 = 12.0f;
		const float outerRadius = 12.0f;
		const int segments = 16;
		const Color hit2Color = { 1.0f,0,0,0.5f };
	};
	ProvisionalData provisionalData_;


};


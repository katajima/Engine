#pragma once
#include "DirectXGame/application/base/Bullet/Base/BaseBullet.h"

/// <summary>
/// 範囲攻撃用弾クラス
/// </summary>
class PlayerRangeBombingBullet :public BaseBullet {
public:
	~PlayerRangeBombingBullet();

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

	// phase0
	float phase0Timer_ = 0.0f;
	float phase0EndTime_ = 0.1f;

	// phase1
	float phase1Timer_ = 0.0f;
	float phase1EndTime_ = 0.25f;
	
	// phase2
	float phase2Timer_ = 0.0f;
	float phase2EndTime_ = 0.25f;


	Vector3 target;


	int phase_ = 0;

	// 上に飛ぶ高さ
	float max_y = 20.0f;
	// 
	Vector3 randPosSky{};
	Vector3 posGround{};
	Vector3 str{};
	
	Vector3 targetPos{};

	Vector3 enemyPos_{};

	float radius_ = 0.5f; // 半径

	float time_ = 0.0f; // 時間

	uint32_t countIndex_ = 0;


	float trailTime_ = 0.0f;

private: //パーティクルエミッター
	std::unique_ptr<Engine::EffectComponent> effectComponent_ = nullptr;

	Engine::GpuParticleEmitterPoint* emitterPoint = nullptr;

	Engine::Object3d* hitObject2_ = nullptr;

private: // 一旦

	struct ProvisionalData {
		const Vector3 objectSize = Vector3::Set(0.2f); // オブジェクトの大きさ
		const float trailLifeTime = 0.15f; // トレイルの寿命
		const float trailWidth = 0.2f; // トレイルの幅
		const float collRadius = 8.0f; // 当たり判定の半径
		const float damage = 50.0f; // ダメージ量
		const float skyX = 1000.0f;
		const float skyY = 600.0f;

		const float cylinderHeight_ = 2.0f; // 円柱の高さ
		const float innerRadius = 4.0f;
		const float outerRadius = 4.0f;
		const int segments = 16;
		const Color hit2Color = { 1.0f,0,1.0f,0.75f };
		const float hitCylinderY = 8.0f;
		
		const float hitObjectY = -3.0f;
		const float hitObjectRotateX = -90.0f;

		
		const float speedPhase1 = 4000.0f;
		const float speedPhase2 = 3.0f;


		const float expPlaneY = 5.0f;

		

		const float distanceToEnemy = 5.0f;
	};
	ProvisionalData provisionalData_;


};


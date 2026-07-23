#pragma once
#include "DirectXGame/application/base/Bullet/Base/BaseBullet.h"
#include <DirectXGame/engine/Effect/EffectComponent.h>
#include <cstddef>
#include <string>

/// <summary>
/// 範囲攻撃用弾クラス
/// </summary>
class PlayerRangeBombingBullet :public BaseBullet {
public:
	~PlayerRangeBombingBullet();

	// 初期化
	void Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables,
		Vector3 position, Engine::Camera* camera) override;

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;
	/// <summary>
	/// 描画エフェクト
	/// </summary>
	void DrawEffect() override;
	/// <summary>
	/// 描画2D
	/// </summary>
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
	const std::string trailName_ = "RangeBombingTrail"; // 弾ごとのEffectComponent内で管理するトレイル名

private: //パーティクルエミッター
	std::unique_ptr<Engine::EffectComponent> effectComponent_ = nullptr;

	Engine::GpuParticleEmitterPoint* emitterPoint = nullptr;

	Engine::Object3d* hitObject2_ = nullptr;

	Engine::WorldTransform trailTransform_;

private: // 一旦

	struct ProvisionalData {
		const Vector3 objectSize = Vector3::Set(0.2f); // オブジェクトの大きさ
		const float trailLifeTime = 0.15f; // トレイルの寿命
		const float trailWidth = 1.0f; // トレイルの幅
		const float trailMinEmitDistance = 0.05f; // トレイル頂点を追加する最小移動距離
		const size_t trailMaxSegmentCount = 96; // トレイルが保持する最大分割数
		const Color trailColor = { 0.35f,0.85f,1.0f,0.85f }; // トレイルの色
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


#pragma once
#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"

#include"NormalEnemyState.h"

class NormalEnemy : public BaseEnemy {
public:
	// コンストラクタ
	NormalEnemy() {
		
	}

	// 初期化
	void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables,Vector3 position, Camera* camera) override;

	// 毎フレーム更新
	void Update() override;

	
	void DrawEffect() override;
	//
	void Draw2D() override;

	void SetPlayer(BasePlayer* player) override;

	void Emit() override;
	// 移動
	void Move() override;

	void Jump() override;

	void Attack() override;

private:
	
	void InitStateMachine() override;


	void InitParticle();

private:
	WorldTransform worldEffect_;

	bool isStopMove_ = false;
	float stopMoveTimer_ = 0.0f;

	DebugTimer debugTimer_;

	//std::unique_ptr<ParticleEmitter> tireEmit_ = nullptr; // タイヤ
	//std::unique_ptr<ParticleEmitter> ductEmit_ = nullptr; // ダクト
	//std::unique_ptr<ParticleEmitter> plankEmit_ = nullptr; // 鋼板
	//std::unique_ptr<ParticleEmitter> gearEmit_ = nullptr; // 歯車
	//std::unique_ptr<ParticleEmitter> fenceEmit_ = nullptr; // 柵
};

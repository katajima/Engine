#pragma once
#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"

class NormalEnemy : public BaseEnemy {
public:
	// 初期化
	void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables,Vector3 position, Camera* camera) override;

	// 毎フレーム更新
	void Update() override;

	
	void DrawEffect() override;
	//
	void Draw2D() override;

	void SetPlayer(BasePlayer* player) override;

	void Emit() override;

	void ATest();

private:
	// 移動
	void Move();

	void InitParticle();

	//通常行動初期化
	void BehaviorRootInitialize();

	//通常行動更新
	void BehaviorRootUpdate();

	//攻撃行動初期化
	void BehaviorAttackInitialize();

	//攻撃行動更新
	void BehaviorAttackUpdate();
	
	// 死亡行動初期化
	void BehaviorDieInitialize();
	// 死亡行動更新
	void BehaviorDieUpdate();

private:
	Vector3 lockonPosition_ = {}; // 位置
	Vector3 subPosition_ = {}; // 位置
	float timer_ = 0.0f; // タイマー
	float attackTimer_ = 3.0f; // 攻撃タイマー
	float rootTimer_ = 3.0f; // 通常タイマー
	float attackSpeed_ = 10.0f; // 攻撃速度
	float dieTimer_ = 3.0f; // 死亡タイマー 
private:

	std::unique_ptr<ParticleEmitter> groundRightEmit_ = nullptr;
	std::unique_ptr<ParticleEmitter> groundLeftEmit_ = nullptr;

	std::unique_ptr<ParticleEmitter> starEmit_ = nullptr;
	std::unique_ptr<ParticleEmitter> traiEmit_ = nullptr;
	std::unique_ptr<ParticleEmitter> hitEmit_ = nullptr;
	std::unique_ptr<ParticleEmitter> dustEmit_ = nullptr;
	std::unique_ptr<ParticleEmitter> dustEmit2_ = nullptr;
	std::unique_ptr<ParticleEmitter> dustEmit3_ = nullptr;

	std::unique_ptr<ParticleEmitter> tireEmit_ = nullptr; // タイヤ

	std::unique_ptr<ParticleEmitter> ductEmit_ = nullptr; // ダクト


	std::unique_ptr<ParticleEmitter> plankEmit_ = nullptr; // 鋼板
	std::unique_ptr<ParticleEmitter> gearEmit_ = nullptr; // 歯車
	std::unique_ptr<ParticleEmitter> fenceEmit_ = nullptr; // 柵

	std::unique_ptr<ParticleEmitter> effectEmit_ = nullptr; // 
};

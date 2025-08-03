#pragma once
#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"

#include"NormalEnemyState.h"

using EnemyStateFactory = std::function<std::unique_ptr<BaseEnemyState>(BaseEnemy*)>;




class NormalEnemy : public BaseEnemy {
public:
	NormalEnemy() {
		// 初期化時に状態名とその生成関数を登録
		RegisterState("Move", [](BaseEnemy* p) {
			return std::make_unique<EnemyStateMove>(p);
			});
		RegisterState("Attack", [](BaseEnemy* p) {
			return std::make_unique<EnemyStateAttack>(p);
			});
		RegisterState("Special", [](BaseEnemy* p) {
			return std::make_unique<EnemyStateSpecial>(p);
			});
		RegisterState("Die", [](BaseEnemy* p) {
			return std::make_unique<EnemyStateDie>(p);
			});
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

	void ChangeState(const std::string& name) override{
		auto it = stateFactoryMap_.find(name);
		if (it != stateFactoryMap_.end()) {
			stateName_ = name;
			ChangeState(it->second(this)); // unique_ptr<BasePlayerState>
		}
		else {
			assert(true);
		}
	}

	std::string GetStateName() override { return stateName_; }

private:
	
	void InitParticle();


	void ChangeState(std::unique_ptr<BaseEnemyState> newState) {
		if (state_) {
			state_->Exit();
		}
		state_ = std::move(newState);
		if (state_) {
			state_->Enter();
		}
	}

	void RegisterState(const std::string& name, EnemyStateFactory factory) {
		stateFactoryMap_[name] = factory;
	}

private:
	std::string stateName_ = "";
	std::unique_ptr<BaseEnemyState> state_;// ステート
	std::unordered_map<std::string, EnemyStateFactory> stateFactoryMap_;	// 
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

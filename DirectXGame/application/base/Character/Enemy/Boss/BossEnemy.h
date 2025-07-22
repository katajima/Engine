#pragma once
#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"
#include"BossEnemyState.h"

using BossEnemyStateFactory = std::function<std::unique_ptr<BaseEnemyState>(BaseEnemy*)>;


class BossEnemy : public BaseEnemy
{
public:
	BossEnemy() {
		// 初期化時に状態名とその生成関数を登録
		RegisterState("Move", [](BaseEnemy* p) {
			return std::make_unique<BossEnemyStateMove>(p);
			});
		RegisterState("Attack", [](BaseEnemy* p) {
			return std::make_unique<BossEnemyStateAttack>(p);
			});
		RegisterState("Special", [](BaseEnemy* p) {
			return std::make_unique<BossEnemyStateSpecial>(p);
			});
		RegisterState("Die", [](BaseEnemy* p) {
			return std::make_unique<BossEnemyStateDie>(p);
			});
	}


	// 初期化
	void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) override;

	// 毎フレーム更新
	void Update() override;


	void DrawEffect() override;
	//
	void Draw2D() override;

	void SetPlayer(BasePlayer* player) { player_ = player; };

	void Emit() override;
	// 移動
	void Move() override;

	void Jump() override;


	void ChangeState(const std::string& name) override {
		auto it = stateFactoryMap_.find(name);
		if (it != stateFactoryMap_.end()) {
			ChangeState(it->second(this)); // unique_ptr<BasePlayerState>
		}
		else {
			assert(true);
		}
	}


private:
	void ChangeState(std::unique_ptr<BaseEnemyState> newState) {
		if (state_) {
			state_->Exit();
		}
		state_ = std::move(newState);
		if (state_) {
			state_->Enter();
		}
	}

	void RegisterState(const std::string& name, BossEnemyStateFactory factory) {
		stateFactoryMap_[name] = factory;
	}
private:
	std::unique_ptr<BaseEnemyState> state_;// ステート
	std::unordered_map<std::string, BossEnemyStateFactory> stateFactoryMap_;// 

};



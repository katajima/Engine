#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"


class MoveSystem;
class JumpSystem;
class MovementComponent;

/// <summary>
/// 移動状態を表すための基底クラス。
/// </summary>
class MovementState {
public:
	virtual ~MovementState() = default;

	virtual void Enter() {}
	virtual void Update(float dt, WorldTransform& object, RigidBodyComponent& rigid, Input* input, 
		MoveSystem* moveSystem, JumpSystem* jumpSystem) = 0;
	virtual void Exit() {}
};

/// <summary>
/// 地上での移動処理
/// </summary>
class GroundState : public MovementState {
public:
	void Update(float dt, WorldTransform& object, RigidBodyComponent& rigid, Input* input,
		MoveSystem* moveSystem, JumpSystem* jumpSystem) override;
};


/// <summary>
/// 空中での移動処理
/// </summary>
class AirState : public MovementState {
public:
	void Update(float dt, WorldTransform& object, RigidBodyComponent& rigid, Input* input,
		MoveSystem* moveSystem, JumpSystem* jumpSystem) override;
};

using MovementStateFactory = std::function<std::unique_ptr<MovementState>(void)>;

class MovementStateMachine {
public:
	// 移動状態の種類
	enum class Type {
		Ground,
		Air,
	};

	void Initialize() {
		// ステート登録
		RegisterState(Type::Ground, []() { return std::make_unique<GroundState>(); });
		RegisterState(Type::Air, []() { return std::make_unique<AirState>();   });

	};

	void Update(float dt, WorldTransform& object, RigidBodyComponent& rigid, Input* input,
		MoveSystem* moveSystem, JumpSystem* jumpSystem);

	


    // 修正: ChangeState メソッド内の呼び出し部分を修正
    void ChangeState(const Type& type) {
        auto it = stateFactoryMap_.find(type);
        if (it != stateFactoryMap_.end()) {
            nowState_ = type;
            ChangeState(it->second()); // 呼び出しが正しく評価されるように修正
        } else {
            assert(false);
        }
    }

	// ---- 現在ステートの取得 ----
	MovementState* GetCurrentState() const { return state_.get(); }

	// 現在のステート取得
	Type GetCurrentMainState() const {
		return nowState_;
	}

	// 登録
	void RegisterState(const Type& name, MovementStateFactory factory) {
		stateFactoryMap_[name] = factory;
	}
private:

	// ステート変更
	void ChangeState(std::unique_ptr<MovementState> newState) {
		if (state_) {
			state_->Exit(); // 終了
		}
		state_ = std::move(newState);
		if (state_) {
			state_->Enter(); // 開始
		}
	}


private:
	std::unique_ptr<MovementState> state_;// ステート
	// 状態管理用Map
	std::unordered_map<Type, MovementStateFactory> stateFactoryMap_;
	//
	Type nowState_;

};


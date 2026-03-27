#pragma once
#include "PlayerCarState.h"

using PlayerCarStateFactory = std::function<std::unique_ptr<PlayerCarState>(PlayerCar*)>;

class PlayerCarStateMachine {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="car"></param>
	/// <param name="initialState"></param>
	void Initialize(PlayerCar* car, CarMainState initialState = CarMainState::Idle);

	// 更新
	void Update(float dt);

	// 変更
	void ChangeState(const CarMainState& name) {
		auto it = stateFactoryMap_.find(name);
		if (it != stateFactoryMap_.end()) {
			if(nowState_ == name) {
				return;
			}
			nowState_ = name;
			ChangeState(it->second(playerCar));
		}
		else {
			assert(true);
		}
	}

	// ---- 現在ステートの取得 ----
	PlayerCarState* GetCurrentState() const { return state_.get(); }

	// 現在のステート取得
	CarMainState GetCurrentMainState() const {
		return state_ ? state_->GetCerMainState() : CarMainState::TitleMove;
	}

	// 登録
	void RegisterState(const CarMainState& name, PlayerCarStateFactory factory) {
		stateFactoryMap_[name] = factory;
	}
private:
	// ステート変更
	void ChangeState(std::unique_ptr<PlayerCarState> newState) {
		if (state_) {
			state_->Exit(); // 終了
		}
		state_ = std::move(newState);
		if (state_) {
			state_->Enter(); // 開始
		}
	}
private:
	// ステート
	std::unique_ptr<PlayerCarState> state_;
	// 状態管理用Map
	std::unordered_map<CarMainState, PlayerCarStateFactory> stateFactoryMap_;
	// 現在のステート
	CarMainState nowState_;
	// 操作状態
	PlayerCar* playerCar = nullptr;
};
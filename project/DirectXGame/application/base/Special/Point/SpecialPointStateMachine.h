#pragma once
#include "SpecialPointState.h"

// 前方宣言
class SpecalPoint;

using SpecialPointStateFactory = std::function<std::unique_ptr<BaseSpecialPointState>(SpecalPoint*)>;

/// <summary>
/// スペシャルポイントステートマシーン
/// </summary>
class SpecialPointStateMachine {
public:
	// 初期化
	void Initialize(SpecalPoint* object, SpecialPointState initialState = SpecialPointState::kPop);
	// 更新
	void Update(float dt);


	// 変更
	void ChangeState(const SpecialPointState& name) {
		auto it = stateFactoryMap_.find(name);
		if (it != stateFactoryMap_.end()) {
			nowState_ = name;
			ChangeState(it->second(object_));
		}
		else {
			assert(true);
		}
	}

	// ---- 現在ステートの取得 ----
	BaseSpecialPointState* GetCurrentState() const { return state_.get(); }

	// 現在のステート取得
	SpecialPointState GetCurrentMainState() const {
		return state_ ? state_->GetSpecialPointState() : SpecialPointState::kMove;
	}

	// 登録
	void RegisterState(const SpecialPointState& name, SpecialPointStateFactory factory) {
		stateFactoryMap_[name] = factory;
	}
private:
	// ステート変更
	void ChangeState(std::unique_ptr<BaseSpecialPointState> newState) {
		if (state_) {
			state_->Exit(); // 終了
		}
		state_ = std::move(newState);
		if (state_) {
			state_->Enter(); // 開始
		}
	}

private:
	std::unique_ptr<BaseSpecialPointState> state_;// ステート
	// 状態管理用Map
	std::unordered_map<SpecialPointState, SpecialPointStateFactory> stateFactoryMap_;
	//
	SpecialPointState nowState_;
private:
	SpecalPoint* object_ = nullptr;

};

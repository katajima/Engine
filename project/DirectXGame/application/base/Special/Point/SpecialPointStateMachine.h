#pragma once
#include "SpecialPointState.h"

// 前方宣言
class SpecialPoint;

using SpecialPointStateFactory = std::function<std::unique_ptr<BaseSpecialPointState>(SpecialPoint*)>;

/// <summary>
/// スペシャルポイントステートマシーン
/// </summary>
class SpecialPointStateMachine {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SpecialPoint* object, SpecialPointState initialState = SpecialPointState::kPop);
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt);


	/// <summary>
	/// 変更
	/// </summary>
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

	/// <summary>
	/// ---- 現在ステートの取得 ----
	/// </summary>
	BaseSpecialPointState* GetCurrentState() const { return state_.get(); }

	/// <summary>
	/// 現在のステート取得
	/// </summary>
	SpecialPointState GetCurrentMainState() const {
		return state_ ? state_->GetSpecialPointState() : SpecialPointState::kMove;
	}

	/// <summary>
	/// 登録
	/// </summary>
	void RegisterState(const SpecialPointState& name, SpecialPointStateFactory factory) {
		stateFactoryMap_[name] = factory;
	}
private:
	/// <summary>
	/// ステート変更
	/// </summary>
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
	SpecialPoint* object_ = nullptr;

};

#pragma once
#include "DirectXGame/application/base/Object/ObjectComponent.h"



enum class BulletState {
	kSpawn,		// 出現
	kAlive,		// 生きている状態
	kHit,		// 何かに当たった瞬間
	kDespawn	// 消滅状態
};

// 前方宣言
class BaseBullet;

// 基底ステートクラス
class BaseBulletState {
public:

	virtual ~BaseBulletState() = default;
	BaseBulletState(const BulletState& state, BaseBullet* object) :state_(state), object_(object) {};
	// 開始
	virtual void Enter() {};
	// 更新
	virtual void Update(float dt) {};
	// 終了
	virtual void Exit() {};
public:
	BulletState GetBulletState() const { return state_; }
private:
	BulletState state_;
protected:
	float timer_ = 0.0f;
	BaseBullet* object_ = nullptr;
};

// ステートファクトリ
using BulletStateFactory = std::function<std::unique_ptr<BaseBulletState>(BaseBullet*)>;

/// <summary>
/// 弾ステートマシーン
/// </summary>
class BulletStateMachine {
public:
	// 初期化
	void Initialize(BaseBullet* object, BulletState initialState = BulletState::kSpawn);
	// 更新
	void Update(float dt);

	// 変更
	void ChangeState(const BulletState& name) {
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
	BaseBulletState* GetCurrentState() const { return state_.get(); }

	// 現在のステート取得
	BulletState GetCurrentMainState() const {
		return state_ ? state_->GetBulletState() : BulletState::kAlive;
	}

	// 登録
	void RegisterState(const BulletState& name, BulletStateFactory factory) {
		stateFactoryMap_[name] = factory;
	}
private:
	// ステート変更
	void ChangeState(std::unique_ptr<BaseBulletState> newState) {
		if (state_) {
			state_->Exit(); // 終了
		}
		state_ = std::move(newState);
		if (state_) {
			state_->Enter(); // 開始
		}
	}
private:
	std::unique_ptr<BaseBulletState> state_;// ステート
	// 状態管理用Map
	std::unordered_map<BulletState, BulletStateFactory> stateFactoryMap_;
	// 現在ステート
	BulletState nowState_;
private:
	BaseBullet* object_ = nullptr;
};

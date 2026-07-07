#pragma once
#include "DirectXGame/application/base/Object/ObjectComponent.h"


// キャラクター行動(一段階)
enum class SpecialPointState {
	kPop,		// 出現時
	kIdle,		// 待機
	kMove,		// 移動
	kEnd,		// 終了
};
class SpecialPoint;

// 基底ステート
class BaseSpecialPointState {
public:

	virtual ~BaseSpecialPointState() = default;
	BaseSpecialPointState(const SpecialPointState& state, SpecialPoint* object) :specialPointState(state),object_(object){};
	// 開始
	virtual void Enter() {}
	// 更新
	virtual void Update(float dt) {}
	// 終了
	virtual void Exit() {}
public:
	// キャラクターメインステート取得
	SpecialPointState GetSpecialPointState() const { return specialPointState; }
protected:
	SpecialPointState specialPointState;
	float timer = 0.0f;
protected:
	SpecialPoint* object_ = nullptr;
};

// 出現時のステート
class SpecialPointPopState : public BaseSpecialPointState {
public:
	SpecialPointPopState(SpecialPoint* object)
	: BaseSpecialPointState(SpecialPointState::kPop,object){}
	// 開始
	void Enter() override;
	// 更新
	void Update(float dt) override;
	// 終了
	void Exit() override;

	// 時間判定
	bool IsNextStateTime() const { return nextStateTimer_ < timer; }

private:
	Vector3 upVelocity_ = {0,700,0};
	float glavityScale_ = 2.0f;
	float nextStateTimer_ = 0.75f;
	//
	float groundY = -1.0f;
};

// 通常時のステート
class SpecialPointIdleState : public BaseSpecialPointState {
public:
	SpecialPointIdleState(SpecialPoint* object)
		:BaseSpecialPointState(SpecialPointState::kIdle, object){}
	// 開始
	void Enter() override;
	// 更新
	void Update(float dt) override;
	// 終了
	void Exit() override;
private:
	// 終了ステート移行時間
	float endStateTimer_ = 10.0f;
	// 回転速度
	float rotateSpeed_ = Math::DegreesToRadians(360.0f);
	// 浮遊速度
	float floatSpeed_ = 0.5f;
	// 浮遊高さ	
	float floatHeight_ = 0.5f;
	// 基準Y座標
	float baseY_ = 0.0f;
};

// 移動時のステート
class SpecialPointMoveState : public BaseSpecialPointState {
public:
	SpecialPointMoveState(SpecialPoint* object)
		: BaseSpecialPointState(SpecialPointState::kMove, object){}
	// 開始
	void Enter() override;
	// 更新
	void Update(float dt) override;
	// 終了
	void Exit() override;
private:
	// 移動速度
	float speed_ = 100;


};

// 終了時のステート
class SpecialPointEndState : public BaseSpecialPointState {
public:
	SpecialPointEndState(SpecialPoint* object)
		: BaseSpecialPointState(SpecialPointState::kEnd, object){}
	// 開始
	void Enter() override;
	// 更新
	void Update(float dt) override;
	// 終了
	void Exit() override;
private:


};
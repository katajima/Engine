#pragma once
#include "DirectXGame/engine/Entity/ObjectComponent.h"


// キャラクター行動(一段階)
enum class SpecialPointState {
	kPop,		// 出現時
	kIdle,		// 待機
	kMove,		// 移動
	kEnd,		// 終了
};
class SpecialPoint;

// 基底ステート
/// <summary>
/// BaseSpecialPointStateを管理・実装するクラス。
/// </summary>
class BaseSpecialPointState {
public:

	virtual ~BaseSpecialPointState() = default;
	BaseSpecialPointState(const SpecialPointState& state, SpecialPoint* object) :specialPointState(state),object_(object){};
	/// <summary>
	/// 開始
	/// </summary>
	virtual void Enter() {}
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update(float dt) {}
	/// <summary>
	/// 終了
	/// </summary>
	virtual void Exit() {}
public:
	/// <summary>
	/// キャラクターメインステート取得
	/// </summary>
	SpecialPointState GetSpecialPointState() const { return specialPointState; }
protected:
	SpecialPointState specialPointState;
	float timer = 0.0f;
protected:
	SpecialPoint* object_ = nullptr;
};

// 出現時のステート
/// <summary>
/// SpecialPointPopStateを管理・実装するクラス。
/// </summary>
class SpecialPointPopState : public BaseSpecialPointState {
public:
	SpecialPointPopState(SpecialPoint* object)
	: BaseSpecialPointState(SpecialPointState::kPop,object){}
	/// <summary>
	/// 開始
	/// </summary>
	void Enter() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt) override;
	/// <summary>
	/// 終了
	/// </summary>
	void Exit() override;

	/// <summary>
	/// 時間判定
	/// </summary>
	bool IsNextStateTime() const { return nextStateTimer_ < timer; }

private:
	Vector3 upVelocity_ = {0,200,0};
	float glavityScale_ = 2.0f;
	float nextStateTimer_ = 0.5f;
	//
	float groundY = -1.0f;
};

// 通常時のステート
/// <summary>
/// SpecialPointIdleStateを管理・実装するクラス。
/// </summary>
class SpecialPointIdleState : public BaseSpecialPointState {
public:
	SpecialPointIdleState(SpecialPoint* object)
		:BaseSpecialPointState(SpecialPointState::kIdle, object){}
	/// <summary>
	/// 開始
	/// </summary>
	void Enter() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt) override;
	/// <summary>
	/// 終了
	/// </summary>
	void Exit() override;
private:
	// 終了ステート移行時間
	float endStateTimer_ = 10.0f;
	/// <summary>
	/// 回転速度
	/// </summary>
	float rotateSpeed_ = Math::DegreesToRadians(360.0f);
	// 浮遊速度
	float floatSpeed_ = 0.5f;
	// 浮遊高さ	
	float floatHeight_ = 0.5f;
	// 基準Y座標
	float baseY_ = 0.0f;
};

// 移動時のステート
/// <summary>
/// SpecialPointMoveStateを管理・実装するクラス。
/// </summary>
class SpecialPointMoveState : public BaseSpecialPointState {
public:
	SpecialPointMoveState(SpecialPoint* object)
		: BaseSpecialPointState(SpecialPointState::kMove, object){}
	/// <summary>
	/// 開始
	/// </summary>
	void Enter() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt) override;
	/// <summary>
	/// 終了
	/// </summary>
	void Exit() override;
private:
	Vector3 upVelocity_ = { 0,50,0 };
	// 移動速度
	float speed_ = 50.0f;


};

// 終了時のステート
/// <summary>
/// SpecialPointEndStateを管理・実装するクラス。
/// </summary>
class SpecialPointEndState : public BaseSpecialPointState {
public:
	SpecialPointEndState(SpecialPoint* object)
		: BaseSpecialPointState(SpecialPointState::kEnd, object){}
	/// <summary>
	/// 開始
	/// </summary>
	void Enter() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt) override;
	/// <summary>
	/// 終了
	/// </summary>
	void Exit() override;
private:


};

#pragma once
#include<string>
#include <memory>
#include <functional>
#include"DirectXGame/engine/math/MathFunctions.h"

enum class CarMainState {
	NotSet,
	Idle,				// 待機
	TitleMove,				// 移動
	PreparationMove,	// 移動準備
	SelectMove,			// 移動(セレクトシーン)
	ResultCashExchange	// 結果シーンのキャッシュ交換
};



class PlayerCar;

/// <summary>
/// プレイヤー車ステート
/// </summary>
class PlayerCarState {
public:
	virtual ~PlayerCarState() = default;
	PlayerCarState(const CarMainState& state, PlayerCar* car) :mainState(state), playerCar(car) {};
	
	// 開始
	virtual void Enter() {}
	// 更新
	virtual void Update(float dt) {}
	// 終了
	virtual void Exit() {}

	// キャラクターメインステート取得
	CarMainState GetCerMainState() const { return mainState; }
protected:
	// 持ち主取得
	PlayerCar* GetPlayerCar() { return playerCar; }
private:
	// ステート
	CarMainState mainState;
	//
	PlayerCar* playerCar  =nullptr;
};

/// <summary>
/// 待機状態
/// </summary>
class CarIdleState : public PlayerCarState {
public:
	CarIdleState(const CarMainState& state, PlayerCar* car) :
		PlayerCarState(state, car) {
	};

	// 開始
	void Enter() override;
	// 更新
	void Update(float dt) override;
	// 終了
	void Exit() override;

private:

};

class CarTitleMoveState : public PlayerCarState {
public:
	CarTitleMoveState(const CarMainState& state, PlayerCar* car) :
		PlayerCarState(state, car) {
	};

	// 開始
	void Enter() override;
	// 更新
	void Update(float dt) override;
	// 終了
	void Exit() override;


private:
	// 速度
	Vector3 velocity_ = { 4.0f,0.0f,0.0f };
	// 位置
	Vector3 pos_ = { -1.25f,0.05f,12.0f };
	// 埃オフセット
	Vector3 dustOffset_ = { 0,-0.2f,0 };
};


class CarPreparationMoveState : public PlayerCarState {
public:
	CarPreparationMoveState(const CarMainState& state, PlayerCar* car) :
		PlayerCarState(state, car) {
	};

	// 開始
	void Enter() override;
	// 更新
	void Update(float dt) override;
	// 終了
	void Exit() override;


private:
	// 速度
	Vector3 velocity_ = { 4.0f,0.0f,0.0f };
	// 位置
	Vector3 pos_ = { -1.25f,0.05f,12.0f };
	// タイマー
	float timer_ = 0.0f;
};


class CarSelectMove : public PlayerCarState {
public:
	CarSelectMove(const CarMainState& state, PlayerCar* car) :
		PlayerCarState(state, car) {
	};

	// 開始
	void Enter() override;
	// 更新
	void Update(float dt) override;
	// 終了
	void Exit() override;


private:
	// 速度
	Vector3 velocity_ = { -10.0f,0.0f,0.0f };
	// 位置
	Vector3 pos_ = { -1.25f,0.05f,12.0f };
	// タイマー
	float timer_ = 0.0f;
	// 埃オフセット
	Vector3 dustOffset_ = { 0,-0.4f,0 };
};


class ResultCashExchangeState : public PlayerCarState {
public:
	ResultCashExchangeState(const CarMainState& state, PlayerCar* car) :
		PlayerCarState(state, car) {
	};

	// 開始
	void Enter() override;
	// 更新
	void Update(float dt) override;
	// 終了
	void Exit() override;


private:
	// 位置
	Vector3 pos_ = { -2.65f,0.05f,-4.0f };
	// 埃オフセット
	Vector3 scrapOffset_ = { 0,0.4f,0 };
	float scrapTimer_ = 0.0f;
	float scrapInterval_ = 0.15f;
};
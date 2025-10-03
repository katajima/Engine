#pragma once
#include<string>
#include <memory>
#include <functional>
#include"DirectXGame/engine/math/MathFanctions.h"

class BaseEnemy;
class BaseEnemyState {
public:
	BaseEnemyState(const std::string& name, BaseEnemy* enemy) :name_(name), enemy_(enemy) {};
	// 更新
	virtual void Update() = 0;
	// 終了
	virtual void Exit() = 0;
	// 初期化
	virtual void Enter() = 0;

	std::string GetName() { return name_; }
protected:
	// 状態名
	std::string name_;

	// 操作状態
	BaseEnemy* enemy_ = nullptr;
};

#pragma once

#include <optional>
#include <memory>
#include <map>
#include <string>

// 前方宣言
class BaseCharacter;

// スペシャルステート
class BaseSpecialState {
public:
	virtual ~BaseSpecialState() = default;
	// 開始
	virtual void Enter(BaseCharacter* owner) = 0;
	// 更新
	virtual void Update(BaseCharacter* owner, float deltaTime) = 0;
	// 終了
	virtual void Exit(BaseCharacter* owner) = 0;
private:
	// ステート名
	std::string name;
	// アニメーション名
	std::string animation;
};

// スペシャルステート
class SpecialNodeState : public BaseSpecialState {
public:
	// 開始
	void Enter(BaseCharacter* owner) override;
	// 更新
	void Update(BaseCharacter* owner, float dt) override;
	// 終了
	void Exit(BaseCharacter* owner) override;


private:
	// 次のステートマップ
	std::map<std::string, std::shared_ptr<SpecialNodeState>> nextStates;
};



// スペシャルステートマシーン
class SpecialStateMachine {


};

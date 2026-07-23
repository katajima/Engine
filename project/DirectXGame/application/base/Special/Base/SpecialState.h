#pragma once

#include <optional>
#include <memory>
#include <map>
#include <string>

// 前方宣言
class BaseCharacter;

// スペシャルステート
/// <summary>
/// BaseSpecialStateを管理・実装するクラス。
/// </summary>
class BaseSpecialState {
public:
	virtual ~BaseSpecialState() = default;
	/// <summary>
	/// 開始
	/// </summary>
	virtual void Enter(BaseCharacter* owner) = 0;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update(BaseCharacter* owner, float deltaTime) = 0;
	/// <summary>
	/// 終了
	/// </summary>
	virtual void Exit(BaseCharacter* owner) = 0;
private:
	// ステート名
	std::string name;
	// アニメーション名
	std::string animation;
};

// スペシャルステート
/// <summary>
/// SpecialNodeStateを管理・実装するクラス。
/// </summary>
class SpecialNodeState : public BaseSpecialState {
public:
	/// <summary>
	/// 開始
	/// </summary>
	void Enter(BaseCharacter* owner) override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update(BaseCharacter* owner, float dt) override;
	/// <summary>
	/// 終了
	/// </summary>
	void Exit(BaseCharacter* owner) override;


private:
	// 次のステートマップ
	std::map<std::string, std::shared_ptr<SpecialNodeState>> nextStates;
};



// スペシャルステートマシーン
/// <summary>
/// SpecialStateMachineを管理・実装するクラス。
/// </summary>
class SpecialStateMachine {


};

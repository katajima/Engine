#pragma once
#include <vector>
#include <memory>
#include <functional>

// 前方宣言
namespace Character {
	class BaseCharacter;
};

// コマンド
class ICommand 
{
public:
	virtual ~ICommand();
	virtual void Exec(Character::BaseCharacter& character) = 0;

};

// 移動コマンド
class MoveCommand :public ICommand 
{
public:
	void Exec(Character::BaseCharacter& character) override;
};
// ジャンプコマンド
class JampCommand : public ICommand
{
public:
	void Exec(Character::BaseCharacter& character) override;
};
// 攻撃コマンド
class AttackCommand : public ICommand
{
public:
	void Exec(Character::BaseCharacter& character) override;
};

// 攻撃コマンド
class HeavyAttackCommand : public ICommand
{
public:
	void Exec(Character::BaseCharacter& character) override;
};


// 前方宣言
namespace Engine {
	class Input;
}
// インプットハンドラー
class InputHander 
{
public:
	// 初期化
	void Initialize(Engine::Input* input) { input_ = input; }


	// 追加：条件とコマンドを登録（優先度順に並べる）
	void Bind(std::function<bool()>&& condition, std::unique_ptr<ICommand> cmd)
	{
		bindings_.push_back({ std::move(condition), std::move(cmd) });
	}

	// ハンドルインプット取得
	ICommand* HandleInput();
private:
	Engine::Input* input_ = nullptr;

	struct Binding
	{
		std::function<bool()> condition;
		std::unique_ptr<ICommand> cmd;
	};

	std::vector<Binding> bindings_;
};
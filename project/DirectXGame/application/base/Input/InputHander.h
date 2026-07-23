#pragma once
#include <vector>
#include <memory>
#include <functional>

// 前方宣言
namespace Engine {
	class Input;
}


namespace Character {

	// 前方宣言
	class BaseCharacter;


	// コマンド
/// <summary>
/// ICommandを管理・実装するクラス。
/// </summary>
	class ICommand
	{
	public:
		virtual ~ICommand();
		virtual void Exec(Character::BaseCharacter& character) = 0;

	};

	// 移動コマンド
/// <summary>
/// MoveCommandを管理・実装するクラス。
/// </summary>
	class MoveCommand :public ICommand
	{
	public:
		void Exec(Character::BaseCharacter& character) override;
	};
	// ジャンプコマンド
/// <summary>
/// JampCommandを管理・実装するクラス。
/// </summary>
	class JampCommand : public ICommand
	{
	public:
		void Exec(Character::BaseCharacter& character) override;
	};
	// 回避コマンド
/// <summary>
/// DodgeCommandを管理・実装するクラス。
/// </summary>
	class DodgeCommand : public ICommand
	{
	public:
		void Exec(Character::BaseCharacter& character) override;
	};
	// 攻撃コマンド
/// <summary>
/// AttackCommandを管理・実装するクラス。
/// </summary>
	class AttackCommand : public ICommand
	{
	public:
		void Exec(Character::BaseCharacter& character) override;
	};

	// 攻撃コマンド
/// <summary>
/// HeavyAttackCommandを管理・実装するクラス。
/// </summary>
	class HeavyAttackCommand : public ICommand
	{
	public:
		void Exec(Character::BaseCharacter& character) override;
	};


	// 攻撃コマンド
/// <summary>
/// SkillAttackCommandを管理・実装するクラス。
/// </summary>
	class SkillAttackCommand : public ICommand
	{
	public:
		void Exec(Character::BaseCharacter& character) override;
	};

	// インプットハンドラー
/// <summary>
/// InputHanderを管理・実装するクラス。
/// </summary>
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
};

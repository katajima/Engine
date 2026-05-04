#include "InputHander.h"
#include "DirectXGame/application/base/Character/base/BaseCharacter.h"
#include"DirectXGame/engine/input/Input.h"

namespace Character {
	ICommand::~ICommand()
	{
	}

	void MoveCommand::Exec(Character::BaseCharacter& character){
		character.Move();
	}
	void JampCommand::Exec(Character::BaseCharacter& character){
		character.Jump();
	}
	void AttackCommand::Exec(Character::BaseCharacter& character){
		character.Attack();
	}
	void HeavyAttackCommand::Exec(Character::BaseCharacter& character){
		character.HeavyAttack();
	}
	void SkillAttackCommand::Exec(Character::BaseCharacter& character){
		character.SkillAttack();
	}

	ICommand* InputHander::HandleInput()
	{
		if (!input_) return nullptr;

		// 未接続なら何もしない（必要ならキーマウも混ぜる）
		if (!input_->IsControllerConnected()) return nullptr;

		// 上から順に評価して、最初に成立したコマンドを返す
		for (auto& b : bindings_)
		{
			if (b.condition())
				return b.cmd.get();
		}
		return nullptr;
	}
}
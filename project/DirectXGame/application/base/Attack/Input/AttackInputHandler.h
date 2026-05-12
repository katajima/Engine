#pragma once
#include<memory>
#include<string>
#include "DirectXGame/application/base/Character/State/MainState.h"


// 動作種類
enum class ActionInput {
	LightAttack,        // 弱攻撃
	HeavyAttack,        // 強攻撃
	Skill,				// スキル攻撃
};

// 攻撃コンテキスト
struct AttackContext {
	Character::CharacterMainState mainState = Character::CharacterMainState::Idle;
	bool isLanding = true;
	bool isSpecial = false;
	int stamina = 0;
};

// 使用攻撃判定データ
struct AttackDecision {
	bool accepted = false;
	bool isComboInput = false;
	ActionInput comboInput = ActionInput::LightAttack;
	bool shouldChangeToAttackState = false;
	std::string startComboName;
	float staminaCost = 0;
};

// 判定で攻撃を選別するクラス
class AttackBranchResolver {
public:
	static AttackDecision Resolve(const AttackContext& ctx, ActionInput input);
};

// 前方宣言
namespace Character {
	class BaseCharacter;
}

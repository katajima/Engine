#pragma once
#include<memory>
#include "DirectXGame/application/base/Character/State/MainState.h"
#include "DirectXGame/engine/Math/MathFunctions.h"

// 動作種類
enum class ActionInput {
	LightAttack,          // 弱攻撃
	HeavyAttack,          // 強攻撃
	Skill,          // スキル攻撃
	Special,        // 必殺技など特殊攻撃
	Cancel,         // コンボキャンセル（ステップや回避などに移行）
	Parry,          // 敵の攻撃に対するカウンター
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

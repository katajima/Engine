#include "AttackInputHandler.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"

AttackDecision AttackBranchResolver::Resolve(const AttackContext& ctx,
    ActionInput input){
    AttackDecision result{};

    if (ctx.isSpecial) {
        return result;
    }

    // 最初か
    const bool canStart =
        (ctx.mainState == Character::CharacterMainState::Move) ||
        (ctx.mainState == Character::CharacterMainState::Idle) ||
        (ctx.mainState == Character::CharacterMainState::Jump);

    // 攻撃中か
    const bool isAttack = (ctx.mainState == Character::CharacterMainState::Attack);

    // 攻撃中なら
    if (isAttack) {
        result.accepted = true;
        result.isComboInput = true;
        result.comboInput = input;
        return result;
    }

    if (!canStart) {
        return result;
    }

    switch (input) {
    case ActionInput::LightAttack:
        result.accepted = true;
        result.shouldChangeToAttackState = true;
        result.startComboName = ctx.isLanding ? "Attack1" : "JumpAttack";
        break;

    case ActionInput::HeavyAttack:
        result.accepted = true;
        result.shouldChangeToAttackState = true;
        result.startComboName = ctx.isLanding ? "Attack10" : "Attack10";
        break;

    case ActionInput::Skill:
        if (ctx.stamina < 25) {
            return result;
        }
        result.accepted = true;
        result.shouldChangeToAttackState = true;
        result.startComboName = ctx.isLanding ? "SkillAttack01" :"JumpSkillAttack01";
        result.staminaCost = 25;
        break;

    default:
        break;
    }

    return result;
}

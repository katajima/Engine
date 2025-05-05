#include "AttackManager.h"
#include "DirectXGame/application/base/Attack/Type/AttackType.h"

void AttackManager::Initialize(Input* input)
{
	input_ = input;
}

void AttackManager::Update(float dt)
{
    if (currentAttack_) {
        currentAttack_->Update(dt);

        if (currentAttack_->IsFinished()) {
            currentAttack_.reset();
            currentAttackId_.clear();
        }
        else {
            TryTransition(); // コンボ分岐チェック
        }
    }
}

void AttackManager::AddAttack(const std::string& attackId)
{
    auto it = attackNodes_.find(attackId);
    if (it == attackNodes_.end()) return;

    const AttackData& data = it->second.data;

    switch (data.attackType)
    {
    case AttackType::Blow:
        currentAttack_ = std::make_unique<BlowAttack>();
        break;
    case AttackType::ConsecutiveHits:
        currentAttack_ = std::make_unique<ConsecutiveHitsAttack>();
        break;
    case AttackType::Duration:
        currentAttack_ = std::make_unique<DurationAttack>();
        break;
    case AttackType::LastConsecutiveHits:
        currentAttack_ = std::make_unique<LastConsecutiveHitsAttack>();
        break;
    case AttackType::LastBlow:
        currentAttack_ = std::make_unique<LastBlowAttack>();
        break;
    case AttackType::Charge:  // ← 追加
        currentAttack_ = std::make_unique<ChargeAttack>();
        break;
    default:
        currentAttack_ = std::make_unique<BlowAttack>();
        break;
    }


    currentAttack_->SetInput(input_);
    currentAttack_->SetTransforms(transforms_); // ← マルチTransform対応
    currentAttack_->SetAttackData(data);
    currentAttack_->Start();
    currentAttackId_ = attackId;
}


void AttackManager::RegisterAttackNode(const AttackNode& node)
{
    attackNodes_[node.id] = node;
}

void AttackManager::SetContext(Input* input, const std::unordered_map<std::string, Transform*>& transforms) {
    input_ = input;
    transforms_ = transforms;
}

void AttackManager::TryTransition()
{
    const auto& node = attackNodes_[currentAttackId_];
    if (!currentAttack_->CanTransition()) return;

    for (const auto& nextId : node.nextNodeIds) {
        const auto& nextNode = attackNodes_[nextId];
        if (nextNode.canCancelFunc && nextNode.canCancelFunc()) {
            AddAttack(nextId);
            break;
        }
    }
}


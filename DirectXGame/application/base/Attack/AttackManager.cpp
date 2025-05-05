#include "AttackManager.h"

void AttackManager::Initialize(Input* input, BaseAttackFactory* factory)
{
	input_ = input;
    factory_ = factory;
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
    if (!factory_) return;

    auto attack = factory_->Create(attackId);
    if (!attack) return;

    attack->SetInput(input_);
    attack->SetTransforms(transforms_);
    attack->SetAttackData(attackNodes_[attackId].data);
    attack->Start();

    currentAttack_ = std::move(attack);
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


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


void AttackManager::AddAttack(const std::string& attackId) {
	
	if (currentAttack_ && !currentAttack_->IsFinished()) {
		return; // 攻撃中は無視する
	}

	auto it = attackNodes_.find(attackId);
	if (it == attackNodes_.end() || !factory_) return;

	const AttackData& data = it->second.data;

	std::unique_ptr<BaseAttack> attack = factory_->Create(attackId);
	if (!attack) return;

	attack->SetInput(input_);

	attack->SetTransforms(transformMap_);

	attack->SetAttackData(data);
	attack->Start();

	currentAttack_ = std::move(attack);
	currentAttackId_ = attackId;
}




void AttackManager::RegisterAttackNode(const std::string& id, const AttackNode& node)
{
	AttackNode copy = node;
	copy.id = id;
	attackNodes_[id] = copy;
}


void AttackManager::SetContext(Input* input, const std::unordered_map<std::string, WorldTransform*>& transforms) {
	input_ = input;
	transformMap_ = transforms;
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


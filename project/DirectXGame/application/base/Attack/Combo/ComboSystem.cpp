#include "ComboSystem.h"

void ComboSystem::Initialize(BaseCharacter* character) {
	comboStateMachine_ = std::make_unique<ComboStateMachine>(character);
}

void ComboSystem::ClearNode() {
	comboNodes_.clear();
	//comboStateMachine_->Reset();
}

void ComboSystem::AddComboNode(const std::string& name, std::shared_ptr<ComboNodeState> node) {
	// 既に存在する場合は追加しない
	if (comboNodes_.find(name) != comboNodes_.end()) {
		return;
	}
	comboNodes_[name] = node;
}

void ComboSystem::AddComboNode(const std::string& name, const ComboData& data){
	// 既に存在する場合は追加しない
	if (comboNodes_.find(name) != comboNodes_.end()) {
		return;
	}
	std::shared_ptr<ComboNodeState> node = std::make_shared<ComboNodeState>(name, data);
	comboNodes_[name] = node;
}

void ComboSystem::ConnectCombo(const std::string& from, AttackInput input, const std::string& to){
	auto itFrom = comboNodes_.find(from);
	auto itTo = comboNodes_.find(to);
	if (itFrom != comboNodes_.end() && itTo != comboNodes_.end()) {
		itFrom->second->SetNextState(input, itTo->second);
	}
}

void ComboSystem::StartCombo(const std::string& name){
	auto it = comboNodes_.find(name);
	if (it != comboNodes_.end()) {
		comboStateMachine_->SetRoot(it->second);
	}
}

#include "DamageUIManager.h"

void DamageUIManager::Initialize(Input* input, Entity2DManager* entity2DManager, GlobalVariables* globalVariables)
{
	input_ = input;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
}

void DamageUIManager::Update() {

	// 削除対象のキーを一時保存する配列
	std::vector<std::string> toDelete;

	for (auto& [key, ui] : damageUi_) {
		if (ui->IsDelete()) {
			toDelete.push_back(key);
		}
	}

	for (auto& ui : damageUi_) {
		ui.second->Update();
	}

	// 削除
	for (auto& key : toDelete) {
		damageUi_.erase(key);
	}

};

void DamageUIManager::Draw() {
	for (auto& ui : damageUi_) {
		if (ui.second) {
			ui.second->Draw();
		}
	}
};

void DamageUIManager::AddUI(std::string name, float damage, float deleteTime, Vector3 pos)
{
	std::unique_ptr<DamageUI> ui = std::make_unique<DamageUI>();
	std::string na = name + std::to_string(count);
	ui->Initialize(input_, entity2DManager_, globalVariables_);
	ui->SetCamera(camera_);
	ui->AddDamegeUI(na, deleteTime, damage, pos);
	
	damageUi_.insert(std::make_pair(na, std::move(ui)));
	count++;
}

#include "BulletWeaponManager.h"
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"

void BulletWeaponManager::Initialize(BulletManager* bulletManager, Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables)
{
	bulletManager_ = bulletManager;
	input_ = input;
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
}


/// <summary>
/// 更新
/// </summary>
void BulletWeaponManager::Update() {

	for(auto& weapon : bulletWeapons_) {
		weapon.second->Update();
	}

}

/// <summary>
/// 描画
/// </summary>
void BulletWeaponManager::Draw() {

}

void BulletWeaponManager::AddBulletWeapon(const std::string& name, const Vector3& pos)
{
	if(bulletWeapons_.find(name) != bulletWeapons_.end()) {
		// すでに存在する場合は追加しない
		return;
	}

	auto weapon = std::make_unique<BulletPlayerWeapon>();
	weapon->SetParent(player_);
	weapon->SetEffect(effect_);
	weapon->Initialize(input_, entity3DManager_, entity2DManager_, nullptr, pos, nullptr);
	weapon->SetBulletManager(bulletManager_);
	bulletWeapons_[name] = std::move(weapon);
}

void BulletWeaponManager::SetTargets(std::vector<BaseEnemy*> targets)
{
	// ターゲットが存在しない場合は終了
	if(targets.empty()) {
		// ターゲットをクリア
		for (auto& weapon : bulletWeapons_) {
			weapon.second->SetTarget(nullptr);
		}
		return;
	}

	int i = 0;
	for (auto& weapon : bulletWeapons_) {
		for (int j = 0; j < weapon.second->MaxTargetNum();j++) {
			targets[i]->SetIsLockOn(true);
			weapon.second->SetTarget(targets[i]);
			i++;
			// ターゲットの量が足りなくなったら終了
			if(i >= targets.size()) {
				return;
			}
		}
	}
}

int BulletWeaponManager::GetMaxtargetNum()
{
	int i = 0;
	for (auto& weapon : bulletWeapons_) {
		i += weapon.second->MaxTargetNum();
	}
	return i;
}

BulletPlayerWeapon* BulletWeaponManager::GetBulletWeapon(const std::string& name)
{
	if(bulletWeapons_.find(name) != bulletWeapons_.end()) {
		return bulletWeapons_[name].get();
	}
	return nullptr;
}

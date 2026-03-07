#include "BulletWeaponManager.h"
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"

void BulletWeaponManager::Initialize(BulletManager* bulletManager, InputSystem* inputSystem, Engine::EntityManager* entityManager,
Engine::GlobalVariables* globalVariables) {
	this->bulletManager = bulletManager;		// 弾管理クラス更新
	this->inputSystem = inputSystem;			// インプット
	this->entityManager = entityManager;	// エンティティ
}


/// <summary>
/// 更新
/// </summary>
void BulletWeaponManager::Update() {
	// 更新
	for(auto& weapon : bulletWeapons_) {
		weapon.second->Update();
	}

}

/// <summary>
/// 描画
/// </summary>
void BulletWeaponManager::Draw() {

}

void BulletWeaponManager::AddBulletWeapon(const std::string& name, const Vector3& pos, const Vector3 pos2)
{
	if(bulletWeapons_.find(name) != bulletWeapons_.end()) {
		// すでに存在する場合は追加しない
		return;
	}

	auto weapon = std::make_unique<BulletPlayerWeapon>();
	weapon->SetParent(player);				// 使用者設定
	weapon->SetEffect(effect);				// エフェクト設定
	weapon->SetModePenetrationPos(pos2);	// 位置設定
	weapon->Initialize(inputSystem, entityManager, nullptr, pos, nullptr);
	weapon->SetBulletManager(bulletManager);
	bulletWeapons_[name] = std::move(weapon);
}

void BulletWeaponManager::SetTargets(std::vector<Character::BaseEnemy*> targets)
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


/// <summary>
/// 通常弾
/// </summary>
void BulletWeaponManager::Normal() {
	for (auto& weapon : bulletWeapons_) {
		weapon.second->SetModeType(BulletPlayerWeapon::ModeType::Normal);
	}
};


/// <summary>
/// 貫通弾
/// </summary>
void BulletWeaponManager::Penetration() {
	for (auto& weapon : bulletWeapons_) {
		weapon.second->SetModeType(BulletPlayerWeapon::ModeType::Penetration);
	}
};
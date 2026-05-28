#include "ProjectileDebug.h"

#include "DirectXGame/application/base/Bullet/Base/BulletManager.h"
#include "DirectXGame/engine/Base/Imgui/ImGuiManager.h"

void Projectile::ProjectileDebug::Initialize(Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, EffectSystem* effectSystem,
	BulletManager* bulletManager, Character::BaseCharacter* target) {
	bulletManager_ = bulletManager;

	// デバッグ用の独立したBulletSpawnを用意し、選択した定義からBaseProjectileを生成する。
	bulletSpawn_ = std::make_unique<BulletSpawn>();
	bulletSpawn_->Initialize(nullptr, entityManager, globalVariables, nullptr, effectSystem, bulletManager);

	// CharacterDebugSceneで試しやすい初期位置。以後はImGuiから調整する。
	spawnInfo_.position = { 10.0f, 2.0f, -40.0f };
	spawnInfo_.direction = { -1.0f, 0.0f, 0.0f };
	spawnInfo_.scale = { 0.5f, 0.5f, 0.5f };
	spawnInfo_.target = target;
}

void Projectile::ProjectileDebug::Update() {
#ifdef _DEBUG
	ImGui::Begin("Projectile Debug");

	const auto& definitions = bulletManager_->GetProjectileDefinitions();
	if (definitions.empty()) {
		ImGui::TextDisabled("Projectile Editor で定義を追加してください。");
		ImGui::End();
		return;
	}

	// 選択中の定義が削除・リロードで消えた場合は、先頭の定義へ戻す。
	if (selectedDefinitionName_.empty() || !definitions.contains(selectedDefinitionName_)) {
		selectedDefinitionName_ = definitions.begin()->first;
	}

	Engine::ImGuiManager::Select("Projectile Definition", selectedDefinitionName_, definitions);
	ImGui::DragFloat3("Position", &spawnInfo_.position.x, 0.1f);
	ImGui::DragFloat3("Direction", &spawnInfo_.direction.x, 0.01f);
	ImGui::DragFloat3("Scale", &spawnInfo_.scale.x, 0.01f, 0.01f);
	if (ImGui::Button("Spawn Projectile")) {
		// 弾定義名からProjectileParamを引いて生成するため、シーン側は詳細を知らなくてよい。
		bulletSpawn_->GenerateProjectile(spawnInfo_, selectedDefinitionName_, spawnInfo_.target);
	}

	ImGui::End();
#endif
}

#include "PlayerSubWeapon.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"

void PlayerSubWeapon::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
	this->inputSystem = inputSystem;						// インプット
	this->entityManager = entityManager; // エンティティ3d

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(entityManager, globalVariables, "PlayerWeapon", "Sword.obj", false, false, this);
	objectComponent_->SetSRT({0,0,0}, {}, position);	// SRT設定

	// 描画する
	GetObject3D()->SetIsDraw(true);
}

void PlayerSubWeapon::Update() {
	// ワールドトランスフォーム更新
	GetObject3D()->UpdateWorldTransform();
}

void PlayerSubWeapon::DrawEffect() {

}

void PlayerSubWeapon::Draw2D() {

}

#include "PlayerWeapon.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

void PlayerWeapon::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
{
	// プレイヤー武器が参照する入力とエンティティ管理を保持する
	this->inputSystem = inputSystem;						// インプット
	this->entityManager = entityManager; // エンティティ3d
	
	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(entityManager, globalVariables, "PlayerWeapon", "Sword.obj", false, false, this);
	objectComponent_->SetSRT(provisionalData_.size, {}, position);	// SRT設定
	// 描画する
	GetObject3D()->SetIsDraw(true);
	// スケール
	GetObject3D()->GetWorldTransform().scale_ = provisionalData_.scale_;
}

void PlayerWeapon::Update(){
	// ワールドトランスフォーム更新
	GetObject3D()->UpdateWorldTransform();
}

void PlayerWeapon::Draw2D(){
	// プレイヤー武器専用の2D表示は未実装
}

void PlayerWeapon::DrawEffect(){
	// トレイルは独立Entityとして生成しているため、ここでは追加描画を行わない
}
